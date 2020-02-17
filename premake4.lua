#!lua

-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

includeDirList = {
    "../shared",
    "../shared/include",
    "../shared/gl3w",
    "../shared/imgui",

    "Velocity/vendor/spdlog/include",

    "Velocity/src"
}

libDirectories = {
    "../lib"
}


if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua"

    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
        "GL",
        "Xinerama",
        "Xcursor",
        "Xxf86vm",
        "Xi",
        "Xrandr",
        "X11",
        "stdc++",
        "dl",
        "pthread"
    }
end

-- Build Options:
if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
end

buildOptions = {"-std=c++11"}

solution "CS488-Projects"
    configurations { "Debug", "Release" }

    project "VelocityProj"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        linkoptions (linkOptionList)
        includedirs (includeDirList)

        --pchheader "vctPCH.h"
      	--pchsource "Velocity/src/vctPCH.cpp"

        files
        {
            "*.cpp",
            "Velocity/src/**.cpp",
            "Velocity/src/**.h"
        }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
