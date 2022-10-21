#include "stdinclude.hpp"

namespace
{
	void console_thread()
	{
		std::string line;

		while (true)
		{
			std::cin >> line;

			std::cout << "\n] " << line << "\n";

			if (line == "reload")
			{
				std::ifstream config_stream{ "config.json" };
				std::vector<std::string> dicts{};
				std::string text_id_dict;

				rapidjson::IStreamWrapper wrapper{ config_stream };
				rapidjson::Document document;

				document.ParseStream(wrapper);

				if (!document.HasParseError())
				{
					if (document.HasMember("dicts"))
					{
						auto& dicts_arr = document["dicts"];
						auto len = dicts_arr.Size();

						for (size_t i = 0; i < len; ++i)
						{
							auto dict = dicts_arr[i].GetString();

							dicts.emplace_back(dict);
						}
					}

					if (document.HasMember("textIdDict"))
					{
						text_id_dict = document["textIdDict"].GetString();
					}
				}

				config_stream.close();
				local::reload_textdb(&dicts);
				if (!text_id_dict.empty())
				{
					local::reload_textId_textdb(text_id_dict);
				}
			}
		}
	}
}

void start_console()
{
#ifdef _DEBUG
	std::thread(console_thread).detach();
#endif
}