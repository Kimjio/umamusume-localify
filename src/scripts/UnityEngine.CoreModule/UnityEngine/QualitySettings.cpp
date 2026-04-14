#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "QualitySettings.hpp"

#include "config/config.hpp"

namespace
{
	void* set_anisotropicFiltering_addr = nullptr;
	void* set_anisotropicFiltering_orig = nullptr;

	void* set_vSyncCount_addr = nullptr;
	void* set_vSyncCount_orig = nullptr;

	void* set_antiAliasing_addr = nullptr;
	void* set_antiAliasing_orig = nullptr;
}

static void set_anisotropicFiltering_hook(int mode)
{
	reinterpret_cast<decltype(set_anisotropicFiltering_hook)*>(set_anisotropicFiltering_orig)(config::anisotropic_filtering);
}

static void set_vSyncCount_hook(int level)
{
	reinterpret_cast<decltype(set_vSyncCount_hook)*>(set_vSyncCount_orig)(config::vsync_count);
}

static void set_antiAliasing_hook(int level)
{
	if (config::anti_aliasing < 0)
	{
		reinterpret_cast<decltype(set_antiAliasing_hook)*>(set_antiAliasing_orig)(level);
		return;
	}

	reinterpret_cast<decltype(set_antiAliasing_hook)*>(set_antiAliasing_orig)(config::anti_aliasing);
}

static void InitAddress()
{
	set_anisotropicFiltering_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_anisotropicFiltering(UnityEngine.AnisotropicFiltering)");
	set_vSyncCount_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_vSyncCount()");
	set_antiAliasing_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_antiAliasing(System.Int32)");
}

static void HookMethods()
{
	if (config::anisotropic_filtering != -1)
	{
		ADD_HOOK(set_anisotropicFiltering, "UnityEngine.QualitySettings.set_anisotropicFiltering(UnityEngine.AnisotropicFiltering) at %p\n");
	}

	if (config::vsync_count != -1)
	{
		ADD_HOOK(set_vSyncCount, "UnityEngine.QualitySettings.set_vSyncCount() at %p\n");
	}

	if (config::anti_aliasing != -1)
	{
		ADD_HOOK(set_antiAliasing, "UnityEngine.QualitySettings::set_antiAliasing at %p\n");
	}
}

static void LateInit()
{
	if (config::anisotropic_filtering != -1)
	{
		set_anisotropicFiltering_hook(config::anisotropic_filtering);
	}

	if (config::vsync_count != -1)
	{
		set_vSyncCount_hook(config::vsync_count);
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
	il2cpp_symbols::late_init_callbacks.emplace_back(LateInit);
}

namespace UnityEngine
{
	void QualitySettings::anisotropicFiltering(int value)
	{
		reinterpret_cast<void (*)(int)>(set_anisotropicFiltering_addr)(value);
	}
	void QualitySettings::vSyncCount(int value)
	{
		reinterpret_cast<void (*)(int)>(set_vSyncCount_addr)(value);
	}

	void QualitySettings::antiAliasing(int value)
	{
		reinterpret_cast<void (*)(int)>(set_antiAliasing_addr)(value);
	}
}
