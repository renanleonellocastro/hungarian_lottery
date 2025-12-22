cmake_minimum_required(VERSION 3.14)
project(hungarian_lottery VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_COVERAGE "Enable gcov/lcov coverage target" OFF)

# When coverage is requested, add compiler and linker flags (GCC/Clang)
if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Enable coverage flags for compiler ${CMAKE_CXX_COMPILER_ID}")
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
  else()
    message(WARNING "Coverage enabled but unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
endif()

find_package(Threads REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
