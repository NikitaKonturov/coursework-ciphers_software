add_library(DRBG_lib STATIC DRBG.cpp)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../HMAC" "${CMAKE_BINARY_DIR}/DRBG/HMAC")


target_include_directories(DRBG_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(DRBG_lib PUBLIC HMAC_lib)




