cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку HPC-cipher
add_library(cipher_exceptions_lib STATIC cipher_exceptions.cpp)

# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(cipher_exceptions_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
