# Matrix Dynamic Tests

This directory contains unit tests for the matrix_dynamic module of the LumosAlgo library.

## Test Coverage

The tests cover the following functionality:

### Matrix Class Basic Operations
- Constructors (default, parameterized, copy, move)
- Assignment operators (copy, move)
- Element access operators
- Basic properties (size, dimensions, data access)
- Memory management

### Matrix Mathematical Operations
- Addition (+)
- Subtraction (-)
- Multiplication (*)
- Division (/)
- Element-wise operations (^)
- Scalar operations
- Unary minus (-)
- Matrix-vector multiplication
- Transpose

### Matrix Mathematical Functions
- Trigonometric functions (sin, cos, sinh, cosh)
- Logarithmic functions (log, log10)
- Exponential functions (exp, pow)
- Square root (sqrt)
- Absolute value (abs)
- Min/max functions
- Element-wise multiplication

### Matrix Utility Functions
- Identity matrix creation (unitMatrix)
- Zero matrix creation (zerosMatrix)
- Ones matrix creation (onesMatrix)
- Filled matrix creation (filledMatrix)
- Rotation matrices (2D and 3D)
- Mesh grid generation

### Matrix Properties
- Min/max values
- Sum calculation
- Fill operations
- Resize operations
- Data buffer operations

## Building and Running Tests

To run the tests, use the following commands from the build directory:

```bash
# Build the tests
make matrix_dynamic_test

# Run the tests
./src/math/lin_alg/matrix_dynamic/test/matrix_dynamic_test

# Or run through CTest
ctest -R MatrixDynamicTest
```
