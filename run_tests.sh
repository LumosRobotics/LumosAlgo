#!/bin/bash
# filepath: /Users/daniel/work/LumosAlgo/run_tests.sh

echo "Building and running LumosAlgo Curves tests..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake ..
make

# Run the tests
echo "Running curves tests..."
./src/math/curves/test/curves_test

# Run with CTest for better output
echo "Running tests with CTest..."
ctest --verbose

echo "Tests completed!"