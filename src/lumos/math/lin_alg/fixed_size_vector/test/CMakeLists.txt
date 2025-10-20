# Test executable for fixed_size_vector module
add_executable(fixed_size_vector_test fixed_size_vector_test.cpp)

# Link with Google Test libraries
target_link_libraries(fixed_size_vector_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(fixed_size_vector_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME FixedSizeVectorTest COMMAND fixed_size_vector_test)
