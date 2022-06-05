--sources = {
--   "loader_example.cc",
--   }

project "tinyobjloader"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"tiny_obj_loader.h",
		"tiny_obj_loader.cc"
	}
	
	includedirs
	{
		"."
	}
	
	filter "system:linux"
		pic "On"

		systemversion "latest"
		--staticruntime "On"

		files
		{
		}

		defines
		{
		}

	filter "system:windows"
		systemversion "latest"
		--staticruntime "on"

		files
		{
		}

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


-- premake4.lua
--solution "TinyObjLoaderSolution"
--   configurations { "Release", "Debug" }
--
--   if (os.is("windows")) then
--      platforms { "x32", "x64" }
--   else
--      platforms { "native", "x32", "x64" }
--   end
--
--   -- A project defines one build target
--   project "tinyobjloader_example"
--      kind "ConsoleApp"
--      language "C++"
--      files { sources }
--
--      configuration "Debug"
--         defines { "DEBUG" } -- -DDEBUG
--         flags { "Symbols" }
--         targetname "loader_example_debug"
--
--      configuration "Release"
--         -- defines { "NDEBUG" } -- -NDEBUG
--         flags { "Symbols", "Optimize" }
--         targetname "loader_example"
