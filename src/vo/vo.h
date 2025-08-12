#pragma once

#include "math/math.h"
#include "vo/distortion.h"

namespace lumos
{

    // Simple SE3 container using Quaternion + translation
    template <typename T>
    struct SE3
    {

        // A pose SE3 is defined as a coordinate system somewhere in space with an
        // orientation and translation. Consider a pose SE3_0 and a point p0 defined in
        // that coordinate system. The point p0 is defined as p0 = SE3_0.transform(p)
        // where p is the point in the world coordinate system. The inverse is
        // SE3_0.inverse().transform(p0) = p, which transforms the point
        // p0 back to the world coordinate system.
        Quaternion<T> q;
        Vec3<T> t;

        SE3()
        {
        }

        Vec3<T> transform(const Vec3<T> &p) const
        {
            // rotate then translate: R*p + t
            FixedSizeMatrix<T, 3, 3> R = q.toRotationMatrix();
            Vec3<T> out;
            out.x = R(0, 0) * p.x + R(0, 1) * p.y + R(0, 2) * p.z + t.x;
            out.y = R(1, 0) * p.x + R(1, 1) * p.y + R(1, 2) * p.z + t.y;
            out.z = R(2, 0) * p.x + R(2, 1) * p.y + R(2, 2) * p.z + t.z;
            return out;
        }

        SE3 inverse() const
        {
            // inverse is R^T * (p - t)
            SE3 inv;
            inv.q = q.conjugate(); // R^T = conj(q)
            Vec3<T> neg_t = {-t.x, -t.y, -t.z};
            inv.t = inv.q.toRotationMatrix() * neg_t; // R^T * (-t)
            return inv;
        }
    };

} // namespace lumos
