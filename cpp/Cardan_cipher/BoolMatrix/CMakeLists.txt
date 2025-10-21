cmake_minimum_required(VERSION 3.10)

project(BoolMatrix_lib)

# target_include_directories(project_name PUBLIC{CMAKE_CURRENT_SOURCE_DIR})
add_library(BoolMatrix_lib STATIC src/BoolMatrix.cpp)
target_include_directories(BoolMatrix_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
