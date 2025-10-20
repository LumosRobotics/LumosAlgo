# Test executable for curves module
add_executable(curves_test curves_test.cpp)

# Link with Google Test libraries
target_link_libraries(curves_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(curves_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME CurvesTest COMMAND curves_test)
