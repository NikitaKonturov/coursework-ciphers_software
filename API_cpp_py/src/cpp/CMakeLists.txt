cmake_minimum_required(VERSION 3.10)
project(MyDLLProject)

set(CMAKE_CXX_STANDARD 11)  # Установите стандарт C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Создание динамической библиотеки (DLL)
add_library(ciphers2_lib SHARED ciphers1.cpp)

# Установка заголовочных файлов
target_include_directories(ciphers2_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}") 

if(WIN32) 
    set_target_properties(ciphers2_lib PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(ciphers2_lib PROPERTIES SUFFIX ".dylib")
else()   
    set_target_properties(ciphers2_lib PROPERTIES SUFFIX ".so")
endif()