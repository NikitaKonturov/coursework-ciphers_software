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

# === Добавляем поддиректорию для внутренней библиотеки ===
add_subdirectory("tg_cutter" "${CMAKE_BINARY_DIR}/tg_cutter")

# === Создаём Python-модуль (.so / .pyd) ===
add_library(${PROJECT_NAME} MODULE main_cutter.cpp)

# === Линкуем зависимости ===
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pybind11::pybind11
        Python3::Python
        tg_cutter_lib
)

# === Единая директория вывода для всех .so ===
set(PYLIB_DIR "${CMAKE_SOURCE_DIR}/../pylib")
file(MAKE_DIRECTORY "${PYLIB_DIR}")

# === Настройки выходного файла ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "telegrams_cutter"          # имя Python-модуля
    LIBRARY_OUTPUT_DIRECTORY "${PYLIB_DIR}" # путь для .so / .pyd
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PYLIB_DIR}"
)

# === Кроссплатформенное расширение ===
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
endif()

# === Пути к include (если нужны) ===
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

