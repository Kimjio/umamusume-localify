#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StorySceneController.hpp"

#include "config/config.hpp"

namespace
{
	void* StorySceneController_UpdateFovFactor_addr = nullptr;
	void* StorySceneController_UpdateFovFactor_orig = nullptr;
}

static void StorySceneController_UpdateFovFactor_hook(Il2CppObject* self, float fovFactor)
{
	reinterpret_cast<decltype(StorySceneController_UpdateFovFactor_hook)*>(StorySceneController_UpdateFovFactor_orig)(self, 1);
}

static void InitAddress()
{
	StorySceneController_UpdateFovFactor_addr = il2cpp_symbols::find_method(ASSEMBLY_NAME, "Gallop", "StorySceneController", [](const MethodInfo* info)
		{
			return info->name == "UpdateFovFactor"s && info->parameters[0]->type == Il2CppTypeEnum::IL2CPP_TYPE_R4;
		}
	);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(StorySceneController_UpdateFovFactor, "Gallop.StorySceneController::UpdateFovFactor at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
