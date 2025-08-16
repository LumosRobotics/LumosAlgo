#include <cmath>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

#include "math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "math/lin_alg/matrix_dynamic/matrix_math_functions.h"
#include "math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "math/lin_alg/vector_dynamic/vector_math_functions.h"

namespace lumos
{
    TEST(MatrixDynamicTest, DefaultConstructor)
    {
        using Mat = Matrix<double>;
        Mat mat{5, 5};
        Mat m0{4, 4};

        m0(RangeInclusive{0, 3}, RangeInclusive{0, 3}) = mat(RangeInclusive{0, 3}, RangeInclusive{0, 3});

        auto m2 = m0(RangeInclusive{0, 3}, RangeInclusive{0, 3});
    }

} // namespace lumos
