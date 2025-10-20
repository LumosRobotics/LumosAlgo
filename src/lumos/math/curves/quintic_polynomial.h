#ifndef LUMOS_MATH_CURVES_QUINTIC_POLYNOMIAL_H_
#define LUMOS_MATH_CURVES_QUINTIC_POLYNOMIAL_H_

#include "lumos/math/curves/class_def/quintic_polynomial.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace lumos
{

  template <typename T, typename VecType>
  QuinticPolynomial<T, VecType>::QuinticPolynomial() : duration_(T(1))
  {
    // Initialize coefficients to zero
    for (int i = 0; i < 6; ++i)
    {
      coefficients_[i] = VecType{} * T(0);
    }
  }

  template <typename T, typename VecType>
  QuinticPolynomial<T, VecType>::QuinticPolynomial(
      const std::array<VecType, 6> &coefficients)
      : coefficients_(coefficients), duration_(T(1)) {}

  template <typename T, typename VecType>
  QuinticPolynomial<T, VecType>::QuinticPolynomial(
      const VecType &start_pos, const VecType &start_vel,
      const VecType &start_acc, const VecType &end_pos, const VecType &end_vel,
      const VecType &end_acc, T duration)
      : duration_(duration)
  {
    setBoundaryConditions(start_pos, start_vel, start_acc, end_pos, end_vel,
                          end_acc, duration);
  }

  template <typename T, typename VecType>
  void QuinticPolynomial<T, VecType>::setCoefficients(
      const std::array<VecType, 6> &coefficients)
  {
    coefficients_ = coefficients;
  }

  template <typename T, typename VecType>
  const std::array<VecType, 6> &
  QuinticPolynomial<T, VecType>::getCoefficients() const
  {
    return coefficients_;
  }

  template <typename T, typename VecType>
  void QuinticPolynomial<T, VecType>::setBoundaryConditions(
      const VecType &start_pos, const VecType &start_vel,
      const VecType &start_acc, const VecType &end_pos, const VecType &end_vel,
      const VecType &end_acc, T duration)
  {
    duration_ = duration;

    // Quintic polynomial: p(t) = a0 + a1*t + a2*t^2 + a3*t^3 + a4*t^4 + a5*t^5
    // Boundary conditions at t=0 and t=T:
    // p(0) = start_pos,    p(T) = end_pos
    // p'(0) = start_vel,   p'(T) = end_vel
    // p''(0) = start_acc,  p''(T) = end_acc

    T T2 = duration * duration;
    T T3 = T2 * duration;
    T T4 = T3 * duration;
    T T5 = T4 * duration;

    // a0 = start_pos
    coefficients_[0] = start_pos;

    // a1 = start_vel
    coefficients_[1] = start_vel;

    // a2 = start_acc / 2
    coefficients_[2] = start_acc * T(0.5);

    // Solve for a3, a4, a5 using end conditions
    VecType h =
        end_pos - start_pos - start_vel * duration - start_acc * T2 * T(0.5);
    VecType dv = end_vel - start_vel - start_acc * duration;
    VecType da = end_acc - start_acc;

    // a3 = (20*h - 8*T*dv - 3*T^2*da) / (2*T^3)
    coefficients_[3] = (h * T(20) - dv * (T(8) * duration) - da * (T(3) * T2)) *
                       (T(1) / (T(2) * T3));

    // a4 = (-30*h + 14*T*dv + 3*T^2*da) / (2*T^4)
    coefficients_[4] = (h * T(-30) + dv * (T(14) * duration) + da * (T(3) * T2)) *
                       (T(1) / (T(2) * T4));

    // a5 = (12*h - 6*T*dv - T^2*da) / (2*T^5)
    coefficients_[5] =
        (h * T(12) - dv * (T(6) * duration) - da * T2) * (T(1) / (T(2) * T5));
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluate(T t) const
  {
    // Clamp t to [0, duration]
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;
    T t4 = t3 * t;
    T t5 = t4 * t;

    return coefficients_[0] + coefficients_[1] * t + coefficients_[2] * t2 +
           coefficients_[3] * t3 + coefficients_[4] * t4 + coefficients_[5] * t5;
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluateVelocity(T t) const
  {
    // First derivative: p'(t) = a1 + 2*a2*t + 3*a3*t^2 + 4*a4*t^3 + 5*a5*t^4
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;
    T t4 = t3 * t;

    return coefficients_[1] + coefficients_[2] * (T(2) * t) +
           coefficients_[3] * (T(3) * t2) + coefficients_[4] * (T(4) * t3) +
           coefficients_[5] * (T(5) * t4);
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluateAcceleration(T t) const
  {
    // Second derivative: p''(t) = 2*a2 + 6*a3*t + 12*a4*t^2 + 20*a5*t^3
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;

    return coefficients_[2] * T(2) + coefficients_[3] * (T(6) * t) +
           coefficients_[4] * (T(12) * t2) + coefficients_[5] * (T(20) * t3);
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluateJerk(T t) const
  {
    // Third derivative: p'''(t) = 6*a3 + 24*a4*t + 60*a5*t^2
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;

    return coefficients_[3] * T(6) + coefficients_[4] * (T(24) * t) +
           coefficients_[5] * (T(60) * t2);
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluateSnap(T t) const
  {
    // Fourth derivative: p''''(t) = 24*a4 + 120*a5*t
    t = std::max(T(0), std::min(duration_, t));

    return coefficients_[4] * T(24) + coefficients_[5] * (T(120) * t);
  }

  template <typename T, typename VecType>
  VecType QuinticPolynomial<T, VecType>::evaluateCrackle(T) const
  {
    // Fifth derivative: p'''''(t) = 120*a5
    return coefficients_[5] * T(120);
  }

  template <typename T, typename VecType>
  VecType
  QuinticPolynomial<T, VecType>::evaluateDerivative(T t,
                                                    int derivative_order) const
  {
    switch (derivative_order)
    {
    case 0:
      return evaluate(t);
    case 1:
      return evaluateVelocity(t);
    case 2:
      return evaluateAcceleration(t);
    case 3:
      return evaluateJerk(t);
    case 4:
      return evaluateSnap(t);
    case 5:
      return evaluateCrackle(t);
    default:
      // Higher derivatives are zero
      return coefficients_[0] * T(0);
    }
  }

  template <typename T, typename VecType>
  T QuinticPolynomial<T, VecType>::getDuration() const
  {
    return duration_;
  }

  template <typename T, typename VecType>
  void QuinticPolynomial<T, VecType>::scaleTime(T scale_factor)
  {
    if (scale_factor <= T(0))
    {
      throw std::invalid_argument("Scale factor must be positive");
    }

    duration_ *= scale_factor;

    // Rescale coefficients appropriately
    T s = scale_factor;
    T s2 = s * s;
    T s3 = s2 * s;
    T s4 = s3 * s;
    T s5 = s4 * s;

    coefficients_[1] = coefficients_[1] * (T(1) / s);
    coefficients_[2] = coefficients_[2] * (T(1) / s2);
    coefficients_[3] = coefficients_[3] * (T(1) / s3);
    coefficients_[4] = coefficients_[4] * (T(1) / s4);
    coefficients_[5] = coefficients_[5] * (T(1) / s5);
  }

  template <typename T, typename VecType>
  void QuinticPolynomial<T, VecType>::scaleAmplitude(T scale_factor)
  {
    for (int i = 0; i < 6; ++i)
    {
      coefficients_[i] = coefficients_[i] * scale_factor;
    }
  }

  template <typename T, typename VecType>
  void QuinticPolynomial<T, VecType>::translateAmplitude(
      const VecType &translation)
  {
    coefficients_[0] = coefficients_[0] + translation;
  }

  // Scalar specialization implementation
  template <typename T>
  QuinticPolynomial1D<T>::QuinticPolynomial1D() : duration_(T(1))
  {
    std::fill(coefficients_.begin(), coefficients_.end(), T(0));
  }

  template <typename T>
  QuinticPolynomial1D<T>::QuinticPolynomial1D(
      const std::array<T, 6> &coefficients)
      : coefficients_(coefficients), duration_(T(1)) {}

  template <typename T>
  QuinticPolynomial1D<T>::QuinticPolynomial1D(T start_pos, T start_vel,
                                              T start_acc, T end_pos, T end_vel,
                                              T end_acc, T duration)
      : duration_(duration)
  {
    setBoundaryConditions(start_pos, start_vel, start_acc, end_pos, end_vel,
                          end_acc, duration);
  }

  template <typename T>
  void QuinticPolynomial1D<T>::setBoundaryConditions(T start_pos, T start_vel,
                                                     T start_acc, T end_pos,
                                                     T end_vel, T end_acc,
                                                     T duration)
  {
    duration_ = duration;

    T T2 = duration * duration;
    T T3 = T2 * duration;
    T T4 = T3 * duration;
    T T5 = T4 * duration;

    coefficients_[0] = start_pos;
    coefficients_[1] = start_vel;
    coefficients_[2] = start_acc * T(0.5);

    T h = end_pos - start_pos - start_vel * duration - start_acc * T2 * T(0.5);
    T dv = end_vel - start_vel - start_acc * duration;
    T da = end_acc - start_acc;

    coefficients_[3] =
        (T(20) * h - T(8) * duration * dv - T(3) * T2 * da) / (T(2) * T3);
    coefficients_[4] =
        (T(-30) * h + T(14) * duration * dv + T(3) * T2 * da) / (T(2) * T4);
    coefficients_[5] = (T(12) * h - T(6) * duration * dv - T2 * da) / (T(2) * T5);
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluate(T t) const
  {
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;
    T t4 = t3 * t;
    T t5 = t4 * t;

    return coefficients_[0] + coefficients_[1] * t + coefficients_[2] * t2 +
           coefficients_[3] * t3 + coefficients_[4] * t4 + coefficients_[5] * t5;
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluateVelocity(T t) const
  {
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;
    T t4 = t3 * t;

    return coefficients_[1] + T(2) * coefficients_[2] * t +
           T(3) * coefficients_[3] * t2 + T(4) * coefficients_[4] * t3 +
           T(5) * coefficients_[5] * t4;
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluateAcceleration(T t) const
  {
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;
    T t3 = t2 * t;

    return T(2) * coefficients_[2] + T(6) * coefficients_[3] * t +
           T(12) * coefficients_[4] * t2 + T(20) * coefficients_[5] * t3;
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluateJerk(T t) const
  {
    t = std::max(T(0), std::min(duration_, t));

    T t2 = t * t;

    return T(6) * coefficients_[3] + T(24) * coefficients_[4] * t +
           T(60) * coefficients_[5] * t2;
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluateSnap(T t) const
  {
    t = std::max(T(0), std::min(duration_, t));

    return T(24) * coefficients_[4] + T(120) * coefficients_[5] * t;
  }

  template <typename T>
  T QuinticPolynomial1D<T>::evaluateCrackle(T) const
  {
    return T(120) * coefficients_[5];
  }

  template <typename T>
  T QuinticPolynomial1D<T>::getDuration() const
  {
    return duration_;
  }

} // namespace lumos

#endif // LUMOS_MATH_CURVES_QUINTIC_POLYNOMIAL_H_
