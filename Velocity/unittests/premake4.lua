--#!lua
workspace "UnitTests"
	architecture "x64"
	configurations { "Debug", "Release" }
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

	includeDirList = {
		"../Velocity/vendor/Catch2/single_include/catch2",
		"../Velocity/src"
	}

	libDirectories = {
		"../lib"
	}

	-- Basic compiler build options
	buildOptions = {"-std=c++11"}

	if os.target() == "macosx" then
		linkLibs = {
			"cs488-framework",
			"imgui",
			"glfw3",
			"lua"

		}
	end

	if os.target() == "linux" then
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

	if os.target() == "windows" then
		linkLibs = {
			"opengl32.lib"
		}
		table.insert(buildOptions, "/openmp")
	end

	-- Build Options:
	if os.target() == "macosx" then
		linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
	end


	-- Unit tests
	include "io_unittest"