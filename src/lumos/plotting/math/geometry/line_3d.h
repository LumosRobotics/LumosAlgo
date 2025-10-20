
#ifndef DUOPLOT_MATH_GEOMETRY_LINE_3D_H_
#define DUOPLOT_MATH_GEOMETRY_LINE_3D_H_

#include <cmath>
#include <vector>

#include "lumos/plotting/math/geometry/class_def/line_3d.h"
#include "lumos/plotting/math/lin_alg.h"

namespace duoplot
{
    template <typename T>
    Line3D<T>::Line3D(const Point3<T> &p_, const Vec3<T> &v_) : p(p_), v(v_) {}

    template <typename T>
    Line3D<T>::Line3D() {}

    template <typename T>
    template <typename Y>
    Line3D<T>::Line3D(const Line3D<Y> &l)
    {
        p = l.p;
        v = l.v;
    }

    template <typename T>
    Point3<T> Line3D<T>::eval(const T t) const
    {
        return p + t * v;
    }

    template <typename T>
    Line3D<T> Line3D<T>::fromTwoPoints(const Point3<T> &p0, const Point3<T> &p1)
    {
        return Line3D<T>(p0, p1 - p0);
    }

    template <typename T>
    Point3<T> Line3D<T>::closestPointOnLineFromPoint(const Point3<T> &q) const
    {
        const T vx = v.x;
        const T vy = v.y;
        const T vz = v.z;

        const T px = p.x;
        const T py = p.y;
        const T pz = p.z;

        const T qx = q.x;
        const T qy = q.y;
        const T qz = q.z;

        // const T t = (vx * (px - qx) + vy * (py - qy) + vz * (pz - qz)) / (vx * vx + vy * vy + vz * vz);
        const T t = (v.x * (q.x - p.x) + v.y * (q.y - p.y) + v.z * (q.z - p.z)) / (v.x * v.x + v.y * v.y + v.z * v.z);

        /*
        p + t * v = q
        p + t * v - q = 0

        (p.x + t * v.x - q.x) ^ 2 + (p.y + t * v.y - q.y) ^ 2 + (p.z + t * v.z - q.z) ^ 2
        The derivative of this is:
        2 * (p.x + t * v.x - q.x) * v.x + 2 * (p.y + t * v.y - q.y) * v.y + 2 * (p.z + t * v.z - q.z) * v.z
        Solved for t:
        t = (v.x * (q.x - p.x) + v.y * (q.y - p.y) + v.z * (q.z - p.z)) / (v.x * v.x + v.y * v.y + v.z * v.z)
        */

        return this->eval(t);
    }

} // namespace duoplot

#endif // DUOPLOT_MATH_GEOMETRY_LINE_3D_H_
