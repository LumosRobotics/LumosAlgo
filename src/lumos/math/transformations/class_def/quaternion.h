#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <cmath>
#include <iostream>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"

namespace lumos
{
  template <typename T>
  class Quaternion
  {
  public:
    T w, x, y, z;

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

    static Quaternion fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
    static Quaternion fromAxisAngle(const AxisAngle<T> &axis_angle);
    static Quaternion fromEulerAngles(const EulerAngles<T> &euler);

    // Normalization
    constexpr void normalize();

    // Output operator
    friend std::ostream &operator<<(std::ostream &os, const Quaternion &q)
    {
      os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
      return os;
    }
  };

  extern template class Quaternion<float>;
  extern template class Quaternion<double>;

} // namespace lumos

#endif // QUATERNION_HPP
