cmake_minimum_required(VERSION 3.10)
project(viginer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_executable(viginer main.cpp)
add_subdirectory(viginer_key_gen)
target_link_libraries(viginer PUBLIC viginer_key_gen)
target_include_directories(viginer PUBLIC
"${PROJECT_BINARY_DIR}"
"${PROJECT_SOURCE_DIR}/viginer_key_gen"
)