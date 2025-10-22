cmake_minimum_required(VERSION 3.12)
project(Telegrams_Cutter LANGUAGES CXX)

# === Общие настройки компиляции ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# === Настройка Python и pybind11 ===
set(PYBIND11_FINDPYTHON ON)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# === Подключаем внутреннюю библиотеку исключений ===
add_subdirectory("cipher_exceptions" "${CMAKE_BINARY_DIR}/cipher_exceptions")

# === Создаём Python-модуль (.so / .pyd) ===
add_library(${PROJECT_NAME} MODULE main_cpp_cipher.cpp)

# === Линкуем зависимости ===
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pybind11::pybind11
        Python3::Python
        cipher_exceptions_lib
)

# === Папка для всех .so ===
set(PYLIB_DIR "${CMAKE_SOURCE_DIR}/../pylib")
file(MAKE_DIRECTORY "${PYLIB_DIR}")

# === Настройки выходного файла ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "cpp_exceptions"  # имя модуля Python
    LIBRARY_OUTPUT_DIRECTORY "${PYLIB_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PYLIB_DIR}"
)

# === Кроссплатформенные расширения ===
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
endif()

# === Include-директории ===
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

