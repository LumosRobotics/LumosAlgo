#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_

#include "math/misc/forward_decl.h"

namespace lumos
{
  template <typename T>
  class EulerAngles;

  template <typename T>
  struct AxisAngle
  {
    T phi;
    T x;
    T y;
    T z;

    AxisAngle(const T phi_, const T x_, const T y_, const T z_);
    AxisAngle(const T x_, const T y_, const T z_);
    AxisAngle(const Vec3<T> &v);
    AxisAngle();

    AxisAngle<T> normalized() const;

    FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;
    Quaternion<T> toQuaternion() const;
    EulerAngles<T> toEulerAngles() const;

    static AxisAngle<T> fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
    static AxisAngle<T> fromQuaternion(const Quaternion<T> &q);
    static AxisAngle<T> fromEulerAngles(const EulerAngles<T> &euler);
  };

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
