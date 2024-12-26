cmake_minimum_required(VERSION 3.12)

add_library(tg_cutter_lib STATIC telgrams_cutter.cpp)

target_include_directories(tg_cutter_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
