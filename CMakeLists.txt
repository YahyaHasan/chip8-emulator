cmake_minimum_required(VERSION 3.14)
project(chip8 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(chip8_lib src/chip8.cpp)
target_include_directories(chip8_lib PUBLIC src)

add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 PRIVATE chip8_lib)

enable_testing()
add_subdirectory(tests)