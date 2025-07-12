// #include <duoplot/duoplot.h>

// #include <glm/glm.hpp>
#include <iostream>
#include <vector>
// #include <nlohmann/json.hpp>

#include "math/math.h"

using namespace lumos;

int main() {
  std::cout << "LumosAlgo Curves Demo" << std::endl;

  // Example 1: Bezier Curve
  std::cout << "\n=== Bezier Curve Example ===" << std::endl;
  std::vector<Vec2d> control_points = {Vec2d(0.0, 0.0), Vec2d(1.0, 2.0),
                                       Vec2d(2.0, 1.0), Vec2d(3.0, 0.0)};

  BezierCurve2Dd bezier(control_points);
  std::cout << "Bezier curve with " << bezier.getNumControlPoints()
            << " control points" << std::endl;
  std::cout << "Degree: " << bezier.getDegree() << std::endl;

  // Evaluate at different parameter values
  for (double t = 0.0; t <= 1.0; t += 0.2) {
    Vec2d point = bezier.evaluate(t);
    std::cout << "t=" << t << ": (" << point.x << ", " << point.y << ")"
              << std::endl;
  }

  // Example 2: B-Spline Curve
  std::cout << "\n=== B-Spline Curve Example ===" << std::endl;
  BSplineCurve2Dd bspline(control_points, {}, 3);
  bspline.generateClampedKnotVector();

  std::cout << "B-spline curve with " << bspline.getNumControlPoints()
            << " control points" << std::endl;
  std::cout << "Degree: " << bspline.getDegree() << std::endl;
  std::cout << "Knot vector size: " << bspline.getNumKnots() << std::endl;

  // Evaluate at different parameter values
  for (double t = 0.0; t <= 1.0; t += 0.2) {
    Vec2d point = bspline.evaluate(t);
    std::cout << "t=" << t << ": (" << point.x << ", " << point.y << ")"
              << std::endl;
  }

  // Example 3: Quintic Polynomial
  std::cout << "\n=== Quintic Polynomial Example ===" << std::endl;
  Vec2d start_pos(0.0, 0.0);
  Vec2d start_vel(1.0, 0.0);
  Vec2d start_acc(0.0, 0.0);
  Vec2d end_pos(5.0, 2.0);
  Vec2d end_vel(0.0, 1.0);
  Vec2d end_acc(0.0, 0.0);
  double duration = 2.0;

  QuinticPolynomial2Dd quintic(start_pos, start_vel, start_acc, end_pos,
                               end_vel, end_acc, duration);

  std::cout << "Quintic polynomial with duration: " << quintic.getDuration()
            << std::endl;

  // Evaluate position, velocity, and acceleration at different times
  for (double t = 0.0; t <= duration; t += 0.4) {
    Vec2d pos = quintic.evaluate(t);
    Vec2d vel = quintic.evaluateVelocity(t);
    Vec2d acc = quintic.evaluateAcceleration(t);

    std::cout << "t=" << t << " pos:(" << pos.x << ", " << pos.y << ")"
              << " vel:(" << vel.x << ", " << vel.y << ")"
              << " acc:(" << acc.x << ", " << acc.y << ")" << std::endl;
  }

  // Example 4: Scalar Quintic Polynomial
  std::cout << "\n=== Scalar Quintic Polynomial Example ===" << std::endl;
  QuinticPolynomial1Dd scalar_quintic(0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 1.0);

  for (double t = 0.0; t <= 1.0; t += 0.2) {
    double pos = scalar_quintic.evaluate(t);
    double vel = scalar_quintic.evaluateVelocity(t);
    double acc = scalar_quintic.evaluateAcceleration(t);

    std::cout << "t=" << t << " pos:" << pos << " vel:" << vel << " acc:" << acc
              << std::endl;
  }

  // Example 5: FIR Filter
  std::cout << "\n=== FIR Filter Example ===" << std::endl;

  // Create a simple moving average filter
  FIRFilterd ma_filter = FIRFilterd::movingAverage(5);
  std::cout << "Moving average filter with window size 5" << std::endl;

  // Test with noisy signal
  std::vector<double> noisy_signal = {1.0, 2.0, 1.5, 3.0, 2.5,
                                      4.0, 3.5, 5.0, 4.5, 6.0};
  std::vector<double> filtered_signal = ma_filter.filter(noisy_signal);

  std::cout << "Input:  ";
  for (double val : noisy_signal) {
    std::cout << val << " ";
  }
  std::cout << std::endl;

  std::cout << "Output: ";
  for (double val : filtered_signal) {
    std::cout << val << " ";
  }
  std::cout << std::endl;

  // Example 6: IIR Filter
  std::cout << "\n=== IIR Filter Example ===" << std::endl;

  // Create a first-order low-pass filter
  IIRFilterd lpf = IIRFilterd::firstOrderLowPass(
      100.0, 1000.0); // 100Hz cutoff, 1kHz sampling
  std::cout << "First-order low-pass filter (100Hz cutoff, 1kHz sampling)"
            << std::endl;

  // Test with the same noisy signal
  std::vector<double> iir_filtered = lpf.filter(noisy_signal);

  std::cout << "Input:  ";
  for (double val : noisy_signal) {
    std::cout << val << " ";
  }
  std::cout << std::endl;

  std::cout << "Output: ";
  for (double val : iir_filtered) {
    std::cout << val << " ";
  }
  std::cout << std::endl;

  // Example 7: Custom FIR Filter
  std::cout << "\n=== Custom FIR Filter Example ===" << std::endl;
  FIRFilterd custom_fir({0.1, 0.2, 0.4, 0.2, 0.1});
  custom_fir.printCoefficients();

  double single_output = custom_fir.filter(2.0);
  std::cout << "Single sample output: " << single_output << std::endl;

  // Example 8: Frequency Response Analysis
  std::cout << "\n=== Frequency Response Analysis ===" << std::endl;

  // Test frequency response of low-pass filter
  IIRFilterd test_lpf = IIRFilterd::firstOrderLowPass(100.0, 1000.0);
  std::vector<double> frequencies = {10.0, 50.0, 100.0, 200.0, 500.0};

  std::cout << "First-order LPF frequency response:" << std::endl;
  for (double freq : frequencies) {
    std::complex<double> response = test_lpf.frequencyResponse(freq, 1000.0);
    double magnitude = std::abs(response);
    double phase = std::arg(response) * 180.0 / M_PI;
    std::cout << "f=" << freq << "Hz: |H|=" << magnitude << ", ∠H=" << phase
              << "°" << std::endl;
  }

  return 0;
}
