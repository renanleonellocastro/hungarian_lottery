add_library(hungarian_lottery STATIC
  players_numbers_reader.cpp
  console_writer.cpp
  console_reader.cpp
  bruteforce_lottery_processor.cpp
  cached_lottery_processor.cpp
  lottery.cpp
)

target_include_directories(hungarian_lottery PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Prefer the glog CMake package; fall back to pkg-config on distributions that only ship a .pc file.
find_package(glog CONFIG QUIET)

if(glog_FOUND)
  message(STATUS "Found glog via CMake config")
else()
  message(STATUS "glog CMake config not found, using pkg-config")
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLOG REQUIRED IMPORTED_TARGET GLOBAL libglog)
  add_library(glog::glog ALIAS PkgConfig::GLOG)
endif()

target_link_libraries(hungarian_lottery PUBLIC glog::glog Threads::Threads)

if(BUILD_TESTS)
  # Exposes the test only helpers declared in the headers.
  target_compile_definitions(hungarian_lottery PUBLIC UNIT_TESTING=1)
endif()

add_executable(lottery main.cpp)
target_link_libraries(lottery PRIVATE hungarian_lottery)
set_target_properties(lottery PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

install(TARGETS lottery RUNTIME DESTINATION bin)
