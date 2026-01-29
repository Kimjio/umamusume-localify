#include <stdinclude.hpp>

#include "config/config.hpp"

#include "string_utils.hpp"

using namespace std;

namespace local
{
	namespace
	{
		unordered_map<size_t, il2cppstring> text_db;
		unordered_map<il2cppstring, il2cppstring> textId_text_db;
		vector<size_t> str_list;
	}

	static void reload_textdb(const vector<il2cppstring>* dicts)
	{
		text_db.clear();
		load_textdb(dicts);
	}

	static void reload_textId_textdb(const il2cppstring& dict)
	{
		textId_text_db.clear();
		load_textId_textdb(dict);
	}

	static void load_textdb(const vector<il2cppstring>* dicts)
	{
		for (il2cppstring dict : *dicts)
		{
			if (filesystem::exists(dict))
			{
				if (filesystem::is_directory(dict))
				{
					wcout << L"Dict directory: " << dict << endl;
					for (auto& file : filesystem::directory_iterator(dict))
					{
						const auto filePath = file.path().IL2CPP_BASIC_STRING();
						const auto fileName = file.path().filename().IL2CPP_BASIC_STRING();
						if (file.is_regular_file())
						{
							il2cppifstream dict_stream{ filePath };
							dict_stream.imbue(locale(".UTF-8"));

							if (!dict_stream.is_open())
							{
								continue;
							}

							wcout << L"Reading " << fileName << L"..." << endl;

							U16IStreamWrapper wrapper{ dict_stream };
							U16Document document;

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
					il2cppifstream dict_stream{ dict };
					dict_stream.imbue(locale(".UTF-8"));

					if (!dict_stream.is_open())
					{
						continue;
					}

					wcout << L"Reading " << dict << L"..." << endl;

					U16IStreamWrapper wrapper{ dict_stream };
					U16Document document;

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

	static void load_textId_textdb(const il2cppstring& dict)
	{
		if (filesystem::exists(dict.data()))
		{
			il2cppifstream dict_stream{ dict };
			dict_stream.imbue(locale(".UTF-8"));

			if (!dict_stream.is_open())
				return;

			wcout << L"Reading " << dict << L"..." << endl;

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

	static bool localify_text(size_t hash, il2cppstring** result)
	{
		if (text_db.contains(hash))
		{
			*result = &text_db[hash];
			return true;
		}

		return false;
	}

	static bool localify_text_by_textId_name(const il2cppstring& textIdName, il2cppstring** result)
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
		il2cppstring* result;

		if (local::localify_text(hash_or_id, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	static Il2CppString* get_localized_string(const il2cppstring& textIdName)
	{
		il2cppstring* result;

		if (local::localify_text_by_textId_name(textIdName, &result))
		{
			return il2cpp_string_new_utf16(result->data(), result->size());
		}

		return nullptr;
	}

	static Il2CppString* get_localized_string(Il2CppString* str)
	{
		il2cppstring* result;

		auto hash = std::hash<il2cppstring>{}(str->chars);

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

		il2cppstring u16str = u8_il2cpp(str);

		il2cppstring* result;

		auto hash = std::hash<il2cppstring>{}(u16str);

		if (local::localify_text(hash, &result))
		{
			string resultU8 = il2cpp_u8(*result);
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
