# CMakeLists.txt for argparse tests

# Set target name
set(TARGET_NAME argparse_test)

# Create executable for argparse tests
add_executable(${TARGET_NAME} argparse_gtest.cpp)

# Link GoogleTest libraries
target_link_libraries(${TARGET_NAME} ${GTEST_LIB_FILES})

# Add include directories (argparse header is in parent directory)
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include
)

# Add to test suite
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

# Set test properties
set_tests_properties(${TARGET_NAME} PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Optional: Set compiler-specific options
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add custom target for running just argparse tests
add_custom_target(run_argparse_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} -R ${TARGET_NAME} --verbose
    DEPENDS ${TARGET_NAME}
    COMMENT "Running argparse tests"
)