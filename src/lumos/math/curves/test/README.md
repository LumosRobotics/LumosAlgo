# Curves Module Unit Tests

This directory contains comprehensive unit tests for the curves module of the LumosAlgo library.

## Test Coverage

The test suite covers three main curve types:

### 1. Bezier Curves (`BezierCurveTest`)
- **Constructor Testing**: Empty constructor, parameterized constructor
- **Control Points**: Setting, getting, adding, inserting, removing control points
- **Evaluation**: Curve evaluation at specific parameters (start, end, midpoint)
- **Derivatives**: First and second derivative evaluation
- **Curve Operations**: Degree elevation, curve splitting
- **3D Support**: Testing 3D Bezier curves
- **Error Handling**: Exception handling for invalid operations

### 2. B-Spline Curves (`BSplineCurveTest`)
- **Constructor Testing**: Empty constructor, parameterized constructor
- **Configuration**: Setting control points, knot vectors, and degree
- **Evaluation**: Curve evaluation at parameter values
- **Knot Vector Operations**: 
  - Finding knot spans
  - Generating uniform, clamped, and open-uniform knot vectors
  - Knot vector validation
- **Basis Functions**: Computing B-spline basis functions
- **Derivatives**: First derivative evaluation
- **3D Support**: Testing 3D B-spline curves
- **Validation**: Configuration validation

### 3. Quintic Polynomials (`QuinticPolynomialTest`)
- **Constructor Testing**: Empty constructor, boundary condition constructor
- **Boundary Conditions**: Verifying start/end position, velocity, and acceleration
- **Derivative Evaluation**: All derivatives up to 5th order (crackle)
- **Generic Derivative Interface**: Testing `evaluateDerivative()` method
- **Higher Order Derivatives**: Testing that 6th+ derivatives are zero
- **Transformations**: 
  - Time scaling
  - Amplitude scaling and translation
- **Time Clamping**: Parameter clamping to valid range
- **1D Specialization**: Testing scalar quintic polynomials (`QuinticPolynomial1DTest`)

### 4. Integration Tests (`CurvesIntegrationTest`)
- **Type Aliases**: Verifying all convenience type aliases work correctly
- **Cross-module Compatibility**: Ensuring all curve types work together

## Test Statistics
- **Total Tests**: 40 tests across 5 test suites
- **Coverage**: All major public methods and edge cases
- **Precision**: Uses appropriate floating-point comparison (`EXPECT_NEAR`, `EXPECT_DOUBLE_EQ`)
- **Error Handling**: Tests exception throwing for invalid operations

## Key Test Features

### Mathematical Correctness
- Verifies mathematical properties of curves (e.g., Bezier curves pass through endpoints)
- Tests derivative relationships and continuity
- Validates boundary conditions for quintic polynomials

### Robustness Testing
- Parameter clamping for out-of-range values
- Exception handling for invalid inputs
- Edge cases like empty curves or insufficient control points

### Performance Considerations
- Tests are designed to run quickly (all tests complete in ~0.01 seconds)
- Uses appropriate test fixtures to avoid redundant setup

## Usage

### Running Tests
```bash
# From project root
./run_tests.sh

# Or using CTest directly
cd build
ctest --verbose
```

### Test Structure
- Each curve type has its own test fixture class
- Tests are organized by functionality (construction, evaluation, derivatives, etc.)
- Common test data is set up in `SetUp()` methods of test fixtures

### Adding New Tests
1. Add test cases to the appropriate test fixture class
2. Follow the existing naming conventions
3. Use appropriate Google Test assertions
4. Include both positive and negative test cases
5. Test both 2D and 3D variants where applicable

## Dependencies
- Google Test framework
- LumosAlgo math library (vectors, matrices)
- C++17 standard library

## Files
- `curves_test.cpp`: Main test file with all test cases
- `CMakeLists.txt`: Build configuration for tests
- This README documentation

The test suite ensures that all curve implementations are mathematically correct, robust, and perform as expected across different use cases.
