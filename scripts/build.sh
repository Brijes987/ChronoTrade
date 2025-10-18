#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building ChronoTrade High-Performance Trading Engine${NC}"

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo -e "${RED}cmake is required but not installed.${NC}" >&2; exit 1; }
command -v make >/dev/null 2>&1 || { echo -e "${RED}make is required but not installed.${NC}" >&2; exit 1; }

# Build configuration
BUILD_TYPE=${1:-Release}
BUILD_DIR="build"
JOBS=$(nproc 2>/dev/null || echo 4)

echo -e "${YELLOW}Build configuration:${NC}"
echo "  Build type: $BUILD_TYPE"
echo "  Build directory: $BUILD_DIR"
echo "  Parallel jobs: $JOBS"

# Create build directory
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_CXX_STANDARD=23 \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCUDA_ARCHITECTURES="75;80;86" \
      ..

# Build
echo -e "${YELLOW}Building...${NC}"
make -j"$JOBS"

# Run tests if in Debug mode
if [ "$BUILD_TYPE" = "Debug" ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
fi

echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}Executable: $BUILD_DIR/trading_engine${NC}"