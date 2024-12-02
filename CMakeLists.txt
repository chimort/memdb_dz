cmake_minimum_required(VERSION 3.10)

project(memdb_project)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(LIB_SOURCES
    src/Database.cpp
    src/Table.cpp
    src/QueryParser.cpp
)

add_library(memdb_lib ${LIB_SOURCES})


add_executable(memdb_project main.cpp)
target_link_libraries(memdb_project memdb_lib)

add_subdirectory(googletest)

set(TEST_FILES
    tests/parser_tests.cpp
    tests/table_tests.cpp
    tests/files_tests.cpp
    tests/commands_tests.cpp
)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests memdb_lib gtest gtest_main)
