httplib = {
	source = path.join(dependencies.basePath, "cpp-httplib"),
}

function httplib.import()
	httplib.includes()
end

function httplib.includes()
	includedirs {
		httplib.source
	}
end

function httplib.project()
end

table.insert(dependencies, httplib)
