#include <cmath>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "lumos/math/lin_alg/matrix_dynamic/matrix_math_functions.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_math_functions.h"

namespace lumos
{

  // Test fixture for Matrix dynamic tests
  class MatrixDynamicTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      mat2x2_a = Matrix<double>(2, 2);
      mat2x2_a(0, 0) = 1.0;
      mat2x2_a(0, 1) = 2.0;
      mat2x2_a(1, 0) = 3.0;
      mat2x2_a(1, 1) = 4.0;

      mat2x2_b = Matrix<double>(2, 2);
      mat2x2_b(0, 0) = 5.0;
      mat2x2_b(0, 1) = 6.0;
      mat2x2_b(1, 0) = 7.0;
      mat2x2_b(1, 1) = 8.0;

      mat3x3_identity = Matrix<double>(3, 3);
      mat3x3_identity(0, 0) = 1.0;
      mat3x3_identity(0, 1) = 0.0;
      mat3x3_identity(0, 2) = 0.0;
      mat3x3_identity(1, 0) = 0.0;
      mat3x3_identity(1, 1) = 1.0;
      mat3x3_identity(1, 2) = 0.0;
      mat3x3_identity(2, 0) = 0.0;
      mat3x3_identity(2, 1) = 0.0;
      mat3x3_identity(2, 2) = 1.0;

      mat2x3 = Matrix<double>(2, 3);
      mat2x3(0, 0) = 1.0;
      mat2x3(0, 1) = 2.0;
      mat2x3(0, 2) = 3.0;
      mat2x3(1, 0) = 4.0;
      mat2x3(1, 1) = 5.0;
      mat2x3(1, 2) = 6.0;

      mat3x2 = Matrix<double>(3, 2);
      mat3x2(0, 0) = 1.0;
      mat3x2(0, 1) = 2.0;
      mat3x2(1, 0) = 3.0;
      mat3x2(1, 1) = 4.0;
      mat3x2(2, 0) = 5.0;
      mat3x2(2, 1) = 6.0;

      vec2 = Vector<double>(2);
      vec2(0) = 1.0;
      vec2(1) = 2.0;

      vec3 = Vector<double>(3);
      vec3(0) = 1.0;
      vec3(1) = 2.0;
      vec3(2) = 3.0;
    }

    Matrix<double> mat2x2_a, mat2x2_b;
    Matrix<double> mat3x3_identity;
    Matrix<double> mat2x3, mat3x2;
    Vector<double> vec2, vec3;
  };

  // CONSTRUCTOR TESTS

  TEST_F(MatrixDynamicTest, DefaultConstructor)
  {
    Matrix<double> mat;

    EXPECT_EQ(mat.numRows(), 0U);
    EXPECT_EQ(mat.numCols(), 0U);
    EXPECT_EQ(mat.size(), 0U);
    EXPECT_EQ(mat.numElements(), 0U);
    EXPECT_EQ(mat.data(), nullptr);
  }

  TEST_F(MatrixDynamicTest, ParameterizedConstructor)
  {
    Matrix<double> mat(3, 4);

    EXPECT_EQ(mat.numRows(), 3U);
    EXPECT_EQ(mat.numCols(), 4U);
    EXPECT_EQ(mat.size(), 12U);
    EXPECT_EQ(mat.numElements(), 12U);
    EXPECT_EQ(mat.numBytes(), 12U * sizeof(double));
    EXPECT_NE(mat.data(), nullptr);
  }

  TEST_F(MatrixDynamicTest, CopyConstructor)
  {
    Matrix<double> mat_copy(mat2x2_a);

    EXPECT_EQ(mat_copy.numRows(), 2U);
    EXPECT_EQ(mat_copy.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_copy(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_copy(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_copy(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_copy(1, 1), 4.0);

    // Verify deep copy
    mat_copy(0, 0) = 999.0;
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 0), 1.0);
  }

  TEST_F(MatrixDynamicTest, MoveConstructor)
  {
    Matrix<double> mat_original(2, 2);
    mat_original(0, 0) = 1.0;
    mat_original(0, 1) = 2.0;
    mat_original(1, 0) = 3.0;
    mat_original(1, 1) = 4.0;

    Matrix<double> mat_moved(std::move(mat_original));

    EXPECT_EQ(mat_moved.numRows(), 2U);
    EXPECT_EQ(mat_moved.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_moved(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_moved(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_moved(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_moved(1, 1), 4.0);

    // Original should be in moved-from state
    EXPECT_EQ(mat_original.numRows(), 0U);
    EXPECT_EQ(mat_original.numCols(), 0U);
    EXPECT_EQ(mat_original.data(), nullptr);
  }

  TEST_F(MatrixDynamicTest, TypeConversionConstructor)
  {
    Matrix<float> mat_float(2, 2);
    mat_float(0, 0) = 1.0f;
    mat_float(0, 1) = 2.0f;
    mat_float(1, 0) = 3.0f;
    mat_float(1, 1) = 4.0f;

    Matrix<double> mat_double(mat_float);

    EXPECT_EQ(mat_double.numRows(), 2U);
    EXPECT_EQ(mat_double.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_double(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_double(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_double(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_double(1, 1), 4.0);
  }

  // ASSIGNMENT OPERATORS TESTS

  TEST_F(MatrixDynamicTest, CopyAssignment)
  {
    Matrix<double> mat_assigned(3, 3);
    mat_assigned = mat2x2_a;

    EXPECT_EQ(mat_assigned.numRows(), 2U);
    EXPECT_EQ(mat_assigned.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_assigned(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_assigned(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_assigned(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_assigned(1, 1), 4.0);

    // Verify deep copy
    mat_assigned(0, 0) = 999.0;
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 0), 1.0);
  }

  TEST_F(MatrixDynamicTest, MoveAssignment)
  {
    Matrix<double> mat_original(2, 2);
    mat_original(0, 0) = 1.0;
    mat_original(0, 1) = 2.0;
    mat_original(1, 0) = 3.0;
    mat_original(1, 1) = 4.0;

    Matrix<double> mat_assigned(3, 3);
    mat_assigned = std::move(mat_original);

    EXPECT_EQ(mat_assigned.numRows(), 2U);
    EXPECT_EQ(mat_assigned.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_assigned(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_assigned(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_assigned(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_assigned(1, 1), 4.0);

    // Original should be in moved-from state
    EXPECT_EQ(mat_original.numRows(), 0U);
    EXPECT_EQ(mat_original.numCols(), 0U);
    EXPECT_EQ(mat_original.data(), nullptr);
  }

  TEST_F(MatrixDynamicTest, SelfAssignment)
  {
    Matrix<double> mat_copy = mat2x2_a;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
    mat_copy = mat_copy;
#pragma GCC diagnostic pop

    EXPECT_EQ(mat_copy.numRows(), 2U);
    EXPECT_EQ(mat_copy.numCols(), 2U);
    EXPECT_DOUBLE_EQ(mat_copy(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat_copy(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat_copy(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat_copy(1, 1), 4.0);
  }

  // ELEMENT ACCESS TESTS

  TEST_F(MatrixDynamicTest, ElementAccessOperator)
  {
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat2x2_a(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(mat2x2_a(1, 1), 4.0);

    // Test modification
    mat2x2_a(0, 0) = 999.0;
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 0), 999.0);
  }

  TEST_F(MatrixDynamicTest, ConstElementAccessOperator)
  {
    const Matrix<double> &const_mat = mat2x2_a;

    EXPECT_DOUBLE_EQ(const_mat(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(const_mat(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(const_mat(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(const_mat(1, 1), 4.0);
  }

  // MATRIX OPERATIONS TESTS

  TEST_F(MatrixDynamicTest, MatrixAddition)
  {
    Matrix<double> result = mat2x2_a + mat2x2_b;

    EXPECT_EQ(result.numRows(), 2U);
    EXPECT_EQ(result.numCols(), 2U);
    EXPECT_DOUBLE_EQ(result(0, 0), 6.0);  // 1 + 5
    EXPECT_DOUBLE_EQ(result(0, 1), 8.0);  // 2 + 6
    EXPECT_DOUBLE_EQ(result(1, 0), 10.0); // 3 + 7
    EXPECT_DOUBLE_EQ(result(1, 1), 12.0); // 4 + 8
  }

  TEST_F(MatrixDynamicTest, MatrixSubtraction)
  {
    Matrix<double> result = mat2x2_b - mat2x2_a;

    EXPECT_EQ(result.numRows(), 2U);
    EXPECT_EQ(result.numCols(), 2U);
    EXPECT_DOUBLE_EQ(result(0, 0), 4.0); // 5 - 1
    EXPECT_DOUBLE_EQ(result(0, 1), 4.0); // 6 - 2
    EXPECT_DOUBLE_EQ(result(1, 0), 4.0); // 7 - 3
    EXPECT_DOUBLE_EQ(result(1, 1), 4.0); // 8 - 4
  }

  TEST_F(MatrixDynamicTest, MatrixMultiplication)
  {
    Matrix<double> result = mat2x2_a * mat2x2_b;

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 19.0); // 1*5 + 2*7
    EXPECT_DOUBLE_EQ(result(0, 1), 22.0); // 1*6 + 2*8
    EXPECT_DOUBLE_EQ(result(1, 0), 43.0); // 3*5 + 4*7
    EXPECT_DOUBLE_EQ(result(1, 1), 50.0); // 3*6 + 4*8
  }

  TEST_F(MatrixDynamicTest, MatrixMultiplicationDifferentDimensions)
  {
    Matrix<double> result = mat2x3 * mat3x2;

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 22.0); // 1*1 + 2*3 + 3*5
    EXPECT_DOUBLE_EQ(result(0, 1), 28.0); // 1*2 + 2*4 + 3*6
    EXPECT_DOUBLE_EQ(result(1, 0), 49.0); // 4*1 + 5*3 + 6*5
    EXPECT_DOUBLE_EQ(result(1, 1), 64.0); // 4*2 + 5*4 + 6*6
  }

  TEST_F(MatrixDynamicTest, ElementWiseMultiplication)
  {
    Matrix<double> result = mat2x2_a ^ mat2x2_b;

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 5.0);  // 1 * 5
    EXPECT_DOUBLE_EQ(result(0, 1), 12.0); // 2 * 6
    EXPECT_DOUBLE_EQ(result(1, 0), 21.0); // 3 * 7
    EXPECT_DOUBLE_EQ(result(1, 1), 32.0); // 4 * 8
  }

  TEST_F(MatrixDynamicTest, ElementWiseDivision)
  {
    Matrix<double> result = mat2x2_b / mat2x2_a;

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 5.0);       // 5 / 1
    EXPECT_DOUBLE_EQ(result(0, 1), 3.0);       // 6 / 2
    EXPECT_DOUBLE_EQ(result(1, 0), 7.0 / 3.0); // 7 / 3
    EXPECT_DOUBLE_EQ(result(1, 1), 2.0);       // 8 / 4
  }

  // SCALAR OPERATIONS TESTS

  TEST_F(MatrixDynamicTest, ScalarMultiplication)
  {
    Matrix<double> result1 = mat2x2_a * 2.0;
    Matrix<double> result2 = 2.0 * mat2x2_a;

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 2; c++)
      {
        EXPECT_DOUBLE_EQ(result1(r, c), mat2x2_a(r, c) * 2.0);
        EXPECT_DOUBLE_EQ(result2(r, c), mat2x2_a(r, c) * 2.0);
        EXPECT_DOUBLE_EQ(result1(r, c), result2(r, c));
      }
    }
  }

  TEST_F(MatrixDynamicTest, ScalarDivision)
  {
    Matrix<double> result1 = mat2x2_a / 2.0;
    Matrix<double> result2 = 2.0 / mat2x2_a;

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 2; c++)
      {
        EXPECT_DOUBLE_EQ(result1(r, c), mat2x2_a(r, c) / 2.0);
        EXPECT_DOUBLE_EQ(result2(r, c), 2.0 / mat2x2_a(r, c));
      }
    }
  }

  TEST_F(MatrixDynamicTest, ScalarAddition)
  {
    Matrix<double> result1 = mat2x2_a + 5.0;
    Matrix<double> result2 = 5.0 + mat2x2_a;

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 2; c++)
      {
        EXPECT_DOUBLE_EQ(result1(r, c), mat2x2_a(r, c) + 5.0);
        EXPECT_DOUBLE_EQ(result2(r, c), mat2x2_a(r, c) + 5.0);
      }
    }
  }

  TEST_F(MatrixDynamicTest, ScalarSubtraction)
  {
    Matrix<double> result1 = mat2x2_a - 1.0;
    Matrix<double> result2 = 1.0 - mat2x2_a;

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 2; c++)
      {
        EXPECT_DOUBLE_EQ(result1(r, c), mat2x2_a(r, c) - 1.0);
        EXPECT_DOUBLE_EQ(result2(r, c), 1.0 - mat2x2_a(r, c));
      }
    }
  }

  TEST_F(MatrixDynamicTest, UnaryMinus)
  {
    Matrix<double> result = -mat2x2_a;

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), -1.0);
    EXPECT_DOUBLE_EQ(result(0, 1), -2.0);
    EXPECT_DOUBLE_EQ(result(1, 0), -3.0);
    EXPECT_DOUBLE_EQ(result(1, 1), -4.0);
  }

  // MATRIX-VECTOR OPERATIONS TESTS

  TEST_F(MatrixDynamicTest, MatrixVectorMultiplication)
  {
    Vector<double> result = mat2x2_a * vec2;

    EXPECT_EQ(result.size(), 2);
    EXPECT_DOUBLE_EQ(result(0), 5.0);  // 1*1 + 2*2
    EXPECT_DOUBLE_EQ(result(1), 11.0); // 3*1 + 4*2
  }

  TEST_F(MatrixDynamicTest, VectorMatrixMultiplication)
  {
    Vector<double> result = vec2 * mat2x2_a;

    EXPECT_EQ(result.size(), 2);
    EXPECT_DOUBLE_EQ(result(0), 7.0);  // 1*1 + 2*3
    EXPECT_DOUBLE_EQ(result(1), 10.0); // 1*2 + 2*4
  }

  // MATRIX PROPERTIES TESTS

  TEST_F(MatrixDynamicTest, MatrixTranspose)
  {
    Matrix<double> result = mat2x3.getTranspose();

    EXPECT_EQ(result.numRows(), 3);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(result(2, 0), 3.0);
    EXPECT_DOUBLE_EQ(result(2, 1), 6.0);
  }

  TEST_F(MatrixDynamicTest, MatrixFill)
  {
    Matrix<double> mat(3, 3);
    mat.fill(7.5);

    for (size_t r = 0; r < 3; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        EXPECT_DOUBLE_EQ(mat(r, c), 7.5);
      }
    }
  }

  TEST_F(MatrixDynamicTest, MatrixResize)
  {
    Matrix<double> mat(2, 2);
    mat.resize(4, 3);

    EXPECT_EQ(mat.numRows(), 4);
    EXPECT_EQ(mat.numCols(), 3);
    EXPECT_EQ(mat.size(), 12);
    EXPECT_EQ(mat.numElements(), 12);
  }

  TEST_F(MatrixDynamicTest, MatrixMaxMinSum)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 5.0;
    mat(1, 0) = 3.0;
    mat(1, 1) = 2.0;

    EXPECT_DOUBLE_EQ(mat.max(), 5.0);
    EXPECT_DOUBLE_EQ(mat.min(), 1.0);
    EXPECT_DOUBLE_EQ(mat.sum(), 11.0);
  }

  TEST_F(MatrixDynamicTest, LastRowColIndex)
  {
    Matrix<double> mat(4, 5);

    EXPECT_EQ(mat.lastRowIdx(), 3);
    EXPECT_EQ(mat.lastColIdx(), 4);
  }

  // UTILITY FUNCTIONS TESTS

  TEST_F(MatrixDynamicTest, UnitMatrix)
  {
    Matrix<double> identity = unitMatrix<double>(3, 3);

    EXPECT_EQ(identity.numRows(), 3);
    EXPECT_EQ(identity.numCols(), 3);

    for (size_t r = 0; r < 3; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        if (r == c)
        {
          EXPECT_DOUBLE_EQ(identity(r, c), 1.0);
        }
        else
        {
          EXPECT_DOUBLE_EQ(identity(r, c), 0.0);
        }
      }
    }
  }

  TEST_F(MatrixDynamicTest, ZerosMatrix)
  {
    Matrix<double> zeros = zerosMatrix<double>(2, 3);

    EXPECT_EQ(zeros.numRows(), 2);
    EXPECT_EQ(zeros.numCols(), 3);

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        EXPECT_DOUBLE_EQ(zeros(r, c), 0.0);
      }
    }
  }

  TEST_F(MatrixDynamicTest, OnesMatrix)
  {
    Matrix<double> ones = onesMatrix<double>(2, 3);

    EXPECT_EQ(ones.numRows(), 2);
    EXPECT_EQ(ones.numCols(), 3);

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        EXPECT_DOUBLE_EQ(ones(r, c), 1.0);
      }
    }
  }

  TEST_F(MatrixDynamicTest, FilledMatrix)
  {
    Matrix<double> filled = filledMatrix<double>(2, 3, 3.14);

    EXPECT_EQ(filled.numRows(), 2);
    EXPECT_EQ(filled.numCols(), 3);

    for (size_t r = 0; r < 2; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        EXPECT_DOUBLE_EQ(filled(r, c), 3.14);
      }
    }
  }

  // MATHEMATICAL FUNCTIONS TESTS

  TEST_F(MatrixDynamicTest, MatrixLog)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 2.0;
    mat(1, 0) = 3.0;
    mat(1, 1) = 4.0;

    Matrix<double> result = log(mat);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), std::log(1.0));
    EXPECT_DOUBLE_EQ(result(0, 1), std::log(2.0));
    EXPECT_DOUBLE_EQ(result(1, 0), std::log(3.0));
    EXPECT_DOUBLE_EQ(result(1, 1), std::log(4.0));
  }

  TEST_F(MatrixDynamicTest, MatrixLog10)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 10.0;
    mat(1, 0) = 100.0;
    mat(1, 1) = 1000.0;

    Matrix<double> result = log10(mat);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 3.0);
  }

  TEST_F(MatrixDynamicTest, MatrixExp)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 0.0;
    mat(0, 1) = 1.0;
    mat(1, 0) = 2.0;
    mat(1, 1) = 3.0;

    Matrix<double> result = exp(mat);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), std::exp(0.0));
    EXPECT_DOUBLE_EQ(result(0, 1), std::exp(1.0));
    EXPECT_DOUBLE_EQ(result(1, 0), std::exp(2.0));
    EXPECT_DOUBLE_EQ(result(1, 1), std::exp(3.0));
  }

  TEST_F(MatrixDynamicTest, MatrixPow)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 2.0;
    mat(0, 1) = 3.0;
    mat(1, 0) = 4.0;
    mat(1, 1) = 5.0;

    Matrix<double> result = pow(mat, 2.0);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 4.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 9.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 16.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 25.0);
  }

  TEST_F(MatrixDynamicTest, MatrixSqrt)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 4.0;
    mat(1, 0) = 9.0;
    mat(1, 1) = 16.0;

    Matrix<double> result = sqrt(mat);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 4.0);
  }

  TEST_F(MatrixDynamicTest, MatrixSinCos)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 0.0;
    mat(0, 1) = M_PI / 2;
    mat(1, 0) = M_PI;
    mat(1, 1) = 3 * M_PI / 2;

    Matrix<double> sin_result = sin(mat);
    Matrix<double> cos_result = cos(mat);

    EXPECT_EQ(sin_result.numRows(), 2);
    EXPECT_EQ(sin_result.numCols(), 2);
    EXPECT_NEAR(sin_result(0, 0), 0.0, 1e-10);
    EXPECT_NEAR(sin_result(0, 1), 1.0, 1e-10);
    EXPECT_NEAR(sin_result(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(sin_result(1, 1), -1.0, 1e-10);

    EXPECT_EQ(cos_result.numRows(), 2);
    EXPECT_EQ(cos_result.numCols(), 2);
    EXPECT_NEAR(cos_result(0, 0), 1.0, 1e-10);
    EXPECT_NEAR(cos_result(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(cos_result(1, 0), -1.0, 1e-10);
    EXPECT_NEAR(cos_result(1, 1), 0.0, 1e-10);
  }

  TEST_F(MatrixDynamicTest, MatrixSinhCosh)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 0.0;
    mat(0, 1) = 1.0;
    mat(1, 0) = -1.0;
    mat(1, 1) = 2.0;

    Matrix<double> sinh_result = sinh(mat);
    Matrix<double> cosh_result = cosh(mat);

    EXPECT_EQ(sinh_result.numRows(), 2);
    EXPECT_EQ(sinh_result.numCols(), 2);
    EXPECT_DOUBLE_EQ(sinh_result(0, 0), std::sinh(0.0));
    EXPECT_DOUBLE_EQ(sinh_result(0, 1), std::sinh(1.0));
    EXPECT_DOUBLE_EQ(sinh_result(1, 0), std::sinh(-1.0));
    EXPECT_DOUBLE_EQ(sinh_result(1, 1), std::sinh(2.0));

    EXPECT_EQ(cosh_result.numRows(), 2);
    EXPECT_EQ(cosh_result.numCols(), 2);
    EXPECT_DOUBLE_EQ(cosh_result(0, 0), std::cosh(0.0));
    EXPECT_DOUBLE_EQ(cosh_result(0, 1), std::cosh(1.0));
    EXPECT_DOUBLE_EQ(cosh_result(1, 0), std::cosh(-1.0));
    EXPECT_DOUBLE_EQ(cosh_result(1, 1), std::cosh(2.0));
  }

  TEST_F(MatrixDynamicTest, MatrixAbs)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = -1.0;
    mat(0, 1) = 2.0;
    mat(1, 0) = -3.0;
    mat(1, 1) = 4.0;

    Matrix<double> result = abs(mat);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 4.0);
  }

  TEST_F(MatrixDynamicTest, MatrixMinMax)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 5.0;
    mat(1, 0) = 3.0;
    mat(1, 1) = 2.0;

    double max_val = max(mat);
    double min_val = min(mat);

    EXPECT_DOUBLE_EQ(max_val, 5.0);
    EXPECT_DOUBLE_EQ(min_val, 1.0);
  }

  TEST_F(MatrixDynamicTest, ElementWiseMultiplyFunction)
  {
    Matrix<double> result = elementWiseMultiply(mat2x2_a, mat2x2_b);

    EXPECT_EQ(result.numRows(), 2);
    EXPECT_EQ(result.numCols(), 2);
    EXPECT_DOUBLE_EQ(result(0, 0), 5.0);  // 1 * 5
    EXPECT_DOUBLE_EQ(result(0, 1), 12.0); // 2 * 6
    EXPECT_DOUBLE_EQ(result(1, 0), 21.0); // 3 * 7
    EXPECT_DOUBLE_EQ(result(1, 1), 32.0); // 4 * 8
  }

  // ROTATION MATRIX TESTS

  TEST_F(MatrixDynamicTest, RotationMatrix2D)
  {
    double angle = M_PI / 4.0; // 45 degrees
    Matrix<double> rot_mat = rotationMatrix2D<double>(angle);

    EXPECT_EQ(rot_mat.numRows(), 2);
    EXPECT_EQ(rot_mat.numCols(), 2);

    double cos_45 = std::cos(angle);
    double sin_45 = std::sin(angle);

    EXPECT_NEAR(rot_mat(0, 0), cos_45, 1e-10);
    EXPECT_NEAR(rot_mat(0, 1), -sin_45, 1e-10);
    EXPECT_NEAR(rot_mat(1, 0), sin_45, 1e-10);
    EXPECT_NEAR(rot_mat(1, 1), cos_45, 1e-10);
  }

  TEST_F(MatrixDynamicTest, RotationMatrixX)
  {
    double angle = M_PI / 6.0; // 30 degrees
    Matrix<double> rot_mat = rotationMatrixX<double>(angle);

    EXPECT_EQ(rot_mat.numRows(), 3);
    EXPECT_EQ(rot_mat.numCols(), 3);

    double cos_30 = std::cos(angle);
    double sin_30 = std::sin(angle);

    EXPECT_NEAR(rot_mat(0, 0), 1.0, 1e-10);
    EXPECT_NEAR(rot_mat(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(0, 2), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(1, 1), cos_30, 1e-10);
    EXPECT_NEAR(rot_mat(1, 2), -sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(2, 0), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 1), sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(2, 2), cos_30, 1e-10);
  }

  TEST_F(MatrixDynamicTest, RotationMatrixY)
  {
    double angle = M_PI / 6.0; // 30 degrees
    Matrix<double> rot_mat = rotationMatrixY<double>(angle);

    EXPECT_EQ(rot_mat.numRows(), 3);
    EXPECT_EQ(rot_mat.numCols(), 3);

    double cos_30 = std::cos(angle);
    double sin_30 = std::sin(angle);

    EXPECT_NEAR(rot_mat(0, 0), cos_30, 1e-10);
    EXPECT_NEAR(rot_mat(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(0, 2), sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(1, 1), 1.0, 1e-10);
    EXPECT_NEAR(rot_mat(1, 2), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 0), -sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(2, 1), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 2), cos_30, 1e-10);
  }

  TEST_F(MatrixDynamicTest, RotationMatrixZ)
  {
    double angle = M_PI / 6.0; // 30 degrees
    Matrix<double> rot_mat = rotationMatrixZ<double>(angle);

    EXPECT_EQ(rot_mat.numRows(), 3);
    EXPECT_EQ(rot_mat.numCols(), 3);

    double cos_30 = std::cos(angle);
    double sin_30 = std::sin(angle);

    EXPECT_NEAR(rot_mat(0, 0), cos_30, 1e-10);
    EXPECT_NEAR(rot_mat(0, 1), -sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(0, 2), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(1, 0), sin_30, 1e-10);
    EXPECT_NEAR(rot_mat(1, 1), cos_30, 1e-10);
    EXPECT_NEAR(rot_mat(1, 2), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 0), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 1), 0.0, 1e-10);
    EXPECT_NEAR(rot_mat(2, 2), 1.0, 1e-10);
  }

  // MESH GRID TESTS

  TEST_F(MatrixDynamicTest, MeshGridFromBoundaries)
  {
    auto mesh_result = meshGrid<double>(0.0, 2.0, 0.0, 1.0, 3, 2);

    Matrix<double> x_mat = mesh_result.first;
    Matrix<double> y_mat = mesh_result.second;

    EXPECT_EQ(x_mat.numRows(), 2);
    EXPECT_EQ(x_mat.numCols(), 3);
    EXPECT_EQ(y_mat.numRows(), 2);
    EXPECT_EQ(y_mat.numCols(), 3);

    // Check x_mat values (should repeat x values in each row)
    EXPECT_DOUBLE_EQ(x_mat(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(x_mat(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(x_mat(0, 2), 2.0);
    EXPECT_DOUBLE_EQ(x_mat(1, 0), 0.0);
    EXPECT_DOUBLE_EQ(x_mat(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(x_mat(1, 2), 2.0);

    // Check y_mat values (should repeat y values in each column)
    EXPECT_DOUBLE_EQ(y_mat(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(y_mat(0, 1), 0.0);
    EXPECT_DOUBLE_EQ(y_mat(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(y_mat(1, 0), 1.0);
    EXPECT_DOUBLE_EQ(y_mat(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(y_mat(1, 2), 1.0);
  }

  // MATRIX INITIALIZER TESTS

  TEST_F(MatrixDynamicTest, MatrixInitializerList)
  {
    Matrix<double> mat(
        MatrixInitializer<double>{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    EXPECT_EQ(mat.numRows(), 2U);
    EXPECT_EQ(mat.numCols(), 3U);
    EXPECT_DOUBLE_EQ(mat(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mat(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mat(0, 2), 3.0);
    EXPECT_DOUBLE_EQ(mat(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(mat(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(mat(1, 2), 6.0);
  }

  // STREAM OPERATOR TESTS

  TEST_F(MatrixDynamicTest, StreamOperator)
  {
    std::ostringstream oss;
    oss << mat2x2_a;
    std::string result = oss.str();

    // Check that the output contains the matrix values
    EXPECT_TRUE(result.find("1.000000") != std::string::npos);
    EXPECT_TRUE(result.find("2.000000") != std::string::npos);
    EXPECT_TRUE(result.find("3.000000") != std::string::npos);
    EXPECT_TRUE(result.find("4.000000") != std::string::npos);
    EXPECT_TRUE(result.find("[") != std::string::npos);
    EXPECT_TRUE(result.find("]") != std::string::npos);
  }

  // MATRIX VIEW TESTS

  TEST_F(MatrixDynamicTest, MatrixView)
  {
    auto view = mat2x2_a.view();

    EXPECT_EQ(view.numRows(), 2);
    EXPECT_EQ(view.numCols(), 2);
    EXPECT_EQ(view.size(), 4);
    EXPECT_EQ(view.numElements(), 4);
    EXPECT_EQ(view.data(), mat2x2_a.data());

    // Test element access through view
    EXPECT_DOUBLE_EQ(view(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(view(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(view(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(view(1, 1), 4.0);

    // Test modification through view
    view(0, 0) = 999.0;
    EXPECT_DOUBLE_EQ(mat2x2_a(0, 0), 999.0);
  }

  TEST_F(MatrixDynamicTest, MatrixConstView)
  {
    auto const_view = mat2x2_a.constView();

    EXPECT_EQ(const_view.numRows(), 2);
    EXPECT_EQ(const_view.numCols(), 2);
    EXPECT_EQ(const_view.size(), 4);
    EXPECT_EQ(const_view.numElements(), 4);
    EXPECT_EQ(const_view.data(), mat2x2_a.data());

    // Test element access through const view
    EXPECT_DOUBLE_EQ(const_view(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(const_view(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(const_view(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(const_view(1, 1), 4.0);
  }

  TEST_F(MatrixDynamicTest, MatrixViewFindMinMax)
  {
    auto view = mat2x2_a.view();
    auto min_max = view.findMinMax();

    EXPECT_DOUBLE_EQ(min_max.first, 1.0);  // min
    EXPECT_DOUBLE_EQ(min_max.second, 4.0); // max
  }

  // BUFFER OPERATIONS TESTS

  TEST_F(MatrixDynamicTest, FillBufferWithData)
  {
    Matrix<double> mat(2, 2);
    mat(0, 0) = 1.0;
    mat(0, 1) = 2.0;
    mat(1, 0) = 3.0;
    mat(1, 1) = 4.0;

    std::vector<uint8_t> buffer(mat.numBytes());
    mat.fillBufferWithData(buffer.data());

    // Convert buffer back to double array to verify
    double *data_ptr = reinterpret_cast<double *>(buffer.data());
    EXPECT_DOUBLE_EQ(data_ptr[0], 1.0);
    EXPECT_DOUBLE_EQ(data_ptr[1], 2.0);
    EXPECT_DOUBLE_EQ(data_ptr[2], 3.0);
    EXPECT_DOUBLE_EQ(data_ptr[3], 4.0);
  }

} // namespace lumos
