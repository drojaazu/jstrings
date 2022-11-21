include(CheckIncludeFiles)

# define project
cmake_minimum_required(VERSION 3.5)

project(jstrings VERSION 1.6 LANGUAGES CXX)

set(PROJECT_CONTACT "Damian R (damian@motoi.pro)")
set(PROJECT_WEBSITE "https://github.com/drojaazu")
set(PROJECT_COPYRIGHT "©2018 Motoi Productions / Released under MIT License")
set(PROJECT_BRIEF "A tool for finding JIS-based Japanese text in binary data.")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/src/app.hpp" ESCAPE_QUOTES)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

# define target
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" SRCFILES)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(${PROJECT_NAME} ${SRCFILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_library(ICONV_LIB iconv)

if(NOT ICONV_LIB)
  message(FATAL_ERROR "libiconv not found")
endif()

target_link_libraries(${PROJECT_NAME} iconv)

install(TARGETS jstrings
  RUNTIME DESTINATION bin)
