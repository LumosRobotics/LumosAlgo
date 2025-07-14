# Test executable for matrix_dynamic module
add_executable(matrix_dynamic_test matrix_dynamic_test.cpp)

# Link with Google Test libraries
target_link_libraries(matrix_dynamic_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(matrix_dynamic_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME MatrixDynamicTest COMMAND matrix_dynamic_test)
