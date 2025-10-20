#ifndef DUOPLOT_MATH_GEOMETRY_CLASS_DEF_LINE_3D_H_
#define DUOPLOT_MATH_GEOMETRY_CLASS_DEF_LINE_3D_H_

#include "lumos/plotting/math/misc/forward_decl.h"

namespace duoplot
{
    template <typename T>
    struct Line3D
    {
        // Point on line
        Point3<T> p;

        // Vector in line direction
        Vec3<T> v;

        Line3D(const Point3<T> &p_, const Vec3<T> &v_);
        template <typename Y>
        Line3D(const Line3D<Y> &l);
        Point3<T> closestPointOnLineFromPoint(const Point3<T> &q) const;
        static Line3D<T> fromTwoPoints(const Point3<T> &p0, const Point3<T> &p1);
        Line3D();

        Point3<T> eval(const T t) const;
    };
} // namespace duoplot

#endif // DUOPLOT_MATH_GEOMETRY_CLASS_DEF_LINE_3D_H_
