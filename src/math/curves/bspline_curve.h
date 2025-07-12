#ifndef LUMOS_MATH_CURVES_BSPLINE_CURVE_H_
#define LUMOS_MATH_CURVES_BSPLINE_CURVE_H_

#include "math/curves/class_def/bspline_curve.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace lumos {

template <typename T, typename VecType>
BSplineCurve<T, VecType>::BSplineCurve() : degree_(3) {}

template <typename T, typename VecType>
BSplineCurve<T, VecType>::BSplineCurve(
    const std::vector<VecType> &control_points,
    const std::vector<T> &knot_vector, int degree)
    : control_points_(control_points), knot_vector_(knot_vector),
      degree_(degree) {}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::setControlPoints(
    const std::vector<VecType> &control_points) {
  control_points_ = control_points;
}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::setKnotVector(
    const std::vector<T> &knot_vector) {
  knot_vector_ = knot_vector;
}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::setDegree(int degree) {
  degree_ = degree;
}

template <typename T, typename VecType>
const std::vector<VecType> &BSplineCurve<T, VecType>::getControlPoints() const {
  return control_points_;
}

template <typename T, typename VecType>
const std::vector<T> &BSplineCurve<T, VecType>::getKnotVector() const {
  return knot_vector_;
}

template <typename T, typename VecType>
int BSplineCurve<T, VecType>::getDegree() const {
  return degree_;
}

template <typename T, typename VecType>
T BSplineCurve<T, VecType>::basisFunction(int i, int p, T t) const {
  if (p == 0) {
    return (t >= knot_vector_[i] && t < knot_vector_[i + 1]) ? T(1) : T(0);
  }

  T left = T(0);
  T right = T(0);

  if (knot_vector_[i + p] != knot_vector_[i]) {
    left = (t - knot_vector_[i]) / (knot_vector_[i + p] - knot_vector_[i]) *
           basisFunction(i, p - 1, t);
  }

  if (knot_vector_[i + p + 1] != knot_vector_[i + 1]) {
    right = (knot_vector_[i + p + 1] - t) /
            (knot_vector_[i + p + 1] - knot_vector_[i + 1]) *
            basisFunction(i + 1, p - 1, t);
  }

  return left + right;
}

template <typename T, typename VecType>
int BSplineCurve<T, VecType>::findKnotSpan(T t) const {
  int n = static_cast<int>(control_points_.size() - 1);

  if (t >= knot_vector_[n + 1]) {
    return n;
  }

  if (t <= knot_vector_[degree_]) {
    return degree_;
  }

  int low = degree_;
  int high = n + 1;
  int mid = (low + high) / 2;

  while (t < knot_vector_[mid] || t >= knot_vector_[mid + 1]) {
    if (t < knot_vector_[mid]) {
      high = mid;
    } else {
      low = mid;
    }
    mid = (low + high) / 2;
  }

  return mid;
}

template <typename T, typename VecType>
std::vector<T> BSplineCurve<T, VecType>::computeBasisFunctions(int span,
                                                               T t) const {
  std::vector<T> basis_functions(degree_ + 1);
  std::vector<T> left(degree_ + 1);
  std::vector<T> right(degree_ + 1);

  basis_functions[0] = T(1);

  for (int j = 1; j <= degree_; ++j) {
    left[j] = t - knot_vector_[span + 1 - j];
    right[j] = knot_vector_[span + j] - t;

    T saved = T(0);
    for (int r = 0; r < j; ++r) {
      T temp = basis_functions[r] / (right[r + 1] + left[j - r]);
      basis_functions[r] = saved + right[r + 1] * temp;
      saved = left[j - r] * temp;
    }
    basis_functions[j] = saved;
  }

  return basis_functions;
}

template <typename T, typename VecType>
VecType BSplineCurve<T, VecType>::evaluate(T t) const {
  if (control_points_.empty() || knot_vector_.empty()) {
    throw std::runtime_error("Control points or knot vector not defined");
  }

  int span = findKnotSpan(t);
  std::vector<T> basis = computeBasisFunctions(span, t);

  VecType result = control_points_[0] * T(0); // Initialize with zero vector

  for (int i = 0; i <= degree_; ++i) {
    result = result + control_points_[span - degree_ + i] * basis[i];
  }

  return result;
}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::generateUniformKnotVector() {
  int n = static_cast<int>(control_points_.size() - 1);
  int m = n + degree_ + 1;

  knot_vector_.resize(m + 1);

  for (int i = 0; i <= m; ++i) {
    knot_vector_[i] = T(i) / T(m);
  }
}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::generateClampedKnotVector() {
  int n = static_cast<int>(control_points_.size() - 1);
  int m = n + degree_ + 1;

  knot_vector_.resize(m + 1);

  // First degree+1 knots are 0
  for (int i = 0; i <= degree_; ++i) {
    knot_vector_[i] = T(0);
  }

  // Last degree+1 knots are 1
  for (int i = n + 1; i <= m; ++i) {
    knot_vector_[i] = T(1);
  }

  // Interior knots are uniformly distributed
  for (int i = degree_ + 1; i <= n; ++i) {
    knot_vector_[i] = T(i - degree_) / T(n - degree_ + 1);
  }
}

template <typename T, typename VecType>
void BSplineCurve<T, VecType>::generateOpenUniformKnotVector() {
  generateClampedKnotVector();
}

template <typename T, typename VecType>
bool BSplineCurve<T, VecType>::isValidKnotVector() const {
  if (knot_vector_.empty())
    return false;

  // Check if knot vector is non-decreasing
  for (size_t i = 1; i < knot_vector_.size(); ++i) {
    if (knot_vector_[i] < knot_vector_[i - 1]) {
      return false;
    }
  }

  return true;
}

template <typename T, typename VecType>
bool BSplineCurve<T, VecType>::isValidConfiguration() const {
  if (control_points_.empty())
    return false;
  if (!isValidKnotVector())
    return false;

  int n = static_cast<int>(control_points_.size() - 1);
  int m = static_cast<int>(knot_vector_.size() - 1);

  // Check if m = n + p + 1
  return (m == n + degree_ + 1);
}

template <typename T, typename VecType>
size_t BSplineCurve<T, VecType>::getNumControlPoints() const {
  return control_points_.size();
}

template <typename T, typename VecType>
size_t BSplineCurve<T, VecType>::getNumKnots() const {
  return knot_vector_.size();
}

template <typename T, typename VecType>
VecType
BSplineCurve<T, VecType>::evaluateDerivative(T t, int derivative_order) const {
  if (derivative_order <= 0) {
    return evaluate(t);
  }

  if (derivative_order > degree_) {
    // Derivative of order higher than degree is zero
    return control_points_[0] * T(0);
  }

  // For simplicity, implement first derivative analytically
  if (derivative_order == 1) {
    int span = findKnotSpan(t);
    std::vector<T> basis = computeBasisFunctions(span, t);

    VecType result = control_points_[0] * T(0); // Initialize with zero vector

    for (int i = 0; i < degree_; ++i) {
      int idx = span - degree_ + i;
      T coeff = T(degree_) /
                (knot_vector_[idx + degree_ + 1] - knot_vector_[idx + 1]);
      VecType diff = control_points_[idx + 1] - control_points_[idx];
      result = result + diff * (coeff * basis[i]);
    }

    return result;
  }

  // For higher derivatives, use recursive approach (simplified)
  throw std::runtime_error("Higher order derivatives not implemented yet");
}

} // namespace lumos

#endif // LUMOS_MATH_CURVES_BSPLINE_CURVE_H_
