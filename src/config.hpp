#pragma once

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/prettywriter.h>

using namespace std;

namespace config
{
	rapidjson::Document config_document;
    rapidjson::Document backup_document;

	bool read_config()
	{
		ifstream config_stream{ "config.json" };

		if (!config_stream.is_open())
		{
			return false;
		}

		rapidjson::IStreamWrapper wrapper{ config_stream };

		config_document.ParseStream(wrapper);
		config_stream.close();

		backup_document.CopyFrom(config_document, backup_document.GetAllocator(), true);

		return !config_document.HasParseError();
	}

	void write_config()
	{
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		config_document.Accept(writer);

		ofstream config_stream{ "config.json" };
		config_stream << buffer.GetString() << endl;
		config_stream.close();
	}

	void rollback_config()
	{
		config_document.CopyFrom(backup_document, config_document.GetAllocator(), true);
	}
}
