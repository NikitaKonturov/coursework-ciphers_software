cmake_minimum_required(VERSION 3.12)
project(dynamicLib)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "Runtime Library" FORCE)
set(pybind11_DIR "C:\\Users\\BEPKUT\\AppData\\Local\\Programs\\Python\\Python312\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11")
# Чтобы получить путь устанавливаем через pip install pybind11
# И выполняем команду python pip -m pybind11 --cmakedir она вернёт нам путь к папке который мы и подставляем в set(pybind11_DIR <path>)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# путь заменить на свой 
set(Python3_LIBRARY "C:\\Users\\BEPKUT\\AppData\\Local\\Programs\\Python\\Python312\\libs\\python312.lib")
find_package(pybind11 REQUIRED)
# добавляем поддиреторию для поиска нашей ипортируемой библиотеки шифра
add_subdirectory("Homophone_cipher" "${CMAKE_BINARY_DIR}/Homophone_cipher")

# создаём библиотеку главное внимание обратите на MODULE именно этот параметр указывает га создагие динамической библиотеки
add_library(Homophone_cipher MODULE main_cipher_file.cpp)
target_link_libraries(Homophone_cipher PRIVATE ${Python3_LIBRARY})
# подключаем библиотеку для экспорта функции в python
target_link_libraries(Homophone_cipher PRIVATE pybind11::pybind11)

#link_directories("C:\\Users\\BEPKUT\\AppData\\Local\\Programs\\Python\\Python311\\libs")

#install(TARGETS Homophone_cipher DESTINATION "${CMAKE_SOURCE_DIR}/../py")
# подключаем библиотеку шифра
target_link_libraries(Homophone_cipher PUBLIC Homophone_cipher_lib)

# настройки выходного файла
set_target_properties(Homophone_cipher PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}\\..\\py"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}\\..\\..\\py\\include_ciphers"
    OUTPUT_NAME "Homophone_cipher" # имя должно совпадать с именем модуля указываемым в PYBIND11_MODULE
)

# обеспечиваем кросплатформенность динамичесих библиотек(может будет достатточно одного .pyd)
if(WIN32)
    set_target_properties(Homophone_cipher PROPERTIES SUFFIX ".pyd")
    set(PYTHON_DLL_PATH "C:\\Users\\BEPKUT\\AppData\\Local\\Programs\\Python\\Python312\\lib")
elseif(APPLE)
    set_target_properties(Homophone_cipher PROPERTIES SUFFIX "dylib")
else()
    set_target_properties(Homophone_cipher PROPERTIES SUFFIX ".so")
endif()
