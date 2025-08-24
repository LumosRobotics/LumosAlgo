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
        Mat m4{4, 4};
        Mat m3{3, 3};

        m4(RangeInclusive{0, 3}, RangeInclusive{0, 3}) = mat(RangeInclusive{0, 3}, RangeInclusive{0, 3});

        auto m2 = m4(RangeInclusive{0, 3}, RangeInclusive{0, 3});
        m2.fill(0.0);
        m3 = m4(Indices{0, 1, 2}, Indices{0, 1, 2});
        Mat m5 = m4(RangeInclusive{0, 3}, RangeInclusive{0, 3});
    }

} // namespace lumos
