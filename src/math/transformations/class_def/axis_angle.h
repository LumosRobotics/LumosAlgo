#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_

#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T> struct AxisAngle {
  T phi;
  T x;
  T y;
  T z;

  AxisAngle(const T phi_, const T x_, const T y_, const T z_);
  AxisAngle(const T x_, const T y_, const T z_);
  AxisAngle(const Vec3<T> &v);
  AxisAngle();

  AxisAngle<T> normalized() const;

  Matrix<T> toRotationMatrix() const;
};

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
