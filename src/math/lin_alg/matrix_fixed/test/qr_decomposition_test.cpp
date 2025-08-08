#include <gtest/gtest.h>
#include <cmath>
#include <sstream>
#include <iostream>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/lin_alg/fixed_size_vector/fixed_size_vector.h"

namespace lumos
{

    // Test fixture for QR decomposition tests
    class QRDecompositionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup common test data
            epsilon = 1e-9;
        }

        double epsilon;

        // Helper function to verify QR decomposition properties
        template <typename T, uint16_t Rows, uint16_t Cols>
        void verifyQRDecomposition(const FixedSizeMatrix<T, Rows, Cols> &A,
                                   const QRResult<T, Rows, Cols> &qr_result)
        {
            const auto &Q = qr_result.q;
            const auto &R_matrix = qr_result.r;

            // 1. Verify A = Q * R
            auto QR = Q * R_matrix;
            for (uint16_t i = 0; i < Rows; ++i)
            {
                for (uint16_t j = 0; j < Cols; ++j)
                {
                    EXPECT_NEAR(A(i, j), QR(i, j), epsilon)
                        << "A != QR at position (" << i << ", " << j << ")";
                }
            }

            // 2. Verify Q has orthonormal columns (Q^T * Q = I)
            auto QtQ = Q.transposed() * Q;
            for (uint16_t i = 0; i < Cols; ++i)
            {
                for (uint16_t j = 0; j < Cols; ++j)
                {
                    T expected = (i == j) ? T(1) : T(0);
                    EXPECT_NEAR(QtQ(i, j), expected, epsilon)
                        << "Q^T * Q is not identity at position (" << i << ", " << j << ")";
                }
            }

            // 3. Verify R is upper triangular
            for (uint16_t i = 0; i < Cols; ++i)
            {
                for (uint16_t j = 0; j < i; ++j)
                {
                    EXPECT_NEAR(R_matrix(i, j), T(0), epsilon)
                        << "R is not upper triangular at position (" << i << ", " << j << ")";
                }
            }
        }
    };

    // ==============================================================================
    // Basic QR Decomposition Tests
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_2x2_Simple)
    {
        // Test a simple 2x2 matrix
        FixedSizeMatrix<double, 2, 2> A;
        A(0, 0) = 1.0;
        A(0, 1) = 1.0;
        A(1, 0) = 0.0;
        A(1, 1) = 1.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_3x3_General)
    {
        // Test a general 3x3 matrix
        FixedSizeMatrix<double, 3, 3> A;
        A(0, 0) = 1.0;
        A(0, 1) = 2.0;
        A(0, 2) = 3.0;
        A(1, 0) = 4.0;
        A(1, 1) = 5.0;
        A(1, 2) = 6.0;
        A(2, 0) = 7.0;
        A(2, 1) = 8.0;
        A(2, 2) = 10.0; // Slightly modified to avoid singularity

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_IdentityMatrix)
    {
        // Test identity matrix
        FixedSizeMatrix<double, 3, 3> I = unitMatrix<double, 3, 3>();

        auto qr_opt = I.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(I, qr_result);

        // For identity matrix, Q should be identity and R should be identity
        const auto &Q = qr_result.q;
        const auto &R_matrix = qr_result.r;

        for (uint16_t i = 0; i < 3; ++i)
        {
            for (uint16_t j = 0; j < 3; ++j)
            {
                double expected = (i == j) ? 1.0 : 0.0;
                EXPECT_NEAR(Q(i, j), expected, epsilon)
                    << "Q should be identity for identity matrix input";
                EXPECT_NEAR(R_matrix(i, j), expected, epsilon)
                    << "R should be identity for identity matrix input";
            }
        }
    }

    TEST_F(QRDecompositionTest, QRDecomposition_UpperTriangular)
    {
        // Test upper triangular matrix
        FixedSizeMatrix<double, 3, 3> A;
        A(0, 0) = 2.0;
        A(0, 1) = 1.0;
        A(0, 2) = 3.0;
        A(1, 0) = 0.0;
        A(1, 1) = 4.0;
        A(1, 2) = 1.0;
        A(2, 0) = 0.0;
        A(2, 1) = 0.0;
        A(2, 2) = 5.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_OrthogonalMatrix)
    {
        // Test an orthogonal matrix (rotation matrix)
        FixedSizeMatrix<double, 2, 2> A;
        double angle = M_PI / 4.0; // 45 degrees
        A(0, 0) = std::cos(angle);
        A(0, 1) = -std::sin(angle);
        A(1, 0) = std::sin(angle);
        A(1, 1) = std::cos(angle);

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);

        // For orthogonal matrix, Q should be the matrix itself and R should be identity
        const auto &Q = qr_result.q;
        const auto &R_matrix = qr_result.r;

        for (uint16_t i = 0; i < 2; ++i)
        {
            for (uint16_t j = 0; j < 2; ++j)
            {
                EXPECT_NEAR(std::abs(Q(i, j)), std::abs(A(i, j)), epsilon)
                    << "Q should be similar to original orthogonal matrix";

                double expected_R = (i == j) ? 1.0 : 0.0;
                EXPECT_NEAR(std::abs(R_matrix(i, j)), std::abs(expected_R), epsilon)
                    << "R should be identity (up to sign) for orthogonal matrix input";
            }
        }
    }

    // ==============================================================================
    // Rectangular Matrix Tests (R > C)
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_RectangularMatrix_TallMatrix)
    {
        // Test 4x3 matrix (more rows than columns)
        FixedSizeMatrix<double, 4, 3> A;
        A(0, 0) = 1.0;
        A(0, 1) = 2.0;
        A(0, 2) = 3.0;
        A(1, 0) = 4.0;
        A(1, 1) = 5.0;
        A(1, 2) = 6.0;
        A(2, 0) = 7.0;
        A(2, 1) = 8.0;
        A(2, 2) = 9.0;
        A(3, 0) = 2.0;
        A(3, 1) = 1.0;
        A(3, 2) = 4.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_RectangularMatrix_5x2)
    {
        // Test 5x2 matrix
        FixedSizeMatrix<double, 5, 2> A;
        A(0, 0) = 1.0;
        A(0, 1) = 2.0;
        A(1, 0) = 3.0;
        A(1, 1) = 4.0;
        A(2, 0) = 5.0;
        A(2, 1) = 6.0;
        A(3, 0) = 7.0;
        A(3, 1) = 8.0;
        A(4, 0) = 9.0;
        A(4, 1) = 10.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    // ==============================================================================
    // Edge Cases and Error Conditions
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_ZeroColumn)
    {
        // Test matrix with a zero column (should return nullopt)
        FixedSizeMatrix<double, 3, 3> A;
        A(0, 0) = 1.0;
        A(0, 1) = 0.0;
        A(0, 2) = 3.0;
        A(1, 0) = 2.0;
        A(1, 1) = 0.0;
        A(1, 2) = 6.0;
        A(2, 0) = 3.0;
        A(2, 1) = 0.0;
        A(2, 2) = 9.0;

        auto qr_opt = A.qrDecomposition();
        EXPECT_FALSE(qr_opt.has_value()) << "QR decomposition should fail for matrix with zero column";
    }

    TEST_F(QRDecompositionTest, QRDecomposition_ZeroMatrix)
    {
        // Test zero matrix (should return nullopt)
        FixedSizeMatrix<double, 3, 3> A = zerosMatrix<double, 3, 3>();

        auto qr_opt = A.qrDecomposition();
        EXPECT_FALSE(qr_opt.has_value()) << "QR decomposition should fail for zero matrix";
    }

    TEST_F(QRDecompositionTest, QRDecomposition_1x1Matrix)
    {
        // Test trivial 1x1 case
        FixedSizeMatrix<double, 1, 1> A;
        A(0, 0) = 5.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        const auto &Q = qr_result.q;
        const auto &R_matrix = qr_result.r;

        // For 1x1: Q should be [1] or [-1], R should be [5] or [-5]
        EXPECT_NEAR(std::abs(Q(0, 0)), 1.0, epsilon) << "Q should be unit for 1x1 matrix";
        EXPECT_NEAR(std::abs(R_matrix(0, 0)), 5.0, epsilon) << "R should preserve magnitude for 1x1 matrix";

        // Verify A = Q * R
        EXPECT_NEAR(A(0, 0), Q(0, 0) * R_matrix(0, 0), epsilon) << "A = QR should hold";
    }

    // ==============================================================================
    // Different Data Types
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_FloatType)
    {
        // Test with float type
        FixedSizeMatrix<float, 3, 3> A;
        A(0, 0) = 1.0f;
        A(0, 1) = 2.0f;
        A(0, 2) = 3.0f;
        A(1, 0) = 4.0f;
        A(1, 1) = 5.0f;
        A(1, 2) = 6.0f;
        A(2, 0) = 7.0f;
        A(2, 1) = 8.0f;
        A(2, 2) = 10.0f;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        const auto &Q = qr_result.q;
        const auto &R_matrix = qr_result.r;

        // Verify A = Q * R with float precision
        auto QR = Q * R_matrix;
        float float_epsilon = 1e-5f;
        for (uint16_t i = 0; i < 3; ++i)
        {
            for (uint16_t j = 0; j < 3; ++j)
            {
                EXPECT_NEAR(A(i, j), QR(i, j), float_epsilon)
                    << "A != QR at position (" << i << ", " << j << ")";
            }
        }

        // Verify Q has orthonormal columns
        auto QtQ = Q.transposed() * Q;
        for (uint16_t i = 0; i < 3; ++i)
        {
            for (uint16_t j = 0; j < 3; ++j)
            {
                float expected = (i == j) ? 1.0f : 0.0f;
                EXPECT_NEAR(QtQ(i, j), expected, float_epsilon)
                    << "Q^T * Q is not identity at position (" << i << ", " << j << ")";
            }
        }
    }

    // ==============================================================================
    // Numerical Stability Tests
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_SmallValues)
    {
        // Test with very small values
        FixedSizeMatrix<double, 2, 2> A;
        A(0, 0) = 1e-10;
        A(0, 1) = 2e-10;
        A(1, 0) = 3e-10;
        A(1, 1) = 4e-10;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed for small values";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_LargeValues)
    {
        // Test with large values
        FixedSizeMatrix<double, 2, 2> A;
        A(0, 0) = 1e10;
        A(0, 1) = 2e10;
        A(1, 0) = 3e10;
        A(1, 1) = 4e10;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed for large values";

        auto qr_result = qr_opt.value();

        // Use relaxed epsilon for large values
        double large_epsilon = 1e-6;
        const auto &Q = qr_result.q;
        const auto &R_matrix = qr_result.r;

        // Verify A = Q * R
        auto QR = Q * R_matrix;
        for (uint16_t i = 0; i < 2; ++i)
        {
            for (uint16_t j = 0; j < 2; ++j)
            {
                EXPECT_NEAR(A(i, j), QR(i, j), large_epsilon * std::abs(A(i, j)))
                    << "A != QR at position (" << i << ", " << j << ")";
            }
        }
    }

    // ==============================================================================
    // Properties and Invariants Tests
    // ==============================================================================

    TEST_F(QRDecompositionTest, QRDecomposition_DiagonalDominant)
    {
        // Test diagonally dominant matrix
        FixedSizeMatrix<double, 3, 3> A;
        A(0, 0) = 10.0;
        A(0, 1) = 1.0;
        A(0, 2) = 2.0;
        A(1, 0) = 1.0;
        A(1, 1) = 15.0;
        A(1, 2) = 3.0;
        A(2, 0) = 2.0;
        A(2, 1) = 3.0;
        A(2, 2) = 20.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);

        // Additional check: R diagonal should be positive (by convention)
        const auto &R_matrix = qr_result.r;
        for (uint16_t i = 0; i < 3; ++i)
        {
            EXPECT_GT(R_matrix(i, i), 0.0) << "R diagonal should be positive at (" << i << ", " << i << ")";
        }
    }

    TEST_F(QRDecompositionTest, QRDecomposition_SymmetricMatrix)
    {
        // Test symmetric positive definite matrix
        FixedSizeMatrix<double, 3, 3> A;
        A(0, 0) = 4.0;
        A(0, 1) = 2.0;
        A(0, 2) = 1.0;
        A(1, 0) = 2.0;
        A(1, 1) = 5.0;
        A(1, 2) = 3.0;
        A(2, 0) = 1.0;
        A(2, 1) = 3.0;
        A(2, 2) = 6.0;

        auto qr_opt = A.qrDecomposition();
        ASSERT_TRUE(qr_opt.has_value()) << "QR decomposition should succeed";

        auto qr_result = qr_opt.value();
        verifyQRDecomposition(A, qr_result);
    }

    TEST_F(QRDecompositionTest, QRDecomposition_ConsistencyCheck)
    {
        // Test the same matrix multiple times to ensure consistency
        FixedSizeMatrix<double, 3, 2> A;
        A(0, 0) = 1.0;
        A(0, 1) = 2.0;
        A(1, 0) = 3.0;
        A(1, 1) = 4.0;
        A(2, 0) = 5.0;
        A(2, 1) = 6.0;

        auto qr_opt1 = A.qrDecomposition();
        auto qr_opt2 = A.qrDecomposition();

        ASSERT_TRUE(qr_opt1.has_value()) << "First QR decomposition should succeed";
        ASSERT_TRUE(qr_opt2.has_value()) << "Second QR decomposition should succeed";

        auto qr1 = qr_opt1.value();
        auto qr2 = qr_opt2.value();

        // Results should be identical (up to numerical precision)
        for (uint16_t i = 0; i < 3; ++i)
        {
            for (uint16_t j = 0; j < 2; ++j)
            {
                EXPECT_NEAR(qr1.q(i, j), qr2.q(i, j), epsilon)
                    << "Q matrices should be identical";
            }
        }

        for (uint16_t i = 0; i < 2; ++i)
        {
            for (uint16_t j = 0; j < 2; ++j)
            {
                EXPECT_NEAR(qr1.r(i, j), qr2.r(i, j), epsilon)
                    << "R matrices should be identical";
            }
        }
    }

} // namespace lumos
