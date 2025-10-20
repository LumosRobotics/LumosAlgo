#ifndef FFT_H
#define FFT_H

#include <vector>
#include <complex>
#include <cstddef>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

// Type aliases for convenience
using Complex = std::complex<double>;
using ComplexVector = std::vector<Complex>;
using RealVector = std::vector<double>;

namespace fft {

// =============================================================================
// Basic FFT Functions
// =============================================================================

// Forward FFT - Computes the discrete Fourier transform
// Input size must be a power of 2
ComplexVector fft(const ComplexVector& input);

// Forward FFT for real input - returns only positive frequencies + DC
// Input size must be a power of 2
ComplexVector fft(const RealVector& input);

// Inverse FFT - Computes the inverse discrete Fourier transform
// Input size must be a power of 2
ComplexVector ifft(const ComplexVector& input);

// Inverse FFT returning real output (for signals that were originally real)
// Input should be conjugate symmetric
RealVector ifft_real(const ComplexVector& input);

// =============================================================================
// Utility Functions
// =============================================================================

// Check if a number is a power of 2
bool is_power_of_2(size_t n);

// Find the next power of 2 greater than or equal to n
size_t next_power_of_2(size_t n);

// Zero-pad input to the next power of 2
ComplexVector zero_pad(const ComplexVector& input);
ComplexVector zero_pad(const RealVector& input);

// =============================================================================
// Frequency Domain Operations
// =============================================================================

// Compute magnitude spectrum
RealVector magnitude(const ComplexVector& fft_result);

// Compute phase spectrum
RealVector phase(const ComplexVector& fft_result);

// Compute power spectrum (magnitude squared)
RealVector power_spectrum(const ComplexVector& fft_result);

// Generate frequency bins for a given sample rate and FFT size
RealVector frequency_bins(double sample_rate, size_t fft_size);

// =============================================================================
// Convolution using FFT
// =============================================================================

// Convolution of two signals using FFT (faster for large signals)
RealVector convolve_fft(const RealVector& signal1, const RealVector& signal2);

} // namespace fft

#endif // FFT_H