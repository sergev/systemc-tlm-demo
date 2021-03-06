cmake_minimum_required(VERSION 3.13.2)

project(tlm_test VERSION 0.0.1 DESCRIPTION "Testing the SystemC TLM2 model")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Werror")

find_library(systemc systemc /usr/local/lib)
link_directories(/usr/local/lib)

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         273f8cb059a4e7b089731036392422b5ef489791
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
endif()

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

include(GoogleTest)
enable_testing()
get_target_property(GTEST_INCLUDE_DIR gtest INCLUDE_DIRECTORIES)
include_directories(simulator ${GTEST_INCLUDE_DIR} /usr/local/include)

add_executable(test_build test_build.cpp)
target_link_libraries(test_build systemc gtest)

add_executable(test_run3 test_run3.cpp)
target_link_libraries(test_run3 systemc gtest)

add_executable(test_thread test_thread.cpp)
target_link_libraries(test_thread systemc gtest)

add_executable(test_mem test_mem.cpp)
target_link_libraries(test_mem systemc gtest)

add_test(test_build test_build)
add_test(test_run3 test_run3)
add_test(test_thread test_thread)
add_test(test_mem test_mem)
