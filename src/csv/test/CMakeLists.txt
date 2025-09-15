# Test executable for csv module
add_executable(csv_test csv_test.cpp)

# Link with Google Test libraries
target_link_libraries(csv_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(csv_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME CSVTest COMMAND csv_test)