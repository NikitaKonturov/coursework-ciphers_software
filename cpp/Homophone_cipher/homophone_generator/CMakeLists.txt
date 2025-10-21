cmake_minimum_required(VERSION 3.12)

add_library(homophone_generator_lib STATIC homophone_generator.cpp)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../HMAC_DRBG/DRBG" "${CMAKE_CURRENT_BINARY_DIR}/DRBG")

target_include_directories(homophone_generator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(homophone_generator_lib PUBLIC DRBG_lib)