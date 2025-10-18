FROM nvidia/cuda:12.0-devel-ubuntu22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    pkg-config \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    libgrpc-dev \
    libbenchmark-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Install modern C++ compiler
RUN apt-get update && apt-get install -y \
    gcc-12 \
    g++-12 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory and compile
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_STANDARD=23 \
          -DCUDA_ARCHITECTURES=75,80,86 \
          .. && \
    make -j$(nproc)

# Set runtime configuration
ENV CUDA_VISIBLE_DEVICES=0
ENV LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

# Expose gRPC port
EXPOSE 50051

# Set CPU affinity and other performance optimizations
RUN echo 'performance' > /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor || true

# Run the trading engine
CMD ["./build/trading_engine"]