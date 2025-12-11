#include <stdinclude.hpp>

#include "config/config.hpp"
#include "string_utils.hpp"

using namespace std;

namespace logger
{
	namespace
	{
		fstream log_file;

		bool enabled = false;
		bool request_exit = false;
		bool has_change = false;

		fstream static_json;
		fstream not_matched_json;
	}

	void init_logger()
	{
		// only output if file exists so regular user will not see it.
		if (config::enable_logger)
		{
			enabled = true;
			log_file.open("dump.txt", ios::app | ios::out);

			thread t([]() {
				while (!request_exit)
				{
					this_thread::sleep_for(1s);

					if (has_change)
					{
						log_file.flush();
						has_change = false;
					}
				}

				log_file.close();
				});

			t.detach();
		}
	}

	void close_logger()
	{
		request_exit = true;
	}

	void write_entry(size_t hash, const u16string& text)
	{
		if (!enabled)
			return;

		auto u8str = u16_u8(text);
		replaceAll(u8str, "\n", "\\n");
		replaceAll(u8str, "\"", "\\\"");

		log_file << "\"" << hash << "\": \"" << u8str << "\",\n";

		has_change = true;
	}

	void write_static_dict(const std::vector<std::u16string>& dict)
	{
		if (config::enable_logger)
		{
			static_json.open("static.json", ios::out);
			static_json << "{\n";
			thread t([dict]() {
				for (int i = 0; i < dict.size(); i++)
				{
					auto hash = std::hash<u16string>{}(dict[i]);
					auto u8str = u16_u8(dict[i]);
					replaceAll(u8str, "\n", "\\n");
					replaceAll(u8str, "\"", "\\\"");
					if (i == dict.size() - 1)
					{
						static_json << "\"" << hash << "\": \"" << u8str << "\"\n";
					}
					else
					{
						static_json << "\"" << hash << "\": \"" << u8str << "\",\n";
					}
				}
				static_json << "}\n";
				static_json.close();
				});

			t.detach();
		}

	}

	void write_text_id_static_dict(const vector<pair<const u16string, const u16string>>& dict,
		const vector<pair<const u16string, const u16string>>& not_matched)
	{
		if (config::enable_logger)
		{
			static_json.open("text_id_static.json", ios::out);
			static_json << "{\n";
			thread t([dict]() {
				for (auto pair = dict.begin(); pair != dict.end(); pair++)
				{
					auto u8key = u16_u8(pair->first);
					auto u8str = u16_u8(pair->second);
					replaceAll(u8str, "\n", "\\n");
					replaceAll(u8str, "\"", "\\\"");
					if (next(pair) == dict.end())
					{
						static_json << "\"" << u8key << "\": \"" << u8str << "\"\n";
					}
					else
					{
						static_json << "\"" << u8key << "\": \"" << u8str << "\",\n";
					}
				}
				static_json << "}\n";
				static_json.close();
				});

			t.detach();
			if (!not_matched.empty())
			{
				not_matched_json.open("text_id_not_matched.json", ios::out);
				not_matched_json << "{\n";
				thread t1([not_matched]() {
					for (auto pair = not_matched.begin(); pair != not_matched.end(); pair++)
					{
						auto u8key = u16_u8(pair->first);
						auto u8str = u16_u8(pair->second);
						replaceAll(u8str, "\n", "\\n");
						replaceAll(u8str, "\"", "\\\"");
						if (next(pair) == not_matched.end())
						{
							not_matched_json << "\"" << u8key << "\": \"" << u8str << "\"\n";
						}
						else
						{
							not_matched_json << "\"" << u8key << "\": \"" << u8str << "\",\n";
						}
					}
					not_matched_json << "}\n";
					not_matched_json.close();
					});

				t1.detach();
			}
		}
	}
}
