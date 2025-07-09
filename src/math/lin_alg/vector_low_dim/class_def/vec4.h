#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC4_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC4_H_

#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T> struct Vec4 {
  T x;
  T y;
  T z;
  T w;

  Vec4(const T x_, const T y_, const T z_, const T w_);
  Vec4();
  template <typename Y> Vec4(const Vec4<Y> &v);

  Vec4<T> normalized() const;
  Vec4<T> vectorBetweenPoints(const Point4<T> &end_point) const;
  Vec4<T> normalizedVectorBetweenPoints(const Point4<T> &end_point) const;
  T squaredNorm() const;
  T norm() const;
  Vec4<T> elementWiseMultiply(const Vec4<T> &factor_vector) const;
  Vec4<T> elementWiseDivide(const Vec4<T> &numerator_vector) const;
  T angleBetweenVectors(const Vec4<T> &v) const;
};
} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC4_H_
