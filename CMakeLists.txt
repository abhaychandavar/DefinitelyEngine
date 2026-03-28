cmake_minimum_required(VERSION 4.0)
project(DefinitelyEngine VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_subdirectory(Engine)
add_subdirectory(Sandbox)