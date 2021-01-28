include(CheckIncludeFiles)
include(GNUInstallDirs)

# define project
cmake_minimum_required (VERSION 3.5)
project (jstrings VERSION 1.1 LANGUAGES CXX)
set(PROJECT_CONTACT "Damian R (damian@sudden-desu.net)")
set(PROJECT_WEBSITE "https://github.com/drojaazu")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/project.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/src/project.hpp")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

# define target
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" SRCFILES)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(${PROJECT_NAME} ${SRCFILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

install(TARGETS jstrings
        RUNTIME DESTINATION bin)
