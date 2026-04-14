#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "FullScreenMoviePlayerStory.hpp"

#include "config/config.hpp"

namespace
{
	void* FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_addr = nullptr;
	void* FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_orig = nullptr;
}

static void FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_hook(Il2CppObject* self, Il2CppObject* rectTransform)
{

}

static void InitAddress()
{
	FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "FullScreenMoviePlayerStory", "AdjustScreenSize_SingleMode", 1);

	if (!FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_addr)
	{
		FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "FullScreenMoviePlayerStory", "AdjustScreenSize_Prologue", 1);
	}
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(FullScreenMoviePlayerStory_AdjustScreenSize_SingleMode, "Gallop.FullScreenMoviePlayerStory::AdjustScreenSize_SingleMode at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
