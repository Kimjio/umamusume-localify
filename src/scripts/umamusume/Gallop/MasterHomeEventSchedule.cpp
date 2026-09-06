#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "MasterHomeEventSchedule.hpp"

namespace
{
	Il2CppMethodPointer MasterHomeEventSchedule_GetEventId_addr = nullptr;
	void* MasterHomeEventSchedule_GetEventId_orig = nullptr;
}

static int MasterHomeEventSchedule_GetEventId_hook(Il2CppObject* self, int64_t serverTime)
{
	if (config::home_bg_override_event_id > 0)
	{
		return config::home_bg_override_event_id;
	}

	if (config::home_bg_override_bg_season > 0)
	{
		return 0;
	}

	return reinterpret_cast<decltype(MasterHomeEventSchedule_GetEventId_hook)*>(MasterHomeEventSchedule_GetEventId_orig)(self, serverTime);
}

static void InitAddress()
{
	auto MasterHomeEventSchedule_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "MasterHomeEventSchedule");
	MasterHomeEventSchedule_GetEventId_addr = il2cpp_symbols::get_method_pointer(MasterHomeEventSchedule_klass, "GetEventId", 1);
}

static void HookMethods()
{
	ADD_HOOK(MasterHomeEventSchedule_GetEventId, "Gallop.MasterHomeEventSchedule::GetEventId at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
