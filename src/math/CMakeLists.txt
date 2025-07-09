cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CPP_LIB_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/transformations/quaternion.cpp)

add_library(math ${CPP_LIB_SOURCE_FILES})
