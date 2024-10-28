cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку HPC-cipher
add_library(HPC-cipher_lib HPC-cipher.cpp)

# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(HPC-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(HPC-cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
