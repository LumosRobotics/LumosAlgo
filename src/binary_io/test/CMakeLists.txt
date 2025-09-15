# Test executable for binary_io module
add_executable(binary_io_test binary_io_test.cpp)

# Link with Google Test libraries
target_link_libraries(binary_io_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(binary_io_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Set C++ standard
target_compile_features(binary_io_test PRIVATE cxx_std_17)

# Add the test to CTest
add_test(NAME BinaryIOTest COMMAND binary_io_test)