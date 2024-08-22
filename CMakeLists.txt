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
	target_compile_options(aoc PUBLIC -Wall -Wextra -pedantic)
endif()

# Executables

# Read template code
file(READ source/template.c TEMPLATE)

# Set this to the list of years needed
set(AOC_YEARS 2015 2016 2017 2018 2019 2020 2021 2022 2023)

foreach(YEAR ${AOC_YEARS})
	foreach(DAY RANGE 1 25)
		# Generate source code (year and day) from template
		string(CONFIGURE "${TEMPLATE}" SOURCE_CODE)

		# Make sure all days in file names have two digits
		string(LENGTH ${DAY} DAY_LENGTH)
		if(DAY_LENGTH EQUAL 1)
			string(PREPEND DAY 0)
		endif()

		# Generate source file if it does not exist
		set(SOURCE_FILENAME source/${YEAR}/${YEAR}-${DAY}.c)
		if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${SOURCE_FILENAME})
			file(WRITE ${SOURCE_FILENAME} "${SOURCE_CODE}")
		endif()

		# Add target
		set(EXECUTABLE_NAME aoc_${YEAR}_${DAY})
		add_executable(${EXECUTABLE_NAME} ${SOURCE_FILENAME})
		target_link_libraries(${EXECUTABLE_NAME} aoc)
	endforeach()
endforeach()
