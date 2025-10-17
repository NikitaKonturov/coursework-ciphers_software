cmake_minimum_required(VERSION 3.12)


add_library(boolmatrix_generator_lib STATIC boolmatrix_generator.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../BoolMatrix" "${CMAKE_BINARY_DIR}/boolmatrix_generator/BoolMatrix")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../HMAC_DRBG/DRBG" "${CMAKE_CURRENT_BINARY_DIR}/DRBG")

target_link_libraries(boolmatrix_generator_lib PUBLIC BoolMatrix_lib)
target_include_directories(boolmatrix_generator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(boolmatrix_generator_lib PUBLIC DRBG_lib)