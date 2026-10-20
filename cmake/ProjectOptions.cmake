# Compiler options shared by every target of the project.

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message(WARNING "Untested compiler ${CMAKE_CXX_COMPILER_ID}: optional flags are disabled")
  return()
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

if(WARNINGS_AS_ERRORS)
  add_compile_options(-Werror)
endif()

if(ENABLE_NATIVE_ARCH)
  add_compile_options($<$<CONFIG:Release>:-march=native>)
endif()

if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR LANGUAGES CXX)

  if(LTO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  else()
    message(STATUS "Link time optimization not supported: ${LTO_ERROR}")
  endif()
endif()

if(ENABLE_COVERAGE)
  message(STATUS "Coverage instrumentation enabled")
  add_compile_options(-O0 -g --coverage)
  add_link_options(--coverage)
endif()

if(ENABLE_SANITIZERS)
  message(STATUS "AddressSanitizer and UndefinedBehaviorSanitizer enabled")
  set(SANITIZER_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
  add_compile_options(${SANITIZER_FLAGS})
  add_link_options(${SANITIZER_FLAGS})
endif()
