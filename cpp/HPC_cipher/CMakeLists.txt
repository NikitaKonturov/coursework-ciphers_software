cmake_minimum_required(VERSION 3.12)
project(HPC_cipher LANGUAGES CXX)

# === Основные настройки ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# === Python и pybind11 ===
set(PYBIND11_FINDPYTHON ON)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# === Поддиректория с кодом шифра ===
add_subdirectory("cipher_first" "${CMAKE_BINARY_DIR}/cipher_first")

# === Создание Python-модуля (.so/.pyd) ===
add_library(${PROJECT_NAME} MODULE main_cipher_file.cpp)

# === Линковка ===
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pybind11::pybind11
        Python3::Python
        HPC-cipher_lib
)

# === Общая директория для .so ===
set(PYLIB_DIR "${CMAKE_SOURCE_DIR}/../pylib")
file(MAKE_DIRECTORY "${PYLIB_DIR}")

# === Настройки выходного файла ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "HPC_cipher"
    LIBRARY_OUTPUT_DIRECTORY "${PYLIB_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PYLIB_DIR}"
)

# === Расширения под разные ОС ===
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
endif()

# === Пути для include ===
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
