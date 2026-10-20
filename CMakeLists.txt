cmake_minimum_required(VERSION 3.16)
project(hungarian_lottery
  VERSION 1.1.0
  DESCRIPTION "Hungarian lottery (Otoslotto) winners engine"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

option(BUILD_TESTS "Build unit and integration tests" ON)
option(ENABLE_NATIVE_ARCH "Optimize Release builds for the host CPU (-march=native)" ON)
option(ENABLE_LTO "Enable link time optimization in Release builds when supported" ON)
option(ENABLE_COVERAGE "Instrument the build for gcov based coverage" OFF)
option(ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

include(cmake/ProjectOptions.cmake)

find_package(Threads REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
