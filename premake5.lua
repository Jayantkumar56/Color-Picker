workspace "Color-Picker"
    architecture   "x64"
    startproject   "Quirk-Editor"
    configurations { "Debug", "Release", "Dist" }
    flags          { "MultiProcessorCompile"    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "Dependency/imgui"
include "Dependency/GLFW"
include "Dependency/Glad"
group ""

project "Color-Picker"
    location "Color-Picker"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"

    targetdir ("bin/" .. "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")
    objdir ("bin-int/" .. "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")

    -- for imgui
    defines { "_CRT_SECURE_NO_WARNINGS" }

    files
    {
        "Color-Picker/src/**.h",
        "Color-Picker/src/**.cpp",
        "Dependency/stb_image/**.h",
        "Dependency/stb_image/**.cpp"
    }

    includedirs
    {
        "Color-Picker/src",
        "Dependency/imgui",
        "Dependency/GLFW/include",
        "Dependency/Glad/include",
        "Dependency/stb_image",
        "Dependency/glm"
    }

    links
    {
        "imgui",
        "GLFW",
        "Glad",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_WINDLL",
            "GLFW_INCLUDE_NONE",
            "PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "On"

    filter "configurations:Dist"
        kind "WindowedApp"
        runtime "Release"
        symbols "Off"
        optimize "Full"
