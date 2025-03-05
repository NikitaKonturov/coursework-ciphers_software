add_library(viginer_key_gen viginer_key_gen.cpp)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../HMAC_DRBG/DRBG" "${CMAKE_CURRENT_BINARY_DIR}/DRBG")
target_link_libraries(viginer_key_gen PUBLIC DRBG_lib)