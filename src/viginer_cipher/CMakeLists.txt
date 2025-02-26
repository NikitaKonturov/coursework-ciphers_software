add_library(viginer_cipher viginer_cipher.cpp)

# Добавляем директорию для поиска файлов библиотеки nlohmann::json
target_include_directories(viginer_cipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
