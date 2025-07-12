cmake_minimum_required(VERSION 3.16)
project(tiny_aig LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_library(tiny_aig INTERFACE)
target_include_directories(tiny_aig INTERFACE include)

add_executable(tests
    tests/test_parse.cpp
)
target_include_directories(tests PRIVATE tests/ext)
target_link_libraries(tests PRIVATE tiny_aig)
add_test(NAME parse_simple COMMAND tests)

# Allow `TEST_DATA_DIR` constant in code
target_compile_definitions(tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests")
