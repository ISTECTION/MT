cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(MT VERSION 1.0 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH "bin")

set(SOURCES
    src/main.cpp
)

set(HEADERS
    include/argparse/argparse.hpp
    include/const_table.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)