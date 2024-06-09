#pragma once

#include <string>
#include <vector>
#include "il2cpp/il2cpp_symbols.hpp"

namespace local
{
	std::string wide_u8(const std::wstring& str);
	std::string u16_u8(const std::u16string& str);
	std::wstring u8_wide(const std::string& str);
	std::string wide_acp(const std::wstring& str);
	std::wstring acp_wide(const std::string& str);
	void load_textdb(const std::vector<std::wstring> *dicts);
	void load_textId_textdb(const std::wstring& dict);
	void reload_textdb(const std::vector<std::wstring>* dicts);
	void reload_textId_textdb(const std::wstring& dict);
	bool localify_text(size_t hash, const std::wstring** result);
	bool localify_text_by_textId_name(const std::wstring& textIdName, std::wstring** result);
	Il2CppString* get_localized_string(size_t hash_or_id);
	Il2CppString* get_localized_string(const std::wstring& textIdName);
	Il2CppString* get_localized_string(Il2CppString* str);
};
