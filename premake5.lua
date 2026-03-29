workspace "DefinitelyEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Sandbox"

    filter "configurations:Debug"
        defines { "DE_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "DE_RELEASE" }
        optimize "On"

    filter {}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

include "Engine/external/GLFW"

project "Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputDir)
    objdir    ("bin-int/" .. outputDir)

    pchheader "depch.h"
    pchsource "Engine/src/depch.cpp"

    files
    {
        "Engine/src/**.h",
        "Engine/src/**.cpp"
    }

    includedirs
    {
        "Engine/src",
        "Engine/external/*/include"
    }

    links { "GLFW" }

    filter "system:windows"
        defines { "DE_PLATFORM_WINDOWS", "DE_BUILD_DLL" }

    filter "system:macosx"
        defines { "DE_PLATFORM_MAC", "DE_BUILD_DLL" }
        links {
            "Cocoa.framework",
            "IOKit.framework",
            "CoreFoundation.framework",
            "CoreGraphics.framework",
            "Carbon.framework",
            "QuartzCore.framework"
        }

    filter "system:linux"
        defines { "DE_PLATFORM_LINUX", "DE_BUILD_DLL" }

    filter {}

project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputDir)
    objdir    ("bin-int/" .. outputDir)

    files
    {
        "Sandbox/src/**.h",
        "Sandbox/src/**.cpp"
    }

    includedirs
    {
        "Engine/src",
        "Engine/external/*/include"
    }

    links { "Engine" }

    filter "system:windows"
        defines { "DE_PLATFORM_WINDOWS" }

    filter "system:macosx"
        defines { "DE_PLATFORM_MAC" }

    filter "system:linux"
        defines { "DE_PLATFORM_LINUX" }

    filter {}
