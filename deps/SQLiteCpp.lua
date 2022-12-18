SQLiteCpp = {
	source = path.join(dependencies.basePath, "SQLiteCpp"),
}

function SQLiteCpp.import()
	links { "SQLiteCpp" }
	SQLiteCpp.includes()
end

function SQLiteCpp.includes()
	includedirs {
		path.join(SQLiteCpp.source, "include"),
		path.join(SQLiteCpp.source, "sqlite3")
	}
end

function SQLiteCpp.project()
	project "SQLiteCpp"
		language "C++"

		SQLiteCpp.includes()

		files {
			path.join(SQLiteCpp.source, "sqlite3/**.c"),
			path.join(SQLiteCpp.source, "src/**.cpp")
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, SQLiteCpp)
