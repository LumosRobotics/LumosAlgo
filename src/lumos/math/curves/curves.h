#ifndef LUMOS_MATH_CURVES_CURVES_H_
#define LUMOS_MATH_CURVES_CURVES_H_

#include "lumos/math/curves/bezier_curve.h"
#include "lumos/math/curves/bspline_curve.h"
#include "lumos/math/curves/quintic_polynomial.h"

namespace lumos
{

    // Common type aliases for convenience
    typedef BezierCurve2D<double> BezierCurve2Dd;
    typedef BezierCurve3D<double> BezierCurve3Dd;
    typedef BezierCurve2D<float> BezierCurve2Df;
    typedef BezierCurve3D<float> BezierCurve3Df;

    typedef BSplineCurve2D<double> BSplineCurve2Dd;
    typedef BSplineCurve3D<double> BSplineCurve3Dd;
    typedef BSplineCurve2D<float> BSplineCurve2Df;
    typedef BSplineCurve3D<float> BSplineCurve3Df;

    typedef QuinticPolynomial2D<double> QuinticPolynomial2Dd;
    typedef QuinticPolynomial3D<double> QuinticPolynomial3Dd;
    typedef QuinticPolynomial2D<float> QuinticPolynomial2Df;
    typedef QuinticPolynomial3D<float> QuinticPolynomial3Df;

    typedef QuinticPolynomial1D<double> QuinticPolynomial1Dd;
    typedef QuinticPolynomial1D<float> QuinticPolynomial1Df;

} // namespace lumos

#endif // LUMOS_MATH_CURVES_CURVES_H_
