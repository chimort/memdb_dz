cmake_minimum_required(VERSION 3.10)
project(memdb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/Database.cpp
    src/Responce.cpp
    src/Table.cpp
    # Add other source files here
)

add_executable(memdb ${SOURCES})
