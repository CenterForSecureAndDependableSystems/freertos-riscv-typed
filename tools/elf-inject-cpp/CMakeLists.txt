cmake_minimum_required(VERSION 3.20)
project(elf-inject
	LANGUAGES C CXX
)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_BUILD_TYPE DEBUG)

option(LIEF_DOC "Build LIEF docs" OFF)
option(LIEF_PYTHON_API "Build LIEF Python API" OFF)
option(LIEF_EXAMPLES "Build LIEF examples" OFF)
option(LIEF_TESTS "Build LIEF tests" OFF)

add_subdirectory(LIEF)

add_executable(elf-inject elf-inject.cpp)
target_link_libraries(elf-inject PUBLIC LIEF::LIEF)
