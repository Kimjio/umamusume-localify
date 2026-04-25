#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "BGManager.hpp"

#include "Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* BGManager_SetMainBgLocalPos_addr = nullptr;
	void* BGManager_SetMainBgLocalPos_orig = nullptr;

	void* BGManager_SetMainBg_addr = nullptr;
	void* BGManager_SetMainBg_orig = nullptr;
}

static void BGManager_SetMainBgLocalPos_hook(Il2CppObject* self, UnityEngine::Vector2 pos)
{
	// no-op
}

static void BGManager_SetMainBg_hook(Il2CppObject* self, Il2CppString* path, int width, int height, float posX, float posY, bool isSetScale)
{
	float ratio = static_cast<float>(width) / static_cast<float>(height);
	int width1 = Gallop::Screen::Width();
	int height1 = Gallop::Screen::Height();

	if (width < height)
	{
		width = width1;
		height = static_cast<int>(width / ratio);
	}
	else
	{
		height = height1;
		width = static_cast<int>(height * ratio);
	}

	reinterpret_cast<decltype(BGManager_SetMainBg_hook)*>(BGManager_SetMainBg_orig)(self, path, width, height, 0, 0, true);
}

static void InitAddress()
{
	BGManager_SetMainBgLocalPos_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "BGManager", "SetMainBgLocalPos", 1);
	BGManager_SetMainBg_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "BGManager", "SetMainBg", 6);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(BGManager_SetMainBgLocalPos, "Gallop.BGManager::SetMainBgLocalPos at %p\n");
	}

	if (config::freeform_window || config::unlock_size || config::resolution_3d_scale != 1.0f)
	{
		ADD_HOOK(BGManager_SetMainBg, "Gallop.BGManager::SetMainBg at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
