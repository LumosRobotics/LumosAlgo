# Matrix Fixed Module Tests

This directory contains unit tests for the FixedSizeMatrix class and related functionality.

## Test Coverage

The tests cover:
- Constructor and copy constructor functionality
- Basic matrix operations (accessors, dimensions, fill)
- Matrix arithmetic operations (addition, subtraction, multiplication, division)
- Scalar operations
- Matrix utility functions (transpose, max, min, sum)
- Special matrix creation functions (unit, zeros, ones matrices)
- Rotation matrix functions
- Edge cases and error conditions

## Running Tests

To build and run the tests:

```bash
cd build
make matrix_fixed_test
./src/math/lin_alg/matrix_fixed/test/matrix_fixed_test
```

Or use CTest:

```bash
ctest -R MatrixFixedTest
```
