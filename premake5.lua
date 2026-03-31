workspace "DefinitelyEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Sandbox"
    toolset "msc-v145"

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
        buildoptions { "/utf-8" }
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Sandbox")
        }

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
        buildoptions { "/utf-8" }

    filter "system:macosx"
        defines { "DE_PLATFORM_MAC" }

    filter "system:linux"
        defines { "DE_PLATFORM_LINUX" }

    filter {}
