#ifndef LUMOS_FIXED_SIZE_VECTOR_H
#define LUMOS_FIXED_SIZE_VECTOR_H

#include <stdint.h>

#include <cmath>
#include <cstdint>
#include <iostream>

template <typename T, uint16_t N> class FixedSizeVector {
private:
  T data_[N];

public:
  constexpr FixedSizeVector() = default;

  constexpr FixedSizeVector(std::initializer_list<T> values);
  constexpr T &operator[](uint16_t index);
  constexpr const T &operator[](uint16_t index) const;

  constexpr FixedSizeVector operator+(const FixedSizeVector &other) const;

  constexpr FixedSizeVector operator-(const FixedSizeVector &other) const;

  constexpr FixedSizeVector operator*(const T scalar) const;
  constexpr T dot(const FixedSizeVector &other) const;

  constexpr T norm() const;
};

// Stream operator declaration
template <typename T, uint16_t N>
std::ostream &operator<<(std::ostream &os, const FixedSizeVector<T, N> &v);

#endif // FIXED_SIZE_VECTOR_H
