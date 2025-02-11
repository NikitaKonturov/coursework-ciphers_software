add_library(HMAC_lib STATIC HMAC.cpp)
target_include_directories(HMAC_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})