cmake_minimum_required(VERSION 3.12)


add_library(permutation_generator_lib STATIC permutation_generator.cpp)

target_include_directories(permutation_generator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
