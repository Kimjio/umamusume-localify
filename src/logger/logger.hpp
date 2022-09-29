#pragma once

namespace logger
{
	void init_logger();
	void close_logger();
	void write_entry(size_t hash, std::wstring text);
	void write_static_dict(std::vector<std::wstring> dict);
}
