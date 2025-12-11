#pragma once

#include <string>
#include <vector>
#include <utility>

namespace logger
{
	void init_logger();
	void close_logger();
	void write_entry(size_t hash, const std::u16string& text);
	void write_static_dict(const std::vector<std::u16string>& dict);
	void write_text_id_static_dict(const std::vector<std::pair<const std::u16string, const std::u16string>>& dict,
		const std::vector<std::pair<const std::u16string, const std::u16string>>& not_matched);
}
