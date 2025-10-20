#ifndef LUMOS_MATH_CURVES_BEZIER_CURVE_H_
#define LUMOS_MATH_CURVES_BEZIER_CURVE_H_

#include "lumos/math/curves/class_def/bezier_curve.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace lumos
{

  template <typename T, typename VecType>
  BezierCurve<T, VecType>::BezierCurve() {}

  template <typename T, typename VecType>
  BezierCurve<T, VecType>::BezierCurve(const std::vector<VecType> &control_points)
      : control_points_(control_points) {}

  template <typename T, typename VecType>
  void BezierCurve<T, VecType>::setControlPoints(
      const std::vector<VecType> &control_points)
  {
    control_points_ = control_points;
  }

  template <typename T, typename VecType>
  const std::vector<VecType> &BezierCurve<T, VecType>::getControlPoints() const
  {
    return control_points_;
  }

  template <typename T, typename VecType>
  T BezierCurve<T, VecType>::binomialCoeff(int n, int k) const
  {
    if (k > n || k < 0)
      return T(0);
    if (k == 0 || k == n)
      return T(1);

    T result = T(1);
    for (int i = 0; i < k; ++i)
    {
      result = result * T(n - i) / T(i + 1);
    }
    return result;
  }

  template <typename T, typename VecType>
  T BezierCurve<T, VecType>::bernsteinBasis(int n, int i, T t) const
  {
    return binomialCoeff(n, i) * std::pow(T(1) - t, n - i) * std::pow(t, i);
  }

  template <typename T, typename VecType>
  VecType BezierCurve<T, VecType>::evaluate(T t) const
  {
    if (control_points_.empty())
    {
      throw std::runtime_error("No control points defined");
    }

    t = std::max(T(0), std::min(T(1), t)); // Clamp t to [0, 1]

    int n = static_cast<int>(control_points_.size() - 1);
    VecType result = control_points_[0] * T(0); // Initialize with zero vector

    for (int i = 0; i <= n; ++i)
    {
      T basis = bernsteinBasis(n, i, t);
      result = result + control_points_[i] * basis;
    }

    return result;
  }

  template <typename T, typename VecType>
  VecType BezierCurve<T, VecType>::evaluateDerivative(T t) const
  {
    if (control_points_.size() < 2)
    {
      throw std::runtime_error("Need at least 2 control points for derivative");
    }

    t = std::max(T(0), std::min(T(1), t)); // Clamp t to [0, 1]

    int n = static_cast<int>(control_points_.size() - 1);
    VecType result = control_points_[0] * T(0); // Initialize with zero vector

    for (int i = 0; i < n; ++i)
    {
      T basis = bernsteinBasis(n - 1, i, t);
      VecType diff = control_points_[i + 1] - control_points_[i];
      result = result + diff * (T(n) * basis);
    }

    return result;
  }

  template <typename T, typename VecType>
  VecType BezierCurve<T, VecType>::evaluateSecondDerivative(T t) const
  {
    if (control_points_.size() < 3)
    {
      throw std::runtime_error(
          "Need at least 3 control points for second derivative");
    }

    t = std::max(T(0), std::min(T(1), t)); // Clamp t to [0, 1]

    int n = static_cast<int>(control_points_.size() - 1);
    VecType result = control_points_[0] * T(0); // Initialize with zero vector

    for (int i = 0; i < n - 1; ++i)
    {
      T basis = bernsteinBasis(n - 2, i, t);
      VecType second_diff = control_points_[i + 2] -
                            control_points_[i + 1] * T(2) + control_points_[i];
      result = result + second_diff * (T(n) * T(n - 1) * basis);
    }

    return result;
  }

  template <typename T, typename VecType>
  void BezierCurve<T, VecType>::addControlPoint(const VecType &point)
  {
    control_points_.push_back(point);
  }

  template <typename T, typename VecType>
  void BezierCurve<T, VecType>::insertControlPoint(size_t index,
                                                   const VecType &point)
  {
    if (index > control_points_.size())
    {
      throw std::out_of_range("Index out of range");
    }
    control_points_.insert(control_points_.begin() + index, point);
  }

  template <typename T, typename VecType>
  void BezierCurve<T, VecType>::removeControlPoint(size_t index)
  {
    if (index >= control_points_.size())
    {
      throw std::out_of_range("Index out of range");
    }
    control_points_.erase(control_points_.begin() + index);
  }

  template <typename T, typename VecType>
  size_t BezierCurve<T, VecType>::getDegree() const
  {
    return control_points_.empty() ? 0 : control_points_.size() - 1;
  }

  template <typename T, typename VecType>
  size_t BezierCurve<T, VecType>::getNumControlPoints() const
  {
    return control_points_.size();
  }

  template <typename T, typename VecType>
  void BezierCurve<T, VecType>::elevateDegree()
  {
    if (control_points_.empty())
      return;

    int n = static_cast<int>(control_points_.size() - 1);
    std::vector<VecType> new_control_points(n + 2);

    new_control_points[0] = control_points_[0];
    new_control_points[n + 1] = control_points_[n];

    for (int i = 1; i <= n; ++i)
    {
      T alpha = T(i) / T(n + 1);
      new_control_points[i] =
          control_points_[i - 1] * (T(1) - alpha) + control_points_[i] * alpha;
    }

    control_points_ = new_control_points;
  }

  template <typename T, typename VecType>
  std::pair<BezierCurve<T, VecType>, BezierCurve<T, VecType>>
  BezierCurve<T, VecType>::splitCurve(T t) const
  {
    if (control_points_.empty())
    {
      throw std::runtime_error("No control points defined");
    }

    t = std::max(T(0), std::min(T(1), t)); // Clamp t to [0, 1]

    int n = static_cast<int>(control_points_.size() - 1);

    // Use de Casteljau's algorithm to split the curve
    std::vector<std::vector<VecType>> temp_points(n + 1);
    temp_points[0] = control_points_;

    for (int i = 1; i <= n; ++i)
    {
      temp_points[i].resize(n + 1 - i);
      for (int j = 0; j < n + 1 - i; ++j)
      {
        temp_points[i][j] =
            temp_points[i - 1][j] * (T(1) - t) + temp_points[i - 1][j + 1] * t;
      }
    }

    std::vector<VecType> left_control_points(n + 1);
    std::vector<VecType> right_control_points(n + 1);

    for (int i = 0; i <= n; ++i)
    {
      left_control_points[i] = temp_points[i][0];
      right_control_points[i] = temp_points[n - i][i];
    }

    return std::make_pair(BezierCurve<T, VecType>(left_control_points),
                          BezierCurve<T, VecType>(right_control_points));
  }

} // namespace lumos

#endif // LUMOS_MATH_CURVES_BEZIER_CURVE_H_
