#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DownloadErrorProcessor.hpp"

#include "taskbar/TaskbarManager.hpp"

void* DownloadErrorProcessor_ExecDownloadErrorProcess_addr = nullptr;
void* DownloadErrorProcessor_ExecDownloadErrorProcess_orig = nullptr;

static void DownloadErrorProcessor_ExecDownloadErrorProcess_hook(Il2CppObject* error, Il2CppDelegate* onRetry, Il2CppDelegate* onGotoTitle) {
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_ERROR);
	}
	reinterpret_cast<decltype(DownloadErrorProcessor_ExecDownloadErrorProcess_hook)*>(DownloadErrorProcessor_ExecDownloadErrorProcess_orig)(error, onRetry, onGotoTitle);
}

static void InitAddress()
{
	DownloadErrorProcessor_ExecDownloadErrorProcess_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadErrorProcessor", "ExecDownloadErrorProcess", 3);
}

static void HookMethods()
{
	ADD_HOOK(DownloadErrorProcessor_ExecDownloadErrorProcess, "Gallop.DownloadErrorProcessor::ExecDownloadErrorProcess at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	void DownloadErrorProcessor::ExecDownloadErrorProcess(Il2CppObject* error, Il2CppDelegate* onRetry, Il2CppDelegate* onGotoTitle)
	{
		reinterpret_cast<void (*)(Il2CppObject *, Il2CppDelegate *, Il2CppDelegate *)>(DownloadErrorProcessor_ExecDownloadErrorProcess_addr)(error, onRetry, onGotoTitle);
	}
}
