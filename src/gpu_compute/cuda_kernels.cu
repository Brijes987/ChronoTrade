#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cub/cub.cuh>

extern "C" {

__global__ void vwap_kernel(const float* prices, const float* quantities, 
                           float* result, int n) {
    __shared__ float shared_prices[256];
    __shared__ float shared_quantities[256];
    
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int idx = bid * blockDim.x + tid;
    
    float price_sum = 0.0f;
    float quantity_sum = 0.0f;
    
    // Load data into shared memory
    if (idx < n) {
        shared_prices[tid] = prices[idx];
        shared_quantities[tid] = quantities[idx];
    } else {
        shared_prices[tid] = 0.0f;
        shared_quantities[tid] = 0.0f;
    }
    
    __syncthreads();
    
    // Calculate weighted sum
    if (idx < n) {
        price_sum = shared_prices[tid] * shared_quantities[tid];
        quantity_sum = shared_quantities[tid];
    }
    
    // Reduce within block
    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            price_sum += shared_prices[tid + stride] * shared_quantities[tid + stride];
            quantity_sum += shared_quantities[tid + stride];
        }
        __syncthreads();
    }
    
    // Write result
    if (tid == 0) {
        atomicAdd(&result[0], price_sum);
        atomicAdd(&result[1], quantity_sum);
    }
}

__global__ void black_scholes_kernel(const float* spot_prices, const float* strikes,
                                   float rate, float volatility, float time,
                                   float* call_prices, float* put_prices, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= n) return;
    
    float S = spot_prices[idx];
    float K = strikes[idx];
    float r = rate;
    float sigma = volatility;
    float T = time;
    
    float d1 = (logf(S / K) + (r + 0.5f * sigma * sigma) * T) / (sigma * sqrtf(T));
    float d2 = d1 - sigma * sqrtf(T);
    
    // Approximation of cumulative normal distribution
    auto norm_cdf = [](float x) -> float {
        return 0.5f * (1.0f + erff(x / sqrtf(2.0f)));
    };
    
    float N_d1 = norm_cdf(d1);
    float N_d2 = norm_cdf(d2);
    float N_neg_d1 = norm_cdf(-d1);
    float N_neg_d2 = norm_cdf(-d2);
    
    // Black-Scholes formula
    call_prices[idx] = S * N_d1 - K * expf(-r * T) * N_d2;
    put_prices[idx] = K * expf(-r * T) * N_neg_d2 - S * N_neg_d1;
}

__global__ void var_calculation_kernel(const float* positions, const float* correlations,
                                     float* var_result, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= n) return;
    
    float portfolio_variance = 0.0f;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float correlation = correlations[i * n + j];
            portfolio_variance += positions[i] * positions[j] * correlation;
        }
    }
    
    // 95% VaR (1.645 standard deviations)
    float var_95 = 1.645f * sqrtf(portfolio_variance);
    
    if (idx == 0) {
        *var_result = var_95;
    }
}

void launch_vwap_kernel(const float* prices, const float* quantities, 
                       float* result, int n, cudaStream_t stream) {
    int block_size = 256;
    int grid_size = (n + block_size - 1) / block_size;
    
    // Initialize result to zero
    cudaMemsetAsync(result, 0, 2 * sizeof(float), stream);
    
    vwap_kernel<<<grid_size, block_size, 0, stream>>>(prices, quantities, result, n);
}

void launch_black_scholes_kernel(const float* spot_prices, const float* strikes,
                               float rate, float volatility, float time,
                               float* call_prices, float* put_prices,
                               int n, cudaStream_t stream) {
    int block_size = 256;
    int grid_size = (n + block_size - 1) / block_size;
    
    black_scholes_kernel<<<grid_size, block_size, 0, stream>>>(
        spot_prices, strikes, rate, volatility, time, call_prices, put_prices, n);
}

void launch_var_calculation_kernel(const float* positions, const float* correlations,
                                 float* var_result, int n, cudaStream_t stream) {
    int block_size = 256;
    int grid_size = (n + block_size - 1) / block_size;
    
    var_calculation_kernel<<<grid_size, block_size, 0, stream>>>(
        positions, correlations, var_result, n);
}

} // extern "C"