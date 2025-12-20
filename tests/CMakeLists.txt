cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB UNIT_SOURCES unit/*.cpp)
file(GLOB INTEGRATION_SOURCES integration/*.cpp)

add_executable(tests_unit ${UNIT_SOURCES})
target_link_libraries(tests_unit PRIVATE gtest_main hungarian_lottery Threads::Threads)
add_test(NAME unit_tests COMMAND tests_unit)

add_executable(tests_integration ${INTEGRATION_SOURCES})
target_link_libraries(tests_integration PRIVATE gtest_main hungarian_lottery Threads::Threads)
add_test(NAME integration_tests COMMAND tests_integration)

if(ENABLE_COVERAGE)
	find_program(LCOV_EXEC lcov)
	find_program(GENHTML_EXEC genhtml)

	if(LCOV_EXEC AND GENHTML_EXEC)
		add_custom_target(coverage
			COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests for coverage..."
			COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -V
			COMMAND ${CMAKE_COMMAND} -E echo "Capturing coverage with lcov..."
			COMMAND ${LCOV_EXEC} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
			COMMAND ${LCOV_EXEC} --remove coverage.info '/usr/*' '*/_deps/*' '*/googletest-src/*' --output-file coverage.info.cleaned
			COMMAND ${LCOV_EXEC} --list coverage.info.cleaned
			COMMAND ${CMAKE_COMMAND} -E echo "Generating HTML report coverage_html..."
			COMMAND ${GENHTML_EXEC} coverage.info.cleaned --output-directory coverage_html
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			DEPENDS tests_unit
			COMMENT "Run tests and produce lcov/genhtml coverage report"
		)
	else()
		add_custom_target(coverage
			COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests - lcov/genhtml not found"
			COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -V
			COMMAND ${CMAKE_COMMAND} -E echo "lcov or genhtml not found. Install lcov and genhtml to generate coverage reports."
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			DEPENDS tests_unit
			COMMENT "Run tests; lcov/genhtml not available"
		)
	endif()
endif()
