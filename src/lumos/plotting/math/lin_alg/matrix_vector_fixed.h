#ifndef DUOPLOT_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_
#define DUOPLOT_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_

#include <cmath>

#include "lumos/plotting/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/plotting/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/plotting/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/plotting/math/lin_alg/vector_low_dim/vec4.h"
#include "lumos/plotting/math/misc/math_macros.h"

namespace lumos
{

    template <typename T>
    MatrixFixed<T, 3, 3> diagMatrixFixed(const Vec3<T> &v)
    {
        MatrixFixed<T, 3, 3> res = unitMatrixFixed<T, 3, 3>();

        res(0, 0) = v.x;
        res(1, 1) = v.y;
        res(2, 2) = v.z;

        return res;
    }

} // namespace lumos

#endif // DUOPLOT_MATH_LIN_ALG_MATRIX_VECTOR_FIXED_H_
