#include "math/lin_alg/fixed_size_vector/class_def/fixed_size_vector.h"
#include <cmath>
#include <initializer_list>
#include <iostream>

namespace lumos
{

  template <typename T, uint16_t N>
  constexpr FixedSizeVector<T, N>::FixedSizeVector(
      std::initializer_list<T> values)
  {
    uint16_t i = 0;
    for (const T &val : values)
    {
      if (i >= N)
      {
        break;
      }
      data_[i++] = val;
    }
  }

  template <typename T, uint16_t N>
  constexpr T &FixedSizeVector<T, N>::operator[](const uint16_t index)
  {
    return data_[index];
  }

  template <typename T, uint16_t N>
  constexpr const T &FixedSizeVector<T, N>::operator[](const uint16_t index) const
  {
    return data_[index];
  }

  template <typename T, uint16_t N>
  constexpr FixedSizeVector<T, N>
  FixedSizeVector<T, N>::operator+(const FixedSizeVector &other) const
  {
    FixedSizeVector result;
    for (uint16_t i = 0; i < N; ++i)
      result[i] = data_[i] + other[i];
    return result;
  }

  template <typename T, uint16_t N>
  constexpr FixedSizeVector<T, N>
  FixedSizeVector<T, N>::operator-(const FixedSizeVector &other) const
  {
    FixedSizeVector result;
    for (uint16_t i = 0; i < N; ++i)
      result[i] = data_[i] - other[i];
    return result;
  }

  template <typename T, uint16_t N>
  constexpr FixedSizeVector<T, N>
  FixedSizeVector<T, N>::operator*(const T scalar) const
  {
    FixedSizeVector result;
    for (uint16_t i = 0; i < N; ++i)
      result[i] = data_[i] * scalar;
    return result;
  }

  template <typename T, uint16_t N>
  constexpr T FixedSizeVector<T, N>::dot(const FixedSizeVector &other) const
  {
    T sum = 0;
    for (uint16_t i = 0; i < N; ++i)
      sum += data_[i] * other[i];
    return sum;
  }

  template <typename T, uint16_t N>
  constexpr T FixedSizeVector<T, N>::norm() const
  {
    return std::sqrt(this->dot(*this));
  }

  template <typename T, uint16_t N>
  std::ostream &operator<<(std::ostream &os, const FixedSizeVector<T, N> &v)
  {
    os << "[";
    for (uint16_t i = 0; i < N; ++i)
    {
      os << v[i];
      if (i < N - 1)
        os << ", ";
    }
    os << "]";
    return os;
  }

  template <typename T, uint16_t N>
  Vector<T> FixedSizeVector<T, N>::toDynamicVector() const
  {
    Vector<T> dynamic_vector(N);
    for (uint16_t i = 0; i < N; ++i)
    {
      dynamic_vector[i] = data_[i];
    }
    return dynamic_vector;
  }

} // namespace lumos
