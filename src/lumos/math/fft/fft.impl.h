#ifndef FFT_IMPL_H
#define FFT_IMPL_H

#include "lumos/math/fft/fft.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

namespace fft
{

    // =============================================================================
    // Helper Functions
    // =============================================================================

    LUMOS_INLINE bool is_power_of_2(size_t n)
    {
        return n > 0 && (n & (n - 1)) == 0;
    }

    LUMOS_INLINE size_t next_power_of_2(size_t n)
    {
        if (n == 0)
            return 1;

        size_t power = 1;
        while (power < n)
        {
            power <<= 1;
        }
        return power;
    }

    // Bit-reversal permutation for FFT
    LUMOS_INLINE void bit_reverse_copy(ComplexVector &data)
    {
        const size_t n = data.size();
        const size_t log_n = static_cast<size_t>(std::log2(n));

        for (size_t i = 0; i < n; ++i)
        {
            size_t reversed = 0;
            size_t temp = i;

            for (size_t j = 0; j < log_n; ++j)
            {
                reversed = (reversed << 1) | (temp & 1);
                temp >>= 1;
            }

            if (i < reversed)
            {
                std::swap(data[i], data[reversed]);
            }
        }
    }

    // =============================================================================
    // Core FFT Implementation (Cooley-Tukey Algorithm)
    // =============================================================================

    LUMOS_INLINE ComplexVector fft_internal(ComplexVector data, bool inverse = false)
    {
        const size_t n = data.size();

        if (!is_power_of_2(n))
        {
            throw std::invalid_argument("FFT input size must be a power of 2");
        }

        if (n <= 1)
        {
            return data;
        }

        // Bit-reversal permutation
        bit_reverse_copy(data);

        // Cooley-Tukey FFT algorithm
        const double sign = inverse ? 1.0 : -1.0;

        for (size_t len = 2; len <= n; len <<= 1)
        {
            const double angle = sign * 2.0 * M_PI / len;
            const Complex w_len(std::cos(angle), std::sin(angle));

            for (size_t i = 0; i < n; i += len)
            {
                Complex w(1.0, 0.0);

                for (size_t j = 0; j < len / 2; ++j)
                {
                    const Complex u = data[i + j];
                    const Complex v = data[i + j + len / 2] * w;

                    data[i + j] = u + v;
                    data[i + j + len / 2] = u - v;

                    w *= w_len;
                }
            }
        }

        // Normalize for inverse FFT
        if (inverse)
        {
            for (auto &x : data)
            {
                x /= static_cast<double>(n);
            }
        }

        return data;
    }

    // =============================================================================
    // Public FFT Functions
    // =============================================================================

    LUMOS_INLINE ComplexVector fft(const ComplexVector &input)
    {
        return fft_internal(input, false);
    }

    LUMOS_INLINE ComplexVector fft(const RealVector &input)
    {
        ComplexVector complex_input;
        complex_input.reserve(input.size());

        for (double val : input)
        {
            complex_input.emplace_back(val, 0.0);
        }

        return fft_internal(complex_input, false);
    }

    LUMOS_INLINE ComplexVector ifft(const ComplexVector &input)
    {
        return fft_internal(input, true);
    }

    LUMOS_INLINE RealVector ifft_real(const ComplexVector &input)
    {
        ComplexVector result = ifft(input);
        RealVector real_result;
        real_result.reserve(result.size());

        for (const auto &c : result)
        {
            real_result.push_back(c.real());
        }

        return real_result;
    }

    // =============================================================================
    // Utility Functions
    // =============================================================================

    LUMOS_INLINE ComplexVector zero_pad(const ComplexVector &input)
    {
        const size_t padded_size = next_power_of_2(input.size());
        ComplexVector padded(padded_size, Complex(0.0, 0.0));

        std::copy(input.begin(), input.end(), padded.begin());
        return padded;
    }

    LUMOS_INLINE ComplexVector zero_pad(const RealVector &input)
    {
        const size_t padded_size = next_power_of_2(input.size());
        ComplexVector padded(padded_size, Complex(0.0, 0.0));

        for (size_t i = 0; i < input.size(); ++i)
        {
            padded[i] = Complex(input[i], 0.0);
        }

        return padded;
    }

    // =============================================================================
    // Frequency Domain Operations
    // =============================================================================

    LUMOS_INLINE RealVector magnitude(const ComplexVector &fft_result)
    {
        RealVector mag;
        mag.reserve(fft_result.size());

        for (const auto &c : fft_result)
        {
            mag.push_back(std::abs(c));
        }

        return mag;
    }

    LUMOS_INLINE RealVector phase(const ComplexVector &fft_result)
    {
        RealVector ph;
        ph.reserve(fft_result.size());

        for (const auto &c : fft_result)
        {
            ph.push_back(std::arg(c));
        }

        return ph;
    }

    LUMOS_INLINE RealVector power_spectrum(const ComplexVector &fft_result)
    {
        RealVector power;
        power.reserve(fft_result.size());

        for (const auto &c : fft_result)
        {
            power.push_back(std::norm(c)); // |c|^2
        }

        return power;
    }

    LUMOS_INLINE RealVector frequency_bins(double sample_rate, size_t fft_size)
    {
        RealVector freqs;
        freqs.reserve(fft_size);

        const double freq_resolution = sample_rate / static_cast<double>(fft_size);

        for (size_t i = 0; i < fft_size; ++i)
        {
            if (i <= fft_size / 2)
            {
                freqs.push_back(i * freq_resolution);
            }
            else
            {
                freqs.push_back((static_cast<double>(i) - static_cast<double>(fft_size)) * freq_resolution);
            }
        }

        return freqs;
    }

    // =============================================================================
    // Convolution using FFT
    // =============================================================================

    LUMOS_INLINE RealVector convolve_fft(const RealVector &signal1, const RealVector &signal2)
    {
        // Determine the size of the result
        const size_t result_size = signal1.size() + signal2.size() - 1;
        const size_t fft_size = next_power_of_2(result_size);

        // Zero-pad both signals to the FFT size
        ComplexVector padded1(fft_size, Complex(0.0, 0.0));
        ComplexVector padded2(fft_size, Complex(0.0, 0.0));

        for (size_t i = 0; i < signal1.size(); ++i)
        {
            padded1[i] = Complex(signal1[i], 0.0);
        }

        for (size_t i = 0; i < signal2.size(); ++i)
        {
            padded2[i] = Complex(signal2[i], 0.0);
        }

        // Compute FFTs
        ComplexVector fft1 = fft(padded1);
        ComplexVector fft2 = fft(padded2);

        // Multiply in frequency domain
        ComplexVector fft_product(fft_size);
        for (size_t i = 0; i < fft_size; ++i)
        {
            fft_product[i] = fft1[i] * fft2[i];
        }

        // Compute inverse FFT
        RealVector result = ifft_real(fft_product);

        // Trim to the actual result size
        result.resize(result_size);

        return result;
    }

} // namespace fft

#endif // FFT_IMPL_H