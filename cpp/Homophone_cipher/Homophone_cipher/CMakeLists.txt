cmake_minimum_required(VERSION 3.12)

# Создаём статическую библиотеку Homophone_cipher
add_library(Homophone_cipher_lib STATIC Homophone_cipher.cpp)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/Homophone_cipher/cipher_exceptions")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../homophone_generator" "${CMAKE_BINARY_DIR}/Homophone_cipher/Homophone_generator")


# Добавляем директорию для поиска .hpp файлов основного проекта
target_include_directories(Homophone_cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(Homophone_cipher_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(Homophone_cipher_lib PUBLIC cipher_exceptions_lib)
target_link_libraries(Homophone_cipher_lib PUBLIC homophone_generator_lib)
