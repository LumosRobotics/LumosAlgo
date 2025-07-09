#include "math/transformations/quaternion.h"

#include <cmath>

namespace lumos {
// Default constructor
template <typename T>
constexpr Quaternion<T>::Quaternion() : w_(1), x_(0), y_(0), z_(0) {}

// Parameterized constructor
template <typename T>
constexpr Quaternion<T>::Quaternion(T w, T x, T y, T z)
    : w_(w), x_(x), y_(y), z_(z) {}

// Constructor from axis-angle
/*template <typename T> constexpr Quaternion<T>::Quaternion(const
FixedSizeVector<T, 3>& axis, T angle)
{
    T halfAngle = angle / 2;
    T sinHalfAngle = std::sin(halfAngle);
    w_ = std::cos(halfAngle);
    x_ = axis[0] * sinHalfAngle;
    y_ = axis[1] * sinHalfAngle;
    z_ = axis[2] * sinHalfAngle;
}*/

// Addition
template <typename T>
constexpr Quaternion<T>
Quaternion<T>::operator+(const Quaternion &other) const {
  return Quaternion(w_ + other.w_, x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

// Subtraction
template <typename T>
constexpr Quaternion<T>
Quaternion<T>::operator-(const Quaternion &other) const {
  return Quaternion(w_ - other.w_, x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

// Multiplication
template <typename T>
constexpr Quaternion<T>
    Quaternion<T>::operator*(const Quaternion &other) const {
  return Quaternion(
      w_ * other.w_ - x_ * other.x_ - y_ * other.y_ - z_ * other.z_,
      w_ * other.x_ + x_ * other.w_ + y_ * other.z_ - z_ * other.y_,
      w_ * other.y_ - x_ * other.z_ + y_ * other.w_ + z_ * other.x_,
      w_ * other.z_ + x_ * other.y_ - y_ * other.x_ + z_ * other.w_);
}

// Scalar multiplication
template <typename T>
constexpr Quaternion<T> Quaternion<T>::operator*(const T scalar) const {
  return Quaternion(w_ * scalar, x_ * scalar, y_ * scalar, z_ * scalar);
}

// Conjugate
template <typename T> constexpr Quaternion<T> Quaternion<T>::conjugate() const {
  return Quaternion(w_, -x_, -y_, -z_);
}

// Normalize
template <typename T> constexpr void Quaternion<T>::normalize() {
  T norm = std::sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);
  if (norm > 0) {
    T inv_norm = static_cast<T>(1.0) / norm;
    w_ *= inv_norm;
    x_ *= inv_norm;
    y_ *= inv_norm;
    z_ *= inv_norm;
  }
}

template <typename T>
constexpr FixedSizeMatrix<T, 3, 3> Quaternion<T>::toRotationMatrix() const {}

template class Quaternion<float>;
template class Quaternion<double>;

} // namespace lumos
