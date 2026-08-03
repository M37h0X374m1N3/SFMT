cmake_minimum_required(VERSION 4.0)

project(SFMT
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SFMT)

target_compile_definitions(SFMT PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_sources(SFMT PRIVATE 
    src/main.cpp
    src/test/_tokenizer_test.cpp
)
