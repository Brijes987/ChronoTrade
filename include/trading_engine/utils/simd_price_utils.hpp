#pragma once

#include <immintrin.h>
#include <cstdint>
#include <array>

namespace trading_engine::utils {

/**
 * SIMD-accelerated price normalization utilities
 * Uses AVX2 instructions for vectorized price operations
 */
class SIMDPriceUtils {
public:
    // Normalize 8 prices at once using AVX2
    static void normalize_prices_avx2(const int64_t* input, int64_t* output, 
                                      int64_t scale_factor, size_t count) {
        const size_t simd_width = 4; // AVX2 processes 4 int64_t at a time
        const size_t simd_iterations = count / simd_width;
        
        __m256i scale = _mm256_set1_epi64x(scale_factor);
        
        for (size_t i = 0; i < simd_iterations; ++i) {
            __m256i prices = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(input + i * simd_width));
            __m256i normalized = _mm256_mul_epi32(prices, scale);
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(output + i * simd_width), normalized);
        }
        
        // Handle remaining elements
        for (size_t i = simd_iterations * simd_width; i < count; ++i) {
            output[i] = input[i] * scale_factor;
        }
    }
    
    // Calculate weighted average price using SIMD
    static int64_t calculate_vwap_simd(const int64_t* prices, 
                                       const int64_t* quantities, 
                                       size_t count) {
        if (count == 0) return 0;
        
        __m256i sum_pq = _mm256_setzero_si256();
        __m256i sum_q = _mm256_setzero_si256();
        
        const size_t simd_width = 4;
        const size_t simd_iterations = count / simd_width;
        
        for (size_t i = 0; i < simd_iterations; ++i) {
            __m256i p = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(prices + i * simd_width));
            __m256i q = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(quantities + i * simd_width));
            
            __m256i pq = _mm256_mul_epi32(p, q);
            sum_pq = _mm256_add_epi64(sum_pq, pq);
            sum_q = _mm256_add_epi64(sum_q, q);
        }
        
        // Horizontal sum
        int64_t total_pq = horizontal_sum_avx2(sum_pq);
        int64_t total_q = horizontal_sum_avx2(sum_q);
        
        // Handle remaining elements
        for (size_t i = simd_iterations * simd_width; i < count; ++i) {
            total_pq += prices[i] * quantities[i];
            total_q += quantities[i];
        }
        
        return total_q > 0 ? total_pq / total_q : 0;
    }
    
private:
    static int64_t horizontal_sum_avx2(__m256i vec) {
        __m128i low = _mm256_castsi256_si128(vec);
        __m128i high = _mm256_extracti128_si256(vec, 1);
        __m128i sum = _mm_add_epi64(low, high);
        
        int64_t result[2];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(result), sum);
        return result[0] + result[1];
    }
};

} // namespace trading_engine::utils
