cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку sADFGVX_cipher
add_library(ADFGVX_cipher_lib STATIC ADFGVX_cipher.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Permutation" "${CMAKE_BINARY_DIR}/cipher_first/Permutation")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_first/cipher_exceptions")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../permutation_generator" "${CMAKE_BINARY_DIR}/cipher_first/permutation_generator")


# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(ADFGVX_cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(ADFGVX_cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(ADFGVX_cipher_lib PUBLIC permutation_lib)
target_link_libraries(ADFGVX_cipher_lib PUBLIC cipher_exceptions_lib)
target_link_libraries(ADFGVX_cipher_lib PUBLIC permutation_generator_lib)
