# LumosAlgo Filter Implementations

This directory contains implementations of digital signal processing filters for the LumosAlgo library.

## Overview

The filters module provides implementations for:
1. **FIR Filters** - Finite Impulse Response filters
2. **IIR Filters** - Infinite Impulse Response filters

## Features

### FIR Filters
- **Finite impulse response** with configurable coefficients
- **Linear phase response** (inherently stable)
- **Convolution-based filtering** with efficient delay line implementation
- **Factory methods** for common filter types:
  - Moving average
  - Low-pass, high-pass, band-pass filters
  - Differentiator and integrator
- **Batch and single-sample processing**
- **Frequency response analysis**

### IIR Filters
- **Infinite impulse response** with numerator and denominator coefficients
- **Feedback structure** for efficient higher-order filtering
- **Factory methods** for common filter types:
  - First and second-order low-pass/high-pass
  - Band-pass and notch filters
  - Butterworth and Chebyshev designs
  - Integrator, differentiator, and DC blocker
- **Stability checking** and pole-zero analysis
- **Frequency response analysis**

## Usage Examples

### FIR Filter Examples

```cpp
#include "math/math.h"
using namespace lumos;

// Example 1: Moving Average Filter
FIRFilterd ma_filter = FIRFilterd::movingAverage(5);
std::vector<double> input = {1.0, 2.0, 1.5, 3.0, 2.5};
std::vector<double> output = ma_filter.filter(input);

// Example 2: Custom FIR Filter
FIRFilterd custom_fir({0.1, 0.2, 0.4, 0.2, 0.1});
double filtered_sample = custom_fir.filter(2.0);

// Example 3: Low-pass Filter
FIRFilterd lpf = FIRFilterd::lowPass(32, 1000.0, 8000.0); // 32nd order, 1kHz cutoff, 8kHz sample rate
std::vector<double> filtered_signal = lpf.filter(noisy_signal);

// Example 4: Batch Processing
FIRFilterf fir_filter({0.25, 0.5, 0.25});
fir_filter.filter(input_buffer, output_buffer, buffer_size);
```

### IIR Filter Examples

```cpp
#include "math/math.h"
using namespace lumos;

// Example 1: First-order Low-pass Filter
IIRFilterd lpf = IIRFilterd::firstOrderLowPass(100.0, 1000.0); // 100Hz cutoff, 1kHz sampling
double filtered_sample = lpf.filter(input_sample);

// Example 2: Second-order Band-pass Filter
IIRFilterd bpf = IIRFilterd::secondOrderBandPass(440.0, 10.0, 44100.0); // 440Hz center, Q=10, 44.1kHz sampling
std::vector<double> filtered_audio = bpf.filter(audio_samples);

// Example 3: Custom IIR Filter
std::vector<double> b_coeffs = {0.1, 0.2, 0.1};  // Numerator
std::vector<double> a_coeffs = {1.0, -0.5, 0.2}; // Denominator
IIRFilterd custom_iir(b_coeffs, a_coeffs);

// Example 4: Notch Filter for 60Hz Hum Removal
IIRFilterd notch = IIRFilterd::secondOrderNotch(60.0, 10.0, 1000.0);
std::vector<double> clean_signal = notch.filter(noisy_signal);
```

## Mathematical Background

### FIR Filters
FIR filters implement the difference equation:
```
y[n] = Σ(k=0 to M) b[k] * x[n-k]
```
where:
- `y[n]` is the output at time n
- `x[n]` is the input at time n
- `b[k]` are the filter coefficients
- `M` is the filter order

### IIR Filters
IIR filters implement the difference equation:
```
y[n] = (Σ(k=0 to M) b[k] * x[n-k] - Σ(k=1 to N) a[k] * y[n-k]) / a[0]
```
where:
- `b[k]` are the numerator coefficients (feedforward)
- `a[k]` are the denominator coefficients (feedback)
- `M` is the numerator order
- `N` is the denominator order

## Filter Design Methods

### FIR Filter Design
- **Windowed sinc method** for low-pass filters
- **Spectral inversion** for high-pass filters
- **Frequency sampling** for arbitrary frequency response
- **Parks-McClellan algorithm** (future implementation)

### IIR Filter Design
- **Bilinear transform** for analog-to-digital conversion
- **Butterworth designs** for maximally flat response
- **Chebyshev designs** for equiripple response
- **Biquad sections** for numerical stability

## Type Aliases

Common type aliases are provided for convenience:

```cpp
// FIR filters
using FIRFilterd = FIRFilter<double>;
using FIRFilterf = FIRFilter<float>;

// IIR filters
using IIRFilterd = IIRFilter<double>;
using IIRFilterf = IIRFilter<float>;
```

## File Structure

```
src/math/filters/
├── class_def/
│   ├── fir_filter.h          # FIR filter class definition
│   └── iir_filter.h          # IIR filter class definition
├── fir_filter.h              # FIR filter implementation
├── iir_filter.h              # IIR filter implementation
├── filters.h                 # Main filters header with all includes
└── README.md                 # This documentation
```

## Performance Considerations

### FIR Filters
- **Memory usage**: O(N) where N is filter order
- **Computational complexity**: O(N) per sample
- **Numerical stability**: Always stable (no feedback)
- **Phase response**: Linear phase possible

### IIR Filters
- **Memory usage**: O(max(M,N)) where M,N are numerator/denominator orders
- **Computational complexity**: O(M+N) per sample
- **Numerical stability**: Depends on pole locations
- **Phase response**: Generally non-linear

## Applications

- **Audio processing**: Equalizers, effects, noise reduction
- **Control systems**: Sensor filtering, actuator control
- **Communications**: Channel equalization, interference rejection
- **Instrumentation**: Signal conditioning, measurement filtering
- **Biomedical**: ECG/EEG filtering, artifact removal
- **Computer vision**: Image smoothing, edge detection

## Building and Testing

The filters module is included in the main LumosAlgo library. To build:

```bash
cd /path/to/LumosAlgo
mkdir -p build
cd build
cmake ..
make
```

Run the demo application:
```bash
./src/applications/simple/simple
```

## Advanced Features

### Filter Analysis
- **Frequency response**: Magnitude and phase analysis
- **Impulse response**: Time-domain characteristics
- **Step response**: Transient behavior
- **Stability analysis**: Pole-zero plots for IIR filters

### Filter Cascading
```cpp
// Cascade multiple filters
IIRFilterd stage1 = IIRFilterd::firstOrderLowPass(100.0, 1000.0);
IIRFilterd stage2 = IIRFilterd::firstOrderLowPass(200.0, 1000.0);

for (double sample : input) {
    double intermediate = stage1.filter(sample);
    double output = stage2.filter(intermediate);
    // Process output...
}
```

### Filter State Management
```cpp
// Save and restore filter state
FIRFilterd filter = FIRFilterd::movingAverage(5);
// ... process some samples ...
filter.reset(); // Clear filter memory
filter.setInitialConditions(saved_state); // Restore specific state
```

## Future Enhancements

- **Adaptive filters**: LMS, RLS algorithms
- **Multirate filtering**: Decimation, interpolation
- **Filter banks**: Octave, constant-Q transforms
- **Optimization**: SIMD acceleration, fixed-point arithmetic
- **GUI tools**: Interactive filter design interface
