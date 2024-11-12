cmake_minimum_required(VERSION 3.10)

project(memdb_project)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(SOURCES
    main.cpp
    src/Database.cpp
    src/Table.cpp
    src/QueryParser.cpp
)

# Add executable
add_executable(memdb_project ${SOURCES})