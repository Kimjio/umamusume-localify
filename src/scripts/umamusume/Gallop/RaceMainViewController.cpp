#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "RaceMainViewController.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer RaceMainViewController_SetLandscapeModeSkillCutInBeltEnable_addr = nullptr;
	void* RaceMainViewController_SetLandscapeModeSkillCutInBeltEnable_orig = nullptr;
}

static void RaceMainViewController_SetLandscapeModeSkillCutInBeltEnable_hook(Il2CppObject* self, bool enable, Il2CppObject* renderTexture, Il2CppObject* material)
{
}

static void InitAddress()
{
	auto RaceMainViewController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "RaceMainViewController");
	RaceMainViewController_SetLandscapeModeSkillCutInBeltEnable_addr = il2cpp_symbols::get_method_pointer(RaceMainViewController_klass, "SetLandscapeModeSkillCutInBeltEnable", 3);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(RaceMainViewController_SetLandscapeModeSkillCutInBeltEnable, "Gallop.RaceMainViewController::SetLandscapeModeSkillCutInBeltEnable at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
