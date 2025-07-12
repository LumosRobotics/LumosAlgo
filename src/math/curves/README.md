# LumosAlgo Curve Implementations

This directory contains implementations of various mathematical curves and polynomials for the LumosAlgo library.

## Overview

The curves module provides implementations for:
1. **Bezier Curves** - Parametric curves defined by control points
2. **B-Spline Curves** - Non-uniform rational basis splines
3. **Quintic Polynomials** - Fifth-degree polynomials with boundary conditions

## Features

### Bezier Curves
- Support for arbitrary degree curves based on control points
- Evaluation at parameter t ∈ [0, 1]
- First and second derivative computation
- Curve subdivision and splitting using de Casteljau's algorithm
- Degree elevation
- Template-based for 2D and 3D curves

### B-Spline Curves
- Configurable degree and knot vectors
- Uniform, open uniform, and clamped knot vector generation
- Evaluation using Cox-de Boor recursion formula
- Basis function computation
- Knot insertion and removal (planned)
- Curve validation functions

### Quintic Polynomials
- Boundary condition specification (position, velocity, acceleration)
- Evaluation of position and all derivatives up to 5th order
- Time scaling and amplitude scaling
- Both vector and scalar implementations
- Smooth trajectory generation for robotics applications

## Usage Examples

### Bezier Curve Example
```cpp
#include "math/math.h"
using namespace lumos;

// Create control points
std::vector<Vec2d> control_points = {
    Vec2d(0.0, 0.0),
    Vec2d(1.0, 2.0),
    Vec2d(2.0, 1.0),
    Vec2d(3.0, 0.0)
};

// Create Bezier curve
BezierCurve2Dd bezier(control_points);

// Evaluate at parameter t
Vec2d point = bezier.evaluate(0.5);
Vec2d velocity = bezier.evaluateDerivative(0.5);
```

### B-Spline Curve Example
```cpp
#include "math/math.h"
using namespace lumos;

// Create B-spline curve
BSplineCurve2Dd bspline(control_points, {}, 3);
bspline.generateClampedKnotVector();

// Evaluate curve
Vec2d point = bspline.evaluate(0.5);
```

### Quintic Polynomial Example
```cpp
#include "math/math.h"
using namespace lumos;

// Define boundary conditions
Vec2d start_pos(0.0, 0.0);
Vec2d start_vel(1.0, 0.0);
Vec2d start_acc(0.0, 0.0);
Vec2d end_pos(5.0, 2.0);
Vec2d end_vel(0.0, 1.0);
Vec2d end_acc(0.0, 0.0);
double duration = 2.0;

// Create quintic polynomial
QuinticPolynomial2Dd quintic(start_pos, start_vel, start_acc,
                             end_pos, end_vel, end_acc, duration);

// Evaluate at time t
Vec2d position = quintic.evaluate(1.0);
Vec2d velocity = quintic.evaluateVelocity(1.0);
Vec2d acceleration = quintic.evaluateAcceleration(1.0);
```

### Scalar Quintic Polynomial Example
```cpp
#include "math/math.h"
using namespace lumos;

// Create 1D quintic polynomial
QuinticPolynomial1Dd scalar_quintic(0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 1.0);

// Evaluate
double position = scalar_quintic.evaluate(0.5);
double velocity = scalar_quintic.evaluateVelocity(0.5);
```

## Type Aliases

Common type aliases are provided for convenience:

```cpp
// Bezier curves
typedef BezierCurve2D<double> BezierCurve2Dd;
typedef BezierCurve3D<double> BezierCurve3Dd;
typedef BezierCurve2D<float> BezierCurve2Df;
typedef BezierCurve3D<float> BezierCurve3Df;

// B-spline curves
typedef BSplineCurve2D<double> BSplineCurve2Dd;
typedef BSplineCurve3D<double> BSplineCurve3Dd;
typedef BSplineCurve2D<float> BSplineCurve2Df;
typedef BSplineCurve3D<float> BSplineCurve3Df;

// Quintic polynomials
typedef QuinticPolynomial2D<double> QuinticPolynomial2Dd;
typedef QuinticPolynomial3D<double> QuinticPolynomial3Dd;
typedef QuinticPolynomial1D<double> QuinticPolynomial1Dd;
typedef QuinticPolynomial1D<float> QuinticPolynomial1Df;
```

## File Structure

```
src/math/curves/
├── class_def/
│   ├── bezier_curve.h          # Bezier curve class definition
│   ├── bspline_curve.h         # B-spline curve class definition
│   └── quintic_polynomial.h    # Quintic polynomial class definition
├── bezier_curve.h              # Bezier curve implementation
├── bspline_curve.h             # B-spline curve implementation
├── quintic_polynomial.h        # Quintic polynomial implementation
└── curves.h                    # Main curves header with all includes
```

## Mathematical Background

### Bezier Curves
Bezier curves are defined by the formula:
```
B(t) = Σ(i=0 to n) P_i * C(n,i) * (1-t)^(n-i) * t^i
```
where P_i are control points and C(n,i) are binomial coefficients.

### B-Spline Curves
B-spline curves are defined by:
```
C(t) = Σ(i=0 to n) P_i * N_i,p(t)
```
where N_i,p(t) are B-spline basis functions of degree p.

### Quintic Polynomials
Quintic polynomials have the form:
```
p(t) = a_0 + a_1*t + a_2*t^2 + a_3*t^3 + a_4*t^4 + a_5*t^5
```
The coefficients are determined by boundary conditions at t=0 and t=T.

## Applications

- **Computer Graphics**: Curve and surface modeling
- **Robotics**: Smooth trajectory generation
- **Animation**: Keyframe interpolation
- **CAD/CAM**: Geometric modeling
- **Computer Vision**: Shape representation

## Building and Testing

The curves module is included in the main LumosAlgo library. To build:

```bash
cd /path/to/LumosAlgo
mkdir -p build
cd build
cmake ..
make
```

Run the demo application:
```bash
./src/applications/simple/simple
```

## Notes

- All implementations are template-based for flexibility
- Memory management is handled automatically
- Error checking is included for invalid parameters
- The implementations prioritize correctness over performance
- Future optimizations may include SIMD acceleration for batch operations
