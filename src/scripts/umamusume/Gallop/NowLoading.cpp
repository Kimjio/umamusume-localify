#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "NowLoading.hpp"

#include "config/config.hpp"

namespace
{
	void* NowLoading_Show_addr = nullptr;
	void* NowLoading_Show_orig = nullptr;

	void* NowLoading_Show3_addr = nullptr;
	void* NowLoading_Show3_orig = nullptr;

	void* NowLoading_Hide_addr = nullptr;
	void* NowLoading_Hide_orig = nullptr;

	void* NowLoading_Hide1_addr = nullptr;
	void* NowLoading_Hide1_orig = nullptr;
}

static void NowLoading_Show_hook(Il2CppObject* self, int type, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType, Il2CppObject* customInEffect, Il2CppObject* customLoopEffect, Il2CppObject* customOutEffect, int charaId)
{
	// NowLoadingOrientation
	if (type == 2 && (config::freeform_window || !config::ui_loading_show_orientation_guide))
	{
		// NowLoadingTips
		type = 0;
	}
	if (!config::hide_now_loading)
	{
		reinterpret_cast<decltype(NowLoading_Show_hook)*>(NowLoading_Show_orig)(
			self,
			type,
			onComplete, overrideDuration, easeType,
			customInEffect, customLoopEffect, customOutEffect, charaId);
	}
	if (onComplete && config::hide_now_loading)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
	}
}

static void NowLoading_Show3_hook(Il2CppObject* self, int type, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration)
{
	// NowLoadingOrientation
	if (type == 2 && (config::freeform_window || !config::ui_loading_show_orientation_guide))
	{
		// NowLoadingTips
		type = 0;
	}
	if (!config::hide_now_loading)
	{
		reinterpret_cast<decltype(NowLoading_Show3_hook)*>(NowLoading_Show3_orig)(
			self,
			type,
			onComplete, overrideDuration);
	}
	if (onComplete && config::hide_now_loading)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
	}
}

static void NowLoading_Hide_hook(Il2CppObject* self, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType, Il2CppDelegate* onUnloadCustomEffectResourcesComplete)
{
	if (!config::hide_now_loading)
	{
		reinterpret_cast<decltype(NowLoading_Hide_hook)*>(NowLoading_Hide_orig)(self, onComplete, overrideDuration, easeType, onUnloadCustomEffectResourcesComplete);
	}
	if (onComplete && config::hide_now_loading)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
	}
}

static void NowLoading_Hide1_hook(Il2CppObject* self, Il2CppDelegate* onComplete)
{
	if (!config::hide_now_loading)
	{
		reinterpret_cast<decltype(NowLoading_Hide1_hook)*>(NowLoading_Hide1_orig)(self, onComplete);
	}
	if (onComplete && config::hide_now_loading)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
	}
}

static void InitAddress()
{
	auto NowLoading_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "NowLoading");
	NowLoading_Show_addr = il2cpp_symbols::get_method_pointer(NowLoading_klass, "Show", 8);
	NowLoading_Hide_addr = il2cpp_symbols::get_method_pointer(NowLoading_klass, "Hide", 4);
	NowLoading_Show3_addr = il2cpp_symbols::get_method_pointer(NowLoading_klass, "Show", 3);
	NowLoading_Hide1_addr = il2cpp_symbols::get_method_pointer(NowLoading_klass, "Hide", 1);
}

static void HookMethods()
{
	ADD_HOOK(NowLoading_Show, "Gallop.NowLoading::Show at %p\n");
	ADD_HOOK(NowLoading_Hide, "Gallop.NowLoading::Hide at %p\n");
	ADD_HOOK(NowLoading_Show3, "Gallop.NowLoading::Show3 at %p\n");
	ADD_HOOK(NowLoading_Hide1, "Gallop.NowLoading::Hide1 at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
