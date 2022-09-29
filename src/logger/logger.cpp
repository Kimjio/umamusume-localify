#include <stdinclude.hpp>

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
	}

	void init_logger()
	{
		// only output if file exists so regular user will not see it.
		if (g_enable_logger)
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

	void write_entry(size_t hash, wstring text)
	{
		if (!enabled)
			return;

		auto u8str = local::wide_u8(text);
		replaceAll(u8str, "\n", "\\n");
		replaceAll(u8str, "\"", "\\\"");

		log_file << "\"" << hash << "\": \"" << u8str << "\",\n";

		has_change = true;
	}

	void write_static_dict(std::vector<std::wstring> dict)
	{
		if (g_enable_logger)
		{
			static_json.open("static.json", ios::out);
			static_json << "{\n";
			thread t([dict]() {
				for (int i = 0; i < dict.size(); i++) 
				{
					auto hash = std::hash<wstring>{}(dict[i]);
					auto u8str = local::wide_u8(dict[i]);
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
}
