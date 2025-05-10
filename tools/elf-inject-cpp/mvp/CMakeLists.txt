cmake_minimum_required(VERSION 3.20)
project(mvp
	LANGUAGES C CXX
)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_BUILD_TYPE DEBUG)

option(LIEF_DOC "Build LIEF docs" OFF)
option(LIEF_PYTHON_API "Build LIEF Python API" OFF)
option(LIEF_EXAMPLES "Build LIEF examples" OFF)
option(LIEF_TESTS "Build LIEF tests" OFF)

add_subdirectory(LIEF-old)

add_executable(min-inject inject.cpp)
target_link_libraries(min-inject PUBLIC LIEF::LIEF)

add_executable(min main.c)
