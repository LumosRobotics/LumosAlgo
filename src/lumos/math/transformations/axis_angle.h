#ifndef LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_
#define LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_

#include "lumos/math/lin_alg.h"
#include "lumos/math/transformations/class_def/axis_angle.h"
#include "lumos/math/transformations/class_def/euler_angles.h"

namespace lumos
{
  template <typename T>
  AxisAngle<T>::AxisAngle(const T phi_, const T x_, const T y_, const T z_)
  {
    phi = phi_;
    const T norm = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    if (norm == 0.0)
    {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }
    else
    {
      x = x_ / norm;
      y = y_ / norm;
      z = z_ / norm;
    }
  }

  template <typename T>
  AxisAngle<T>::AxisAngle(const T x_, const T y_, const T z_)
  {
    phi = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    if (phi == 0.0)
    {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }
    else
    {
      x = x_ / phi;
      y = y_ / phi;
      z = z_ / phi;
    }
  }

  template <typename T>
  AxisAngle<T>::AxisAngle(const Vec3<T> &v)
  {
    phi = v.norm();
    if (phi == 0.0)
    {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }
    else
    {
      x = v.x / phi;
      y = v.y / phi;
      z = v.z / phi;
    }
  }

  template <typename T>
  AxisAngle<T>::AxisAngle() {}

  template <typename T>
  AxisAngle<T> AxisAngle<T>::normalized() const
  {
    AxisAngle<T> normalized_axis_angle;
    T d = std::sqrt(x * x + y * y + z * z);
    if (d == 0)
    {
      normalized_axis_angle.x = 0.0;
      normalized_axis_angle.y = 0.0;
      normalized_axis_angle.z = 0.0;
    }
    else
    {
      normalized_axis_angle.x = x / d;
      normalized_axis_angle.y = y / d;
      normalized_axis_angle.z = z / d;
    }
    normalized_axis_angle.phi = phi;
    return normalized_axis_angle;
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> AxisAngle<T>::toRotationMatrix() const
  {
    AxisAngle<T> normalized_axis_angle = normalized();

    const FixedSizeMatrix<T, 3, 3> k_matrix =
        Vec3<T>(normalized_axis_angle.x, normalized_axis_angle.y,
                normalized_axis_angle.z)
            .toCrossProductMatrix();

    const FixedSizeMatrix<T, 3, 3> unit_matrix = unitMatrix<T>(3, 3);

    const FixedSizeMatrix<T, 3, 3> rotation_matrix =
        unit_matrix + std::sin(normalized_axis_angle.phi) * k_matrix +
        (1.0 - std::cos(normalized_axis_angle.phi)) * k_matrix * k_matrix;

    return rotation_matrix;
  }

  template <typename T>
  Quaternion<T> AxisAngle<T>::toQuaternion() const
  {
    const T half_angle = phi * T(0.5);
    const T s = std::sin(half_angle);
    const T c = std::cos(half_angle);
    const Vec3<T> u = Vec3<T>(x, y, z).normalized(); // Ensure axis is a unit vector

    return Quaternion{
        c,
        u.x * s,
        u.y * s,
        u.z * s};
  }

  // Non class methods

  template <typename T>
  AxisAngle<T> AxisAngle<T>::fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m)
  {
    // Reference:
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/
    AxisAngle<T> axis_angle;
    const T a0 = m(2, 1) - m(1, 2);
    const T a1 = m(0, 2) - m(2, 0);
    const T a2 = m(1, 0) - m(0, 1);
    const T inv_den = 1.0 / std::sqrt(a0 * a0 + a1 * a1 + a2 * a2);

    axis_angle.phi = std::acos((m(0, 0) + m(1, 1) + m(2, 2) - 1.0) / 2.0);
    if (std::abs(axis_angle.phi) < 1e-8)
    {
      axis_angle.x = 1.0;
      axis_angle.y = 0.0;
      axis_angle.z = 0.0;
    }
    else
    {
      axis_angle.x = a0 * inv_den;
      axis_angle.y = a1 * inv_den;
      axis_angle.z = a2 * inv_den;
    }

    return axis_angle;
  }

  template <typename T>
  AxisAngle<T> AxisAngle<T>::fromQuaternion(const Quaternion<T> &q)
  {
    AxisAngle<T> axis_angle;
    const T epsilon = T(1e-8);

    const T sin_half_angle = std::sqrt(q.x_ * q.x_ + q.y_ * q.y_ + q.z_ * q.z_);

    // Clamp w to [-1, 1] to avoid acos domain errors
    const T w_clamped = std::clamp(q.w_, T(-1), T(1));
    axis_angle.phi = T{2.0} * std::acos(w_clamped);

    if (sin_half_angle < epsilon)
    {
      // Angle is close to 0, arbitrary axis
      axis_angle.x = T{1.0};
      axis_angle.y = T{0.0};
      axis_angle.z = T{0.0};
    }
    else
    {
      axis_angle.x = q.x_ / sin_half_angle;
      axis_angle.y = q.y_ / sin_half_angle;
      axis_angle.z = q.z_ / sin_half_angle;
    }

    return axis_angle;
  }

  template <typename T>
  EulerAngles<T> AxisAngle<T>::toEulerAngles() const
  {
    // Convert axis-angle to rotation matrix first, then to Euler angles
    FixedSizeMatrix<T, 3, 3> rot_matrix = toRotationMatrix();

    EulerAngles<T> euler;

    // Extract Euler angles from rotation matrix (ZYX convention)
    // This assumes the rotation matrix follows ZYX Euler angle convention
    const T sin_pitch = -rot_matrix(2, 0);

    // Check for gimbal lock
    if (std::abs(sin_pitch) >= T(1.0) - T(1e-6))
    {
      // Gimbal lock case
      euler.pitch = std::copysign(T(M_PI) / T(2), sin_pitch);
      euler.yaw = std::atan2(-rot_matrix(0, 1), rot_matrix(1, 1));
      euler.roll = T(0); // Set roll to 0 in gimbal lock
    }
    else
    {
      euler.pitch = std::asin(sin_pitch);
      euler.yaw = std::atan2(rot_matrix(1, 0), rot_matrix(0, 0));
      euler.roll = std::atan2(rot_matrix(2, 1), rot_matrix(2, 2));
    }

    return euler;
  }

  template <typename T>
  AxisAngle<T> AxisAngle<T>::fromEulerAngles(const EulerAngles<T> &euler)
  {
    // Convert Euler angles to rotation matrix first, then to axis-angle
    FixedSizeMatrix<T, 3, 3> rot_matrix = euler.toRotationMatrix();

    return fromRotationMatrix(rot_matrix);
  }

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_
