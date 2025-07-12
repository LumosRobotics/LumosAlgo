#ifndef LUMOS_MATH_FILTERS_IIR_FILTER_H_
#define LUMOS_MATH_FILTERS_IIR_FILTER_H_

#include "math/filters/class_def/iir_filter.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace lumos {

template <typename T>
IIRFilter<T>::IIRFilter() : numerator_order_(0), denominator_order_(0) {}

template <typename T>
IIRFilter<T>::IIRFilter(const std::vector<T> &b_coeffs,
                        const std::vector<T> &a_coeffs)
    : b_coefficients_(b_coeffs), a_coefficients_(a_coeffs),
      numerator_order_(b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0),
      denominator_order_(a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0) {

  if (a_coefficients_.empty() || a_coefficients_[0] == T(0)) {
    throw std::invalid_argument(
        "First denominator coefficient (a[0]) must be non-zero");
  }

  x_delay_line_.resize(b_coefficients_.size(), T(0));
  y_delay_line_.resize(a_coefficients_.size(), T(0));
}

template <typename T>
IIRFilter<T>::IIRFilter(std::initializer_list<T> b_coeffs,
                        std::initializer_list<T> a_coeffs)
    : b_coefficients_(b_coeffs), a_coefficients_(a_coeffs),
      numerator_order_(b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0),
      denominator_order_(a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0) {

  if (a_coefficients_.empty() || a_coefficients_[0] == T(0)) {
    throw std::invalid_argument(
        "First denominator coefficient (a[0]) must be non-zero");
  }

  x_delay_line_.resize(b_coefficients_.size(), T(0));
  y_delay_line_.resize(a_coefficients_.size(), T(0));
}

template <typename T>
IIRFilter<T>::IIRFilter(size_t b_order, const T *b_coeffs, size_t a_order,
                        const T *a_coeffs)
    : b_coefficients_(b_coeffs, b_coeffs + b_order + 1),
      a_coefficients_(a_coeffs, a_coeffs + a_order + 1),
      numerator_order_(b_order), denominator_order_(a_order) {

  if (a_coefficients_.empty() || a_coefficients_[0] == T(0)) {
    throw std::invalid_argument(
        "First denominator coefficient (a[0]) must be non-zero");
  }

  x_delay_line_.resize(b_coefficients_.size(), T(0));
  y_delay_line_.resize(a_coefficients_.size(), T(0));
}

template <typename T>
IIRFilter<T>::IIRFilter(const IIRFilter &other)
    : b_coefficients_(other.b_coefficients_),
      a_coefficients_(other.a_coefficients_),
      x_delay_line_(other.x_delay_line_), y_delay_line_(other.y_delay_line_),
      numerator_order_(other.numerator_order_),
      denominator_order_(other.denominator_order_) {}

template <typename T>
IIRFilter<T>::IIRFilter(IIRFilter &&other) noexcept
    : b_coefficients_(std::move(other.b_coefficients_)),
      a_coefficients_(std::move(other.a_coefficients_)),
      x_delay_line_(std::move(other.x_delay_line_)),
      y_delay_line_(std::move(other.y_delay_line_)),
      numerator_order_(other.numerator_order_),
      denominator_order_(other.denominator_order_) {}

template <typename T>
IIRFilter<T> &IIRFilter<T>::operator=(const IIRFilter &other) {
  if (this != &other) {
    b_coefficients_ = other.b_coefficients_;
    a_coefficients_ = other.a_coefficients_;
    x_delay_line_ = other.x_delay_line_;
    y_delay_line_ = other.y_delay_line_;
    numerator_order_ = other.numerator_order_;
    denominator_order_ = other.denominator_order_;
  }
  return *this;
}

template <typename T>
IIRFilter<T> &IIRFilter<T>::operator=(IIRFilter &&other) noexcept {
  if (this != &other) {
    b_coefficients_ = std::move(other.b_coefficients_);
    a_coefficients_ = std::move(other.a_coefficients_);
    x_delay_line_ = std::move(other.x_delay_line_);
    y_delay_line_ = std::move(other.y_delay_line_);
    numerator_order_ = other.numerator_order_;
    denominator_order_ = other.denominator_order_;
  }
  return *this;
}

template <typename T> T IIRFilter<T>::filter(T input) {
  if (b_coefficients_.empty() || a_coefficients_.empty()) {
    return T(0);
  }

  // Add new input to delay line
  x_delay_line_.push_front(input);
  if (x_delay_line_.size() > b_coefficients_.size()) {
    x_delay_line_.pop_back();
  }

  // Compute feedforward part (numerator)
  T output = T(0);
  for (size_t i = 0; i < std::min(x_delay_line_.size(), b_coefficients_.size());
       ++i) {
    output += b_coefficients_[i] * x_delay_line_[i];
  }

  // Compute feedback part (denominator, excluding a[0])
  for (size_t i = 1; i < std::min(y_delay_line_.size(), a_coefficients_.size());
       ++i) {
    output -= a_coefficients_[i] * y_delay_line_[i - 1];
  }

  // Normalize by a[0]
  if (a_coefficients_[0] != T(1)) {
    output /= a_coefficients_[0];
  }

  // Add output to delay line
  y_delay_line_.push_front(output);
  if (y_delay_line_.size() > a_coefficients_.size()) {
    y_delay_line_.pop_back();
  }

  return output;
}

template <typename T>
std::vector<T> IIRFilter<T>::filter(const std::vector<T> &input) {
  std::vector<T> output;
  output.reserve(input.size());

  for (const T &sample : input) {
    output.push_back(filter(sample));
  }

  return output;
}

template <typename T>
void IIRFilter<T>::filter(const T *input, T *output, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    output[i] = filter(input[i]);
  }
}

template <typename T> void IIRFilter<T>::reset() {
  std::fill(x_delay_line_.begin(), x_delay_line_.end(), T(0));
  std::fill(y_delay_line_.begin(), y_delay_line_.end(), T(0));
}

template <typename T>
void IIRFilter<T>::setInitialConditions(const std::vector<T> &x_initial,
                                        const std::vector<T> &y_initial) {
  if (x_initial.size() != b_coefficients_.size()) {
    throw std::invalid_argument(
        "Input initial state size must match numerator order");
  }
  if (y_initial.size() != a_coefficients_.size()) {
    throw std::invalid_argument(
        "Output initial state size must match denominator order");
  }

  x_delay_line_.assign(x_initial.begin(), x_initial.end());
  y_delay_line_.assign(y_initial.begin(), y_initial.end());
}

template <typename T>
void IIRFilter<T>::setCoefficients(const std::vector<T> &b_coeffs,
                                   const std::vector<T> &a_coeffs) {
  if (a_coeffs.empty() || a_coeffs[0] == T(0)) {
    throw std::invalid_argument(
        "First denominator coefficient (a[0]) must be non-zero");
  }

  b_coefficients_ = b_coeffs;
  a_coefficients_ = a_coeffs;
  numerator_order_ = b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0;
  denominator_order_ = a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0;

  x_delay_line_.resize(b_coefficients_.size(), T(0));
  y_delay_line_.resize(a_coefficients_.size(), T(0));
}

template <typename T>
const std::vector<T> &IIRFilter<T>::getNumeratorCoefficients() const {
  return b_coefficients_;
}

template <typename T>
const std::vector<T> &IIRFilter<T>::getDenominatorCoefficients() const {
  return a_coefficients_;
}

template <typename T> size_t IIRFilter<T>::getNumeratorOrder() const {
  return numerator_order_;
}

template <typename T> size_t IIRFilter<T>::getDenominatorOrder() const {
  return denominator_order_;
}

template <typename T> size_t IIRFilter<T>::getOrder() const {
  return std::max(numerator_order_, denominator_order_);
}

template <typename T> bool IIRFilter<T>::isEmpty() const {
  return b_coefficients_.empty() && a_coefficients_.empty();
}

template <typename T>
std::complex<T> IIRFilter<T>::frequencyResponse(T frequency,
                                                T sample_rate) const {
  if (b_coefficients_.empty() || a_coefficients_.empty()) {
    return std::complex<T>(0, 0);
  }

  T omega = 2 * M_PI * frequency / sample_rate;

  // Compute numerator response
  std::complex<T> numerator(0, 0);
  for (size_t n = 0; n < b_coefficients_.size(); ++n) {
    T phase = -omega * n;
    numerator +=
        b_coefficients_[n] * std::complex<T>(std::cos(phase), std::sin(phase));
  }

  // Compute denominator response
  std::complex<T> denominator(0, 0);
  for (size_t n = 0; n < a_coefficients_.size(); ++n) {
    T phase = -omega * n;
    denominator +=
        a_coefficients_[n] * std::complex<T>(std::cos(phase), std::sin(phase));
  }

  return numerator / denominator;
}

template <typename T> void IIRFilter<T>::printCoefficients() const {
  std::cout << "IIR Filter Coefficients:" << std::endl;
  std::cout << "Numerator (b): ";
  for (size_t i = 0; i < b_coefficients_.size(); ++i) {
    std::cout << b_coefficients_[i];
    if (i < b_coefficients_.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;
  std::cout << "Denominator (a): ";
  for (size_t i = 0; i < a_coefficients_.size(); ++i) {
    std::cout << a_coefficients_[i];
    if (i < a_coefficients_.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;
}

template <typename T>
IIRFilter<T> IIRFilter<T>::firstOrderLowPass(T cutoff_freq, T sample_rate) {
  T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
  T alpha = std::exp(-omega_c);

  std::vector<T> b_coeffs = {1 - alpha};
  std::vector<T> a_coeffs = {1, -alpha};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::firstOrderHighPass(T cutoff_freq, T sample_rate) {
  T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
  T alpha = std::exp(-omega_c);

  std::vector<T> b_coeffs = {(1 + alpha) / 2, -(1 + alpha) / 2};
  std::vector<T> a_coeffs = {1, -alpha};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::secondOrderLowPass(T cutoff_freq, T q_factor,
                                              T sample_rate) {
  T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
  T alpha = std::sin(omega_c) / (2 * q_factor);

  T b0 = (1 - std::cos(omega_c)) / 2;
  T b1 = 1 - std::cos(omega_c);
  T b2 = (1 - std::cos(omega_c)) / 2;
  T a0 = 1 + alpha;
  T a1 = -2 * std::cos(omega_c);
  T a2 = 1 - alpha;

  std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
  std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::secondOrderHighPass(T cutoff_freq, T q_factor,
                                               T sample_rate) {
  T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
  T alpha = std::sin(omega_c) / (2 * q_factor);

  T b0 = (1 + std::cos(omega_c)) / 2;
  T b1 = -(1 + std::cos(omega_c));
  T b2 = (1 + std::cos(omega_c)) / 2;
  T a0 = 1 + alpha;
  T a1 = -2 * std::cos(omega_c);
  T a2 = 1 - alpha;

  std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
  std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::secondOrderBandPass(T center_freq, T q_factor,
                                               T sample_rate) {
  T omega_c = 2 * M_PI * center_freq / sample_rate;
  T alpha = std::sin(omega_c) / (2 * q_factor);

  T b0 = q_factor * alpha;
  T b1 = 0;
  T b2 = -q_factor * alpha;
  T a0 = 1 + alpha;
  T a1 = -2 * std::cos(omega_c);
  T a2 = 1 - alpha;

  std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
  std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::secondOrderNotch(T center_freq, T q_factor,
                                            T sample_rate) {
  T omega_c = 2 * M_PI * center_freq / sample_rate;
  T alpha = std::sin(omega_c) / (2 * q_factor);

  T b0 = 1;
  T b1 = -2 * std::cos(omega_c);
  T b2 = 1;
  T a0 = 1 + alpha;
  T a1 = -2 * std::cos(omega_c);
  T a2 = 1 - alpha;

  std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
  std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T> IIRFilter<T> IIRFilter<T>::integrator(T sample_rate) {
  T Ts = 1.0 / sample_rate;
  std::vector<T> b_coeffs = {Ts / 2, Ts / 2};
  std::vector<T> a_coeffs = {1, -1};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T> IIRFilter<T> IIRFilter<T>::differentiator(T sample_rate) {
  T Ts = 1.0 / sample_rate;
  std::vector<T> b_coeffs = {1 / Ts, -1 / Ts};
  std::vector<T> a_coeffs = {1};

  return IIRFilter<T>(b_coeffs, a_coeffs);
}

template <typename T>
IIRFilter<T> IIRFilter<T>::dcBlocker(T cutoff_freq, T sample_rate) {
  return firstOrderHighPass(cutoff_freq, sample_rate);
}

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_IIR_FILTER_H_
