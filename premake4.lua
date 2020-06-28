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
		"Velocity/vendor/dirent/include",
		"Velocity/vendor/tinyOBJ",
		"Velocity/vendor/OpenFBX/src",

		"Velocity/src"
	}

	include "Velocity/vendor/GLFW" -- includes the premake file as a whole
	include "Velocity/vendor/Glad"
	include "Velocity/vendor/imgui"
	include "Velocity/vendor/OpenFBX"

	libDirectories = {
		"../lib"
	}

	-- Basic compiler build options
	buildOptions = {"-std=c++11"}

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
		
		table.insert(buildOptions, "-fopenmp")
		table.insert(buildOptions, "-DUSE_OPENMP")

		linkOptionList = {"-fopenmp"}
	end

	if os.get() == "windows" then
		linkLibs = {
			"GLFW",
			"Glad",
			"imGui",
			"OpenFBX",
			"opengl32.lib"
		}
		table.insert(buildOptions, "/openmp")
	end

	-- Build Options:
	if os.get() == "macosx" then
		linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
	end

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
		currentDir	= os.getcwd()
		assetDir	= currentDir.."/Assets/"
		--shaderDir	= currentDir.."/Velocity/src/Engine/Renderer/Shaders/"
		shaderDir	= currentDir.."/Content/Shaders/"
		contentDir	= currentDir.."/Content/";

		os.mkdir(assetDir)
		defines {"ASSET_DIR=".."\""..assetDir.."\"", 
				"SHADER_DIR=".."\""..shaderDir.."\"", 
				"ROOT_DIR=".."\""..currentDir.."\"",
				"CONTENT_DIR=".."\""..contentDir.."\""}

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/src/**.inl",
			"%{prj.name}/vendor/glm/glm/**.hpp",
			"%{prj.name}/vendor/glm/glm/**.inl",
			"%{prj.name}/vendor/tinyOBJ/**.cpp",
			--"%{prj.name}/vendor/tinyOBJ/**.h"
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
