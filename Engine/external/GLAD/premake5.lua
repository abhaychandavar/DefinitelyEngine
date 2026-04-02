project "GLAD"
    kind "StaticLib"
    language "C"

    targetdir ("bin/" .. outputDir)
    objdir    ("bin-int/" .. outputDir)

    files { "src/glad.c" }

    includedirs { "include" }

    filter "system:windows"
        staticruntime "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter {}
