#include "../../umamusume.hpp"
#include "../../../ScriptInternal.hpp"
#include "Director.hpp"
#include "Master3dLive.hpp"

#include "../Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* Director_SetupOrientation_addr = nullptr;
	void* Director_SetupOrientation_orig = nullptr;

	void* Director_Live3DSettings_get_ScreenMode_addr = nullptr;

	void* Director_Live3DSettings_set_ScreenMode_addr = nullptr;

	void* Director_Live3DSettings_SetData_addr = nullptr;
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

	ADD_HOOK(Director_Live3DSettings_SetData, "Gallop.Live.Director.Live3DSettings::SetData at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop::Live
{
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
