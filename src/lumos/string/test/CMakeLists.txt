# Test executable for string module
add_executable(string_test string_test.cpp)

# Link with Google Test libraries
target_link_libraries(string_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(string_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME StringTest COMMAND string_test)