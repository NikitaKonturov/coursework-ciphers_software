cmake_minimum_required(VERSION 3.10)

project(permutation_lib)

# target_include_directories(project_name PUBLIC{CMAKE_CURRENT_SOURCE_DIR})
add_library(permutation_lib STATIC src/Permutation.cpp)
target_include_directories(permutation_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)