cmake_minimum_required(VERSION 4.0)
project(Engine VERSION 0.1.0 LANGUAGES CXX)

add_library(Engine SHARED
    src/DefinitelyEngine/Application.cpp
)

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    target_compile_definitions(Engine PUBLIC DE_PLATFORM_WINDOWS PRIVATE DE_BUILD_DLL)
elseif(APPLE)
    target_compile_definitions(Engine PUBLIC DE_PLATFORM_MAC PRIVATE DE_BUILD_DLL)
elseif(UNIX)
    target_compile_definitions(Engine PUBLIC DE_PLATFORM_LINUX PRIVATE DE_BUILD_DLL)
endif()

set_property(TARGET Engine PROPERTY CXX_STANDARD 17)
