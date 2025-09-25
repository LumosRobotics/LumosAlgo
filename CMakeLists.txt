# Minimum CMake version required
cmake_minimum_required(VERSION 3.14)

# Project name and languages
project(PhysicsSim C CXX)

# Set the C++ standard (adjust according to your needs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally enable debugging support
set(CMAKE_BUILD_TYPE Debug)

# Optionally define a version
set(PROJECT_VERSION "1.0.0")

# OpenCV
# set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/src/external/opencv/build/" CACHE PATH "Path to local OpenCVConfig.cmake")
# find_package(OpenCV REQUIRED)
# message(STATUS "OpenCV version: ${OpenCV_VERSION}")
# message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")

# Googletest
link_directories(${CMAKE_SOURCE_DIR}/src/external/googletest/lib)
include_directories(${CMAKE_SOURCE_DIR}/src/external/googletest/googletest/include)

set(GTEST_LIB_FILES libgtest.a
                    libgtest_main.a
                    pthread)

# Nlohmann
include_directories(${CMAKE_SOURCE_DIR}/external/nlohmann/include)

add_compile_options(-Wall -Wextra -pedantic)

# STB
set(STB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/external/stb)

# Include
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${STB_SOURCE_DIR})

# Enable testing
enable_testing()

add_subdirectory(src/applications/simple)
add_subdirectory(src/applications/image)
add_subdirectory(src/math/curves/test)
add_subdirectory(src/math/lin_alg/fixed_size_vector/test)
add_subdirectory(src/math/lin_alg/matrix_fixed/test)
add_subdirectory(src/math/lin_alg/matrix_dynamic/test)
add_subdirectory(src/math/filters/test)
add_subdirectory(src/math/geometry/test)
add_subdirectory(src/math/fft/test)
add_subdirectory(src/test_reader)
add_subdirectory(src/binary_io/test)
add_subdirectory(src/json/test)
add_subdirectory(src/argparse/test)
