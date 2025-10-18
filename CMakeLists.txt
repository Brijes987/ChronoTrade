cmake_minimum_required(VERSION 3.20)
project(TradingEngine VERSION 1.0.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Performance optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address,undefined")

# Find packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(CUDA REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Core library
add_subdirectory(src/core)
add_subdirectory(src/matching_engine)
add_subdirectory(src/market_data)
add_subdirectory(src/risk_management)
add_subdirectory(src/networking)
add_subdirectory(src/gpu_compute)
add_subdirectory(src/grpc_api)

# Main executable
add_executable(trading_engine src/main.cpp)
target_link_libraries(trading_engine 
    core 
    matching_engine 
    market_data 
    risk_management
    networking
    gpu_compute
    grpc_api
    Threads::Threads
)

# Tests
enable_testing()
add_subdirectory(tests)

# Benchmarks
add_subdirectory(benchmarks)