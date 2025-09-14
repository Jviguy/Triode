cmake_minimum_required(VERSION 4.0)
project(Triode)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)
add_subdirectory(arch)
add_subdirectory(triode)
add_subdirectory(app)
add_subdirectory(assembler)