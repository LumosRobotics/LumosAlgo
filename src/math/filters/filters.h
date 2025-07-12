#ifndef LUMOS_MATH_FILTERS_FILTERS_H_
#define LUMOS_MATH_FILTERS_FILTERS_H_

#include "math/filters/fir_filter.h"
#include "math/filters/iir_filter.h"

namespace lumos {

// Common type aliases for convenience
typedef FIRFilter<double> FIRFilterd;
typedef FIRFilter<float> FIRFilterf;
typedef IIRFilter<double> IIRFilterd;
typedef IIRFilter<float> IIRFilterf;

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_FILTERS_H_
