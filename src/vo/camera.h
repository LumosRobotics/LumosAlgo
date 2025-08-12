#pragma once

#include "math/math.h"

namespace lumos
{
    template <typename T>
    struct Camera
    {
        FixedSizeMatrix<T, 3, 3> K;

        Camera() = default;
        Camera(const T fx, const T fy, const T cx, const T cy)
        {
            K(0, 0) = fx; // focal length in x
            K(0, 1) = 0;  // skew
            K(0, 2) = cx; // principal point x

            K(1, 0) = 0;  // skew
            K(1, 1) = fy; // focal length in y
            K(1, 2) = cy; // principal point y

            K(2, 0) = 0;
            K(2, 1) = 0;
            K(2, 2) = 1;
        }

        // project a 3D point in camera coordinates to pixel coordinates
        Vec2<T> projectCam(const Vec3<T> &p_cam) const
        {
            const T x = p_cam.x;
            const T y = p_cam.y;
            const T z = p_cam.z;
            assert(z != 0);
            T u = (K(0, 0) * x + K(0, 2) * z) / z + 0; // K(0,1) assumed 0

            // more correctly: u = (fx * x / z) + cx
            u = (K(0, 0) * x) / z + K(0, 2);
            const T v = (K(1, 1) * y) / z + K(1, 2);
            return Vec2<T>(u, v);
        }

        // return normalized image coordinate (x/z, y/z, 1) from pixel
        Vec3<T> unprojectPixel(const Vec2<T> &uv) const
        {
            // inverse K * [u,v,1]
            auto KinvOpt = K.inverse();
            assert(KinvOpt.has_value());
            FixedSizeMatrix<T, 3, 3> Kinv = *KinvOpt;

            T u = uv.x;
            T v = uv.y;

            Vec3<T> hom{u, v, T(1)};
            Vec3<T> res;
            for (size_t r = 0; r < 3; ++r)
            {
                T acc = 0;
                for (size_t c = 0; c < 3; ++c)
                {
                    acc += Kinv(r, c) * ((&hom.x)[c]);
                }
                (&res.x)[r] = acc;
            }

            return res; // not normalized to unit depth; it's K^{-1} * [u,v,1]^T
        }
    };

} // namespace lumos
