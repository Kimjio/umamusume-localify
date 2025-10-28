dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

include "deps/cpp-httplib.lua"
include "deps/minhook.lua"
include "deps/rapidjson.lua"
include "deps/msgpack11.lua"

workspace "umamusume-localify"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

	architecture "x64"
	platforms "x64"

	configurations {
		"Debug",
		"Release",
	}

	toolset "clang"
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Off"
	characterset "Unicode"
	cppdialect "C++latest"
	cdialect "C23"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
	}

	staticruntime "Off"

	filter "configurations:Release"
		optimize "Full"
		buildoptions "/Os"

	filter "configurations:Debug"
		optimize "Debug"

	dependencies.projects()

	project "umamusume-localify"
	targetname "localify"

	language "C++"
	kind "SharedLib"

	files {
		"./src/**.hpp",
		"./src/**.cpp",
		"./src/**.h",
		"./src/**.c",
		"./src/**.asm",
		"./src/**.def",
		"./src/**.rc",
	}

	links {
		"Shlwapi",
		"WinINet",
		"ntdll",
		"RuntimeObject",
		"Version",
		"Crypt32",
		"./deps/discord_game_sdk/discord_game_sdk.dll.lib"
	}

	includedirs {
		"./src",
		"%{prj.location}/src",
	}

	nuget {
		"Microsoft.Web.WebView2:1.0.3537.50",
		"Microsoft.Windows.ImplementationLibrary:1.0.250325.1"
	}

	dependencies.imports()

	filter "configurations:Release"
		linkoptions "/SAFESEH:NO"
		syslibdirs {
			"./libs/Release",
		}

	filter "configurations:Debug"
		linkoptions "/SAFESEH:NO"
		syslibdirs {
			"./libs/Debug",
		}
