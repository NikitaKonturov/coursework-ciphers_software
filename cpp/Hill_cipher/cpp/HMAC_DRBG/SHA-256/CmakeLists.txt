add_library(SHA-256_lib STATIC SHA-256.cpp)

target_include_directories(SHA-256_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
