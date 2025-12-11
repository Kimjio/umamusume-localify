#pragma once

#include <string>
#include <vector>
#include "il2cpp/il2cpp_symbols.hpp"

namespace local
{
	void load_textdb(const std::vector<std::u16string> *dicts);
	void load_textId_textdb(const std::u16string& dict);
	void reload_textdb(const std::vector<std::u16string>* dicts);
	void reload_textId_textdb(const std::u16string& dict);
	bool localify_text(size_t hash, const std::u16string** result);
	bool localify_text_by_textId_name(const std::u16string& textIdName, std::u16string** result);
	Il2CppString* get_localized_string(size_t hash_or_id);
	Il2CppString* get_localized_string(const std::u16string& textIdName);
	Il2CppString* get_localized_string(Il2CppString* str);
	const char* get_localized_string(const char* str);
};
