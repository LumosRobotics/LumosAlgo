
#ifndef LUMOS_MATH_GEOMETRY_LINE_2D_H_
#define LUMOS_MATH_GEOMETRY_LINE_2D_H_

#include <cmath>
#include <utility>

#include "math/geometry/class_def/line_2d.h"
#include "math/lin_alg.h"

namespace lumos {
template <typename T>
HomogeneousLine2D<T>::HomogeneousLine2D(const T a_, const T b_, const T c_)
    : a(a_), b(b_), c(c_) {}

template <typename T>
HomogeneousLine2D<T>::HomogeneousLine2D(const ParametricLine2D<T> &pl) {
  a = -pl.v.y;
  b = pl.v.x;
  c = -(a * pl.p.x + b * pl.p.y);
}

template <typename T> HomogeneousLine2D<T>::HomogeneousLine2D() {}

template <typename T>
template <typename Y>
HomogeneousLine2D<T>::HomogeneousLine2D(const HomogeneousLine2D<Y> &l) {
  a = l.a;
  b = l.b;
  c = l.c;
}

template <typename T> T HomogeneousLine2D<T>::evalX(const T x) const {
  return -(a * x + c) / b;
}

template <typename T> T HomogeneousLine2D<T>::evalY(const T y) const {
  return -(b * y + c) / a;
}

template <typename T> T HomogeneousLine2D<T>::eval(const Point2<T> &p) const {
  return a * p.x + b * p.y + c;
}

// Non class functions

// ParametricLine2D

template <typename T>
ParametricLine2D<T>::ParametricLine2D(const T px_, const T py_, const T vx_,
                                      const T vy_) {
  p.x = px_;
  p.y = py_;
  v.x = vx_;
  v.y = vy_;
}

template <typename T>
ParametricLine2D<T>::ParametricLine2D(const Point2<T> &p_, const Vec2<T> &v_) {
  p = p_;
  v = v_;
}

template <typename T>
ParametricLine2D<T>::ParametricLine2D(const HomogeneousLine2D<T> &hl) {
  v.x = -hl.b;
  v.y = hl.a;

  if (std::fabs(hl.b) > std::fabs(hl.a)) {
    const T x = 1.0;
    const T y = -(hl.a * x + hl.c) / hl.b;
    p.x = x;
    p.y = y;
  } else {
    const T y = 1.0;
    const T x = -(hl.b * y + hl.c) / hl.a;
    p.x = x;
    p.y = y;
  }
}

template <typename T> ParametricLine2D<T>::ParametricLine2D() {}

template <typename T>
template <typename Y>
ParametricLine2D<T>::ParametricLine2D(const ParametricLine2D<Y> &l) {
  p = l.p;
  v = l.v;
}

template <typename T> T ParametricLine2D<T>::tFromX(const T x) const {
  return (x - p.x) / v.x;
}

template <typename T> T ParametricLine2D<T>::tFromY(const T y) const {
  return (y - p.y) / v.y;
}

template <typename T> Vec2<T> ParametricLine2D<T>::eval(const T t) const {
  return p + t * v;
}

} // namespace lumos

#endif // LUMOS_MATH_GEOMETRY_LINE_2D_H_
