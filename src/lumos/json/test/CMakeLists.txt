# Test executable for json module
add_executable(json_test json_test.cpp)

# Link with Google Test libraries
target_link_libraries(json_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(json_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME JSONTest COMMAND json_test)