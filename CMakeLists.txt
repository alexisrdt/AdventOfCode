cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(AOC C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Common library
add_library(aoc STATIC source/aoc.c)

target_include_directories(aoc PUBLIC include)

if(WIN32)
	target_compile_definitions(aoc PUBLIC UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)
	target_link_libraries(aoc PUBLIC Wininet)
endif()

if(MSVC)
	target_compile_options(aoc PUBLIC /W4 /utf-8)
else()
	target_compile_options(-Wall -Wextra -pedantic)
endif()

# Executables
add_executable(aoc_2015_01 source/2015-01.c)
target_link_libraries(aoc_2015_01 aoc)

add_executable(aoc_2015_02 source/2015-02.c)
target_link_libraries(aoc_2015_02 aoc)
