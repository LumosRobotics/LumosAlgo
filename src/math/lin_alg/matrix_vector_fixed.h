#ifndef LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_

#include <cmath>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/lin_alg/vector_low_dim/vec2.h"
#include "math/lin_alg/vector_low_dim/vec3.h"
#include "math/lin_alg/vector_low_dim/vec4.h"
#include "math/misc/math_macros.h"

namespace lumos {
template <typename T>
FixedSizeMatrix<T, 3, 3> diagFixedSizeMatrix(const Vec3<T> &v) {
  FixedSizeMatrix<T, 3, 3> res = unitFixedSizeMatrix<T, 3, 3>();

  res(0, 0) = v.x;
  res(1, 1) = v.y;
  res(2, 2) = v.z;

  return res;
}

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_
