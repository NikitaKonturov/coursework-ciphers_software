cmake_minimum_required(VERSION 3.12)

add_library(homophone_generator_lib STATIC homophone_generator.cpp)

target_include_directories(homophone_generator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
