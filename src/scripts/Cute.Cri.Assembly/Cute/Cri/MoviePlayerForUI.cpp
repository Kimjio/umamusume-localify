#include "../../Cute.Cri.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "MoviePlayerForUI.hpp"

#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"

#include "config/config.hpp"

namespace
{
	void* MoviePlayerForUI_AdjustScreenSize_addr = nullptr;
	void* MoviePlayerForUI_AdjustScreenSize_orig = nullptr;
}

static void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* self, UnityEngine::Vector2 dispRectWH, bool isPanScan)
{
	auto movieInfo = Cute::Cri::MoviePlayerForUI(self).MovieInfo();
	if (!movieInfo)
	{
		return;
	}

	if (movieInfo->width < movieInfo->height && !Gallop::StandaloneWindowResize::IsVirt())
	{
		auto ratio1 = static_cast<float>(movieInfo->width) / static_cast<float>(movieInfo->height);
		dispRectWH.x = dispRectWH.y * ratio1;
	}

	reinterpret_cast<decltype(MoviePlayerForUI_AdjustScreenSize_hook)*>(MoviePlayerForUI_AdjustScreenSize_orig)(self, dispRectWH, isPanScan);
}

static void InitAddress()
{
	MoviePlayerForUI_AdjustScreenSize_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Cute.Cri", "MoviePlayerForUI", "AdjustScreenSize", 2);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(MoviePlayerForUI_AdjustScreenSize, "MoviePlayerForUI::AdjustScreenSize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Cute::Cri
{
	void MoviePlayerForUI::AdjustScreenSize(UnityEngine::Vector2 dispRectWH, bool isPanScan)
	{
		reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2, bool)>(MoviePlayerForUI_AdjustScreenSize_addr)(instance, dispRectWH, isPanScan);
	}
}
