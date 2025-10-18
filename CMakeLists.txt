cmake_minimum_required(VERSION 3.20)
project(TradingEngine VERSION 1.0.0 LANGUAGES CXX)

# Options for CI/CD flexibility
option(ENABLE_CUDA "Enable CUDA support" OFF)
option(ENABLE_GRPC "Enable gRPC support" OFF)
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

# Use C++17 for maximum CI compatibility
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Performance optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Find required packages
find_package(Threads REQUIRED)

# Optional packages
if(ENABLE_GRPC)
    find_package(Protobuf)
    find_package(gRPC)
endif()

if(BUILD_TESTING)
    find_package(GTest)
endif()

if(BUILD_BENCHMARKS)
    find_package(benchmark)
endif()

if(ENABLE_CUDA)
    enable_language(CUDA)
    find_package(CUDA)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Core library (always build)
add_subdirectory(src/core)
add_subdirectory(src/matching_engine)
add_subdirectory(src/market_data)
add_subdirectory(src/risk_management)
add_subdirectory(src/networking)

# Optional components
if(ENABLE_CUDA)
    add_subdirectory(src/gpu_compute)
endif()

if(ENABLE_GRPC)
    add_subdirectory(src/grpc_api)
endif()

# Main executable
add_executable(trading_engine src/main.cpp)

# Link core libraries
target_link_libraries(trading_engine 
    core 
    matching_engine 
    market_data 
    risk_management
    networking
    Threads::Threads
)

# Link optional libraries
if(ENABLE_CUDA)
    target_link_libraries(trading_engine gpu_compute)
endif()

if(ENABLE_GRPC)
    target_link_libraries(trading_engine grpc_api)
endif()

# Tests (optional)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks (optional)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()