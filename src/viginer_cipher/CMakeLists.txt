add_library(viginer_cipher viginer_cipher.cpp)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/viginer_cipher/cipher_exceptions")
target_link_libraries(viginer_cipher PUBLIC cipher_exceptions_lib)
target_include_directories(viginer_cipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)