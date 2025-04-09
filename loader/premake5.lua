dependencies = {
	basePath = "../deps"
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

include "../deps/minhook.lua"

workspace "umamusume-localify-loader"
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
	cppdialect "C++20"
	cdialect "C17"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
	}

	staticruntime "Off"

	filter "configurations:Release"
		optimize "Off"
		buildoptions "/Oi /MD"

	filter "configurations:Debug"
		optimize "Debug"

	dependencies.projects()

	project "umamusume-localify-loader"
	targetname "imm32"

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
		"ntdll",
	}

	includedirs {
		"./src",
		"%{prj.location}/src",
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
