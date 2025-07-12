#ifndef LUMOS_MATH_CURVES_CLASS_DEF_QUINTIC_POLYNOMIAL_H_
#define LUMOS_MATH_CURVES_CLASS_DEF_QUINTIC_POLYNOMIAL_H_

#include "math/misc/forward_decl.h"
#include <array>

namespace lumos {

template <typename T, typename VecType> class QuinticPolynomial {
private:
  std::array<VecType, 6> coefficients_; // a0, a1, a2, a3, a4, a5

public:
  QuinticPolynomial();
  explicit QuinticPolynomial(const std::array<VecType, 6> &coefficients);

  // Construction from boundary conditions
  QuinticPolynomial(const VecType &start_pos, const VecType &start_vel,
                    const VecType &start_acc, const VecType &end_pos,
                    const VecType &end_vel, const VecType &end_acc, T duration);

  void setCoefficients(const std::array<VecType, 6> &coefficients);
  const std::array<VecType, 6> &getCoefficients() const;

  void setBoundaryConditions(const VecType &start_pos, const VecType &start_vel,
                             const VecType &start_acc, const VecType &end_pos,
                             const VecType &end_vel, const VecType &end_acc,
                             T duration);

  VecType evaluate(T t) const;
  VecType evaluateVelocity(T t) const;
  VecType evaluateAcceleration(T t) const;
  VecType evaluateJerk(T t) const;
  VecType evaluateSnap(T t) const;    // 4th derivative
  VecType evaluateCrackle(T t) const; // 5th derivative

  // Utility methods
  VecType evaluateDerivative(T t, int derivative_order) const;
  T getDuration() const;

  // Analysis methods
  std::vector<T> findRoots(int component_index) const;
  std::vector<T> findExtrema(int component_index) const;
  T getMaxVelocity() const;
  T getMaxAcceleration() const;
  T getMaxJerk() const;

  // Scaling and transformation
  void scaleTime(T scale_factor);
  void translateTime(T time_offset);
  void scaleAmplitude(T scale_factor);
  void translateAmplitude(const VecType &translation);

private:
  T duration_;
  void computeCoefficientsFromBoundaryConditions();
};

// Specialized template for common 2D and 3D cases
template <typename T> using QuinticPolynomial2D = QuinticPolynomial<T, Vec2<T>>;

template <typename T> using QuinticPolynomial3D = QuinticPolynomial<T, Vec3<T>>;

// Specialized template for scalar case
template <typename T> class QuinticPolynomial1D {
private:
  std::array<T, 6> coefficients_; // a0, a1, a2, a3, a4, a5
  T duration_;

public:
  QuinticPolynomial1D();
  explicit QuinticPolynomial1D(const std::array<T, 6> &coefficients);

  QuinticPolynomial1D(T start_pos, T start_vel, T start_acc, T end_pos,
                      T end_vel, T end_acc, T duration);

  void setCoefficients(const std::array<T, 6> &coefficients);
  const std::array<T, 6> &getCoefficients() const;

  void setBoundaryConditions(T start_pos, T start_vel, T start_acc, T end_pos,
                             T end_vel, T end_acc, T duration);

  T evaluate(T t) const;
  T evaluateVelocity(T t) const;
  T evaluateAcceleration(T t) const;
  T evaluateJerk(T t) const;
  T evaluateSnap(T t) const;
  T evaluateCrackle(T t) const;

  T evaluateDerivative(T t, int derivative_order) const;
  T getDuration() const;

  std::vector<T> findRoots() const;
  std::vector<T> findExtrema() const;
  T getMaxVelocity() const;
  T getMaxAcceleration() const;
  T getMaxJerk() const;

  void scaleTime(T scale_factor);
  void translateTime(T time_offset);
  void scaleAmplitude(T scale_factor);
  void translateAmplitude(T translation);

private:
  void computeCoefficientsFromBoundaryConditions();
};

} // namespace lumos

#endif // LUMOS_MATH_CURVES_CLASS_DEF_QUINTIC_POLYNOMIAL_H_
