#include "fft.impl.h"
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    std::cout << "FFT Library Example\n";
    std::cout << "==================\n\n";

    // Example 1: Basic FFT of a real signal
    std::cout << "1. Basic FFT Example:\n";
    RealVector signal = {1.0, 2.0, 3.0, 4.0, 0.0, 0.0, 0.0, 0.0};
    
    std::cout << "Input signal: ";
    for (double x : signal) {
        std::cout << std::fixed << std::setprecision(1) << x << " ";
    }
    std::cout << "\n";
    
    ComplexVector fft_result = fft::fft(signal);
    std::cout << "FFT result:\n";
    for (size_t i = 0; i < fft_result.size(); ++i) {
        std::cout << "  bin " << i << ": " 
                  << std::fixed << std::setprecision(3) 
                  << fft_result[i].real() << " + " << fft_result[i].imag() << "i\n";
    }
    
    // Example 2: FFT of a sinusoidal signal
    std::cout << "\n2. Sinusoidal Signal FFT:\n";
    const size_t N = 32;
    const double frequency = 4.0;  // 4 cycles over N samples
    const double amplitude = 1.0;
    
    RealVector sine_wave(N);
    for (size_t i = 0; i < N; ++i) {
        sine_wave[i] = amplitude * std::sin(2.0 * M_PI * frequency * i / N);
    }
    
    ComplexVector sine_fft = fft::fft(sine_wave);
    RealVector magnitude = fft::magnitude(sine_fft);
    
    std::cout << "Magnitude spectrum of " << frequency << " Hz sine wave:\n";
    for (size_t i = 0; i < N/2; ++i) {
        std::cout << "  bin " << std::setw(2) << i << ": " 
                  << std::fixed << std::setprecision(3) 
                  << magnitude[i] << "\n";
    }
    
    // Find peak
    auto max_iter = std::max_element(magnitude.begin() + 1, magnitude.begin() + N/2);
    size_t peak_bin = std::distance(magnitude.begin(), max_iter);
    std::cout << "Peak at bin " << peak_bin << " (expected: " << frequency << ")\n";
    
    // Example 3: Round-trip test (FFT -> IFFT)
    std::cout << "\n3. Round-trip Test (FFT -> IFFT):\n";
    RealVector original = {1.0, 2.0, 3.0, 4.0};
    ComplexVector fft_orig = fft::fft(original);
    RealVector reconstructed = fft::ifft_real(fft_orig);
    
    std::cout << "Original:      ";
    for (double x : original) {
        std::cout << std::fixed << std::setprecision(6) << x << " ";
    }
    std::cout << "\nReconstructed: ";
    for (double x : reconstructed) {
        std::cout << std::fixed << std::setprecision(6) << x << " ";
    }
    std::cout << "\n";
    
    // Example 4: Convolution using FFT
    std::cout << "\n4. Convolution Example:\n";
    RealVector sig1 = {1.0, 2.0, 3.0};
    RealVector sig2 = {0.5, 1.0, 0.5};
    
    RealVector conv_result = fft::convolve_fft(sig1, sig2);
    
    std::cout << "Signal 1: ";
    for (double x : sig1) std::cout << x << " ";
    std::cout << "\nSignal 2: ";
    for (double x : sig2) std::cout << x << " ";
    std::cout << "\nConvolution result: ";
    for (double x : conv_result) {
        std::cout << std::fixed << std::setprecision(3) << x << " ";
    }
    std::cout << "\n";
    
    // Example 5: Frequency bins
    std::cout << "\n5. Frequency Bins Example:\n";
    double sample_rate = 1000.0;  // 1000 Hz
    size_t fft_size = 8;
    RealVector freqs = fft::frequency_bins(sample_rate, fft_size);
    
    std::cout << "Sample rate: " << sample_rate << " Hz, FFT size: " << fft_size << "\n";
    std::cout << "Frequency bins: ";
    for (double f : freqs) {
        std::cout << std::fixed << std::setprecision(1) << f << " ";
    }
    std::cout << "Hz\n";
    
    std::cout << "\nAll examples completed successfully!\n";
    return 0;
}