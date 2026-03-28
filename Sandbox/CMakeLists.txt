cmake_minimum_required(VERSION 4.0)
project(Sandbox VERSION 0.1.0 LANGUAGES CXX)

add_executable(Sandbox
    src/Sandbox.cpp
)

target_link_libraries(Sandbox
    PRIVATE
        Engine
)

set_property(TARGET Sandbox PROPERTY CXX_STANDARD 17)