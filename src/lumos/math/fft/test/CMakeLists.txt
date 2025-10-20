# Test executable for FFT module
add_executable(fft_test fft_test.cpp)

# Link with Google Test libraries
target_link_libraries(fft_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(fft_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Set C++ standard
target_compile_features(fft_test PRIVATE cxx_std_17)

# Add the test to CTest
add_test(NAME FFTTest COMMAND fft_test)