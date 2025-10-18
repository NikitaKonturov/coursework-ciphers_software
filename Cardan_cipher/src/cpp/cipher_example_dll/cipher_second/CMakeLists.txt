cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку Cardan-cipher
add_library(Cardan-cipher_lib STATIC Cardan-cipher.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_second/cipher_exceptions")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../boolmatrix_generator" "${CMAKE_BINARY_DIR}/cipher_second/boolmatrix_generator")


# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(Cardan-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(Cardan-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(Cardan-cipher_lib PUBLIC cipher_exceptions_lib)
target_link_libraries(Cardan-cipher_lib PUBLIC boolmatrix_generator_lib)
