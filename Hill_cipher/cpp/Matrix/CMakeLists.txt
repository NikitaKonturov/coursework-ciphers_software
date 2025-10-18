cmake_minimum_required(VERSION 3.10)
add_library(Matrix_lib STATIC Matrix.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../HMAC_DRBG/DRBG" "${CMAKE_BINARY_DIR}/mtrxDRBG")

target_include_directories(Matrix_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Matrix_lib PUBLIC DRBG_lib)
