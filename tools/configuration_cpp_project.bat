@echo off

rem Проверка и создание директории build
if not exist "build" (
    mkdir build
)

rem Проверка и создание директории src
if not exist "src" (
    mkdir src
)

rem Создание CMakeLists.txt в корневой директории
(
    echo cmake_minimum_required^(VERSION 3.10^)
    echo project^(project_name^)
    echo set^(CMAKE_CXX_STANDARD 20^)
    echo set^(CMAKE_CXX_STANDARD_REQUIRED TRUE^)
    echo add_executable^(project_name src/main.cpp^)
) > CMakeLists.txt

rem Переход в директорию src и создание main.cpp
cd src
(
    echo #include ^<iostream^>
    echo.
    echo int main^(int args, char** argv^) {
    echo.
    echo.
    echo return 0;
    echo }
) > main.cpp
cd ..

rem Переход в директорию build и генерация проекта с использованием CMake
cd build
cmake ..
pause
