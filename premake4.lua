--#!lua
workspace "Velocity"
	architecture "x64"
	configurations { "Debug", "Release" }
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

	includeDirList = {
		"Velocity/vendor/GLFW/include",
		"Velocity/vendor/Glad/include",
		"Velocity/vendor/imgui",
		"Velocity/vendor/glm",
		"Velocity/vendor/spdlog/include",

		"Velocity/src"
	}

	include "Velocity/vendor/GLFW" -- includes the premake file as a whole
	include "Velocity/vendor/Glad"
	include "Velocity/vendor/imgui"

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

	if os.get() == "windows" then
		linkLibs = {
			"GLFW",
			"Glad",
			"imGui",
			"opengl32.lib"
		}
	end

	-- Build Options:
	if os.get() == "macosx" then
		linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
	end

	buildOptions = {"-std=c++11"}

	project "Velocity"
		location "Velocity"
		kind "ConsoleApp"
		language "C++"
		objdir "build"
		targetdir "."
		buildoptions (buildOptions)
		--libdirs (libDirectories)
		links (linkLibs)
		linkoptions (linkOptionList)
		includedirs (includeDirList)

		pchheader "vctPCH.h"
      	pchsource "Velocity/src/vctPCH.cpp"

		-- Assets Directory
		currentDir = os.getcwd()
		assetDir = currentDir.."/Assets/"
		os.mkdir(assetDir)
		defines {"ASSET_DIR=".."\""..assetDir.."\""}

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/src/**.inl",
			"%{prj.name}/vendor/glm/glm/**.hpp",
			"%{prj.name}/vendor/glm/glm/**.inl"
		}

		configuration "Debug"
			defines { "DEBUG", "VCT_DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG", "VCT_RELEASE" }
			flags { "Optimize" }

	----------------------------------------------------
	--				PLATFORM SPECIFICS
	----------------------------------------------------
		filter "system:windows"
			systemversion "latest"

			defines
			{
				"VCT_PLATFORM_WINDOWS",
				"GLFW_INCLUDE_NONE"
			}
	
		filter "system:linux"
			systemversion "latest"

			defines
			{
				"VCT_PLATFORM_LINUX",
				"GLFW_INCLUDE_NONE"
			}
