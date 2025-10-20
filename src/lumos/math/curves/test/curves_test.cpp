#include <array>
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

#include "lumos/math/curves/curves.h"
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"

namespace lumos
{

  // Test fixture for Bezier curve tests
  class BezierCurveTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      control_points_2d = {Vec2<double>(0.0, 0.0), Vec2<double>(1.0, 1.0),
                           Vec2<double>(2.0, 0.0)};

      control_points_3d = {Vec3<double>(0.0, 0.0, 0.0),
                           Vec3<double>(1.0, 1.0, 0.0),
                           Vec3<double>(2.0, 0.0, 1.0)};
    }

    std::vector<Vec2<double>> control_points_2d;
    std::vector<Vec3<double>> control_points_3d;
  };

  // Test fixture for B-spline curve tests
  class BSplineCurveTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      control_points_2d = {Vec2<double>(0.0, 0.0), Vec2<double>(1.0, 1.0),
                           Vec2<double>(2.0, 0.0), Vec2<double>(3.0, 1.0)};

      control_points_3d = {
          Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(1.0, 1.0, 0.0),
          Vec3<double>(2.0, 0.0, 1.0), Vec3<double>(3.0, 1.0, 0.0)};

      // Clamped knot vector for degree 3 curve with 4 control points
      knot_vector = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    }

    std::vector<Vec2<double>> control_points_2d;
    std::vector<Vec3<double>> control_points_3d;
    std::vector<double> knot_vector;
  };

  // Test fixture for Quintic Polynomial tests
  class QuinticPolynomialTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup boundary conditions
      start_pos_2d = Vec2<double>(0.0, 0.0);
      start_vel_2d = Vec2<double>(1.0, 0.0);
      start_acc_2d = Vec2<double>(0.0, 0.0);
      end_pos_2d = Vec2<double>(10.0, 5.0);
      end_vel_2d = Vec2<double>(0.0, 1.0);
      end_acc_2d = Vec2<double>(0.0, 0.0);

      duration = 2.0;
    }

    Vec2<double> start_pos_2d, start_vel_2d, start_acc_2d;
    Vec2<double> end_pos_2d, end_vel_2d, end_acc_2d;
    double duration;
  };

  // BEZIER CURVE TESTS

  TEST_F(BezierCurveTest, Constructor)
  {
    BezierCurve2Dd curve(control_points_2d);
    EXPECT_EQ(curve.getControlPoints().size(), 3u);
    EXPECT_EQ(curve.getDegree(), 2u);
  }

  TEST_F(BezierCurveTest, EmptyConstructor)
  {
    BezierCurve2Dd curve;
    EXPECT_EQ(curve.getControlPoints().size(), 0u);
    EXPECT_EQ(curve.getDegree(), 0u);
  }

  TEST_F(BezierCurveTest, SetControlPoints)
  {
    BezierCurve2Dd curve;
    curve.setControlPoints(control_points_2d);
    EXPECT_EQ(curve.getControlPoints().size(), 3u);
    EXPECT_EQ(curve.getDegree(), 2u);
  }

  TEST_F(BezierCurveTest, EvaluateAtStartAndEnd)
  {
    BezierCurve2Dd curve(control_points_2d);

    // At t=0, curve should be at first control point
    Vec2<double> start_point = curve.evaluate(0.0);
    EXPECT_DOUBLE_EQ(start_point.x, control_points_2d[0].x);
    EXPECT_DOUBLE_EQ(start_point.y, control_points_2d[0].y);

    // At t=1, curve should be at last control point
    Vec2<double> end_point = curve.evaluate(1.0);
    EXPECT_DOUBLE_EQ(end_point.x, control_points_2d[2].x);
    EXPECT_DOUBLE_EQ(end_point.y, control_points_2d[2].y);
  }

  TEST_F(BezierCurveTest, EvaluateAtMidpoint)
  {
    BezierCurve2Dd curve(control_points_2d);

    // At t=0.5 for quadratic Bezier
    Vec2<double> mid_point = curve.evaluate(0.5);

    // For quadratic Bezier with points (0,0), (1,1), (2,0)
    // at t=0.5, point should be (1, 0.5)
    EXPECT_DOUBLE_EQ(mid_point.x, 1.0);
    EXPECT_DOUBLE_EQ(mid_point.y, 0.5);
  }

  TEST_F(BezierCurveTest, EvaluateDerivative)
  {
    BezierCurve2Dd curve(control_points_2d);

    // Test derivative at t=0
    Vec2<double> derivative_start = curve.evaluateDerivative(0.0);

    // For quadratic Bezier, derivative at t=0 is 2*(P1-P0)
    Vec2<double> expected_derivative =
        (control_points_2d[1] - control_points_2d[0]) * 2.0;
    EXPECT_DOUBLE_EQ(derivative_start.x, expected_derivative.x);
    EXPECT_DOUBLE_EQ(derivative_start.y, expected_derivative.y);
  }

  TEST_F(BezierCurveTest, EvaluateSecondDerivative)
  {
    BezierCurve2Dd curve(control_points_2d);

    // Test second derivative (should be constant for quadratic)
    Vec2<double> second_derivative = curve.evaluateSecondDerivative(0.0);
    Vec2<double> second_derivative_mid = curve.evaluateSecondDerivative(0.5);

    // Should be the same at all points for quadratic
    EXPECT_DOUBLE_EQ(second_derivative.x, second_derivative_mid.x);
    EXPECT_DOUBLE_EQ(second_derivative.y, second_derivative_mid.y);
  }

  TEST_F(BezierCurveTest, AddControlPoint)
  {
    BezierCurve2Dd curve(control_points_2d);
    Vec2<double> new_point(3.0, 2.0);

    curve.addControlPoint(new_point);

    EXPECT_EQ(curve.getNumControlPoints(), 4u);
    EXPECT_EQ(curve.getDegree(), 3u);
  }

  TEST_F(BezierCurveTest, InsertControlPoint)
  {
    BezierCurve2Dd curve(control_points_2d);
    Vec2<double> new_point(0.5, 0.5);

    curve.insertControlPoint(1, new_point);

    EXPECT_EQ(curve.getNumControlPoints(), 4u);
    EXPECT_DOUBLE_EQ(curve.getControlPoints()[1].x, 0.5);
    EXPECT_DOUBLE_EQ(curve.getControlPoints()[1].y, 0.5);
  }

  TEST_F(BezierCurveTest, RemoveControlPoint)
  {
    BezierCurve2Dd curve(control_points_2d);

    curve.removeControlPoint(1);

    EXPECT_EQ(curve.getNumControlPoints(), 2u);
    EXPECT_EQ(curve.getDegree(), 1u);
  }

  TEST_F(BezierCurveTest, ElevateDegree)
  {
    BezierCurve2Dd curve(control_points_2d);
    size_t original_degree = curve.getDegree();

    curve.elevateDegree();

    EXPECT_EQ(curve.getDegree(), original_degree + 1u);
    EXPECT_EQ(curve.getNumControlPoints(), original_degree + 2u);
  }

  TEST_F(BezierCurveTest, SplitCurve)
  {
    BezierCurve2Dd curve(control_points_2d);

    auto split_curves = curve.splitCurve(0.5);

    BezierCurve2Dd left_curve = split_curves.first;
    BezierCurve2Dd right_curve = split_curves.second;

    // Both curves should have same degree as original
    EXPECT_EQ(left_curve.getDegree(), curve.getDegree());
    EXPECT_EQ(right_curve.getDegree(), curve.getDegree());

    // Check continuity at split point
    Vec2<double> left_end = left_curve.evaluate(1.0);
    Vec2<double> right_start = right_curve.evaluate(0.0);

    EXPECT_NEAR(left_end.x, right_start.x, 1e-10);
    EXPECT_NEAR(left_end.y, right_start.y, 1e-10);
  }

  TEST_F(BezierCurveTest, 3DCurve)
  {
    BezierCurve3Dd curve(control_points_3d);

    // Test 3D curve evaluation
    Vec3<double> start_point = curve.evaluate(0.0);
    Vec3<double> end_point = curve.evaluate(1.0);

    EXPECT_DOUBLE_EQ(start_point.x, control_points_3d[0].x);
    EXPECT_DOUBLE_EQ(start_point.y, control_points_3d[0].y);
    EXPECT_DOUBLE_EQ(start_point.z, control_points_3d[0].z);

    EXPECT_DOUBLE_EQ(end_point.x, control_points_3d[2].x);
    EXPECT_DOUBLE_EQ(end_point.y, control_points_3d[2].y);
    EXPECT_DOUBLE_EQ(end_point.z, control_points_3d[2].z);
  }

  TEST_F(BezierCurveTest, ExceptionHandling)
  {
    BezierCurve2Dd empty_curve;

    // Should throw exception for empty curve
    EXPECT_THROW(empty_curve.evaluate(0.5), std::runtime_error);

    BezierCurve2Dd single_point_curve;
    single_point_curve.addControlPoint(Vec2<double>(1.0, 1.0));

    // Should throw exception for derivative with insufficient points
    EXPECT_THROW(single_point_curve.evaluateDerivative(0.5), std::runtime_error);

    // Should throw exception for second derivative with insufficient points
    EXPECT_THROW(single_point_curve.evaluateSecondDerivative(0.5),
                 std::runtime_error);
  }

  // B-SPLINE CURVE TESTS

  TEST_F(BSplineCurveTest, Constructor)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);

    EXPECT_EQ(curve.getControlPoints().size(), 4u);
    EXPECT_EQ(curve.getDegree(), 3u);
    EXPECT_EQ(curve.getKnotVector().size(), 8u);
  }

  TEST_F(BSplineCurveTest, EmptyConstructor)
  {
    BSplineCurve2Dd curve;
    EXPECT_EQ(curve.getDegree(), 3u); // Default degree
  }

  TEST_F(BSplineCurveTest, SettersAndGetters)
  {
    BSplineCurve2Dd curve;

    curve.setControlPoints(control_points_2d);
    curve.setKnotVector(knot_vector);
    curve.setDegree(3);

    EXPECT_EQ(curve.getControlPoints().size(), 4u);
    EXPECT_EQ(curve.getKnotVector().size(), 8u);
    EXPECT_EQ(curve.getDegree(), 3u);
  }

  TEST_F(BSplineCurveTest, EvaluateAtStartAndEnd)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);

    // For clamped B-spline, curve should pass through first and last control
    // points
    Vec2<double> start_point = curve.evaluate(0.0);
    Vec2<double> end_point = curve.evaluate(1.0);

    EXPECT_NEAR(start_point.x, control_points_2d[0].x, 1e-10);
    EXPECT_NEAR(start_point.y, control_points_2d[0].y, 1e-10);

    EXPECT_NEAR(end_point.x, control_points_2d[3].x, 1e-10);
    EXPECT_NEAR(end_point.y, control_points_2d[3].y, 1e-10);
  }

  TEST_F(BSplineCurveTest, FindKnotSpan)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);

    // Test knot span finding
    int span_start = curve.findKnotSpan(0.0);
    int span_mid = curve.findKnotSpan(0.5);
    int span_end = curve.findKnotSpan(1.0);

    EXPECT_EQ(span_start, 3u);
    EXPECT_EQ(span_mid, 3u);
    EXPECT_EQ(span_end, 3u);
  }

  TEST_F(BSplineCurveTest, ComputeBasisFunctions)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);

    int span = curve.findKnotSpan(0.5);
    std::vector<double> basis = curve.computeBasisFunctions(span, 0.5);

    // Basis functions should sum to 1
    double sum = 0.0;
    for (double b : basis)
    {
      sum += b;
    }
    EXPECT_NEAR(sum, 1.0, 1e-10);
  }

  TEST_F(BSplineCurveTest, GenerateClampedKnotVector)
  {
    BSplineCurve2Dd curve;
    curve.setControlPoints(control_points_2d);
    curve.setDegree(3);

    curve.generateClampedKnotVector();

    std::vector<double> knots = curve.getKnotVector();

    // Check that first and last knots are repeated degree+1 times
    EXPECT_EQ(knots.size(), 8u);
    for (int i = 0; i <= 3; ++i)
    {
      EXPECT_DOUBLE_EQ(knots[i], 0.0);
      EXPECT_DOUBLE_EQ(knots[7 - i], 1.0);
    }
  }

  TEST_F(BSplineCurveTest, GenerateUniformKnotVector)
  {
    BSplineCurve2Dd curve;
    curve.setControlPoints(control_points_2d);
    curve.setDegree(3);

    curve.generateUniformKnotVector();

    std::vector<double> knots = curve.getKnotVector();

    // Check that knots are uniformly distributed
    EXPECT_EQ(knots.size(), 8u);
    for (size_t i = 1; i < knots.size(); ++i)
    {
      EXPECT_GE(knots[i], knots[i - 1]);
    }
  }

  TEST_F(BSplineCurveTest, IsValidKnotVector)
  {
    BSplineCurve2Dd curve;

    // Valid knot vector
    std::vector<double> valid_knots = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    curve.setKnotVector(valid_knots);
    EXPECT_TRUE(curve.isValidKnotVector());

    // Invalid knot vector (decreasing)
    std::vector<double> invalid_knots = {0.0, 0.5, 0.3, 1.0};
    curve.setKnotVector(invalid_knots);
    EXPECT_FALSE(curve.isValidKnotVector());
  }

  TEST_F(BSplineCurveTest, IsValidConfiguration)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);
    EXPECT_TRUE(curve.isValidConfiguration());

    // Invalid configuration (wrong knot vector size)
    std::vector<double> wrong_knots = {0.0, 1.0};
    curve.setKnotVector(wrong_knots);
    EXPECT_FALSE(curve.isValidConfiguration());
  }

  TEST_F(BSplineCurveTest, EvaluateDerivative)
  {
    BSplineCurve2Dd curve(control_points_2d, knot_vector, 3);

    // Test first derivative evaluation
    Vec2<double> derivative = curve.evaluateDerivative(0.5, 1);

    // Derivative should be finite
    EXPECT_TRUE(std::isfinite(derivative.x));
    EXPECT_TRUE(std::isfinite(derivative.y));
  }

  TEST_F(BSplineCurveTest, 3DCurve)
  {
    BSplineCurve3Dd curve(control_points_3d, knot_vector, 3);

    Vec3<double> start_point = curve.evaluate(0.0);
    Vec3<double> end_point = curve.evaluate(1.0);

    EXPECT_NEAR(start_point.x, control_points_3d[0].x, 1e-10);
    EXPECT_NEAR(start_point.y, control_points_3d[0].y, 1e-10);
    EXPECT_NEAR(start_point.z, control_points_3d[0].z, 1e-10);

    EXPECT_NEAR(end_point.x, control_points_3d[3].x, 1e-10);
    EXPECT_NEAR(end_point.y, control_points_3d[3].y, 1e-10);
    EXPECT_NEAR(end_point.z, control_points_3d[3].z, 1e-10);
  }

  // QUINTIC POLYNOMIAL TESTS

  TEST_F(QuinticPolynomialTest, Constructor)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    EXPECT_DOUBLE_EQ(poly.getDuration(), duration);
  }

  TEST_F(QuinticPolynomialTest, EmptyConstructor)
  {
    QuinticPolynomial2Dd poly;
    EXPECT_DOUBLE_EQ(poly.getDuration(), 1.0);
  }

  TEST_F(QuinticPolynomialTest, BoundaryConditions)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    // Test start conditions
    Vec2<double> pos_start = poly.evaluate(0.0);
    Vec2<double> vel_start = poly.evaluateVelocity(0.0);
    Vec2<double> acc_start = poly.evaluateAcceleration(0.0);

    EXPECT_NEAR(pos_start.x, start_pos_2d.x, 1e-10);
    EXPECT_NEAR(pos_start.y, start_pos_2d.y, 1e-10);
    EXPECT_NEAR(vel_start.x, start_vel_2d.x, 1e-10);
    EXPECT_NEAR(vel_start.y, start_vel_2d.y, 1e-10);
    EXPECT_NEAR(acc_start.x, start_acc_2d.x, 1e-10);
    EXPECT_NEAR(acc_start.y, start_acc_2d.y, 1e-10);

    // Test end conditions
    Vec2<double> pos_end = poly.evaluate(duration);
    Vec2<double> vel_end = poly.evaluateVelocity(duration);
    Vec2<double> acc_end = poly.evaluateAcceleration(duration);

    EXPECT_NEAR(pos_end.x, end_pos_2d.x, 1e-10);
    EXPECT_NEAR(pos_end.y, end_pos_2d.y, 1e-10);
    EXPECT_NEAR(vel_end.x, end_vel_2d.x, 1e-10);
    EXPECT_NEAR(vel_end.y, end_vel_2d.y, 1e-10);
    EXPECT_NEAR(acc_end.x, end_acc_2d.x, 1e-10);
    EXPECT_NEAR(acc_end.y, end_acc_2d.y, 1e-10);
  }

  TEST_F(QuinticPolynomialTest, EvaluateAllDerivatives)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    double t = 1.0;

    Vec2<double> pos = poly.evaluate(t);
    Vec2<double> vel = poly.evaluateVelocity(t);
    Vec2<double> acc = poly.evaluateAcceleration(t);
    Vec2<double> jerk = poly.evaluateJerk(t);
    Vec2<double> snap = poly.evaluateSnap(t);
    Vec2<double> crackle = poly.evaluateCrackle(t);

    // All should be finite
    EXPECT_TRUE(std::isfinite(pos.x) && std::isfinite(pos.y));
    EXPECT_TRUE(std::isfinite(vel.x) && std::isfinite(vel.y));
    EXPECT_TRUE(std::isfinite(acc.x) && std::isfinite(acc.y));
    EXPECT_TRUE(std::isfinite(jerk.x) && std::isfinite(jerk.y));
    EXPECT_TRUE(std::isfinite(snap.x) && std::isfinite(snap.y));
    EXPECT_TRUE(std::isfinite(crackle.x) && std::isfinite(crackle.y));
  }

  TEST_F(QuinticPolynomialTest, EvaluateDerivativeGeneric)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    double t = 1.0;

    Vec2<double> pos = poly.evaluateDerivative(t, 0);
    Vec2<double> vel = poly.evaluateDerivative(t, 1);
    Vec2<double> acc = poly.evaluateDerivative(t, 2);
    Vec2<double> jerk = poly.evaluateDerivative(t, 3);
    Vec2<double> snap = poly.evaluateDerivative(t, 4);
    Vec2<double> crackle = poly.evaluateDerivative(t, 5);

    // Should match specific derivative functions
    Vec2<double> pos_specific = poly.evaluate(t);
    Vec2<double> vel_specific = poly.evaluateVelocity(t);
    Vec2<double> acc_specific = poly.evaluateAcceleration(t);
    Vec2<double> jerk_specific = poly.evaluateJerk(t);
    Vec2<double> snap_specific = poly.evaluateSnap(t);
    Vec2<double> crackle_specific = poly.evaluateCrackle(t);

    EXPECT_NEAR(pos.x, pos_specific.x, 1e-10);
    EXPECT_NEAR(pos.y, pos_specific.y, 1e-10);
    EXPECT_NEAR(vel.x, vel_specific.x, 1e-10);
    EXPECT_NEAR(vel.y, vel_specific.y, 1e-10);
    EXPECT_NEAR(acc.x, acc_specific.x, 1e-10);
    EXPECT_NEAR(acc.y, acc_specific.y, 1e-10);
    EXPECT_NEAR(jerk.x, jerk_specific.x, 1e-10);
    EXPECT_NEAR(jerk.y, jerk_specific.y, 1e-10);
    EXPECT_NEAR(snap.x, snap_specific.x, 1e-10);
    EXPECT_NEAR(snap.y, snap_specific.y, 1e-10);
    EXPECT_NEAR(crackle.x, crackle_specific.x, 1e-10);
    EXPECT_NEAR(crackle.y, crackle_specific.y, 1e-10);
  }

  TEST_F(QuinticPolynomialTest, HigherOrderDerivatives)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    // 6th and higher derivatives should be zero
    Vec2<double> sixth_derivative = poly.evaluateDerivative(1.0, 6);
    Vec2<double> seventh_derivative = poly.evaluateDerivative(1.0, 7);

    EXPECT_DOUBLE_EQ(sixth_derivative.x, 0.0);
    EXPECT_DOUBLE_EQ(sixth_derivative.y, 0.0);
    EXPECT_DOUBLE_EQ(seventh_derivative.x, 0.0);
    EXPECT_DOUBLE_EQ(seventh_derivative.y, 0.0);
  }

  TEST_F(QuinticPolynomialTest, TimeScaling)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    double scale_factor = 2.0;
    double original_duration = poly.getDuration();

    poly.scaleTime(scale_factor);

    EXPECT_DOUBLE_EQ(poly.getDuration(), original_duration * scale_factor);
  }

  TEST_F(QuinticPolynomialTest, AmplitudeScaling)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    double scale_factor = 2.0;
    Vec2<double> original_pos = poly.evaluate(1.0);

    poly.scaleAmplitude(scale_factor);

    Vec2<double> scaled_pos = poly.evaluate(1.0);
    EXPECT_NEAR(scaled_pos.x, original_pos.x * scale_factor, 1e-10);
    EXPECT_NEAR(scaled_pos.y, original_pos.y * scale_factor, 1e-10);
  }

  TEST_F(QuinticPolynomialTest, AmplitudeTranslation)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    Vec2<double> translation(5.0, 3.0);
    Vec2<double> original_pos = poly.evaluate(1.0);

    poly.translateAmplitude(translation);

    Vec2<double> translated_pos = poly.evaluate(1.0);
    EXPECT_NEAR(translated_pos.x, original_pos.x + translation.x, 1e-10);
    EXPECT_NEAR(translated_pos.y, original_pos.y + translation.y, 1e-10);
  }

  TEST_F(QuinticPolynomialTest, TimeClamping)
  {
    QuinticPolynomial2Dd poly(start_pos_2d, start_vel_2d, start_acc_2d,
                              end_pos_2d, end_vel_2d, end_acc_2d, duration);

    // Test that evaluation is clamped to [0, duration]
    Vec2<double> pos_negative = poly.evaluate(-1.0);
    Vec2<double> pos_zero = poly.evaluate(0.0);
    Vec2<double> pos_beyond = poly.evaluate(duration + 1.0);
    Vec2<double> pos_duration = poly.evaluate(duration);

    EXPECT_NEAR(pos_negative.x, pos_zero.x, 1e-10);
    EXPECT_NEAR(pos_negative.y, pos_zero.y, 1e-10);
    EXPECT_NEAR(pos_beyond.x, pos_duration.x, 1e-10);
    EXPECT_NEAR(pos_beyond.y, pos_duration.y, 1e-10);
  }

  // QUINTIC POLYNOMIAL 1D TESTS

  TEST(QuinticPolynomial1DTest, Constructor)
  {
    QuinticPolynomial1Dd poly(0.0, 1.0, 0.0, 10.0, 0.0, 0.0, 2.0);

    EXPECT_DOUBLE_EQ(poly.getDuration(), 2.0);
  }

  TEST(QuinticPolynomial1DTest, BoundaryConditions)
  {
    double start_pos = 0.0, start_vel = 1.0, start_acc = 0.0;
    double end_pos = 10.0, end_vel = 0.0, end_acc = 0.0;
    double duration = 2.0;

    QuinticPolynomial1Dd poly(start_pos, start_vel, start_acc, end_pos, end_vel,
                              end_acc, duration);

    // Test start conditions
    EXPECT_NEAR(poly.evaluate(0.0), start_pos, 1e-10);
    EXPECT_NEAR(poly.evaluateVelocity(0.0), start_vel, 1e-10);
    EXPECT_NEAR(poly.evaluateAcceleration(0.0), start_acc, 1e-10);

    // Test end conditions
    EXPECT_NEAR(poly.evaluate(duration), end_pos, 1e-10);
    EXPECT_NEAR(poly.evaluateVelocity(duration), end_vel, 1e-10);
    EXPECT_NEAR(poly.evaluateAcceleration(duration), end_acc, 1e-10);
  }

  TEST(QuinticPolynomial1DTest, AllDerivatives)
  {
    QuinticPolynomial1Dd poly(0.0, 1.0, 0.0, 10.0, 0.0, 0.0, 2.0);

    double t = 1.0;

    double pos = poly.evaluate(t);
    double vel = poly.evaluateVelocity(t);
    double acc = poly.evaluateAcceleration(t);
    double jerk = poly.evaluateJerk(t);
    double snap = poly.evaluateSnap(t);
    double crackle = poly.evaluateCrackle(t);

    // All should be finite
    EXPECT_TRUE(std::isfinite(pos));
    EXPECT_TRUE(std::isfinite(vel));
    EXPECT_TRUE(std::isfinite(acc));
    EXPECT_TRUE(std::isfinite(jerk));
    EXPECT_TRUE(std::isfinite(snap));
    EXPECT_TRUE(std::isfinite(crackle));
  }

  // INTEGRATION TESTS

  TEST(CurvesIntegrationTest, TypeAliases)
  {
    // Test that all type aliases are working
    BezierCurve2Dd bezier_2d;
    BezierCurve3Dd bezier_3d;
    BezierCurve2Df bezier_2f;
    BezierCurve3Df bezier_3f;

    BSplineCurve2Dd bspline_2d;
    BSplineCurve3Dd bspline_3d;
    BSplineCurve2Df bspline_2f;
    BSplineCurve3Df bspline_3f;

    QuinticPolynomial2Dd quintic_2d;
    QuinticPolynomial3Dd quintic_3d;
    QuinticPolynomial2Df quintic_2f;
    QuinticPolynomial3Df quintic_3f;

    QuinticPolynomial1Dd quintic_1d;
    QuinticPolynomial1Df quintic_1f;

    // Basic functionality test
    EXPECT_EQ(bezier_2d.getDegree(), 0u);
    EXPECT_EQ(bspline_2d.getDegree(), 3u);
    EXPECT_DOUBLE_EQ(quintic_2d.getDuration(), 1.0);
    EXPECT_DOUBLE_EQ(quintic_1d.getDuration(), 1.0);
  }

} // namespace lumos
