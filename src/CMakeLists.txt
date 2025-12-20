add_library(hungarian_lottery
  players_numbers_reader.cpp
  console_writer.cpp
  console_reader.cpp
  bruteforce_lottery_processor.cpp
  cached_lottery_processor.cpp
  lottery.cpp
)

target_include_directories(hungarian_lottery PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(glog QUIET)
if(glog_FOUND)
  target_compile_definitions(hungarian_lottery PRIVATE HAVE_GLOG=1)
  target_include_directories(hungarian_lottery PRIVATE ${glog_INCLUDE_DIRS})
  target_link_libraries(hungarian_lottery PRIVATE ${glog_LIBRARIES})
endif()

## No top-level app target here; tests and other consumers link to the library.
