#include <stdinclude.hpp>
#include <codecvt>

using namespace std;

namespace local
{
	namespace
	{
		unordered_map<size_t, string> text_db;
		unordered_map<string, string> textId_text_db;
		std::vector<size_t> str_list;
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
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
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

	void reload_textdb(const vector<string>* dicts)
	{
		text_db.clear();
		load_textdb(dicts);
	}

	void reload_textId_textdb(const std::string& dict)
	{
		textId_text_db.clear();
		load_textId_textdb(dict);
	}

	void load_textdb(const vector<string>* dicts)
	{
		for (auto dict : *dicts)
		{
			if (filesystem::exists(dict))
			{
				if (filesystem::is_directory(dict))
				{
					cout << "Dict directory: " << dict << endl;
					for (auto& file : std::filesystem::directory_iterator(dict))
					{
						const auto filePath = local::wide_u8(local::acp_wide(file.path().string()));
						const auto fileName = local::wide_u8(local::acp_wide(file.path().filename().string()));
						if (file.is_regular_file())
						{
							std::ifstream dict_stream{ filePath };

							if (!dict_stream.is_open())
								continue;

							cout << "Reading " << fileName << "..." << endl;

							rapidjson::IStreamWrapper wrapper{ dict_stream };
							rapidjson::Document document;

							document.ParseStream(wrapper);

							for (auto iter = document.MemberBegin();
								iter != document.MemberEnd(); ++iter)
							{
								auto key = std::stoull(iter->name.GetString());
								auto value = iter->value.GetString();

								text_db.emplace(key, value);
							}

							dict_stream.close();
						}
					}
				}
				else
				{
					std::ifstream dict_stream{ dict };

					if (!dict_stream.is_open())
						continue;

					cout << "Reading " << dict << "..." << endl;

					rapidjson::IStreamWrapper wrapper{ dict_stream };
					rapidjson::Document document;

					document.ParseStream(wrapper);

					for (auto iter = document.MemberBegin();
						iter != document.MemberEnd(); ++iter)
					{
						auto key = std::stoull(iter->name.GetString());
						auto value = iter->value.GetString();

						text_db.emplace(key, value);
					}

					dict_stream.close();
				}
			}
		}

		cout << "loaded " << text_db.size() << " localized entries." << endl;
	}

	void load_textId_textdb(const std::string& dict)
	{
		if (filesystem::exists(dict.data()))
		{
			std::ifstream dict_stream{ dict };

			if (!dict_stream.is_open())
				return;

			cout << "Reading " << dict << "..." << endl;

			rapidjson::IStreamWrapper wrapper{ dict_stream };
			rapidjson::Document document;

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

	bool localify_text(size_t hash, string** result)
	{
		if (text_db.contains(hash))
		{
			*result = &text_db[hash];
			return true;
		}

		return false;
	}

	bool localify_text_by_textId_name(const string& textIdName, string** result)
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
		string* result;

		if (local::localify_text(hash_or_id, &result))
		{
			return il2cpp_string_new(result->data());
		}

		return nullptr;
	}

	Il2CppString* get_localized_string(const string& textIdName)
	{
		string* result;

		if (local::localify_text_by_textId_name(textIdName, &result))
		{
			return il2cpp_string_new(result->data());
		}

		return nullptr;
	}

	Il2CppString* get_localized_string(Il2CppString* str)
	{
		string* result;

		auto hash = std::hash<wstring>{}(str->start_char);

		if (local::localify_text(hash, &result))
		{
			return il2cpp_string_new(result->data());
		}

		if (g_enable_logger && !std::any_of(str_list.begin(), str_list.end(), [hash](size_t hash1) { return hash1 == hash; }))
		{
			str_list.emplace_back(hash);

			logger::write_entry(hash, str->start_char);
		}

		return str;
	}
}
