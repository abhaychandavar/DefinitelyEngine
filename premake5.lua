workspace "DefinitelyEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Sandbox"

    filter "system:windows"
        toolset "msc-v145"

    filter "configurations:Debug"
        defines { "DE_DEBUG", "DE_ENABLE_ASSERTS" }
        symbols "On"

    filter "configurations:Release"
        defines { "DE_RELEASE" }
        optimize "On"

    filter {}

outputBaseDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

include "Engine/external/GLFW"
include "Engine/external/GLAD"
include "Engine/external/imgui"

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
        "Engine/external/*/include",
        "Engine/external/imgui",
        "Engine/external/GLM"
    }

    links { "GLFW", "GLAD", "ImGui" }

    filter "system:windows"
        defines { "DE_PLATFORM_WINDOWS", "DE_BUILD_DLL", "GLFW_INCLUDE_NONE" }
        buildoptions { "/utf-8" }
        postbuildcommands
        {
            ("{MKDIR} bin/" .. outputBaseDir .. "/Sandbox"),
            ("{COPY} %{cfg.buildtarget.relpath} bin/" .. outputBaseDir .. "/Sandbox")
        }

    filter "system:macosx"
        defines { "DE_PLATFORM_MAC", "DE_BUILD_DLL", "GLFW_INCLUDE_NONE" }
        links {
            "Cocoa.framework",
            "IOKit.framework",
            "CoreFoundation.framework",
            "CoreGraphics.framework",
            "Carbon.framework",
            "QuartzCore.framework"
        }

    filter "system:linux"
        defines { "DE_PLATFORM_LINUX", "DE_BUILD_DLL", "GLFW_INCLUDE_NONE" }

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
        "Engine/external/*/include",
        "Engine/external/GLM"
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
