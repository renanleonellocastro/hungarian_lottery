include(FetchContent)
FetchContent_Declare(googletest	URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB UNIT_SOURCES unit/*.cpp)
file(GLOB INTEGRATION_SOURCES integration/*.cpp)

# Add the custom GoogleTest main so both test executables include it.
set(TEST_MAIN ${CMAKE_SOURCE_DIR}/tests/gtest_main.cpp)

## Unit tests executable
add_executable(tests_unit ${UNIT_SOURCES} ${TEST_MAIN})
target_link_libraries(tests_unit PRIVATE gtest hungarian_lottery Threads::Threads)

# Expose test-only helpers in headers when compiling the unit test binary.
target_compile_definitions(tests_unit PRIVATE UNIT_TESTING=1)

# Register individual GoogleTest cases with CTest so each unit test appears
# as its own test (improves reporting and allows running a subset).
include(GoogleTest)
gtest_discover_tests(tests_unit)

## Integration tests executable
add_executable(tests_integration ${INTEGRATION_SOURCES} ${TEST_MAIN})
target_link_libraries(tests_integration PRIVATE gtest hungarian_lottery Threads::Threads)

## Copy test data into the build tree so tests can find files at runtime.
add_custom_target(copy_test_data
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/data ${CMAKE_BINARY_DIR}/tests/data
	COMMENT "Copy tests/data -> ${CMAKE_BINARY_DIR}/tests/data"
)

add_dependencies(tests_integration copy_test_data)
add_test(NAME integration_tests	COMMAND tests_integration)

# Provide the build-time path to the copied test data so the tests can
# reliably locate files regardless of the current working directory.
target_compile_definitions(tests_integration PRIVATE TEST_DATA_DIR=\"${CMAKE_BINARY_DIR}/tests/data\")

# Ensure integration tests are built with Release performance flags even if the
# overall build type differs. Users can still override per-target flags.
target_compile_options(tests_integration PRIVATE $<$<CONFIG:Release>:-O3> $<$<CONFIG:Release>:-Ofast> $<$<CONFIG:Release>:-march=native> $<$<CONFIG:Release>:-flto>)
target_link_options(tests_integration PRIVATE $<$<CONFIG:Release>:-flto>)

if(ENABLE_COVERAGE)
	find_program(LCOV_EXEC lcov)
	find_program(GENHTML_EXEC genhtml)

	if(LCOV_EXEC AND GENHTML_EXEC)
		add_custom_target(coverage
			COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests for coverage..."
			COMMAND ${CMAKE_BINARY_DIR}/tests/tests_unit --gtest_color=no --gtest_output=xml:tests_unit_results.xml
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
			COMMAND ${CMAKE_COMMAND} -E echo "lcov or genhtml not found"
			COMMAND ${CMAKE_COMMAND} -E echo "Install lcov and genhtml to generate coverage reports."
			COMMENT "lcov/genhtml not available"
		)
	endif()
endif()
