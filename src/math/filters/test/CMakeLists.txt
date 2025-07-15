# Test executable for filters module
add_executable(filters_test filters_test.cpp)

# Link with Google Test libraries
target_link_libraries(filters_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(filters_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME FiltersTest COMMAND filters_test)
