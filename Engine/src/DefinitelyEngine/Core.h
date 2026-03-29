#pragma once

#ifdef DE_PLATFORM_WINDOWS
    #ifdef DE_BUILD_DLL
        #define DEFINITELY_ENGINE_API __declspec(dllexport)
    #else
        #define DEFINITELY_ENGINE_API __declspec(dllimport)
    #endif
#elif defined(DE_PLATFORM_MAC) || defined(DE_PLATFORM_LINUX)
    #ifdef DE_BUILD_DLL
        #define DEFINITELY_ENGINE_API __attribute__((visibility("default")))
    #else
        #define DEFINITELY_ENGINE_API
    #endif
#else
    #error "Unsupported platform!"
#endif

#define Bit(x) (1 << x)

struct Vector2
{
    float x, y;
};