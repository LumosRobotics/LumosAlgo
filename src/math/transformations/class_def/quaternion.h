#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <cmath>
#include <iostream>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"

namespace lumos {
template <typename T> class Quaternion {
public:
  T w_, x_, y_, z_;

  // Constructors
  constexpr Quaternion();
  constexpr Quaternion(T w, T x, T y, T z);
  // constexpr Quaternion(const FixedSizeVector<T, 3>& axis, T angle);

  // Quaternion operations
  constexpr Quaternion operator+(const Quaternion &other) const;
  constexpr Quaternion operator-(const Quaternion &other) const;
  constexpr Quaternion operator*(const Quaternion &other) const;
  constexpr Quaternion operator*(const T scalar) const;
  constexpr Quaternion conjugate() const;
  constexpr Quaternion inverse() const;
  constexpr FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;
  // constexpr FixedSizeVector<T, 3> toEulerAngles() const;
  // constexpr FixedSizeVector<T, 4> toAxisAngle() const;

  // Normalization
  constexpr void normalize();

  // Output operator
  friend std::ostream &operator<<(std::ostream &os, const Quaternion &q) {
    os << "(" << q.w_ << ", " << q.x_ << ", " << q.y_ << ", " << q.z_ << ")";
    return os;
  }
};

extern template class Quaternion<float>;
extern template class Quaternion<double>;

} // namespace lumos

#endif // QUATERNION_HPP
