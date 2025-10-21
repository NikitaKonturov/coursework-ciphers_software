cmake_minimum_required(VERSION 3.12)
project(ADFGVX_cipher LANGUAGES CXX)

# === Общие настройки компиляции ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# === Настройка Python и pybind11 ===
set(PYBIND11_FINDPYTHON ON)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# === Добавляем поддиректорию библиотеки шифра ===
add_subdirectory("cipher_first" "${CMAKE_BINARY_DIR}/cipher_first")

# === Создаём Python-модуль (.so / .pyd) ===
add_library(${PROJECT_NAME} MODULE main_cipher_file.cpp)

# === Линкуем зависимости ===
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pybind11::pybind11
        Python3::Python
        ADFGVX_cipher_lib
)

# === Куда сохранять собранную библиотеку ===
set(PYLIB_DIR "${CMAKE_SOURCE_DIR}/../pylib")  # единая папка cpp/pylib
file(MAKE_DIRECTORY "${PYLIB_DIR}")

# === Настройки выходного файла ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ADFGVX_cipher"             # имя модуля для Python
    LIBRARY_OUTPUT_DIRECTORY "${PYLIB_DIR}" # куда класть .so / .pyd
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PYLIB_DIR}"
)

# === Расширение в зависимости от платформы ===
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
endif()

# === Include пути (если есть заголовки) ===
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
