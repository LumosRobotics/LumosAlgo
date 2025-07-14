#!/bin/bash
# filepath: /Users/daniel/work/LumosAlgo/run_tests.sh

echo "Building and running LumosAlgo tests..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake ..
make

# Run the curves tests
echo "Running curves tests..."
./src/math/curves/test/curves_test

# Run the fixed_size_vector tests
echo "Running fixed_size_vector tests..."
./src/math/lin_alg/fixed_size_vector/test/fixed_size_vector_test

# Run with CTest for better output
echo "Running all tests with CTest..."
ctest --verbose

echo "Tests completed!"