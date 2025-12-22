if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Prefer high-performance Release flags by default. Users can still override these
# by specifying `-DCMAKE_BUILD_TYPE=...` or setting their own CXX flags.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Ofast -march=native -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")

add_library(hungarian_lottery
  players_numbers_reader.cpp
  console_writer.cpp
  console_reader.cpp
  bruteforce_lottery_processor.cpp
  cached_lottery_processor.cpp
  lottery.cpp
)

target_include_directories(hungarian_lottery PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Find google log external library
set(GLOG_TARGET glog::glog)
find_package(glog CONFIG QUIET)

if (glog_FOUND)
    message(STATUS "Found glog via CMake config")
else()
    message(STATUS "glog CMake config not found, using pkg-config")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLOG REQUIRED libglog)

    add_library(glog::glog INTERFACE IMPORTED)
    target_include_directories(glog::glog INTERFACE ${GLOG_INCLUDE_DIRS})
    target_link_libraries(glog::glog INTERFACE ${GLOG_LIBRARIES})
endif()

target_link_libraries(hungarian_lottery PRIVATE ${GLOG_TARGET})

# Expose test helpers when building tests so unit tests can exercise internal helpers.
if (BUILD_TESTS)
  target_compile_definitions(hungarian_lottery PRIVATE UNIT_TESTING=1)
endif()

# Tests and other consumers link to the library.
# Places the runtime binary in the top-level binary directory so helper scripts
# can find it at `${CMAKE_BINARY_DIR}/lottery`.
add_executable(lottery main.cpp)
target_link_libraries(lottery PRIVATE hungarian_lottery Threads::Threads)
set_target_properties(lottery PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Ensure the executable benefits from the same Release optimizations when the
# build configuration is Release.
target_compile_options(lottery PRIVATE $<$<CONFIG:Release>:-O3> $<$<CONFIG:Release>:-Ofast> $<$<CONFIG:Release>:-march=native> $<$<CONFIG:Release>:-flto>)
target_link_options(lottery PRIVATE $<$<CONFIG:Release>:-flto>)
