#include <stdinclude.hpp>
#include <codecvt>

#include "config/config.hpp"

using namespace std;

namespace local
{
	namespace
	{
		unordered_map<size_t, wstring> text_db;
		unordered_map<wstring, wstring> textId_text_db;
		vector<size_t> str_list;
	}

	string wide_u8(const wstring& str)
	{
		string result;
		result.resize(str.length() * 4);

		int len = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), result.data(), result.length(), nullptr, nullptr);

		result.resize(len);

		return result;
	}

	string u16_u8(const u16string& str)
	{
		wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
		return utf16conv.to_bytes(str);
	}

	wstring u8_wide(const string& str)
	{
		wstring result;
		result.resize(str.length() * 4);

		int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), result.data(), result.length());

		result.resize(len);

		return result;
	}

	string wide_acp(const wstring& str)
	{
		string result;
		result.resize(str.length() * 4);

		int len = WideCharToMultiByte(CP_ACP, 0, str.data(), str.length(), result.data(), result.length(), nullptr, nullptr);

		result.resize(len);

		return result;
	}

	wstring acp_wide(const string& str)
	{
		wstring result;
		result.resize(str.length() * 4);

		int len = MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), result.data(), result.length());

		result.resize(len);

		return result;
	}

	void reload_textdb(const vector<wstring>* dicts)
	{
		text_db.clear();
		load_textdb(dicts);
	}

	void reload_textId_textdb(const wstring& dict)
	{
		textId_text_db.clear();
		load_textId_textdb(dict);
	}

	void load_textdb(const vector<wstring>* dicts)
	{
		for (wstring dict : *dicts)
		{
			if (filesystem::exists(dict))
			{
				if (filesystem::is_directory(dict))
				{
					wcout << L"Dict directory: " << dict << endl;
					for (auto& file : filesystem::directory_iterator(dict))
					{
						const auto filePath = local::acp_wide(file.path().string());
						const auto fileName = local::acp_wide(file.path().filename().string());
						if (file.is_regular_file())
						{
							wifstream dict_stream{ filePath };
							dict_stream.imbue(locale(""));

							if (!dict_stream.is_open())
								continue;

							wcout << L"Reading " << fileName << L"..." << endl;

							rapidjson::WIStreamWrapper wrapper{ dict_stream };
							WDocument document;

							document.ParseStream(wrapper);

							for (auto iter = document.MemberBegin();
								iter != document.MemberEnd(); ++iter)
							{
								auto key = stoull(iter->name.GetString());
								auto value = iter->value.GetString();

								text_db.emplace(key, value);
							}

							dict_stream.close();
						}
					}
				}
				else
				{
					wifstream dict_stream{ dict };
					dict_stream.imbue(locale(""));

					if (!dict_stream.is_open())
						continue;

					wcout << L"Reading " << dict << L"..." << endl;

					rapidjson::WIStreamWrapper wrapper{ dict_stream };
					WDocument document;

					document.ParseStream(wrapper);

					for (auto iter = document.MemberBegin();
						iter != document.MemberEnd(); ++iter)
					{
						auto key = stoull(iter->name.GetString());
						auto value = iter->value.GetString();

						text_db.emplace(key, value);
					}

					dict_stream.close();
				}
			}
		}

		cout << "loaded " << text_db.size() << " localized entries." << endl;
	}

	void load_textId_textdb(const wstring& dict)
	{
		if (filesystem::exists(dict.data()))
		{
			wifstream dict_stream{ dict };
			dict_stream.imbue(locale(""));

			if (!dict_stream.is_open())
				return;

			wcout << L"Reading " << dict << L"..." << endl;

			rapidjson::WIStreamWrapper wrapper{ dict_stream };
			WDocument document;

			document.ParseStream(wrapper);

			for (auto iter = document.MemberBegin();
				iter != document.MemberEnd(); ++iter)
			{
				auto key = iter->name.GetString();
				auto value = iter->value.GetString();

				textId_text_db.emplace(key, value);
			}

			dict_stream.close();
		}

		cout << "loaded " << textId_text_db.size() << " TextId localized entries." << endl;
	} 

	bool localify_text(size_t hash, wstring** result)
	{
		if (text_db.contains(hash))
		{
			*result = &text_db[hash];
			return true;
		}

		return false;
	}

	bool localify_text_by_textId_name(const wstring& textIdName, wstring** result)
	{
		if (textId_text_db.contains(textIdName))
		{
			*result = &textId_text_db[textIdName];
			return true;
		}
		return false;
	}

	Il2CppString* get_localized_string(size_t hash_or_id)
	{
		wstring* result;

		if (local::localify_text(hash_or_id, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	Il2CppString* get_localized_string(const wstring& textIdName)
	{
		wstring* result;

		if (local::localify_text_by_textId_name(textIdName, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	Il2CppString* get_localized_string(Il2CppString* str)
	{
		wstring* result;

		auto hash = std::hash<wstring>{}(str->start_char);

		if (local::localify_text(hash, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		if (config::enable_logger && !any_of(str_list.begin(), str_list.end(), [hash](size_t hash1) { return hash1 == hash; }))
		{
			str_list.emplace_back(hash);

			logger::write_entry(hash, str->start_char);
		}

		return str;
	}
}
