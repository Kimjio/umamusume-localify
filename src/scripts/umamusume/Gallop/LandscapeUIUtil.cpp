#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LandscapeUIUtil.hpp"

#include "config/config.hpp"

namespace
{
	void* LandscapeUIUtil_ActivateWhiteMaskExcludeLog_addr = nullptr;
	void* LandscapeUIUtil_ActivateWhiteMaskExcludeLog_orig = nullptr;

	void* LandscapeUIUtil_DeactivateWhiteMaskExcludeLog_addr = nullptr;
	void* LandscapeUIUtil_DeactivateWhiteMaskExcludeLog_orig = nullptr;
}

static void LandscapeUIUtil_ActivateWhiteMaskExcludeLog_hook()
{
	// no-op
}

static void LandscapeUIUtil_DeactivateWhiteMaskExcludeLog_hook()
{
	// no-op
}

static void InitAddress()
{
	auto LandscapeUIUtil = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LandscapeUIUtil");
	LandscapeUIUtil_ActivateWhiteMaskExcludeLog_addr = il2cpp_symbols::get_method_pointer(LandscapeUIUtil, "ActivateWhiteMaskExcludeLog", 0);
	LandscapeUIUtil_DeactivateWhiteMaskExcludeLog_addr = il2cpp_symbols::get_method_pointer(LandscapeUIUtil, "DeactivateWhiteMaskExcludeLog", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(LandscapeUIUtil_ActivateWhiteMaskExcludeLog, "Gallop.LandscapeUIUtil::ActivateWhiteMaskExcludeLog at %p\n");
		ADD_HOOK(LandscapeUIUtil_DeactivateWhiteMaskExcludeLog, "Gallop.LandscapeUIUtil::DeactivateWhiteMaskExcludeLog at %p\n");
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
