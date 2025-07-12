#ifndef LUMOS_MATH_CURVES_CLASS_DEF_BSPLINE_CURVE_H_
#define LUMOS_MATH_CURVES_CLASS_DEF_BSPLINE_CURVE_H_

#include "math/misc/forward_decl.h"
#include <vector>

namespace lumos {

template <typename T, typename VecType> class BSplineCurve {
private:
  std::vector<VecType> control_points_;
  std::vector<T> knot_vector_;
  int degree_;

  T basisFunction(int i, int p, T t) const;
  T basisFunctionDerivative(int i, int p, T t, int derivative_order) const;

public:
  BSplineCurve();
  BSplineCurve(const std::vector<VecType> &control_points,
               const std::vector<T> &knot_vector, int degree);

  void setControlPoints(const std::vector<VecType> &control_points);
  void setKnotVector(const std::vector<T> &knot_vector);
  void setDegree(int degree);

  const std::vector<VecType> &getControlPoints() const;
  const std::vector<T> &getKnotVector() const;
  int getDegree() const;

  VecType evaluate(T t) const;
  VecType evaluateDerivative(T t, int derivative_order = 1) const;

  // Knot vector utilities
  void generateUniformKnotVector();
  void generateOpenUniformKnotVector();
  void generateClampedKnotVector();

  // Curve modification
  void insertKnot(T knot_value, int multiplicity = 1);
  void removeKnot(T knot_value);
  void elevateDegree();
  void reduceDegree();

  // Curve analysis
  int findKnotSpan(T t) const;
  std::vector<T> computeBasisFunctions(int span, T t) const;
  std::vector<std::vector<T>>
  computeBasisFunctionsDerivatives(int span, T t, int derivative_order) const;

  // Validation
  bool isValidKnotVector() const;
  bool isValidConfiguration() const;

  size_t getNumControlPoints() const;
  size_t getNumKnots() const;
};

// Specialized template for common 2D and 3D cases
template <typename T> using BSplineCurve2D = BSplineCurve<T, Vec2<T>>;

template <typename T> using BSplineCurve3D = BSplineCurve<T, Vec3<T>>;

} // namespace lumos

#endif // LUMOS_MATH_CURVES_CLASS_DEF_BSPLINE_CURVE_H_
