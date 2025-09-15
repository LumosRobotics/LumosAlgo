# Text Reader Module

# Create a test executable for the text reader
add_executable(text_reader_test test_example.cpp)

# Include directories for the test
target_include_directories(text_reader_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Set C++ standard
target_compile_features(text_reader_test PRIVATE cxx_std_17)

# Optional: Add the test to CTest if you want it to run with the test suite
# add_test(NAME TextReaderTest COMMAND text_reader_test)