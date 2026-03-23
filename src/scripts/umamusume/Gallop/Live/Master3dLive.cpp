#include "../../umamusume.hpp"
#include "../../../ScriptInternal.hpp"
#include "Master3dLive.hpp"

namespace
{
	Il2CppClass* Master3dLive_klass = nullptr;
	Il2CppClass* Master3dLive_Live3dData_klass = nullptr;

	void* Master3dLive_ctor_addr = nullptr;

	void* Master3dLive_Live3dData_get_ScreenMode_addr = nullptr;

	void* Master3dLive_Live3dData_set_ScreenMode_addr = nullptr;

	void* Master3dLive_Live3dData_ctor_addr = nullptr;

	FieldInfo* Master3dLive_LiveData = nullptr;
}

static void InitAddress()
{
	Master3dLive_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop.Live", "Master3dLive");
	Master3dLive_Live3dData_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop.Live", "Master3dLive/Live3dData");
	Master3dLive_ctor_addr = il2cpp_symbols::get_method_pointer(Master3dLive_klass, ".ctor", 1);
	Master3dLive_LiveData = il2cpp_class_get_field_from_name(Master3dLive_klass, "LiveData");
	Master3dLive_Live3dData_get_ScreenMode_addr = il2cpp_symbols::get_method_pointer(Master3dLive_Live3dData_klass, "get_ScreenMode", 0);
	Master3dLive_Live3dData_set_ScreenMode_addr = il2cpp_symbols::get_method_pointer(Master3dLive_Live3dData_klass, "set_ScreenMode", 1);
	Master3dLive_Live3dData_ctor_addr = il2cpp_symbols::get_method_pointer(Master3dLive_Live3dData_klass, ".ctor", 2);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	namespace Live
	{
		Master3dLive::Master3dLive(int musicId) : Object(nullptr)
		{
			instance = il2cpp_object_new(Master3dLive_klass);
			reinterpret_cast<void (*)(Il2CppObject*, int)>(Master3dLive_ctor_addr)(instance, musicId);
		}

		Master3dLive::Live3dData Master3dLive::LiveData()
		{
			Il2CppObject* LiveData;
			il2cpp_field_get_value(instance, Master3dLive_LiveData, &LiveData);
			return LiveData;
		}

		void Master3dLive::Live3dData::ScreenMode(int value)
		{
			if (!Master3dLive_Live3dData_set_ScreenMode_addr)
			{
				return;
			}

			reinterpret_cast<void (*)(Il2CppObject*, int)>(Master3dLive_Live3dData_set_ScreenMode_addr)(instance, value);
		}

		int Master3dLive::Live3dData::ScreenMode()
		{
			if (!Master3dLive_Live3dData_get_ScreenMode_addr)
			{
				return 0;
			}

			return reinterpret_cast<int (*)(Il2CppObject*)>(Master3dLive_Live3dData_get_ScreenMode_addr)(instance);
		}
	}
}
