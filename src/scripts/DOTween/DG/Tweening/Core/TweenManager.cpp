#include "scripts/DOTween/DOTween.hpp"
#include "scripts/ScriptInternal.hpp"
#include "TweenManager.hpp"

#include "config/config.hpp"

namespace
{
	void* TweenManager_Update_addr = nullptr;
	void* TweenManager_Update_orig = nullptr;
}

static void* TweenManager_Update_hook(int updateType, float deltaTime, float independentTime)
{
	return reinterpret_cast<decltype(TweenManager_Update_hook)*>(TweenManager_Update_orig)(updateType, deltaTime * config::ui_animation_scale, independentTime * config::ui_animation_scale);
}

static void InitAddress()
{
	TweenManager_Update_addr = il2cpp_symbols::get_method_pointer("DOTween.dll", "DG.Tweening.Core", "TweenManager", "Update", 3);
}

static void HookMethods()
{
	ADD_HOOK(TweenManager_Update, "DG.Tweening.Core.TweenManager::Update at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
