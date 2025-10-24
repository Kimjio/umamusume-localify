#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DownloadView.hpp"

#include "taskbar/TaskbarManager.hpp"

namespace
{
	void* DownloadView_UpdateView_addr = nullptr;
	void* DownloadView_UpdateView_orig = nullptr;
}

static void DownloadView_UpdateView_hook(Il2CppObject* self, float downloadSize, float allDownloadSize) {
	if (config::taskbar_show_progress_on_download)
	{
		float progress = downloadSize / allDownloadSize;
		if (progress == 1.0f)
		{
			TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
		}
		else
		{
			TaskbarManager::SetProgressState(TBPF_NORMAL);
			TaskbarManager::SetProgressValue(static_cast<ULONGLONG>(progress * 10000), 10000);
		}
	}
	reinterpret_cast<void(*)(Il2CppObject*, float, float)>(DownloadView_UpdateView_orig)(self, downloadSize, allDownloadSize);
}

static void InitAddress()
{
	DownloadView_UpdateView_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadView", "UpdateView", 2);
}

static void HookMethods()
{
	ADD_HOOK(DownloadView_UpdateView, "Gallop.DownloadView::UpdateView at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	void DownloadView::UpdateView(float downloadSize, float allDownloadSize)
	{
		reinterpret_cast<void (*)(Il2CppObject*, float, float)>(DownloadView_UpdateView_addr)(instance, downloadSize, allDownloadSize);
	}
}
