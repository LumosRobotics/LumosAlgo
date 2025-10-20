#include <gtest/gtest.h>
#include <cmath>
#include <algorithm>

#include "lumos/math/fft/fft.impl.h"

namespace lumos
{

    class FFTTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Test tolerance for floating point comparisons
            tolerance = 1e-10;
        }

        double tolerance;

        // Helper function to compare complex vectors
        void ExpectComplexVectorNear(const ComplexVector &expected, const ComplexVector &actual, double tol = 1e-10)
        {
            ASSERT_EQ(expected.size(), actual.size());
            for (size_t i = 0; i < expected.size(); ++i)
            {
                EXPECT_NEAR(expected[i].real(), actual[i].real(), tol) << "Mismatch at index " << i << " (real part)";
                EXPECT_NEAR(expected[i].imag(), actual[i].imag(), tol) << "Mismatch at index " << i << " (imag part)";
            }
        }

        // Helper function to compare real vectors
        void ExpectRealVectorNear(const RealVector &expected, const RealVector &actual, double tol = 1e-10)
        {
            ASSERT_EQ(expected.size(), actual.size());
            for (size_t i = 0; i < expected.size(); ++i)
            {
                EXPECT_NEAR(expected[i], actual[i], tol) << "Mismatch at index " << i;
            }
        }
    };

    // =============================================================================
    // Utility Function Tests
    // =============================================================================

    TEST_F(FFTTest, IsPowerOf2)
    {
        EXPECT_TRUE(fft::is_power_of_2(1));
        EXPECT_TRUE(fft::is_power_of_2(2));
        EXPECT_TRUE(fft::is_power_of_2(4));
        EXPECT_TRUE(fft::is_power_of_2(8));
        EXPECT_TRUE(fft::is_power_of_2(16));
        EXPECT_TRUE(fft::is_power_of_2(1024));

        EXPECT_FALSE(fft::is_power_of_2(0));
        EXPECT_FALSE(fft::is_power_of_2(3));
        EXPECT_FALSE(fft::is_power_of_2(5));
        EXPECT_FALSE(fft::is_power_of_2(6));
        EXPECT_FALSE(fft::is_power_of_2(12));
    }

    TEST_F(FFTTest, NextPowerOf2)
    {
        EXPECT_EQ(fft::next_power_of_2(0), 1U);
        EXPECT_EQ(fft::next_power_of_2(1), 1U);
        EXPECT_EQ(fft::next_power_of_2(2), 2U);
        EXPECT_EQ(fft::next_power_of_2(3), 4U);
        EXPECT_EQ(fft::next_power_of_2(4), 4U);
        EXPECT_EQ(fft::next_power_of_2(5), 8U);
        EXPECT_EQ(fft::next_power_of_2(15), 16U);
        EXPECT_EQ(fft::next_power_of_2(16), 16U);
        EXPECT_EQ(fft::next_power_of_2(17), 32U);
    }

    // =============================================================================
    // Basic FFT Tests
    // =============================================================================

    TEST_F(FFTTest, FFTSize1)
    {
        ComplexVector input = {Complex(5.0, 0.0)};
        ComplexVector expected = {Complex(5.0, 0.0)};

        ComplexVector result = fft::fft(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    TEST_F(FFTTest, FFTSize2)
    {
        ComplexVector input = {Complex(1.0, 0.0), Complex(2.0, 0.0)};
        ComplexVector expected = {Complex(3.0, 0.0), Complex(-1.0, 0.0)};

        ComplexVector result = fft::fft(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    TEST_F(FFTTest, FFTSize4DCComponent)
    {
        // Test with DC component only
        ComplexVector input = {Complex(1.0, 0.0), Complex(1.0, 0.0), Complex(1.0, 0.0), Complex(1.0, 0.0)};
        ComplexVector expected = {Complex(4.0, 0.0), Complex(0.0, 0.0), Complex(0.0, 0.0), Complex(0.0, 0.0)};

        ComplexVector result = fft::fft(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    TEST_F(FFTTest, FFTRealInput)
    {
        RealVector input = {1.0, 2.0, 3.0, 4.0};
        ComplexVector expected = {Complex(10.0, 0.0), Complex(-2.0, 2.0), Complex(-2.0, 0.0), Complex(-2.0, -2.0)};

        ComplexVector result = fft::fft(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    // =============================================================================
    // Inverse FFT Tests
    // =============================================================================

    TEST_F(FFTTest, FFTInverseProperty)
    {
        ComplexVector original = {Complex(1.0, 0.5), Complex(2.0, -1.0), Complex(3.0, 2.0), Complex(4.0, -0.5)};

        ComplexVector fft_result = fft::fft(original);
        ComplexVector reconstructed = fft::ifft(fft_result);

        ExpectComplexVectorNear(original, reconstructed, tolerance);
    }

    TEST_F(FFTTest, IFFTReal)
    {
        RealVector original = {1.0, 2.0, 3.0, 4.0};

        ComplexVector fft_result = fft::fft(original);
        RealVector reconstructed = fft::ifft_real(fft_result);

        ExpectRealVectorNear(original, reconstructed, tolerance);
    }

    // =============================================================================
    // Sinusoidal Signal Tests
    // =============================================================================

    TEST_F(FFTTest, SingleFrequency)
    {
        const size_t N = 64;
        const double freq = 5.0; // 5 cycles over N samples
        const double amplitude = 2.0;

        RealVector signal(N);
        for (size_t i = 0; i < N; ++i)
        {
            signal[i] = amplitude * std::cos(2.0 * M_PI * freq * i / N);
        }

        ComplexVector fft_result = fft::fft(signal);
        RealVector magnitude = fft::magnitude(fft_result);

        // Check that peak occurs at the expected frequency
        auto max_iter = std::max_element(magnitude.begin() + 1, magnitude.begin() + N / 2);
        size_t peak_index = std::distance(magnitude.begin(), max_iter);

        EXPECT_EQ(peak_index, static_cast<size_t>(freq));
        EXPECT_NEAR(magnitude[peak_index], N * amplitude / 2.0, tolerance * N);
    }

    // =============================================================================
    // Frequency Domain Operations Tests
    // =============================================================================

    TEST_F(FFTTest, MagnitudeAndPhase)
    {
        ComplexVector input = {
            Complex(1.0, 0.0),
            Complex(0.0, 1.0),
            Complex(-1.0, 0.0),
            Complex(0.0, -1.0)};

        RealVector expected_magnitude = {1.0, 1.0, 1.0, 1.0};
        RealVector expected_phase = {0.0, M_PI / 2.0, M_PI, -M_PI / 2.0};

        RealVector magnitude = fft::magnitude(input);
        RealVector phase = fft::phase(input);

        ExpectRealVectorNear(expected_magnitude, magnitude, tolerance);
        ExpectRealVectorNear(expected_phase, phase, tolerance);
    }

    TEST_F(FFTTest, PowerSpectrum)
    {
        ComplexVector input = {Complex(1.0, 1.0), Complex(2.0, -1.0), Complex(0.0, 2.0), Complex(-1.0, -1.0)};
        RealVector expected = {2.0, 5.0, 4.0, 2.0}; // |z|^2 for each element

        RealVector power = fft::power_spectrum(input);
        ExpectRealVectorNear(expected, power, tolerance);
    }

    TEST_F(FFTTest, FrequencyBins)
    {
        double sample_rate = 100.0;
        size_t fft_size = 8;

        RealVector expected = {0.0, 12.5, 25.0, 37.5, 50.0, -37.5, -25.0, -12.5};
        RealVector result = fft::frequency_bins(sample_rate, fft_size);

        ExpectRealVectorNear(expected, result, tolerance);
    }

    // =============================================================================
    // Zero Padding Tests
    // =============================================================================

    TEST_F(FFTTest, ZeroPadComplex)
    {
        ComplexVector input = {Complex(1.0, 0.0), Complex(2.0, 1.0), Complex(3.0, -1.0)};
        ComplexVector expected = {Complex(1.0, 0.0), Complex(2.0, 1.0), Complex(3.0, -1.0), Complex(0.0, 0.0)};

        ComplexVector result = fft::zero_pad(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    TEST_F(FFTTest, ZeroPadReal)
    {
        RealVector input = {1.0, 2.0, 3.0};
        ComplexVector expected = {Complex(1.0, 0.0), Complex(2.0, 0.0), Complex(3.0, 0.0), Complex(0.0, 0.0)};

        ComplexVector result = fft::zero_pad(input);
        ExpectComplexVectorNear(expected, result, tolerance);
    }

    // =============================================================================
    // Convolution Tests
    // =============================================================================

    TEST_F(FFTTest, ConvolutionFFT)
    {
        RealVector signal1 = {1.0, 2.0, 3.0};
        RealVector signal2 = {0.5, 1.0};

        // Expected result of convolution
        RealVector expected = {0.5, 1.5, 2.5, 3.0};

        RealVector result = fft::convolve_fft(signal1, signal2);
        ExpectRealVectorNear(expected, result, tolerance);
    }

    TEST_F(FFTTest, ConvolutionIdentity)
    {
        RealVector signal = {1.0, 2.0, 3.0, 4.0};
        RealVector impulse = {1.0};

        RealVector result = fft::convolve_fft(signal, impulse);
        ExpectRealVectorNear(signal, result, tolerance);
    }

    // =============================================================================
    // Error Handling Tests
    // =============================================================================

    TEST_F(FFTTest, NonPowerOfTwoError)
    {
        ComplexVector input = {Complex(1.0, 0.0), Complex(2.0, 0.0), Complex(3.0, 0.0)}; // Size 3

        EXPECT_THROW(fft::fft(input), std::invalid_argument);
        EXPECT_THROW(fft::ifft(input), std::invalid_argument);
    }

    // =============================================================================
    // Performance Test (Optional)
    // =============================================================================

    TEST_F(FFTTest, LargeFFT)
    {
        const size_t N = 1024;
        RealVector signal(N);

        // Generate a test signal with multiple frequency components
        for (size_t i = 0; i < N; ++i)
        {
            signal[i] = std::sin(2.0 * M_PI * 10.0 * i / N) +
                        0.5 * std::cos(2.0 * M_PI * 25.0 * i / N) +
                        0.2 * std::sin(2.0 * M_PI * 50.0 * i / N);
        }

        ComplexVector fft_result = fft::fft(signal);
        RealVector reconstructed = fft::ifft_real(fft_result);

        // Verify round-trip accuracy
        ExpectRealVectorNear(signal, reconstructed, 1e-12);

        // Verify we get the expected size
        EXPECT_EQ(fft_result.size(), N);
    }

} // namespace lumos