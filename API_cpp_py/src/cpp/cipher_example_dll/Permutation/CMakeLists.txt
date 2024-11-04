cmake_minimum_required(VERSION 3.10)

project(permutation_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# target_include_directories(project_name PUBLIC{CMAKE_CURRENT_SOURCE_DIR})
add_library(permutation_lib src/Permutation.cpp)
target_include_directories(permutation_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)