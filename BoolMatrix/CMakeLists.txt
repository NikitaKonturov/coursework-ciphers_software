cmake_minimum_required(VERSION 3.10)

project(BoolMatrix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(BoolMatrix src/main.cpp src/BoolMatrix.cpp)
add_subdirectory("src/cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_exceptions")
target_link_libraries(BoolMatrix cipher_exceptions_lib)