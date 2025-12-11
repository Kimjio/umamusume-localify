#include <stdinclude.hpp>

#include "config/config.hpp"

#include "string_utils.hpp"

using namespace std;

namespace local
{
	namespace
	{
		unordered_map<size_t, u16string> text_db;
		unordered_map<u16string, u16string> textId_text_db;
		vector<size_t> str_list;
	}

	static void reload_textdb(const vector<u16string>* dicts)
	{
		text_db.clear();
		load_textdb(dicts);
	}

	static void reload_textId_textdb(const u16string& dict)
	{
		textId_text_db.clear();
		load_textId_textdb(dict);
	}

	static void load_textdb(const vector<u16string>* dicts)
	{
		for (u16string dict : *dicts)
		{
			if (filesystem::exists(dict))
			{
				if (filesystem::is_directory(dict))
				{
					wcout << L"Dict directory: " << u16_wide(dict) << endl;
					for (auto& file : filesystem::directory_iterator(dict))
					{
						const auto filePath = file.path().u16string();
						const auto fileName = file.path().filename().u16string();
						if (file.is_regular_file())
						{
							u16ifstream dict_stream{ u16_wide(filePath) };
							dict_stream.imbue(locale(".UTF-8"));

							if (!dict_stream.is_open())
							{
								continue;
							}

							wcout << L"Reading " << u16_wide(fileName) << L"..." << endl;

							U16IStreamWrapper wrapper{ dict_stream };
							U16Document document;

							document.ParseStream(wrapper);

							for (auto iter = document.MemberBegin();
								iter != document.MemberEnd(); ++iter)
							{
								auto key = stoull(u16_wide(iter->name.GetString()));
								auto value = iter->value.GetString();

								text_db.emplace(key, value);
							}

							dict_stream.close();
						}
					}
				}
				else
				{
					u16ifstream dict_stream{ u16_wide(dict) };
					dict_stream.imbue(locale(".UTF-8"));

					if (!dict_stream.is_open())
					{
						continue;
					}

					wcout << L"Reading " << u16_wide(dict) << L"..." << endl;

					U16IStreamWrapper wrapper{ dict_stream };
					U16Document document;

					document.ParseStream(wrapper);

					for (auto iter = document.MemberBegin();
						iter != document.MemberEnd(); ++iter)
					{
						auto key = stoull(u16_wide(iter->name.GetString()));
						auto value = iter->value.GetString();

						text_db.emplace(key, value);
					}

					dict_stream.close();
				}
			}
		}

		cout << "loaded " << text_db.size() << " localized entries." << endl;
	}

	static void load_textId_textdb(const u16string& dict)
	{
		if (filesystem::exists(dict.data()))
		{
			u16ifstream dict_stream{ u16_wide(dict) };
			dict_stream.imbue(locale(".UTF-8"));

			if (!dict_stream.is_open())
				return;

			wcout << L"Reading " << u16_wide(dict) << L"..." << endl;

			U16IStreamWrapper wrapper{ dict_stream };
			U16Document document;

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

	static bool localify_text(size_t hash, u16string** result)
	{
		if (text_db.contains(hash))
		{
			*result = &text_db[hash];
			return true;
		}

		return false;
	}

	static bool localify_text_by_textId_name(const u16string& textIdName, u16string** result)
	{
		if (textId_text_db.contains(textIdName))
		{
			*result = &textId_text_db[textIdName];
			return true;
		}
		return false;
	}

	static Il2CppString* get_localized_string(size_t hash_or_id)
	{
		u16string* result;

		if (local::localify_text(hash_or_id, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	static Il2CppString* get_localized_string(const u16string& textIdName)
	{
		u16string* result;

		if (local::localify_text_by_textId_name(textIdName, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	static Il2CppString* get_localized_string(Il2CppString* str)
	{
		u16string* result;

		auto hash = std::hash<u16string>{}(str->chars);

		if (local::localify_text(hash, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		if (config::enable_logger && !any_of(str_list.begin(), str_list.end(), [hash](size_t hash1) { return hash1 == hash; }))
		{
			str_list.emplace_back(hash);

			logger::write_entry(hash, str->chars);
		}

		return str;
	}

	static const char* get_localized_string(const char* str)
	{
		if (!str)
		{
			return nullptr;
		}

		u16string u16str = u8_u16(str);

		u16string* result;

		auto hash = std::hash<u16string>{}(u16str);

		if (local::localify_text(hash, &result))
		{
			string resultU8 = u16_u8(*result);
			auto dest = new char[resultU8.size() + 1];
			strncpy(dest, resultU8.data(), resultU8.size());
			dest[resultU8.size()] = '\0';
			return dest;
		}

		if (config::enable_logger && !any_of(str_list.begin(), str_list.end(), [hash](size_t hash1) { return hash1 == hash; }))
		{
			str_list.emplace_back(hash);

			logger::write_entry(hash, u16str);
		}

		return str;
	}
}
