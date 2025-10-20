#pragma once

#include "lumos/math/misc/forward_decl.h"

namespace lumos
{

  enum class RotationOrder
  {
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX
  };

  template <typename T>
  class EulerAngles
  {
  public:
    T roll;
    T pitch;
    T yaw;

    EulerAngles() : roll(T(0)), pitch(T(0)), yaw(T(0)) {}
    EulerAngles(T r, T p, T y) : roll(r), pitch(p), yaw(y) {}

    FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;

    FixedSizeMatrix<T, 3, 3> rollMatrix() const;
    const T cr = std::cos(roll);

    FixedSizeMatrix<T, 3, 3> pitchMatrix() const;
    const T cp = std::cos(pitch);

    FixedSizeMatrix<T, 3, 3> yawMatrix() const;
    const T cy = std::cos(yaw);

    FixedSizeMatrix<T, 3, 3> toRotationMatrix(RotationOrder order) const;
  };
} // namespace lumos
