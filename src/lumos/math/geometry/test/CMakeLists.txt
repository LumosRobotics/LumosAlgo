# Test executable for geometry module
add_executable(geometry_test geometry_test.cpp)

# Link with Google Test libraries
target_link_libraries(geometry_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(geometry_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME GeometryTest COMMAND geometry_test)
