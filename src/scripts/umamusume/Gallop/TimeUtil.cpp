#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TimeUtil.hpp"

namespace
{
	Il2CppMethodPointer TimeUtil_GetSeasonForHome_addr = nullptr;
	void* TimeUtil_GetSeasonForHome_orig = nullptr;

	Il2CppMethodPointer TimeUtil_GetTimeZoneForHome_addr = nullptr;
	void* TimeUtil_GetTimeZoneForHome_orig = nullptr;
}

static int TimeUtil_GetSeasonForHome_hook(unsigned long dateTime)
{
	if (config::home_bg_override_bg_season > 0)
	{
		return config::home_bg_override_bg_season;
	}

	return reinterpret_cast<decltype(TimeUtil_GetSeasonForHome_hook)*>(TimeUtil_GetSeasonForHome_orig)(dateTime);
}

static int TimeUtil_GetTimeZoneForHome_hook(unsigned long dateTime)
{
	if (config::home_bg_override_time > 0)
	{
		return config::home_bg_override_time;
	}

	return reinterpret_cast<decltype(TimeUtil_GetTimeZoneForHome_hook)*>(TimeUtil_GetTimeZoneForHome_orig)(dateTime);
}

static void InitAddress()
{
	auto TimeUtil_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "TimeUtil");
	TimeUtil_GetSeasonForHome_addr = il2cpp_symbols::get_method_pointer(TimeUtil_klass, "GetSeasonForHome", 1);
	TimeUtil_GetTimeZoneForHome_addr = il2cpp_symbols::get_method_pointer(TimeUtil_klass, "GetTimeZoneForHome", 1);
}

static void HookMethods()
{
	ADD_HOOK(TimeUtil_GetSeasonForHome, "Gallop.TimeUtil::GetSeasonForHome at %p\n");
	ADD_HOOK(TimeUtil_GetTimeZoneForHome, "Gallop.TimeUtil::GetTimeZoneForHome at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
