INCLUDE (CheckIncludeFiles)

# define project
cmake_minimum_required (VERSION 3.5)
project (jstrings VERSION 1.1 LANGUAGES CXX)

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
aux_source_directory(${PROJECT_SOURCE_DIR}/src CPPFILES)
add_executable(jstrings ${CPPFILES}) 

target_include_directories(jstrings PUBLIC "${PROJECT_SOURCE_DIR}/inc")
target_compile_features(jstrings PUBLIC cxx_std_11)
target_link_libraries(jstrings png)

install(TARGETS jstrings
        RUNTIME DESTINATION bin)
