cmake_minimum_required(VERSION 3.12)
project(viginer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "Runtime Library" FORCE)
#set(pybind11_DIR "C:\\Users\\Lenovo IdeaPad5\\AppData\\Local\\Programs\\Python\\Python312\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11")
#find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
#set(Python3_LIBRARY "C:\\Users\\Lenovo IdeaPad5\\AppData\\Local\\Programs\\Python\\Python312\\libs\\python312.lib")
#find_package(pybind11 REQUIRED)
add_executable(viginer main.cpp)
add_subdirectory(viginer_key_gen)
add_subdirectory(viginer_cipher)
target_link_libraries(viginer PUBLIC viginer_key_gen)
target_link_libraries(viginer PUBLIC viginer_cipher)
target_include_directories(viginer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(viginer PUBLIC
"${PROJECT_BINARY_DIR}"
"${PROJECT_SOURCE_DIR}/viginer_key_gen"
"${PROJECT_SOURCE_DIR}/viginer_cipher"
)
#add_library(jb_cipher MODULE main_cipher_file.cpp)
#target_link_libraries(jb_cipher PRIVATE ${Python3_LIBRARY})
#set_target_properties(viginer PROPERTIES
#    PREFIX ""
#    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}\\..\\py"
#    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}\\..\\..\\py\\include_ciphers"
#    OUTPUT_NAME "jb_cipher" # имя должно совпадать с именем модуля указываемым в PYBIND11_MODULE
#)
#target_link_libraries(jb_cipher PRIVATE pybind11::pybind11)

# обеспечиваем кросплатформенность динамичесих библиотек(может будет достатточно одного .pyd)
#if(WIN32)
#    set_target_properties(jb_cipher PROPERTIES SUFFIX ".pyd")
#    set(PYTHON_DLL_PATH "C:\\Users\\Lenovo IdeaPad5\\AppData\\Local\\Programs\\Python\\Python312\\python312.dll")
    # Копируем python311.dll в папку сборки библиотеки
#    add_custom_command(TARGET jb_cipher POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${PYTHON_DLL_PATH}"
#        "$<TARGET_FILE_DIR:jb_cipher>"
#    )
#elseif(APPLE)
#    set_target_properties(jb_cipher PROPERTIES SUFFIX "dylib")
#else()
#    set_target_properties(jb_cipher PROPERTIES SUFFIX ".so")
#endif()


# Добавляем команду install для HPC_cipher, чтобы сохранить библиотеку в целевой папке
#install(TARGETS  DESTINATION "${CMAKE_SOURCE_DIR}/../py")

# Добавляем команду install для python311.dll, чтобы она также была установлена
#install(FILES "${PYTHON_DLL_PATH}" DESTINATION "${CMAKE_SOURCE_DIR}/../py")
