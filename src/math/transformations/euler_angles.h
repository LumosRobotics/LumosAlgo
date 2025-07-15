#include <cmath>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/transformations/class_def/euler_angles.h"

namespace lumos {

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::toRotationMatrix() const {
  const T cr = std::cos(roll);
  const T sr = std::sin(roll);
  const T cp = std::cos(pitch);
  const T sp = std::sin(pitch);
  const T cy = std::cos(yaw);
  const T sy = std::sin(yaw);

  FixedSizeMatrix<T, 3, 3> r_mat;

  r_mat(0, 0) = cy * cp;
  r_mat(0, 1) = cy * sp * sr - sy * cr;
  r_mat(0, 2) = cy * sp * cr + sy * sr;
  r_mat(1, 0) = sy * cp;
  r_mat(1, 1) = sy * sp * sr + cy * cr;
  r_mat(1, 2) = sy * sp * cr - cy * sr;
  r_mat(2, 0) = -sp;
  r_mat(2, 1) = cp * sr;
  r_mat(2, 2) = cp * cr;

  return r_mat;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::rollMatrix() const {
  const T cr = std::cos(roll);
  const T sr = std::sin(roll);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = 1;
  r(0, 1) = 0;
  r(0, 2) = 0;
  r(1, 0) = 0;
  r(1, 1) = cr;
  r(1, 2) = -sr;
  r(2, 0) = 0;
  r(2, 1) = sr;
  r(2, 2) = cr;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::pitchMatrix() const {
  const T cp = std::cos(pitch);
  const T sp = std::sin(pitch);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = cp;
  r(0, 1) = 0;
  r(0, 2) = sp;
  r(1, 0) = 0;
  r(1, 1) = 1;
  r(1, 2) = 0;
  r(2, 0) = -sp;
  r(2, 1) = 0;
  r(2, 2) = cp;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::yawMatrix() const {
  const T cy = std::cos(yaw);
  const T sy = std::sin(yaw);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = cy;
  r(0, 1) = -sy;
  r(0, 2) = 0;
  r(1, 0) = sy;
  r(1, 1) = cy;
  r(1, 2) = 0;
  r(2, 0) = 0;
  r(2, 1) = 0;
  r(2, 2) = 1;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3>
EulerAngles<T>::toRotationMatrix(RotationOrder order) const {
  switch (order) {
  case RotationOrder::XYZ:
    return yawMatrix() * pitchMatrix() * rollMatrix();
  case RotationOrder::XZY:
    return pitchMatrix() * yawMatrix() * rollMatrix();
  case RotationOrder::YXZ:
    return yawMatrix() * rollMatrix() * pitchMatrix();
  case RotationOrder::YZX:
    return rollMatrix() * yawMatrix() * pitchMatrix();
  case RotationOrder::ZXY:
    return pitchMatrix() * rollMatrix() * yawMatrix();
  case RotationOrder::ZYX:
    return rollMatrix() * pitchMatrix() * yawMatrix();
  default:
    return toRotationMatrix(); // Default to ZYX
  }
}

} // namespace lumos
