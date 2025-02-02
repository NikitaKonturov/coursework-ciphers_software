cmake_minimum_required(VERSION 3.12)

project(permutation_lib)


add_library(permutation_lib STATIC src/Permutation.cpp)
target_include_directories(permutation_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
