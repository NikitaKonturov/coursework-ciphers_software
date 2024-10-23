cmake_minimum_required(VERSION 3.10)

project(project_name)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# target_include_directories(project_name PUBLIC{CMAKE_CURRENT_SOURCE_DIR})
add_executable(project_name src/main.cpp src/Permutation.cpp)
