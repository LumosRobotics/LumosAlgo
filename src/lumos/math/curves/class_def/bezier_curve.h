#ifndef LUMOS_MATH_CURVES_CLASS_DEF_BEZIER_CURVE_H_
#define LUMOS_MATH_CURVES_CLASS_DEF_BEZIER_CURVE_H_

#include "lumos/math/misc/forward_decl.h"
#include <vector>

namespace lumos
{

  template <typename T, typename VecType>
  class BezierCurve
  {
  private:
    std::vector<VecType> control_points_;

    T binomialCoeff(int n, int k) const;
    T bernsteinBasis(int n, int i, T t) const;

  public:
    BezierCurve();
    explicit BezierCurve(const std::vector<VecType> &control_points);

    void setControlPoints(const std::vector<VecType> &control_points);
    const std::vector<VecType> &getControlPoints() const;

    VecType evaluate(T t) const;
    VecType evaluateDerivative(T t) const;
    VecType evaluateSecondDerivative(T t) const;

    void addControlPoint(const VecType &point);
    void insertControlPoint(size_t index, const VecType &point);
    void removeControlPoint(size_t index);

    size_t getDegree() const;
    size_t getNumControlPoints() const;

    // Specialized methods for common cases
    void elevateDegree();
    BezierCurve<T, VecType> subdivideCurve(T t) const;
    std::pair<BezierCurve<T, VecType>, BezierCurve<T, VecType>>
    splitCurve(T t) const;
  };

  // Specialized template for common 2D and 3D cases
  template <typename T>
  using BezierCurve2D = BezierCurve<T, Vec2<T>>;

  template <typename T>
  using BezierCurve3D = BezierCurve<T, Vec3<T>>;

} // namespace lumos

#endif // LUMOS_MATH_CURVES_CLASS_DEF_BEZIER_CURVE_H_
