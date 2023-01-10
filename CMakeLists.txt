cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 14)

project(lexer)

add_subdirectory(lib/lexy)

add_executable(lexer
        src/main.cpp)

target_link_libraries(lexer foonathan::lexy::core)
target_link_libraries(lexer foonathan::lexy::file)


add_custom_command(
        TARGET lexer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/input.data
        ${CMAKE_CURRENT_BINARY_DIR}/input.data
)


project(example)

add_executable(example
        src/example.cpp)

target_link_libraries(example foonathan::lexy::core)
target_link_libraries(example foonathan::lexy::file)



add_custom_command(
        TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/input.data
                ${CMAKE_CURRENT_BINARY_DIR}/input.data
)


