#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "RaceUtil.hpp"

namespace
{
	Il2CppMethodPointer GetRaceDynamicCameraSettingData_addr = nullptr;
	void* GetRaceDynamicCameraSettingData_orig = nullptr;

	Il2CppMethodPointer IsEnableDynamicRaceType_addr = nullptr;
	void* IsEnableDynamicRaceType_orig = nullptr;
}

static void InitAddress()
{
	GetRaceDynamicCameraSettingData_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceUtil", "GetRaceDynamicCameraSettingData", 1);
	IsEnableDynamicRaceType_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceUtil", "IsEnableDynamicRaceType", 1);
}

static void HookMethods()
{
	auto GetRaceDynamicCameraSettingData_hook = Gallop::RaceUtil::GetRaceDynamicCameraSettingData;
	ADD_HOOK(GetRaceDynamicCameraSettingData, "Gallop.RaceUtil::GetRaceDynamicCameraSettingData at %p\n");

	auto IsEnableDynamicRaceType_hook = Gallop::RaceUtil::IsEnableDynamicRaceType;
	ADD_HOOK(IsEnableDynamicRaceType, "Gallop.RaceUtil::IsEnableDynamicRaceType at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	bool RaceUtil::GetRaceDynamicCameraSettingData(int bootMode)
	{
		auto RaceInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>(ASSEMBLY_NAME, "Gallop", "RaceManager", "get_RaceInfo", 0)();

		if (RaceInfo)
		{
			auto RaceType = il2cpp_symbols::get_method_pointer<int(*)(Il2CppObject*)>(RaceInfo->klass, "get_RaceType", 0)(RaceInfo);

			if (RaceType == 3)
			{
				return false;
			}
		}

		auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SaveDataManager"));
		auto SaveLoader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)(SaveDataManager);
		bool IsTryRaceDynamicCamera = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>(SaveLoader->klass, "get_IsTryRaceDynamicCamera", 0)(SaveLoader);
		return IsTryRaceDynamicCamera;
	}

	bool RaceUtil::IsEnableDynamicRaceType(int raceType)
	{
		return true;
	}
}
