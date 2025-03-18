cmake_minimum_required(VERSION 3.12)
project(YourProjectName)

# Добавляем поддиректории
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Permutation" "${CMAKE_BINARY_DIR}/cipher_first/Permutation")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_first/cipher_exceptions")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../permutation_generator" "${CMAKE_BINARY_DIR}/cipher_first/permutation_generator")

# SQLite3 Configuration
add_library(sqlite STATIC 
    ${CMAKE_SOURCE_DIR}/sqlite/sqlite3.c
)

target_include_directories(sqlite PUBLIC
    ${CMAKE_SOURCE_DIR}/sqlite
)

# Кроссплатформенные настройки компиляции
if(MSVC)
    # Настройки для Windows/Visual Studio
    target_compile_definitions(sqlite PRIVATE
        _CRT_SECURE_NO_WARNINGS
        SQLITE_WIN32_THREADS
    )
    target_compile_options(sqlite PRIVATE /MT)
else()
    # Настройки для Unix-подобных систем
    target_compile_definitions(sqlite PRIVATE
        SQLITE_THREADSAFE=1
        SQLITE_USE_UNIX_THREADS
    )
    find_package(Threads REQUIRED)
endif()

# Общие настройки для всех платформ
target_compile_definitions(sqlite PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_JSON1
    SQLITE_OMIT_DEPRECATED
)

# Основная библиотека проекта
add_library(viginer_cipher_lib STATIC viginer_cipher.cpp)

# Настройка путей для заголовков
target_include_directories(viginer_cipher_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sqlite
    ${CMAKE_SOURCE_DIR}/include
)

# Кроссплатформенная линковка
target_link_libraries(viginer_cipher_lib PUBLIC
    permutation_lib
    cipher_exceptions_lib
    permutation_generator_lib
    DRBG_lib
    sqlite
)

if(WIN32)
    # Windows-specific libraries
    target_link_libraries(viginer_cipher_lib PRIVATE
        advapi32
        user32
        kernel32
    )
else()
    # Unix-specific libraries
    target_link_libraries(viginer_cipher_lib PRIVATE
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
endif()

# Для Python расширений
if(WIN32 AND CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    target_link_libraries(viginer_cipher_lib PRIVATE WindowsApp.lib)
endif()