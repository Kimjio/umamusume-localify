#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "FrameRateController.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"

namespace
{
	void* FrameRateController_GetLayerFrameRate_addr = nullptr;
	void* FrameRateController_GetLayerFrameRate_orig = nullptr;

	void* FrameRateController_ReflectionFrameRate_addr = nullptr;
	void* FrameRateController_ReflectionFrameRate_orig = nullptr;

	void* FrameRateController_OnFinalize_addr = nullptr;
	void* FrameRateController_OnFinalize_orig = nullptr;
}

static int FrameRateController_GetLayerFrameRate_hook(Il2CppObject* self, Gallop::FrameRateController::FrameRateOverrideLayer layer)
{
	if (config::runtime::useDefaultFPS)
	{
		return reinterpret_cast<int (*)(Il2CppObject*, Gallop::FrameRateController::FrameRateOverrideLayer)>(FrameRateController_GetLayerFrameRate_orig)(self, layer);
	}
	return UnityEngine::Application::targetFrameRate();
}

static void FrameRateController_ReflectionFrameRate_hook(Il2CppObject* self)
{
	if (config::runtime::useDefaultFPS)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(FrameRateController_ReflectionFrameRate_orig)(self);
	}
	else
	{
		UnityEngine::Application::targetFrameRate(config::max_fps);
	}
}

static void FrameRateController_OnFinalize_hook(Il2CppObject* self)
{
	if (config::runtime::useDefaultFPS)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(FrameRateController_OnFinalize_orig)(self);
	}
	else
	{
		UnityEngine::Application::targetFrameRate(config::max_fps);
	}
}

static void InitAddress()
{
	auto FrameRateController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "FrameRateController");
	FrameRateController_GetLayerFrameRate_addr = il2cpp_symbols::get_method_pointer(FrameRateController_klass, "GetLayerFrameRate", 1);
	FrameRateController_ReflectionFrameRate_addr = il2cpp_symbols::get_method_pointer(FrameRateController_klass, "ReflectionFrameRate", 0);
	FrameRateController_OnFinalize_addr = il2cpp_symbols::get_method_pointer(FrameRateController_klass, "OnFinalize", 0);
}

static void HookMethods()
{
	if (config::max_fps > -1)
	{
		// break 30-40fps limit
		ADD_HOOK(FrameRateController_GetLayerFrameRate, "Gallop.FrameRateController::GetLayerFrameRate at %p\n");
		ADD_HOOK(FrameRateController_ReflectionFrameRate, "Gallop.FrameRateController::ReflectionFrameRate at %p\n");
		ADD_HOOK(FrameRateController_OnFinalize, "Gallop.FrameRateController::OnFinalize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* FrameRateController::namespaze = "Gallop";
	const char* FrameRateController::klassName = "FrameRateController";

	int FrameRateController::GetLayerFrameRate(FrameRateOverrideLayer layer)
	{
		return reinterpret_cast<int (*)(Il2CppObject*, FrameRateOverrideLayer)>(FrameRateController_GetLayerFrameRate_addr)(instance, layer);
	}
	
	void FrameRateController::ReflectionFrameRate()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(FrameRateController_ReflectionFrameRate_addr)(instance);
	}
}
