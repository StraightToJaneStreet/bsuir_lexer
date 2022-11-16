cmake_minimum_required(VERSION 3.23)
project(lexer)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/lexy)

add_executable(lexer
        src/main.cpp)

target_link_libraries(lexer foonathan::lexy::core)
