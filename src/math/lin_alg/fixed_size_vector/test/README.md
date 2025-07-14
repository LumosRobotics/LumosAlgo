# FixedSizeVector Module Unit Tests

This directory contains comprehensive unit tests for the `FixedSizeVector` class, a template-based fixed-size vector implementation in the LumosAlgo library.

## Test Coverage

The test suite covers all functionality of the `FixedSizeVector<T, N>` template class:

### 1. Constructor Tests
- **Default Constructor**: Tests uninitialized vector creation
- **Initializer List Constructor**: Tests initialization with `{...}` syntax
- **Partial Initialization**: Tests behavior with fewer elements than vector size
- **Over-initialization**: Tests behavior with more elements than vector size (should ignore excess)
- **Empty Initialization**: Tests initialization with empty `{}`

### 2. Element Access Tests
- **Subscript Operator**: Tests `operator[]` for both const and non-const access
- **Element Modification**: Tests that elements can be modified via `operator[]`
- **Const Correctness**: Ensures const vectors work correctly with const `operator[]`

### 3. Arithmetic Operations Tests
- **Addition (`operator+`)**: Element-wise vector addition
- **Subtraction (`operator-`)**: Element-wise vector subtraction
- **Scalar Multiplication (`operator*`)**: Vector scaling by scalar values
- **Special Cases**: Tests with zero vectors, negative scalars, zero scalars

### 4. Vector Operations Tests
- **Dot Product (`dot`)**: Tests dot product computation
- **Norm (`norm`)**: Tests Euclidean norm calculation
- **Commutative Properties**: Verifies dot product commutativity
- **Mathematical Properties**: Tests norm-dot product relationship

### 5. Stream Operations Tests
- **Output Stream (`operator<<`)**: Tests formatted output to streams
- **Different Dimensions**: Tests output for 1D, 2D, 3D, and 4D vectors
- **Different Types**: Tests output for various numeric types

### 6. Template Specialization Tests
- **Different Types**: Tests with `int`, `float`, `double` types
- **Different Sizes**: Tests with various vector dimensions (1D to 100D)
- **Type Safety**: Ensures different template instantiations work correctly

### 7. Mathematical Property Tests
- **Distributive Properties**: Tests algebraic properties of operations
- **Associative Properties**: Tests operation associativity
- **Commutative Properties**: Tests operation commutativity
- **Cauchy-Schwarz Inequality**: Tests fundamental vector inequality
- **Norm Properties**: Tests relationship between norm and dot product

### 8. Edge Cases and Robustness Tests
- **Large Vectors**: Tests with vectors of size 100
- **Chained Operations**: Tests complex operation sequences
- **Floating-Point Precision**: Tests numerical precision with small values
- **Performance/Stress Tests**: Tests stability under many operations

### 9. Integration Tests
- **Type Compatibility**: Tests interoperability between different instantiations
- **Cross-template Usage**: Ensures templates work across different contexts

## Test Statistics
- **Total Tests**: 49 tests in 1 test suite
- **Coverage**: All public methods and operators
- **Precision**: Uses appropriate floating-point comparisons (`EXPECT_NEAR`, `EXPECT_FLOAT_EQ`, `EXPECT_DOUBLE_EQ`)
- **Type Safety**: Tests with multiple numeric types (`int`, `float`, `double`)

## Key Mathematical Verifications

### Vector Operations
- Verifies that vector addition and subtraction work element-wise
- Ensures scalar multiplication scales all components
- Tests that dot product follows mathematical definition: `a·b = Σ(aᵢ * bᵢ)`

### Norm Calculations
- Verifies norm calculation: `||v|| = √(v·v)`
- Tests special cases like zero vectors and unit vectors
- Validates 3-4-5 triangle and other known geometric relationships

### Algebraic Properties
- **Distributive**: `a·(b+c) = a·b + a·c`
- **Commutative**: `a+b = b+a` and `a·b = b·a`
- **Associative**: `(a+b)+c = a+(b+c)`
- **Cauchy-Schwarz**: `|a·b| ≤ ||a|| * ||b||`

## Template Features Tested

### Type System
- Tests with integral types (`int`)
- Tests with floating-point types (`float`, `double`)
- Verifies template parameter deduction works correctly

### Size Flexibility
- Tests with vectors from 1D to 100D
- Ensures compile-time size checking works
- Validates memory layout for different sizes

### Constexpr Support
- Tests compile-time evaluation capabilities
- Verifies constexpr operations work correctly
- Note: Some constexpr features may be limited by implementation

## Performance Characteristics

### Memory Efficiency
- Uses stack allocation for fixed-size arrays
- No dynamic memory allocation
- Efficient element access via direct indexing

### Computational Efficiency
- Simple loops for arithmetic operations
- Inline-friendly design for optimization
- Minimal function call overhead

## Usage Examples

### Basic Usage
```cpp
FixedSizeVector<double, 3> v1{1.0, 2.0, 3.0};
FixedSizeVector<double, 3> v2{4.0, 5.0, 6.0};

auto sum = v1 + v2;           // Element-wise addition
auto dot = v1.dot(v2);        // Dot product
auto norm = v1.norm();        // Euclidean norm
auto scaled = v1 * 2.0;       // Scalar multiplication
```

### Different Types and Sizes
```cpp
FixedSizeVector<int, 4> int_vec{1, 2, 3, 4};
FixedSizeVector<float, 2> float_vec{1.5f, 2.5f};
FixedSizeVector<double, 1> scalar{42.0};
```

## Running Tests

### Individual Test Execution
```bash
# From build directory
./src/math/lin_alg/fixed_size_vector/test/fixed_size_vector_test
```

### CTest Integration
```bash
# Run all tests
ctest --verbose

# Run only FixedSizeVector tests
ctest -R FixedSizeVector --verbose
```

### Test Organization
- Tests are organized by functionality (constructors, operations, etc.)
- Each test focuses on a specific aspect of the class
- Test fixtures provide common setup for related tests
- Clear test names describe what is being tested

## Dependencies
- Google Test framework for test infrastructure
- C++ Standard Library for `<cmath>`, `<iostream>`, etc.
- LumosAlgo math library headers

## Files
- `fixed_size_vector_test.cpp`: Main test file with all test cases
- `CMakeLists.txt`: Build configuration for tests
- This README documentation

The test suite ensures that the `FixedSizeVector` class is mathematically correct, type-safe, and performs efficiently across all supported use cases.
