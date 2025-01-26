cmake_minimum_required(VERSION 3.30)
project(dynamicLib01)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Пути к pybind11 и Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Поддиректория с библиотекой шифра
add_subdirectory("cipher_first" "${CMAKE_BINARY_DIR}/cipher_first")

# Создаём библиотеку
add_library(Simple_substitution_cipher SHARED main_cipher_file.cpp)
target_link_libraries(Simple_substitution_cipher PRIVATE ${Python3_LIBRARY} pybind11::pybind11)
target_link_libraries(Simple_substitution_cipher PUBLIC simple_substitution_cipher_lib)

# Настройки выходного файла
set_target_properties(Simple_substitution_cipher PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../py"
    OUTPUT_NAME "Simple_substitution_cipher" # Имя модуля для PYBIND11_MODULE
)

# Обеспечиваем кроссплатформенность
if(WIN32)
    set_target_properties(Simple_substitution_cipher PROPERTIES SUFFIX ".pyd")
    set(PYTHON_DLL_PATH "C:/Users/Lenovo IdeaPad5/AppData/Local/Programs/Python/Python312/python312.dll")
    add_custom_command(TARGET Simple_substitution_cipher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PYTHON_DLL_PATH}"
        "$<TARGET_FILE_DIR:Simple_substitution_cipher>"
    )
elseif(APPLE)
    set_target_properties(Simple_substitution_cipher PROPERTIES SUFFIX "dylib")
else()
    set_target_properties(Simple_substitution_cipher PROPERTIES SUFFIX ".so")
endif()

# Команда install
install(TARGETS Simple_substitution_cipher DESTINATION "${CMAKE_SOURCE_DIR}/../py")
install(FILES "${PYTHON_DLL_PATH}" DESTINATION "${CMAKE_SOURCE_DIR}/../py")
