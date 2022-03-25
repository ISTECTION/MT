cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(mt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_RELEASE "-O1")
set(EXECUTABLE_OUTPUT_PATH  "bin")
set(SOURCES src/main.cpp)

set(HEADERS
    include/argparse/argparse.hpp
    include/cxxtimer/cxxtimer.hpp
    include/tabulate/tabulate.hpp
    include/const_table.hpp
    include/translator.hpp
    include/var_table.hpp
    include/assembler.hpp
    include/toml++/toml.h
    include/lexeme.hpp
    include/token.hpp
    include/error.hpp
    include/place.hpp
    include/parse.hpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED ON)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        # -Wall
        # -Wextra
        # -pedantic
        # -Werror
        )
endif()

# Путь до рабочего каталога
set(WORKING_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Установка рабочего каталога для VS
set_target_properties(${PROJECT_NAME} PROPERTIES
VS_DEBUGGER_WORKING_DIRECTORY ${WORKING_PATH})

# Попробовать это переделать
# set_property - старая функция CMake < 3.6
# Назначение проекта в качестве запускаемовго
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MT)

# -DGTEST_COMPILE=ON
if(GTEST_COMPILE)
    message("GTEST_COMPILE=ON")
    add_subdirectory(googletest)
endif()
