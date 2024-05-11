msgpack11 = {
	source = path.join(dependencies.basePath, "msgpack11"),
}

function msgpack11.import()
	links { "msgpack11" }
	msgpack11.includes()
end

function msgpack11.includes()
	includedirs {
		msgpack11.source
	}
end

function msgpack11.project()
	project "msgpack11"
		language "C++"

		msgpack11.includes()

		files {
			path.join(msgpack11.source, "msgpack11.cpp")
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, msgpack11)
