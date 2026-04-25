#include "../CriMw.CriWare.Runtime.hpp"
#include "../../ScriptInternal.hpp"
#include "CriAtomExAcb.hpp"

#include "CriAtomEx.hpp"

#include <regex>

#include "localify/NotificationManager.hpp"

#include "string_utils.hpp"

namespace
{
	void* criAtomExAcb_GetCueInfoById_addr = nullptr;
	void* criAtomExAcb_GetCueInfoById_orig = nullptr;

	void* criAtomExAcb_GetCueInfoByName_addr = nullptr;
	void* criAtomExAcb_GetCueInfoByName_orig = nullptr;

	void* criAtomExAcb_Release_addr = nullptr;
	void* criAtomExAcb_Release_orig = nullptr;
}

static bool criAtomExAcb_GetCueInfoById_hook(void* acb_hn, int id, CriWare::CriAtomEx::CueInfo* info)
{
	auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoById_hook)*>(criAtomExAcb_GetCueInfoById_orig)(acb_hn, id, info);

	if (!info || !info->name)
	{
		return result;
	}
	const regex r(R"(_(?:9)*(\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
	smatch stringMatch;
	const auto cueSheet = string(info->name);
	const auto cueSheet16 = u8_il2cpp(cueSheet);
	regex_search(cueSheet, stringMatch, r);
	if (!stringMatch.empty())
	{
		auto charaIdStr = stringMatch[1].str();
		if (!stringMatch[2].str().empty() && !stringMatch[3].str().empty())
		{
			auto index = stoi(stringMatch[3].str());

			if (index == 1)
			{
				charaIdStr = stringMatch[2].str();
			}
		}

		Il2CppObject* textList =
			il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int)>("umamusume.dll", "Gallop", "MasterCharacterSystemText", "GetByCharaId", 1)(stoi(charaIdStr));
		if (textList)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(textList->klass, "_items");
			Il2CppArraySize_t<Il2CppObject*>* textArr;
			il2cpp_field_get_value(textList, itemsField, &textArr);

			if (textArr)
			{
				for (int i = 0; i < textArr->max_length; i++)
				{
					auto elem = textArr->vector[i];
					if (elem)
					{
						auto elemCueIdField = il2cpp_class_get_field_from_name(elem->klass, "CueId");
						auto elemCueSheetField = il2cpp_class_get_field_from_name(elem->klass, "CueSheet");

						Il2CppString* elemCueSheet;
						il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

						int elemCueId;
						il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

						if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->chars) && info->id == elemCueId) {
							Localify::NotificationManager::currentAcbHandles.insert(acb_hn);
							Localify::NotificationManager::currentElem = elem;
							break;
						}
					}
				}
			}
		}
	}

	return result;
}

static bool criAtomExAcb_GetCueInfoByName_hook(void* acb_hn, char* name, CriWare::CriAtomEx::CueInfo* info)
{
	auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoByName_hook)*>(criAtomExAcb_GetCueInfoByName_orig)(acb_hn, name, info);

	if (!info || !info->name)
	{
		return result;
	}

	const regex r(R"((\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
	smatch stringMatch;
	const auto cueSheet = string(info->name);
	const auto cueSheet16 = u8_il2cpp(cueSheet);
	regex_search(cueSheet, stringMatch, r);
	if (!stringMatch.empty())
	{
		auto charaIdStr = stringMatch[1].str();
		if (!stringMatch[2].str().empty() && !stringMatch[3].str().empty())
		{
			auto index = stoi(stringMatch[3].str());

			if (index == 1)
			{
				charaIdStr = stringMatch[2].str();
			}
		}

		Il2CppObject* textList =
			il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int)>("umamusume.dll", "Gallop", "MasterCharacterSystemText", "GetByCharaId", 1)(stoi(charaIdStr));
		if (textList)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(textList->klass, "_items");
			Il2CppArraySize_t<Il2CppObject*>* textArr;
			il2cpp_field_get_value(textList, itemsField, &textArr);

			if (textArr)
			{
				for (int i = 0; i < textArr->max_length; i++)
				{
					auto elem = textArr->vector[i];
					if (elem)
					{
						auto elemCueIdField = il2cpp_class_get_field_from_name(elem->klass, "CueId");
						auto elemCueSheetField = il2cpp_class_get_field_from_name(elem->klass, "CueSheet");

						Il2CppString* elemCueSheet;
						il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

						int elemCueId;
						il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

						if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->chars) && info->id == elemCueId)
						{
							Localify::NotificationManager::currentAcbHandles.insert(acb_hn);
							Localify::NotificationManager::currentElem = elem;
							break;
						}
					}
				}
			}
		}
	}

	return result;
}

static void criAtomExAcb_Release_hook(void* acb_hn)
{
	if (Localify::NotificationManager::currentAcbHandles.contains(acb_hn))
	{
		Localify::NotificationManager::currentAcbHandles.erase(acb_hn);
	}
	reinterpret_cast<decltype(criAtomExAcb_Release_hook)*>(criAtomExAcb_Release_orig)(acb_hn);
}

static void InitAddress()
{
#ifdef _MSC_VER
	wstring module_name;
	module_name.resize(MAX_PATH);
	module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

	filesystem::path module_path(module_name);

	wstring name = module_path.filename().replace_extension();

	SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());

	auto criware = LoadLibraryExW(L"cri_ware_unity.dll", nullptr, 0);

	SetDllDirectoryW(nullptr);

	criAtomExAcb_GetCueInfoById_addr = GetProcAddress(criware, "criAtomExAcb_GetCueInfoById");
	criAtomExAcb_GetCueInfoByName_addr = GetProcAddress(criware, "criAtomExAcb_GetCueInfoByName");
	criAtomExAcb_Release_addr = GetProcAddress(criware, "criAtomExAcb_Release");
#else
	auto criware = dlopen("libcri_ware_unity.so", RTLD_NOW);

	criAtomExAcb_GetCueInfoById_addr = dlsym(criware, "criAtomExAcb_GetCueInfoById");
	criAtomExAcb_GetCueInfoByName_addr = dlsym(criware, "criAtomExAcb_GetCueInfoByName");
	criAtomExAcb_Release_addr = dlsym(criware, "criAtomExAcb_Release");
#endif
}

static void HookMethods()
{
	if (config::character_system_text_caption)
	{
		ADD_HOOK(criAtomExAcb_GetCueInfoById, "CriWare.CriAtomExAcb::criAtomExAcb_GetCueInfoById");
		ADD_HOOK(criAtomExAcb_GetCueInfoByName, "CriWare.CriAtomExAcb::criAtomExAcb_GetCueInfoByName");
		ADD_HOOK(criAtomExAcb_Release, "CriWare.CriAtomExAcb::criAtomExAcb_Release");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
