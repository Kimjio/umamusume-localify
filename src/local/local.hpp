#pragma once

#include <string>
#include <vector>
#include "il2cpp/il2cpp_symbols.hpp"

namespace local
{
	void load_textdb(const std::vector<il2cppstring> *dicts);
	void load_textId_textdb(const il2cppstring& dict);
	void reload_textdb(const std::vector<il2cppstring>* dicts);
	void reload_textId_textdb(const il2cppstring& dict);
	bool localify_text(size_t hash, const il2cppstring** result);
	bool localify_text_by_textId_name(const il2cppstring& textIdName, il2cppstring** result);
	Il2CppString* get_localized_string(size_t hash_or_id);
	Il2CppString* get_localized_string(const il2cppstring& textIdName);
	Il2CppString* get_localized_string(Il2CppString* str);
	const char* get_localized_string(const char* str);
};
