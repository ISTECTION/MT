cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(MT_test)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        609281088cfefc76f9d0ce82e1ff6c30cc3591e5)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCE_TEST

    src/test.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_TEST})

target_include_directories(
    ${PROJECT_NAME}  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(${PROJECT_NAME} gtest_main)