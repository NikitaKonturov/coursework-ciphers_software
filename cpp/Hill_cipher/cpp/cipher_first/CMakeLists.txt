cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку Hill-cipher
add_library(Hill-cipher_lib STATIC Hill-cipher.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_first/cipher_exceptions")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Matrix" "${CMAKE_BINARY_DIR}/Matrix")

# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(Hill-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(Hill-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(Hill-cipher_lib PUBLIC cipher_exceptions_lib)
target_link_libraries(Hill-cipher_lib PUBLIC Matrix_lib)