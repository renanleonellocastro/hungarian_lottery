find_package(GTest REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
include(GoogleTest)

set(TEST_MAIN ${PROJECT_SOURCE_DIR}/tests/gtest_main.cpp)
set(TEST_DATA_DIR ${PROJECT_BINARY_DIR}/tests/data)
set(DATASET_GENERATOR ${PROJECT_SOURCE_DIR}/tools/generate_players.py)

# Unit tests

file(GLOB UNIT_SOURCES CONFIGURE_DEPENDS unit/*.cpp)
add_executable(tests_unit ${UNIT_SOURCES} ${TEST_MAIN})
target_link_libraries(tests_unit PRIVATE GTest::gtest hungarian_lottery)
gtest_discover_tests(tests_unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES LABELS unit
  DISCOVERY_TIMEOUT 60)

# Deterministic datasets, generated at build time by an independent brute force oracle.
# They stay small so the whole suite runs in seconds, including under sanitizers.

function(add_test_dataset name players draws seed)
  set(output_dir ${TEST_DATA_DIR}/${name})
  add_custom_command(
    OUTPUT ${output_dir}/players.txt ${output_dir}/draws.txt ${output_dir}/expected.txt
    COMMAND Python3::Interpreter ${DATASET_GENERATOR}
            --players ${players} --draws ${draws} --seed ${seed} --out-dir ${output_dir}
    DEPENDS ${DATASET_GENERATOR}
    COMMENT "Generating ${name} dataset (${players} players, ${draws} draws)")
  add_custom_target(dataset_${name} DEPENDS ${output_dir}/expected.txt)
  set(TEST_DATASET_TARGETS ${TEST_DATASET_TARGETS} dataset_${name} PARENT_SCOPE)
endfunction()

add_test_dataset(small 1000 20 1)
add_test_dataset(medium 200000 20 2)

# Integration tests

file(GLOB INTEGRATION_SOURCES CONFIGURE_DEPENDS integration/*.cpp)
add_executable(tests_integration ${INTEGRATION_SOURCES} ${TEST_MAIN})
target_link_libraries(tests_integration PRIVATE GTest::gtest hungarian_lottery)
target_compile_definitions(tests_integration PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")
add_dependencies(tests_integration ${TEST_DATASET_TARGETS})
gtest_discover_tests(tests_integration
  PROPERTIES LABELS integration
  DISCOVERY_TIMEOUT 60)

foreach(dataset small medium)
  add_test(NAME cli_${dataset}
           COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/integration/cli_test.sh $<TARGET_FILE:lottery> ${TEST_DATA_DIR}/${dataset})
  set_tests_properties(cli_${dataset} PROPERTIES LABELS integration)
endforeach()
