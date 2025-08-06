#include <iostream>
#include <cmath>
#include "src/math/transformations/axis_angle.h"
#include "src/math/transformations/euler_angles.h"

using namespace lumos;

int main()
{
    // Test conversion from Euler angles to axis-angle and back
    EulerAngles<double> original_euler(0.5, 0.3, 0.7); // roll, pitch, yaw in radians

    std::cout << "Original Euler angles:" << std::endl;
    std::cout << "Roll: " << original_euler.roll << " rad" << std::endl;
    std::cout << "Pitch: " << original_euler.pitch << " rad" << std::endl;
    std::cout << "Yaw: " << original_euler.yaw << " rad" << std::endl;

    // Convert to axis-angle
    AxisAngle<double> axis_angle = AxisAngle<double>::fromEulerAngles(original_euler);

    std::cout << "\nAxis-Angle representation:" << std::endl;
    std::cout << "Angle: " << axis_angle.phi << " rad" << std::endl;
    std::cout << "Axis: (" << axis_angle.x << ", " << axis_angle.y << ", " << axis_angle.z << ")" << std::endl;

    // Convert back to Euler angles
    EulerAngles<double> converted_euler = axis_angle.toEulerAngles();

    std::cout << "\nConverted back to Euler angles:" << std::endl;
    std::cout << "Roll: " << converted_euler.roll << " rad" << std::endl;
    std::cout << "Pitch: " << converted_euler.pitch << " rad" << std::endl;
    std::cout << "Yaw: " << converted_euler.yaw << " rad" << std::endl;

    // Check if the conversion is approximately correct
    const double tolerance = 1e-10;
    bool roll_ok = std::abs(original_euler.roll - converted_euler.roll) < tolerance;
    bool pitch_ok = std::abs(original_euler.pitch - converted_euler.pitch) < tolerance;
    bool yaw_ok = std::abs(original_euler.yaw - converted_euler.yaw) < tolerance;

    std::cout << "\nConversion accuracy:" << std::endl;
    std::cout << "Roll difference: " << std::abs(original_euler.roll - converted_euler.roll) << std::endl;
    std::cout << "Pitch difference: " << std::abs(original_euler.pitch - converted_euler.pitch) << std::endl;
    std::cout << "Yaw difference: " << std::abs(original_euler.yaw - converted_euler.yaw) << std::endl;

    if (roll_ok && pitch_ok && yaw_ok)
    {
        std::cout << "\n✓ Test PASSED: Conversion is accurate within tolerance!" << std::endl;
    }
    else
    {
        std::cout << "\n✗ Test FAILED: Conversion exceeded tolerance!" << std::endl;
    }

    return 0;
}
