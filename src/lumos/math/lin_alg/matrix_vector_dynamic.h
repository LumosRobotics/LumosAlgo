#ifndef LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_DYNAMIC_H_

#include <cmath>

#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/lin_alg/vector_low_dim/vec4.h"
#include "lumos/math/misc/math_macros.h"

namespace lumos
{
  template <typename T>
  Matrix<T> Vec3<T>::toCrossProductMatrix() const
  {
    Matrix<T> m(3, 3);
    m(0, 0) = 0.0;
    m(0, 1) = -z;
    m(0, 2) = y;

    m(1, 0) = z;
    m(1, 1) = 0.0;
    m(1, 2) = -x;

    m(2, 0) = -y;
    m(2, 1) = x;
    m(2, 2) = 0.0;
    return m;
  }

  template <typename T>
  Matrix<T> diagMatrix(const Vec3<T> &v)
  {
    Matrix<T> res = unitMatrix<T>(3, 3);

    res(0, 0) = v.x;
    res(1, 1) = v.y;
    res(2, 2) = v.z;

    return res;
  }

  template <typename T>
  std::tuple<Vector<T>, Vector<T>, Vector<T>>
  unzip(const VectorConstView<Vec3<T>> &v)
  {
    Vector<T> x{v.size()}, y{v.size()}, z{v.size()};

    for (size_t k = 0; k < x.size(); k++)
    {
      x(k) = v(k).x;
      y(k) = v(k).y;
      z(k) = v(k).z;
    }

    return {x, y, z};
  }

  template <typename T>
  std::tuple<Vector<T>, Vector<T>> unzip(const VectorConstView<Vec2<T>> &v)
  {
    Vector<T> x{v.size()}, y{v.size()};

    for (size_t k = 0; k < x.size(); k++)
    {
      x(k) = v(k).x;
      y(k) = v(k).y;
    }

    return {x, y};
  }

  template <typename T>
  Vector<Vec3<T>> zip(const VectorConstView<T> &x, const VectorConstView<T> &y,
                      const VectorConstView<T> &z)
  {
    Vector<Vec3<T>> res{x.size()};
    ASSERT(x.size() == y.size())
        << "Size mismatch between x: " << x.size() << " and y: " << y.size();
    ASSERT(x.size() == z.size())
        << "Size mismatch between x: " << x.size() << " and z: " << z.size();

    for (size_t k = 0; k < x.size(); k++)
    {
      res(k).x = x(k);
      res(k).y = y(k);
      res(k).z = z(k);
    }

    return res;
  }

  template <typename T>
  Vector<Vec2<T>> zip(const VectorConstView<T> &x, const VectorConstView<T> &y)
  {
    Vector<Vec2<T>> res{x.size()};
    ASSERT(x.size() == y.size())
        << "Size mismatch between x: " << x.size() << " and y: " << y.size();

    for (size_t k = 0; k < x.size(); k++)
    {
      res(k).x = x(k);
      res(k).y = y(k);
    }

    return res;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_VECTOR_DYNAMIC_H_
