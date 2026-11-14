#include "../../umamusume.hpp"
#include "../../../ScriptInternal.hpp"
#include "Director.hpp"
#include "Master3dLive.hpp"

#include "../Screen.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer Director_SetupOrientation_addr = nullptr;
	void* Director_SetupOrientation_orig = nullptr;

	Il2CppMethodPointer Director_get_Instance_addr = nullptr;

    Il2CppMethodPointer Director_get_IsInitialized_addr = nullptr;

    Il2CppMethodPointer Director_get_IsStarted_addr = nullptr;

	Il2CppMethodPointer Director_RegisterDownloadExtraResource_addr = nullptr;
	void* Director_RegisterDownloadExtraResource_orig = nullptr;

	Il2CppMethodPointer Director_Live3DSettings_get_ScreenMode_addr = nullptr;

	Il2CppMethodPointer Director_Live3DSettings_set_ScreenMode_addr = nullptr;

	Il2CppMethodPointer Director_Live3DSettings_SetData_addr = nullptr;
	void* Director_Live3DSettings_SetData_orig = nullptr;
}

static void Director_SetupOrientation_hook(Il2CppObject* self, Gallop::Live::Director::DisplayMode targetDisplayMode)
{
	if (!Gallop::Screen::IsVertical())
	{
		targetDisplayMode = Gallop::Live::Director::DisplayMode::Landscape;
	}

	reinterpret_cast<decltype(Director_SetupOrientation_hook)*>(Director_SetupOrientation_orig)(self, targetDisplayMode);
}

static void Director_RegisterDownloadExtraResource_hook(Il2CppObject* _register, int extraResourceID)
{
	if (config::champions_live_show_text)
	{
		auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
		auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

		if (musicId == 1054)
		{
			auto raceInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_raceInfo", 0)->methodPointer(loadSettings);

			auto resourceId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_ChampionsMeetingResourceId", 0)->methodPointer(raceInfo);

			if (resourceId == 0)
			{
				auto charaNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_defaults.string_class, 9);
				auto trainerNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_defaults.string_class, 9);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArray", 1)->methodPointer(raceInfo, charaNameArray);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArray", 1)->methodPointer(raceInfo, trainerNameArray);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);

				auto charaInfoList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_CharacterInfoList", 0)->methodPointer(loadSettings);

				FieldInfo* itemsField = il2cpp_class_get_field_from_name(charaInfoList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* charaInfoArr;
				il2cpp_field_get_value(charaInfoList, itemsField, &charaInfoArr);

				for (int i = 0; i < 9; i++)
				{
					auto info = charaInfoArr->vector[i];
					auto charaId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_CharaId", 0)->methodPointer(info);

					Il2CppString* charaName;
					if (charaId == 1)
					{
						auto mobId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_MobId", 0)->methodPointer(info);
						charaName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, int)>("umamusume.dll", "Gallop", "TextUtil", "GetMasterText", 2)(59, mobId);
					}
					else
					{
						charaName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, int)>("umamusume.dll", "Gallop", "TextUtil", "GetMasterText", 2)(6, charaId);
					}

					il2cpp_array_setref(charaNameArray, i, charaName);
					il2cpp_array_setref(trainerNameArray, i, il2cpp_string_new(""));
				}

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(raceInfo->klass, "set_ChampionsMeetingResourceId", 1)->methodPointer(raceInfo, config::champions_live_resource_id);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(raceInfo->klass, "set_DateYear", 1)->methodPointer(raceInfo, config::champions_live_year);
			}
		}
	}

	reinterpret_cast<decltype(Director_RegisterDownloadExtraResource_hook)*>(Director_RegisterDownloadExtraResource_orig)(_register, extraResourceID);
}

static void Director_Live3DSettings_SetData_hook(Il2CppObject* self, Il2CppObject* info, Il2CppObject* loadSettings)
{
	if (config::runtime::LiveStartSettingIsOrientationFullPortrait.has_value())
	{
		if (config::runtime::LiveStartSettingIsOrientationFullPortrait.value())
		{
			Gallop::Live::Master3dLive::Live3dData(info).ScreenMode(1);
		}
		else
		{
			Gallop::Live::Master3dLive::Live3dData(info).ScreenMode(0);
		}
	}

	reinterpret_cast<decltype(Director_Live3DSettings_SetData_hook)*>(Director_Live3DSettings_SetData_orig)(self, info, loadSettings);
}

static void InitAddress()
{
	Director_SetupOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director", "SetupOrientation", 1);
	Director_get_Instance_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director", "get_Instance", 0);
    Director_get_IsInitialized_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director", "get_IsInitialized", 0);
	Director_get_IsStarted_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director", "get_IsStarted", 0);
	Director_RegisterDownloadExtraResource_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director", "RegisterDownloadExtraResource", 2);
	Director_Live3DSettings_get_ScreenMode_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director/Live3DSettings", "get_ScreenMode", 0);
	Director_Live3DSettings_set_ScreenMode_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director/Live3DSettings", "set_ScreenMode", 1);
	Director_Live3DSettings_SetData_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop.Live", "Director/Live3DSettings", "SetData", 2);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(Director_SetupOrientation, "Gallop.Live.Director::SetupOrientation at %p\n");
	}

	ADD_HOOK(Director_RegisterDownloadExtraResource, "Gallop.Live.Director::RegisterDownloadExtraResource at %p\n");
	ADD_HOOK(Director_Live3DSettings_SetData, "Gallop.Live.Director.Live3DSettings::SetData at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop::Live
{
	Director Director::Instance()
	{
		return reinterpret_cast<Il2CppObject* (*)()>(Director_get_Instance_addr)();
	}

    bool Director::IsInitialized()
    {
        return reinterpret_cast<bool (*)(Il2CppObject*)>(Director_get_IsInitialized_addr)(instance);
    }

    bool Director::IsStarted()
    {
        return reinterpret_cast<bool (*)(Il2CppObject*)>(Director_get_IsStarted_addr)(instance);
    }

	void Director::Live3DSettings::ScreenMode(int value)
	{
		if (!Director_Live3DSettings_set_ScreenMode_addr)
		{
			return;
		}

		reinterpret_cast<void (*)(Il2CppObject*, int)>(Director_Live3DSettings_set_ScreenMode_addr)(instance, value);
	}

	int Director::Live3DSettings::ScreenMode()
	{
		if (!Director_Live3DSettings_get_ScreenMode_addr)
		{
			return 0;
		}

		return reinterpret_cast<int (*)(Il2CppObject*)>(Director_Live3DSettings_get_ScreenMode_addr)(instance);
	}
}
