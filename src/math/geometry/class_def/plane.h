#ifndef LUMOS_MATH_GEOMETRY_CLASS_DEF_PLANE_H_
#define LUMOS_MATH_GEOMETRY_CLASS_DEF_PLANE_H_

#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T> struct Plane {
  T a;
  T b;
  T c;
  T d;

  Plane(const T a_, const T b_, const T c_, const T d_);
  Plane(const Point3<T> &point, const Vec3<T> &normal_vector);
  template <typename Y> Plane(const Plane<Y> &p);
  Plane();

  T eval(const Point3<T> &p) const;
  T evalXY(const T x, const T y) const;
  T evalXZ(const T x, const T z) const;
  T evalYZ(const T y, const T z) const;
};
} // namespace lumos

#endif // LUMOS_MATH_GEOMETRY_CLASS_DEF_PLANE_H_
