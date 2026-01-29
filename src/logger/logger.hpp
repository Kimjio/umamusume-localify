#pragma once

#include <string>
#include <vector>
#include <utility>

#include "il2cpp/il2cpp-api-types.h"

namespace logger
{
	void init_logger();
	void close_logger();
	void write_entry(size_t hash, const il2cppstring& text);
	void write_static_dict(const std::vector<il2cppstring>& dict);
	void write_text_id_static_dict(const std::vector<std::pair<const il2cppstring, const il2cppstring>>& dict,
		const std::vector<std::pair<const il2cppstring, const il2cppstring>>& not_matched);
}
