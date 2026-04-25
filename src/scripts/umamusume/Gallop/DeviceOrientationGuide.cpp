#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DeviceOrientationGuide.hpp"

#include <cstdint>

#include "scripts/UnityEngine.CoreModule/UnityEngine/Color.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Color32.hpp"
#include "scripts/mscorlib/System/Collections/Generic/Dictionary.hpp"

#include "config/config.hpp"

#include "local/local.hpp"

namespace
{
	void* DeviceOrientationGuide_Show_addr = nullptr;
	void* DeviceOrientationGuide_Show_orig = nullptr;
}

static void DeviceOrientationGuide_Show_hook(Il2CppObject* self, bool isTargetOrientationPortrait, int target)
{
	if (!config::freeform_window)
	{
		reinterpret_cast<decltype(DeviceOrientationGuide_Show_hook)*>(DeviceOrientationGuide_Show_orig)(self, isTargetOrientationPortrait, target);
	}
}

static void InitAddress()
{
	DeviceOrientationGuide_Show_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DeviceOrientationGuide", "Show", 2);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(DeviceOrientationGuide_Show, "DeviceOrientationGuide::Show at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
