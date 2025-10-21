add_library(HMAC_lib STATIC HMAC.cpp)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../SHA-256" "${CMAKE_BINARY_DIR}/HMAC/SHA-256")


target_include_directories(HMAC_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(HMAC_lib PUBLIC SHA-256_lib)
