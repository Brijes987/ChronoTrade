#!/bin/bash

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}Running ChronoTrade Performance Benchmarks${NC}"

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please run build.sh first."
    exit 1
fi

cd "$BUILD_DIR"

# Create results directory
mkdir -p benchmark_results

echo -e "${YELLOW}Running matching engine benchmarks...${NC}"
./benchmarks/matching_engine_benchmark \
    --benchmark_format=json \
    --benchmark_out=benchmark_results/matching_engine.json

echo -e "${YELLOW}Running market data benchmarks...${NC}"
./benchmarks/market_data_benchmark \
    --benchmark_format=json \
    --benchmark_out=benchmark_results/market_data.json

echo -e "${YELLOW}Running GPU calculation benchmarks...${NC}"
./benchmarks/gpu_calculations_benchmark \
    --benchmark_format=json \
    --benchmark_out=benchmark_results/gpu_calculations.json

echo -e "${GREEN}Benchmarks completed!${NC}"
echo "Results saved in: $BUILD_DIR/benchmark_results/"