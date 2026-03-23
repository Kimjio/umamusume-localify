#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LiveViewController.hpp"

#include "Screen.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppClass* LiveViewController_klass = nullptr;

	void* LiveViewController_SetOrientationLandscape_addr = nullptr;

	void* LiveViewController_SetOrientationPortrait_addr = nullptr;
	void* LiveViewController_SetOrientationPortrait_orig = nullptr;
}

static void LiveViewController_SetOrientationPortrait_hook(Il2CppObject* self)
{
	if (!Gallop::Screen::IsVertical())
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(LiveViewController_SetOrientationLandscape_addr)(self);
		return;
	}

	reinterpret_cast<decltype(LiveViewController_SetOrientationPortrait_hook)*>(LiveViewController_SetOrientationPortrait_orig)(self);
}

static void InitAddress()
{
	LiveViewController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LiveViewController");
	LiveViewController_SetOrientationLandscape_addr = il2cpp_symbols::get_method_pointer(LiveViewController_klass, "SetOrientationLandscape", 0);
	LiveViewController_SetOrientationPortrait_addr = il2cpp_symbols::get_method_pointer(LiveViewController_klass, "SetOrientationPortrait", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(LiveViewController_SetOrientationPortrait, "Gallop.LiveViewController::SetOrientationPortrait at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}

