#include "math/transformations/class_def/quaternion.h"

#include <cmath>

namespace lumos
{
  // Default constructor
  template <typename T>
  constexpr Quaternion<T>::Quaternion() : w(1), x(0), y(0), z(0) {}

  // Parameterized constructor
  template <typename T>
  constexpr Quaternion<T>::Quaternion(T w, T x, T y, T z)
      : w(w), x(x), y(y), z(z) {}

  // Constructor from axis-angle
  /*template <typename T> constexpr Quaternion<T>::Quaternion(const
  FixedSizeVector<T, 3>& axis, T angle)
  {
      T halfAngle = angle / 2;
      T sinHalfAngle = std::sin(halfAngle);
      w = std::cos(halfAngle);
      x = axis[0] * sinHalfAngle;
      y = axis[1] * sinHalfAngle;
      z = axis[2] * sinHalfAngle;
  }*/

  // Addition
  template <typename T>
  constexpr Quaternion<T>
  Quaternion<T>::operator+(const Quaternion &other) const
  {
    return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
  }

  // Subtraction
  template <typename T>
  constexpr Quaternion<T>
  Quaternion<T>::operator-(const Quaternion &other) const
  {
    return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
  }

  // Multiplication
  template <typename T>
  constexpr Quaternion<T>
  Quaternion<T>::operator*(const Quaternion &other) const
  {
    return Quaternion(
        w * other.w - x * other.x - y * other.y - z * other.z,
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w);
  }

  // Scalar multiplication
  template <typename T>
  constexpr Quaternion<T> Quaternion<T>::operator*(const T scalar) const
  {
    return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
  }

  // Conjugate
  template <typename T>
  constexpr Quaternion<T> Quaternion<T>::conjugate() const
  {
    return Quaternion(w, -x, -y, -z);
  }

  // Normalize
  template <typename T>
  constexpr void Quaternion<T>::normalize()
  {
    const T norm = std::sqrt(w * w + x * x + y * y + z * z);
    if (norm > 0)
    {
      const T inv_norm = static_cast<T>(1.0) / norm;
      w *= inv_norm;
      x *= inv_norm;
      y *= inv_norm;
      z *= inv_norm;
    }
  }

  template <typename T>
  constexpr FixedSizeMatrix<T, 3, 3> Quaternion<T>::toRotationMatrix() const
  {
    FixedSizeMatrix<T, 3, 3> m;
    const T ww = w * w;
    const T xx = x * x;
    const T yy = y * y;
    const T zz = z * z;
    const T wx = w * x;
    const T wy = w * y;
    const T wz = w * z;
    const T xy = x * y;
    const T xz = x * z;
    const T yz = y * z;

    m(0, 0) = ww + xx - yy - zz;
    m(0, 1) = 2 * (xy - wz);
    m(0, 2) = 2 * (xz + wy);

    m(1, 0) = 2 * (xy + wz);
    m(1, 1) = ww - xx + yy - zz;
    m(1, 2) = 2 * (yz - wx);

    m(2, 0) = 2 * (xz - wy);
    m(2, 1) = 2 * (yz + wx);
    m(2, 2) = ww - xx - yy + zz;

    return m;
  }

  template <typename T>
  Quaternion<T> fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m)
  {
    Quaternion<T> q;
    const T trace = m(0, 0) + m(1, 1) + m(2, 2);
    if (trace > 0)
    {
      T s = std::sqrt(trace + 1.0);
      q.w = s * T{0.5};
      s = T{0.5} / s;
      q.x = (m(2, 1) - m(1, 2)) * s;
      q.y = (m(0, 2) - m(2, 0)) * s;
      q.z = (m(1, 0) - m(0, 1)) * s;
    }
    else
    {
      T s;
      if (m(1, 1) > m(0, 0) && m(1, 1) > m(2, 2))
      {
        // i = 1 (y)
        s = std::sqrt(m(1, 1) - m(0, 0) - m(2, 2) + 1.0);
        q.y = s * T{0.5};
        s = T{0.5} / s;
        q.w = (m(0, 2) - m(2, 0)) * s;
        q.x = (m(0, 1) + m(1, 0)) * s;
        q.z = (m(1, 2) + m(2, 1)) * s;
      }
      else if (m(2, 2) > m(0, 0))
      {
        // i = 2 (z)
        s = std::sqrt(m(2, 2) - m(0, 0) - m(1, 1) + 1.0);
        q.z = s * T{0.5};
        s = T{0.5} / s;
        q.w = (m(1, 0) - m(0, 1)) * s;
        q.x = (m(2, 0) + m(0, 2)) * s;
        q.y = (m(2, 1) + m(1, 2)) * s;
      }
      else
      {
        // i = 0 (x)
        s = std::sqrt(m(0, 0) - m(1, 1) - m(2, 2) + 1.0);
        q.x = s * T{0.5};
        s = T{0.5} / s;
        q.w = (m(2, 1) - m(1, 2)) * s;
        q.y = (m(0, 1) + m(1, 0)) * s;
        q.z = (m(0, 2) + m(2, 0)) * s;
      }
    }

    q.normalize();

    return q;
  }

  template <typename T>
  Quaternion<T> Quaternion<T>::fromAxisAngle(const AxisAngle<T> &axis_angle)
  {
    const T half_angle = axis_angle.phi / T(2.0);
    const T sin_half_angle = std::sin(half_angle);
    return Quaternion<T>(std::cos(half_angle), axis_angle.x * sin_half_angle,
                         axis_angle.y * sin_half_angle, axis_angle.z * sin_half_angle);
  }

  template <typename T>
  Quaternion<T> Quaternion<T>::fromEulerAngles(const EulerAngles<T> &euler)
  {
    T half_roll = euler.roll / T(2.0);
    T half_pitch = euler.pitch / T(2.0);
    T half_yaw = euler.yaw / T(2.0);

    T cr = std::cos(half_roll);
    T sr = std::sin(half_roll);
    T cp = std::cos(half_pitch);
    T sp = std::sin(half_pitch);
    T cy = std::cos(half_yaw);
    T sy = std::sin(half_yaw);

    return Quaternion<T>(cr * cp * cy + sr * sp * sy,
                         sr * cp * cy - cr * sp * sy,
                         cr * sp * cy + sr * cp * sy,
                         cr * cp * sy - sr * sp * cy);
  }

  template class Quaternion<float>;
  template class Quaternion<double>;

} // namespace lumos
