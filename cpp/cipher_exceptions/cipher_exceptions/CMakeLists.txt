cmake_minimum_required(VERSION 3.12)

# === Создаём статическую библиотеку исключений ===
add_library(cipher_exceptions_lib STATIC cipher_exceptions.cpp)

# === Пути для заголовков (.hpp) ===
target_include_directories(cipher_exceptions_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

