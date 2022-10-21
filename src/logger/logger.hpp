#pragma once

namespace logger
{
	void init_logger();
	void close_logger();
	void write_entry(size_t hash, const std::wstring& text);
	void write_static_dict(const std::vector<std::wstring>& dict);
	void write_text_id_static_dict(const std::vector<std::pair<const std::string, const std::wstring>>& dict);
}
