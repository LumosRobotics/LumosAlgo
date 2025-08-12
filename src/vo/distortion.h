#pragma once

#include <utility>

#include "math/math.h"

namespace lumos
{

    struct CameraIntrinsics
    {
        double fx, fy;             // focal lengths
        double cx, cy;             // principal point
        double k1, k2, p1, p2, k3; // distortion coefficients
    };

    struct RemapTable
    {
        Vector<float> map_x;
        Vector<float> map_y;
        size_t width;
        size_t height;
    };

    template <typename T>
    struct Distortion
    {
        // Radial distortion coefficients (k1, k2, k3)
        T k1 = T(0);
        T k2 = T(0);
        T k3 = T(0);

        // Tangential distortion coefficients (p1, p2)
        T p1 = T(0);
        T p2 = T(0);

        Distortion() = default;
        Distortion(T k1_, T k2_, T p1_, T p2_, T k3_ = T(0))
            : k1(k1_), k2(k2_), k3(k3_), p1(p1_), p2(p2_) {}
    };

    // Forward distortion using Brown–Conrady model
    static inline void distortPoint(double x, double y,
                                    const CameraIntrinsics &cam,
                                    double &xd, double &yd)
    {
        double r2 = x * x + y * y;
        double radial = 1.0 + cam.k1 * r2 + cam.k2 * r2 * r2 + cam.k3 * r2 * r2 * r2;
        double x_rad = x * radial;
        double y_rad = y * radial;

        double x_tan = 2.0 * cam.p1 * x * y + cam.p2 * (r2 + 2.0 * x * x);
        double y_tan = cam.p1 * (r2 + 2.0 * y * y) + 2.0 * cam.p2 * x * y;

        xd = x_rad + x_tan;
        yd = y_rad + y_tan;
    }

    // Iteratively invert distortion
    static inline void undistortIterative(double x, double y,
                                          const CameraIntrinsics &cam,
                                          double &xu, double &yu)
    {
        // Initial guess: assume undistorted == distorted
        xu = x;
        yu = y;

        for (int iter = 0; iter < 5; ++iter)
        {
            double xd, yd;
            distortPoint(xu, yu, cam, xd, yd);

            // Error between target distorted point and current distorted guess
            double ex = xd - x;
            double ey = yd - y;

            // Update guess — simple fixed-point iteration
            xu -= ex;
            yu -= ey;
        }
    }

    RemapTable computeUndistortRemap(const CameraIntrinsics &cam, int width, int height)
    {
        RemapTable table;
        table.width = width;
        table.height = height;
        table.map_x.resize(width * height);
        table.map_y.resize(width * height);

        for (int v = 0; v < height; ++v)
        {
            for (int u = 0; u < width; ++u)
            {
                // Normalize to camera coordinates
                double x = (u - cam.cx) / cam.fx;
                double y = (v - cam.cy) / cam.fy;

                // Find the *distorted* coords that map to this undistorted point
                double xu, yu;
                undistortIterative(x, y, cam, xu, yu);

                // Project back to pixel coordinates in distorted image
                double u_dist = cam.fx * xu + cam.cx;
                double v_dist = cam.fy * yu + cam.cy;

                int idx = v * width + u;
                table.map_x(idx) = static_cast<float>(u_dist);
                table.map_y(idx) = static_cast<float>(v_dist);
            }
        }

        return table;
    }

}