#include <cmath>
#include <gtest/gtest.h>
#include <limits>

#include "lumos/math/geometry/line_2d.h"
#include "lumos/math/geometry/line_3d.h"
#include "lumos/math/geometry/plane.h"
#include "lumos/math/geometry/triangle.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"

namespace lumos
{

  // Constants for testing
  constexpr double EPSILON = 1e-9;

  // Test fixture for 2D line tests
  class Line2DTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      p1 = Point2<double>(0.0, 0.0);
      p2 = Point2<double>(1.0, 1.0);
      v1 = Vec2<double>(1.0, 0.0); // Horizontal vector
      v2 = Vec2<double>(0.0, 1.0); // Vertical vector
      v3 = Vec2<double>(1.0, 1.0); // Diagonal vector
    }

    Point2<double> p1, p2;
    Vec2<double> v1, v2, v3;
  };

  // Test fixture for 3D line tests
  class Line3DTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      p1 = Point3<double>(0.0, 0.0, 0.0);
      p2 = Point3<double>(1.0, 1.0, 1.0);
      p3 = Point3<double>(2.0, 0.0, 0.0);
      v1 = Vec3<double>(1.0, 0.0, 0.0); // X-axis vector
      v2 = Vec3<double>(0.0, 1.0, 0.0); // Y-axis vector
      v3 = Vec3<double>(0.0, 0.0, 1.0); // Z-axis vector
      v4 = Vec3<double>(1.0, 1.0, 1.0); // Diagonal vector
    }

    Point3<double> p1, p2, p3;
    Vec3<double> v1, v2, v3, v4;
  };

  // Test fixture for plane tests
  class PlaneTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      p1 = Point3<double>(0.0, 0.0, 0.0);
      p2 = Point3<double>(1.0, 0.0, 0.0);
      p3 = Point3<double>(0.0, 1.0, 0.0);
      p4 = Point3<double>(1.0, 1.0, 0.0);

      normal_z = Vec3<double>(0.0, 0.0, 1.0); // Z-axis normal
      normal_x = Vec3<double>(1.0, 0.0, 0.0); // X-axis normal
      normal_y = Vec3<double>(0.0, 1.0, 0.0); // Y-axis normal
    }

    Point3<double> p1, p2, p3, p4;
    Vec3<double> normal_z, normal_x, normal_y;
  };

  // Test fixture for triangle tests
  class TriangleTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data for 2D triangles
      p2d_1 = Point2<double>(0.0, 0.0);
      p2d_2 = Point2<double>(1.0, 0.0);
      p2d_3 = Point2<double>(0.5, 1.0);

      // Setup common test data for 3D triangles
      p3d_1 = Point3<double>(0.0, 0.0, 0.0);
      p3d_2 = Point3<double>(1.0, 0.0, 0.0);
      p3d_3 = Point3<double>(0.5, 1.0, 0.0);
      p3d_4 = Point3<double>(0.0, 0.0, 1.0);
    }

    Point2<double> p2d_1, p2d_2, p2d_3;
    Point3<double> p3d_1, p3d_2, p3d_3, p3d_4;
  };

  // HomogeneousLine2D Tests
  TEST_F(Line2DTest, HomogeneousLine2DConstructor)
  {
    // Test coefficient constructor
    HomogeneousLine2D<double> line(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(line.a, 1.0);
    EXPECT_DOUBLE_EQ(line.b, 2.0);
    EXPECT_DOUBLE_EQ(line.c, 3.0);
  }

  TEST_F(Line2DTest, HomogeneousLine2DFromParametric)
  {
    // Create parametric line: point (0,0) with direction (1,0)
    ParametricLine2D<double> parametric(p1, v1);
    HomogeneousLine2D<double> homogeneous(parametric);

    // For a horizontal line y = 0, homogeneous form should be 0x + 1y + 0 = 0
    EXPECT_DOUBLE_EQ(homogeneous.a, 0.0);
    EXPECT_DOUBLE_EQ(homogeneous.b, 1.0);
    EXPECT_DOUBLE_EQ(homogeneous.c, 0.0);
  }

  TEST_F(Line2DTest, HomogeneousLine2DEvalX)
  {
    // Line: x + y - 1 = 0 (diagonal line through (0,1) and (1,0))
    HomogeneousLine2D<double> line(1.0, 1.0, -1.0);

    // When x = 0, y should be 1
    EXPECT_DOUBLE_EQ(line.evalX(0.0), 1.0);

    // When x = 1, y should be 0
    EXPECT_DOUBLE_EQ(line.evalX(1.0), 0.0);
  }

  TEST_F(Line2DTest, HomogeneousLine2DEvalY)
  {
    // Line: x + y - 1 = 0
    HomogeneousLine2D<double> line(1.0, 1.0, -1.0);

    // When y = 0, x should be 1
    EXPECT_DOUBLE_EQ(line.evalY(0.0), 1.0);

    // When y = 1, x should be 0
    EXPECT_DOUBLE_EQ(line.evalY(1.0), 0.0);
  }

  TEST_F(Line2DTest, HomogeneousLine2DEvalPoint)
  {
    // Line: x + y - 1 = 0
    HomogeneousLine2D<double> line(1.0, 1.0, -1.0);

    // Point (0,1) should be on the line
    EXPECT_NEAR(line.eval(Point2<double>(0.0, 1.0)), 0.0, EPSILON);

    // Point (1,0) should be on the line
    EXPECT_NEAR(line.eval(Point2<double>(1.0, 0.0)), 0.0, EPSILON);

    // Point (0,0) should be below the line (negative)
    EXPECT_LT(line.eval(Point2<double>(0.0, 0.0)), 0.0);
  }

  // ParametricLine2D Tests
  TEST_F(Line2DTest, ParametricLine2DConstructor)
  {
    // Test parameter constructor
    ParametricLine2D<double> line(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(line.p.x, 1.0);
    EXPECT_DOUBLE_EQ(line.p.y, 2.0);
    EXPECT_DOUBLE_EQ(line.v.x, 3.0);
    EXPECT_DOUBLE_EQ(line.v.y, 4.0);

    // Test point-vector constructor
    ParametricLine2D<double> line2(p1, v1);
    EXPECT_DOUBLE_EQ(line2.p.x, p1.x);
    EXPECT_DOUBLE_EQ(line2.p.y, p1.y);
    EXPECT_DOUBLE_EQ(line2.v.x, v1.x);
    EXPECT_DOUBLE_EQ(line2.v.y, v1.y);
  }

  TEST_F(Line2DTest, ParametricLine2DFromHomogeneous)
  {
    // Create homogeneous line: x + y - 1 = 0
    HomogeneousLine2D<double> homogeneous(1.0, 1.0, -1.0);
    ParametricLine2D<double> parametric(homogeneous);

    // Direction vector should be perpendicular to normal (1,1)
    EXPECT_DOUBLE_EQ(parametric.v.x, -1.0);
    EXPECT_DOUBLE_EQ(parametric.v.y, 1.0);

    // Point should be on the line
    EXPECT_NEAR(homogeneous.eval(parametric.p), 0.0, EPSILON);
  }

  TEST_F(Line2DTest, ParametricLine2DTFromX)
  {
    // Line through origin with direction (1,0)
    ParametricLine2D<double> line(p1, v1);

    // For horizontal line, t should equal x
    EXPECT_DOUBLE_EQ(line.tFromX(0.0), 0.0);
    EXPECT_DOUBLE_EQ(line.tFromX(1.0), 1.0);
    EXPECT_DOUBLE_EQ(line.tFromX(2.0), 2.0);
  }

  TEST_F(Line2DTest, ParametricLine2DTFromY)
  {
    // Line through origin with direction (0,1)
    ParametricLine2D<double> line(p1, v2);

    // For vertical line, t should equal y
    EXPECT_DOUBLE_EQ(line.tFromY(0.0), 0.0);
    EXPECT_DOUBLE_EQ(line.tFromY(1.0), 1.0);
    EXPECT_DOUBLE_EQ(line.tFromY(2.0), 2.0);
  }

  TEST_F(Line2DTest, ParametricLine2DEval)
  {
    // Line through origin with direction (1,1)
    ParametricLine2D<double> line(p1, v3);

    // At t=0, should be at origin
    Vec2<double> result = line.eval(0.0);
    EXPECT_DOUBLE_EQ(result.x, 0.0);
    EXPECT_DOUBLE_EQ(result.y, 0.0);

    // At t=1, should be at (1,1)
    result = line.eval(1.0);
    EXPECT_DOUBLE_EQ(result.x, 1.0);
    EXPECT_DOUBLE_EQ(result.y, 1.0);
  }

  // Line3D Tests
  TEST_F(Line3DTest, Line3DConstructor)
  {
    Line3D<double> line(p1, v1);

    EXPECT_DOUBLE_EQ(line.p.x, p1.x);
    EXPECT_DOUBLE_EQ(line.p.y, p1.y);
    EXPECT_DOUBLE_EQ(line.p.z, p1.z);
    EXPECT_DOUBLE_EQ(line.v.x, v1.x);
    EXPECT_DOUBLE_EQ(line.v.y, v1.y);
    EXPECT_DOUBLE_EQ(line.v.z, v1.z);
  }

  TEST_F(Line3DTest, Line3DFromTwoPoints)
  {
    Line3D<double> line = Line3D<double>::fromTwoPoints(p1, p2);

    // Point should be p1
    EXPECT_DOUBLE_EQ(line.p.x, p1.x);
    EXPECT_DOUBLE_EQ(line.p.y, p1.y);
    EXPECT_DOUBLE_EQ(line.p.z, p1.z);

    // Direction should be p2 - p1
    EXPECT_DOUBLE_EQ(line.v.x, p2.x - p1.x);
    EXPECT_DOUBLE_EQ(line.v.y, p2.y - p1.y);
    EXPECT_DOUBLE_EQ(line.v.z, p2.z - p1.z);
  }

  TEST_F(Line3DTest, Line3DEval)
  {
    Line3D<double> line(p1, v4);

    // At t=0, should be at p1
    Point3<double> result = line.eval(0.0);
    EXPECT_DOUBLE_EQ(result.x, p1.x);
    EXPECT_DOUBLE_EQ(result.y, p1.y);
    EXPECT_DOUBLE_EQ(result.z, p1.z);

    // At t=1, should be at p1 + v4
    result = line.eval(1.0);
    EXPECT_DOUBLE_EQ(result.x, p1.x + v4.x);
    EXPECT_DOUBLE_EQ(result.y, p1.y + v4.y);
    EXPECT_DOUBLE_EQ(result.z, p1.z + v4.z);
  }

  TEST_F(Line3DTest, Line3DClosestPointOnLineFromPoint)
  {
    // Line along x-axis from origin
    Line3D<double> line(p1, v1);

    // Point (0,1,0) - closest point on line should be (0,0,0)
    Point3<double> query(0.0, 1.0, 0.0);
    Point3<double> closest = line.closestPointOnLineFromPoint(query);

    EXPECT_NEAR(closest.x, 0.0, EPSILON);
    EXPECT_NEAR(closest.y, 0.0, EPSILON);
    EXPECT_NEAR(closest.z, 0.0, EPSILON);

    // Point (1,1,0) - closest point should be (1,0,0)
    query = Point3<double>(1.0, 1.0, 0.0);
    closest = line.closestPointOnLineFromPoint(query);

    EXPECT_NEAR(closest.x, 1.0, EPSILON);
    EXPECT_NEAR(closest.y, 0.0, EPSILON);
    EXPECT_NEAR(closest.z, 0.0, EPSILON);
  }

  // Plane Tests
  TEST_F(PlaneTest, PlaneConstructorFromCoefficients)
  {
    Plane<double> plane(1.0, 2.0, 3.0, 4.0);

    EXPECT_DOUBLE_EQ(plane.a, 1.0);
    EXPECT_DOUBLE_EQ(plane.b, 2.0);
    EXPECT_DOUBLE_EQ(plane.c, 3.0);
    EXPECT_DOUBLE_EQ(plane.d, 4.0);
  }

  TEST_F(PlaneTest, PlaneConstructorFromPointAndNormal)
  {
    // XY plane (z=0) with normal pointing up
    Plane<double> plane(p1, normal_z);

    EXPECT_DOUBLE_EQ(plane.a, 0.0);
    EXPECT_DOUBLE_EQ(plane.b, 0.0);
    EXPECT_DOUBLE_EQ(plane.c, 1.0);
    EXPECT_DOUBLE_EQ(plane.d, 0.0);
  }

  TEST_F(PlaneTest, PlaneEval)
  {
    // XY plane: z = 0
    Plane<double> plane(0.0, 0.0, 1.0, 0.0);

    // Points on the plane should evaluate to 0
    EXPECT_DOUBLE_EQ(plane.eval(p1), 0.0);
    EXPECT_DOUBLE_EQ(plane.eval(p2), 0.0);
    EXPECT_DOUBLE_EQ(plane.eval(p3), 0.0);

    // Point above the plane should be positive
    Point3<double> above(0.0, 0.0, 1.0);
    EXPECT_GT(plane.eval(above), 0.0);

    // Point below the plane should be negative
    Point3<double> below(0.0, 0.0, -1.0);
    EXPECT_LT(plane.eval(below), 0.0);
  }

  TEST_F(PlaneTest, PlaneEvalXY)
  {
    // Plane: z = x + y
    Plane<double> plane(1.0, 1.0, -1.0, 0.0);

    // When x=0, y=0, z should be 0
    EXPECT_DOUBLE_EQ(plane.evalXY(0.0, 0.0), 0.0);

    // When x=1, y=0, z should be 1
    EXPECT_DOUBLE_EQ(plane.evalXY(1.0, 0.0), 1.0);

    // When x=1, y=1, z should be 2
    EXPECT_DOUBLE_EQ(plane.evalXY(1.0, 1.0), 2.0);
  }

  TEST_F(PlaneTest, PlaneEvalXZ)
  {
    // Plane: y = x + z
    Plane<double> plane(1.0, -1.0, 1.0, 0.0);

    // When x=0, z=0, y should be 0
    EXPECT_DOUBLE_EQ(plane.evalXZ(0.0, 0.0), 0.0);

    // When x=1, z=0, y should be 1
    EXPECT_DOUBLE_EQ(plane.evalXZ(1.0, 0.0), 1.0);

    // When x=1, z=1, y should be 2
    EXPECT_DOUBLE_EQ(plane.evalXZ(1.0, 1.0), 2.0);
  }

  TEST_F(PlaneTest, PlaneEvalYZ)
  {
    // Plane: x = y + z
    Plane<double> plane(-1.0, 1.0, 1.0, 0.0);

    // When y=0, z=0, x should be 0
    EXPECT_DOUBLE_EQ(plane.evalYZ(0.0, 0.0), 0.0);

    // When y=1, z=0, x should be 1
    EXPECT_DOUBLE_EQ(plane.evalYZ(1.0, 0.0), 1.0);

    // When y=1, z=1, x should be 2
    EXPECT_DOUBLE_EQ(plane.evalYZ(1.0, 1.0), 2.0);
  }

  TEST_F(PlaneTest, PlaneFromThreePoints)
  {
    // Create plane from three points on XY plane
    Plane<double> plane = planeFromThreePoints(p1, p2, p3);

    // All three points should be on the plane
    EXPECT_NEAR(plane.eval(p1), 0.0, EPSILON);
    EXPECT_NEAR(plane.eval(p2), 0.0, EPSILON);
    EXPECT_NEAR(plane.eval(p3), 0.0, EPSILON);

    // Normal should point in z direction (may be positive or negative)
    // The cross product of two normalized vectors has magnitude <= 1
    EXPECT_NEAR(plane.a, 0.0, EPSILON);
    EXPECT_NEAR(plane.b, 0.0, EPSILON);
    EXPECT_GT(std::abs(plane.c), 0.0); // Non-zero z component
  }

  // Triangle2D Tests
  TEST_F(TriangleTest, Triangle2DConstructor)
  {
    Triangle2D<double> triangle(p2d_1, p2d_2, p2d_3);

    EXPECT_DOUBLE_EQ(triangle.p0.x, p2d_1.x);
    EXPECT_DOUBLE_EQ(triangle.p0.y, p2d_1.y);
    EXPECT_DOUBLE_EQ(triangle.p1.x, p2d_2.x);
    EXPECT_DOUBLE_EQ(triangle.p1.y, p2d_2.y);
    EXPECT_DOUBLE_EQ(triangle.p2.x, p2d_3.x);
    EXPECT_DOUBLE_EQ(triangle.p2.y, p2d_3.y);
  }

  TEST_F(TriangleTest, Triangle2DDefaultConstructor)
  {
    Triangle2D<double> triangle;

    // Default constructor doesn't initialize values to zero
    // Just verify the object can be created without errors
    // Individual member values are uninitialized and can contain garbage
    EXPECT_NO_THROW(Triangle2D<double> t);
  }

  TEST_F(TriangleTest, Triangle2DCopyConstructor)
  {
    Triangle2D<double> original(p2d_1, p2d_2, p2d_3);
    Triangle2D<float> copy(original);

    EXPECT_FLOAT_EQ(copy.p0.x, static_cast<float>(original.p0.x));
    EXPECT_FLOAT_EQ(copy.p0.y, static_cast<float>(original.p0.y));
    EXPECT_FLOAT_EQ(copy.p1.x, static_cast<float>(original.p1.x));
    EXPECT_FLOAT_EQ(copy.p1.y, static_cast<float>(original.p1.y));
    EXPECT_FLOAT_EQ(copy.p2.x, static_cast<float>(original.p2.x));
    EXPECT_FLOAT_EQ(copy.p2.y, static_cast<float>(original.p2.y));
  }

  // Triangle3D Tests
  TEST_F(TriangleTest, Triangle3DConstructor)
  {
    Triangle3D<double> triangle(p3d_1, p3d_2, p3d_3);

    EXPECT_DOUBLE_EQ(triangle.p0.x, p3d_1.x);
    EXPECT_DOUBLE_EQ(triangle.p0.y, p3d_1.y);
    EXPECT_DOUBLE_EQ(triangle.p0.z, p3d_1.z);
    EXPECT_DOUBLE_EQ(triangle.p1.x, p3d_2.x);
    EXPECT_DOUBLE_EQ(triangle.p1.y, p3d_2.y);
    EXPECT_DOUBLE_EQ(triangle.p1.z, p3d_2.z);
    EXPECT_DOUBLE_EQ(triangle.p2.x, p3d_3.x);
    EXPECT_DOUBLE_EQ(triangle.p2.y, p3d_3.y);
    EXPECT_DOUBLE_EQ(triangle.p2.z, p3d_3.z);
  }

  TEST_F(TriangleTest, Triangle3DDefaultConstructor)
  {
    Triangle3D<double> triangle;

    // Default constructor doesn't initialize values to zero
    // Just verify the object can be created without errors
    // Individual member values are uninitialized and can contain garbage
    EXPECT_NO_THROW(Triangle3D<double> t);
  }

  TEST_F(TriangleTest, Triangle3DCopyConstructor)
  {
    Triangle3D<double> original(p3d_1, p3d_2, p3d_4);
    Triangle3D<float> copy(original);

    EXPECT_FLOAT_EQ(copy.p0.x, static_cast<float>(original.p0.x));
    EXPECT_FLOAT_EQ(copy.p0.y, static_cast<float>(original.p0.y));
    EXPECT_FLOAT_EQ(copy.p0.z, static_cast<float>(original.p0.z));
    EXPECT_FLOAT_EQ(copy.p1.x, static_cast<float>(original.p1.x));
    EXPECT_FLOAT_EQ(copy.p1.y, static_cast<float>(original.p1.y));
    EXPECT_FLOAT_EQ(copy.p1.z, static_cast<float>(original.p1.z));
    EXPECT_FLOAT_EQ(copy.p2.x, static_cast<float>(original.p2.x));
    EXPECT_FLOAT_EQ(copy.p2.y, static_cast<float>(original.p2.y));
    EXPECT_FLOAT_EQ(copy.p2.z, static_cast<float>(original.p2.z));
  }

  // Edge case and error handling tests
  TEST_F(Line2DTest, HomogeneousLine2DVerticalLine)
  {
    // Vertical line: x = 1 (represented as 1x + 0y - 1 = 0)
    HomogeneousLine2D<double> line(1.0, 0.0, -1.0);

    // evalY should work for vertical lines
    EXPECT_DOUBLE_EQ(line.evalY(0.0), 1.0);
    EXPECT_DOUBLE_EQ(line.evalY(1.0), 1.0);

    // Points on the line should evaluate to 0
    EXPECT_NEAR(line.eval(Point2<double>(1.0, 0.0)), 0.0, EPSILON);
    EXPECT_NEAR(line.eval(Point2<double>(1.0, 5.0)), 0.0, EPSILON);
  }

  TEST_F(Line2DTest, ParametricLine2DVerticalLine)
  {
    // Vertical line through origin
    ParametricLine2D<double> line(p1, v2);

    // tFromY should work correctly
    EXPECT_DOUBLE_EQ(line.tFromY(0.0), 0.0);
    EXPECT_DOUBLE_EQ(line.tFromY(1.0), 1.0);

    // Evaluation should give correct points
    Vec2<double> result = line.eval(2.0);
    EXPECT_DOUBLE_EQ(result.x, 0.0);
    EXPECT_DOUBLE_EQ(result.y, 2.0);
  }

  TEST_F(Line3DTest, Line3DParallelToAxis)
  {
    // Line parallel to each axis
    Line3D<double> x_line(p1, v1);
    Line3D<double> y_line(p1, v2);
    Line3D<double> z_line(p1, v3);

    // Test closest point calculation for each axis
    Point3<double> query(1.0, 1.0, 1.0);

    Point3<double> closest_x = x_line.closestPointOnLineFromPoint(query);
    EXPECT_NEAR(closest_x.x, 1.0, EPSILON);
    EXPECT_NEAR(closest_x.y, 0.0, EPSILON);
    EXPECT_NEAR(closest_x.z, 0.0, EPSILON);

    Point3<double> closest_y = y_line.closestPointOnLineFromPoint(query);
    EXPECT_NEAR(closest_y.x, 0.0, EPSILON);
    EXPECT_NEAR(closest_y.y, 1.0, EPSILON);
    EXPECT_NEAR(closest_y.z, 0.0, EPSILON);

    Point3<double> closest_z = z_line.closestPointOnLineFromPoint(query);
    EXPECT_NEAR(closest_z.x, 0.0, EPSILON);
    EXPECT_NEAR(closest_z.y, 0.0, EPSILON);
    EXPECT_NEAR(closest_z.z, 1.0, EPSILON);
  }

  TEST_F(PlaneTest, PlaneCoplanarPoints)
  {
    // Test with four coplanar points
    Point3<double> p5(0.5, 0.5, 0.0);

    Plane<double> plane = planeFromThreePoints(p1, p2, p3);

    // Fourth coplanar point should also be on the plane
    EXPECT_NEAR(plane.eval(p4), 0.0, EPSILON);
    EXPECT_NEAR(plane.eval(p5), 0.0, EPSILON);
  }

} // namespace lumos

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
