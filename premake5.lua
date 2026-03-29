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
project "Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputDir)
    objdir    ("bin-int/" .. outputDir)

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

    filter "system:windows"
        defines { "DE_PLATFORM_WINDOWS", "DE_BUILD_DLL" }

    filter "system:macosx"
        defines { "DE_PLATFORM_MAC", "DE_BUILD_DLL" }

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
