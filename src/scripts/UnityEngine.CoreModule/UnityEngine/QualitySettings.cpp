#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "QualitySettings.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer set_anisotropicFiltering_addr = nullptr;

	Il2CppMethodPointer set_vSyncCount_addr = nullptr;

	Il2CppMethodPointer set_antiAliasing_addr = nullptr;
}

static void set_anisotropicFiltering_hook(int mode)
{
	reinterpret_cast<decltype(set_anisotropicFiltering_hook)*>(set_anisotropicFiltering_addr)(config::anisotropic_filtering);
}

static void set_vSyncCount_hook(int level)
{
	reinterpret_cast<decltype(set_vSyncCount_hook)*>(set_vSyncCount_addr)(config::vsync_count);
}

static void set_antiAliasing_hook(int level)
{
	if (config::anti_aliasing < 0)
	{
		reinterpret_cast<decltype(set_antiAliasing_hook)*>(set_antiAliasing_addr)(level);
		return;
	}

	reinterpret_cast<decltype(set_antiAliasing_hook)*>(set_antiAliasing_addr)(config::anti_aliasing);
}

static void InitAddress()
{
	set_anisotropicFiltering_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_anisotropicFiltering");
	set_vSyncCount_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_vSyncCount");
	set_antiAliasing_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_antiAliasing");
}

static void HookMethods()
{
	if (config::anisotropic_filtering != -1)
	{
		il2cpp_add_internal_call("UnityEngine.QualitySettings.set_anisotropicFiltering", reinterpret_cast<Il2CppMethodPointer>(set_anisotropicFiltering_hook));
	}

	if (config::vsync_count != -1)
	{
		il2cpp_add_internal_call("UnityEngine.QualitySettings.set_vSyncCount", reinterpret_cast<Il2CppMethodPointer>(set_vSyncCount_hook));
	}

	if (config::anti_aliasing != -1)
	{
		il2cpp_add_internal_call("UnityEngine.QualitySettings::set_antiAliasing", reinterpret_cast<Il2CppMethodPointer>(set_antiAliasing_hook));
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
