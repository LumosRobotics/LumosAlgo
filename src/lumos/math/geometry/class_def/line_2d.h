#ifndef LUMOS_MATH_GEOMETRY_CLASS_DEF_LINE_2D_H_
#define LUMOS_MATH_GEOMETRY_CLASS_DEF_LINE_2D_H_

#include "lumos/math/misc/forward_decl.h"

namespace lumos
{
  template <typename T>
  struct HomogeneousLine2D
  {
    T a;
    T b;
    T c;

    HomogeneousLine2D(const T a_, const T b_, const T c_);
    HomogeneousLine2D(const ParametricLine2D<T> &pl);
    HomogeneousLine2D();
    template <typename Y>
    HomogeneousLine2D(const HomogeneousLine2D<Y> &l);

    T evalX(const T x) const;
    T evalY(const T y) const;
    T eval(const Point2<T> &p) const;
  };

  template <typename T>
  struct ParametricLine2D
  {
    // Point on line
    Point2<T> p;

    // Vector in line direction
    Vec2<T> v;

    ParametricLine2D(const T px_, const T py_, const T vx_, const T vy_);
    ParametricLine2D(const Point2<T> &p_, const Vec2<T> &v_);
    ParametricLine2D(const HomogeneousLine2D<T> &hl);
    ParametricLine2D();
    template <typename Y>
    ParametricLine2D(const ParametricLine2D<Y> &l);

    T tFromX(const T x) const;
    T tFromY(const T y) const;
    Vec2<T> eval(const T t) const;
  };

} // namespace lumos

#endif // LUMOS_MATH_GEOMETRY_CLASS_DEF_LINE_2D_H_
