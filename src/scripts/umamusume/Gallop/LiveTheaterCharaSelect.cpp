#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LiveTheaterCharaSelect.hpp"

#include "config/config.hpp"

namespace
{
	void* LiveTheaterCharaSelect_CheckSwapChara_addr = nullptr;
	void* LiveTheaterCharaSelect_CheckSwapChara_orig = nullptr;
}

static void LiveTheaterCharaSelect_CheckSwapChara_hook(Il2CppObject* self, int index, int oldCharaId, int oldDressId, int oldDressColorId, int oldDressId2, int oldDressColorId2, int newCharaId)
{
	// no-op
}

static void InitAddress()
{
	LiveTheaterCharaSelect_CheckSwapChara_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "LiveTheaterCharaSelect", "CheckSwapChara", 7);
}

static void HookMethods()
{
	if (config::unlock_live_chara)
	{
		ADD_HOOK(LiveTheaterCharaSelect_CheckSwapChara, "Gallop.LiveTheaterCharaSelect::CheckSwapChara at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
