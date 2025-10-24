#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "RaceUtil.hpp"

namespace
{
	void* GetRaceDynamicCameraSettingData_addr = nullptr;
	void* GetRaceDynamicCameraSettingData_orig = nullptr;

	void* IsEnableDynamicRaceType_addr = nullptr;
	void* IsEnableDynamicRaceType_orig = nullptr;
}

static bool GetRaceDynamicCameraSettingData_hook(uint64_t bootMode)
{
	auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SaveDataManager"));
	auto SaveLoader = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)->methodPointer(SaveDataManager);
	bool IsTryRaceDynamicCamera = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(SaveLoader->klass, "get_IsTryRaceDynamicCamera", 0)->methodPointer(SaveLoader);
	return IsTryRaceDynamicCamera;
}

static bool IsEnableDynamicRaceType_hook(uint64_t raceType)
{
	return true;
}

static void InitAddress()
{
	GetRaceDynamicCameraSettingData_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceUtil", "GetRaceDynamicCameraSettingData", 1);
	IsEnableDynamicRaceType_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceUtil", "IsEnableDynamicRaceType", 1);
}

static void HookMethods()
{
	ADD_HOOK(GetRaceDynamicCameraSettingData, "Gallop.RaceUtil::GetRaceDynamicCameraSettingData at %p\n");
	ADD_HOOK(IsEnableDynamicRaceType, "Gallop.RaceUtil::IsEnableDynamicRaceType at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	bool RaceUtil::GetRaceDynamicCameraSettingData(uint64_t bootMode)
	{
		return reinterpret_cast<decltype(GetRaceDynamicCameraSettingData)*>(GetRaceDynamicCameraSettingData_addr)(bootMode);
	}

	bool RaceUtil::IsEnableDynamicRaceType(uint64_t raceType)
	{
		return reinterpret_cast<decltype(IsEnableDynamicRaceType)*>(IsEnableDynamicRaceType_addr)(raceType);
	}
}
