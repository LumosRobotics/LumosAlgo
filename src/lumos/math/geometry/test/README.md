# Geometry Module Tests

This directory contains comprehensive unit tests for the geometry module of the LumosAlgo library.

## Test Coverage

The test suite covers the following geometry classes and functions:

### 2D Lines
- **HomogeneousLine2D**: Tests for homogeneous line representation (ax + by + c = 0)
  - Constructor from coefficients
  - Constructor from parametric line
  - Point evaluation (eval)
  - X and Y coordinate evaluation (evalX, evalY)
  - Type conversion constructors

- **ParametricLine2D**: Tests for parametric line representation (p + t*v)
  - Constructor from parameters and point/vector
  - Constructor from homogeneous line
  - Parameter calculation from coordinates (tFromX, tFromY)
  - Point evaluation at parameter t
  - Type conversion constructors

### 3D Lines
- **Line3D**: Tests for 3D parametric lines
  - Constructor from point and direction vector
  - Static factory method fromTwoPoints
  - Point evaluation at parameter t
  - Closest point on line from external point
  - Type conversion constructors

### Planes
- **Plane**: Tests for 3D plane representation (ax + by + cz + d = 0)
  - Constructor from coefficients
  - Constructor from point and normal vector
  - Point evaluation (eval)
  - Coordinate evaluation (evalXY, evalXZ, evalYZ)
  - Type conversion constructors

- **planeFromThreePoints**: Tests for plane creation from three points
  - Verification that all three points lie on the plane
  - Normal vector calculation correctness

### Triangles
- **Triangle2D**: Tests for 2D triangles
  - Constructor from three points
  - Default constructor
  - Type conversion constructors

- **Triangle3D**: Tests for 3D triangles
  - Constructor from three points
  - Default constructor
  - Type conversion constructors

## Test Structure

The tests are organized using Google Test framework with the following test fixtures:

- `Line2DTest`: Common setup for 2D line tests
- `Line3DTest`: Common setup for 3D line tests
- `PlaneTest`: Common setup for plane tests
- `TriangleTest`: Common setup for triangle tests

Each test fixture provides commonly used points, vectors, and geometric objects to avoid code duplication.

## Edge Cases Tested

The test suite includes comprehensive edge case testing:

- **Vertical and horizontal lines**: Special cases for 2D lines
- **Axis-aligned lines**: Lines parallel to coordinate axes
- **Coplanar points**: Multiple points on the same plane
- **Degenerate cases**: Lines with zero-length direction vectors
- **Type conversions**: Between different numeric types (double, float)

## Running the Tests

To run the geometry tests:

```bash
# From the build directory
make geometry_test
./src/math/geometry/test/geometry_test

# Or using CTest
ctest -R GeometryTest
```

## Test Constants

- `EPSILON = 1e-9`: Used for floating-point comparisons to handle numerical precision issues

## Future Enhancements

Potential areas for test expansion:
- Line-line intersection tests
- Line-plane intersection tests  
- Point-in-triangle tests
- Triangle area calculations
- Distance calculations between geometric objects
- Geometric transformations applied to objects
