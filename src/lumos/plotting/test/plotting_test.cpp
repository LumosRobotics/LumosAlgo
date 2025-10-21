#include <iostream>
#include <cmath>

#include "lumos/math.h"
#include "lumos/plotting/plot_functions.h"

using namespace lumos;

int main()
{
    std::cout << "=== Plotting Library Test ===\n\n";

    // Test 1: Create basic vectors
    std::cout << "Test 1: Creating basic vectors\n";
    Vector<double> x(10), y(10);

    for (size_t i = 0; i < 10; i++)
    {
        x(i) = static_cast<double>(i);
        y(i) = std::sin(static_cast<double>(i) * 0.5);
    }

    std::cout << "  Created x vector with " << x.size() << " elements\n";
    std::cout << "  Created y vector with " << y.size() << " elements\n";
    std::cout << "  First few points: ";
    for (size_t i = 0; i < 3; i++)
    {
        std::cout << "(" << x(i) << ", " << y(i) << ") ";
    }
    std::cout << "\n";

    // Test 2: Create 3D points
    std::cout << "\nTest 2: Creating 3D points\n";
    Vector<Point3<double>> points(5);

    for (size_t i = 0; i < 5; i++)
    {
        points(i) = Point3<double>(
            static_cast<double>(i),
            static_cast<double>(i * i),
            std::sin(static_cast<double>(i)));
    }

    std::cout << "  Created " << points.size() << " 3D points\n";
    std::cout << "  First point: (" << points(0).x << ", " << points(0).y << ", " << points(0).z << ")\n";
    std::cout << "  Last point: (" << points(4).x << ", " << points(4).y << ", " << points(4).z << ")\n";

    // Test 3: Create index triplets (for mesh drawing)
    std::cout << "\nTest 3: Creating index triplets\n";
    Vector<IndexTriplet> indices(2);
    indices(0) = IndexTriplet(0, 1, 2);
    indices(1) = IndexTriplet(2, 3, 4);

    std::cout << "  Created " << indices.size() << " triangles\n";
    std::cout << "  Triangle 0: (" << indices(0).i0 << ", " << indices(0).i1 << ", " << indices(0).i2 << ")\n";
    std::cout << "  Triangle 1: (" << indices(1).i0 << ", " << indices(1).i1 << ", " << indices(1).i2 << ")\n";

    // Test 4: Create a matrix
    std::cout << "\nTest 4: Creating a matrix\n";
    Matrix<double> mat(3, 3);

    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            mat(r, c) = static_cast<double>(r * 3 + c);
        }
    }

    std::cout << "  Created " << mat.numRows() << "x" << mat.numCols() << " matrix\n";
    std::cout << "  Matrix values:\n";
    for (size_t r = 0; r < 3; r++)
    {
        std::cout << "    ";
        for (size_t c = 0; c < 3; c++)
        {
            std::cout << mat(r, c) << " ";
        }
        std::cout << "\n";
    }

    // Test 5: Create an RGB image
    std::cout << "\nTest 5: Creating an RGB image\n";
    ImageRGB<uint8_t> img(100, 100);

    // Fill with a gradient
    for (size_t r = 0; r < 100; r++)
    {
        for (size_t c = 0; c < 100; c++)
        {
            img(r, c, 0) = static_cast<uint8_t>(r * 255 / 100); // Red channel
            img(r, c, 1) = static_cast<uint8_t>(c * 255 / 100); // Green channel
            img(r, c, 2) = 128;                                 // Blue channel
        }
    }

    std::cout << "  Created " << img.numRows() << "x" << img.numCols() << " RGB image\n";
    std::cout << "  Sample pixel (50,50): RGB("
              << static_cast<int>(img(50, 50, 0)) << ", "
              << static_cast<int>(img(50, 50, 1)) << ", "
              << static_cast<int>(img(50, 50, 2)) << ")\n";

    // Test 6: Create 2D points
    std::cout << "\nTest 6: Creating 2D points\n";
    Point2<float> p1(1.0f, 2.0f);
    Point2<float> p2(3.0f, 4.0f);

    std::cout << "  Point 1: (" << p1.x << ", " << p1.y << ")\n";
    std::cout << "  Point 2: (" << p2.x << ", " << p2.y << ")\n";

    // Test 7: Vector operations
    std::cout << "\nTest 7: Vector operations\n";
    Vector<double> v1(3);
    Vector<double> v2(3);

    v1(0) = 1.0;
    v1(1) = 2.0;
    v1(2) = 3.0;
    v2(0) = 4.0;
    v2(1) = 5.0;
    v2(2) = 6.0;

    Vector<double> v3 = v1 + v2;

    std::cout << "  v1 = [" << v1(0) << ", " << v1(1) << ", " << v1(2) << "]\n";
    std::cout << "  v2 = [" << v2(0) << ", " << v2(1) << ", " << v2(2) << "]\n";
    std::cout << "  v1 + v2 = [" << v3(0) << ", " << v3(1) << ", " << v3(2) << "]\n";

    std::cout << "\n=== All tests completed successfully! ===\n";

    return 0;
}
