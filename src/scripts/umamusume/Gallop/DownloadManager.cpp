#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DownloadManager.hpp"

#include "taskbar/TaskbarManager.hpp"

void* DownloadProgressUIGame_Show_addr = nullptr;
void* DownloadProgressUIGame_Show_orig = nullptr;

void* DownloadProgressUIGame_Hide_addr = nullptr;
void* DownloadProgressUIGame_Hide_orig = nullptr;

void* DownloadProgressUIGame_SetProgress_addr = nullptr;
void* DownloadProgressUIGame_SetProgress_orig = nullptr;

void* BackgroundDownloadProgressUI_Show_addr = nullptr;
void* BackgroundDownloadProgressUI_Show_orig = nullptr;

void* BackgroundDownloadProgressUI_Hide_addr = nullptr;
void* BackgroundDownloadProgressUI_Hide_orig = nullptr;

void* BackgroundDownloadProgressUI_SetProgress_addr = nullptr;
void* BackgroundDownloadProgressUI_SetProgress_orig = nullptr;

static void DownloadProgressUIGame_Show_hook(Il2CppObject* self)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NORMAL);
	}
	reinterpret_cast<decltype(DownloadProgressUIGame_Show_hook)*>(DownloadProgressUIGame_Show_orig)(self);
}

static void DownloadProgressUIGame_Hide_hook(Il2CppObject* self, bool isDelay)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
	}
	reinterpret_cast<decltype(DownloadProgressUIGame_Hide_hook)*>(DownloadProgressUIGame_Hide_orig)(self, isDelay);
}

static void DownloadProgressUIGame_SetProgress_hook(Il2CppObject* self, float progress)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NORMAL);
		TaskbarManager::SetProgressValue(progress * 10000, 10000);
	}
	reinterpret_cast<decltype(DownloadProgressUIGame_SetProgress_hook)*>(DownloadProgressUIGame_SetProgress_orig)(self, progress);
}

static void BackgroundDownloadProgressUI_Show_hook(Il2CppObject* self)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NORMAL);
	}
	reinterpret_cast<decltype(BackgroundDownloadProgressUI_Show_hook)*>(BackgroundDownloadProgressUI_Show_orig)(self);
}

static void BackgroundDownloadProgressUI_Hide_hook(Il2CppObject* self, bool isDelay)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
	}
	reinterpret_cast<decltype(BackgroundDownloadProgressUI_Hide_hook)*>(BackgroundDownloadProgressUI_Hide_orig)(self, isDelay);
}

static void BackgroundDownloadProgressUI_SetProgress_hook(Il2CppObject* self, float progress)
{
	if (config::taskbar_show_progress_on_download)
	{
		TaskbarManager::SetProgressState(TBPF_NORMAL);
		TaskbarManager::SetProgressValue(progress * 10000, 10000);
	}
	reinterpret_cast<decltype(BackgroundDownloadProgressUI_SetProgress_hook)*>(BackgroundDownloadProgressUI_SetProgress_orig)(self, progress);
}

static void InitAddress()
{
	DownloadProgressUIGame_Show_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/DownloadProgressUIGame", "Show", 0);
	DownloadProgressUIGame_Hide_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/DownloadProgressUIGame", "Hide", 1);
	DownloadProgressUIGame_SetProgress_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/DownloadProgressUIGame", "SetProgress", 1);

	BackgroundDownloadProgressUI_Show_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/BackgroundDownloadProgressUI", "Show", 0);
	BackgroundDownloadProgressUI_Hide_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/BackgroundDownloadProgressUI", "Hide", 1);
	BackgroundDownloadProgressUI_SetProgress_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DownloadManager/BackgroundDownloadProgressUI", "SetProgress", 1);
}

static void HookMethods()
{
	ADD_HOOK(DownloadProgressUIGame_Show, "Gallop.DownloadManager/DownloadProgressUIGame::Show at %p\n");
	ADD_HOOK(DownloadProgressUIGame_Hide, "Gallop.DownloadManager/DownloadProgressUIGame::Hide at %p\n");
	ADD_HOOK(DownloadProgressUIGame_SetProgress, "Gallop.DownloadManager/DownloadProgressUIGame::SetProgress at %p\n");

	ADD_HOOK(BackgroundDownloadProgressUI_Show, "Gallop.DownloadManager/BackgroundDownloadProgressUI::Show at %p\n");
	ADD_HOOK(BackgroundDownloadProgressUI_Hide, "Gallop.DownloadManager/BackgroundDownloadProgressUI::Hide at %p\n");
	ADD_HOOK(BackgroundDownloadProgressUI_SetProgress, "Gallop.DownloadManager/BackgroundDownloadProgressUI::SetProgress at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* DownloadManager::namespaze = "Gallop";
	const char* DownloadManager::klassName = "DownloadManager";
}
