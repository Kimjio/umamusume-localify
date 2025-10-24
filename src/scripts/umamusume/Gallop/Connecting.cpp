#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Connecting.hpp"

#include "taskbar/TaskbarManager.hpp"

namespace
{
	void* Connecting_Show_addr = nullptr;
	void* Connecting_Show_orig = nullptr;

	void* Connecting_Hide_addr = nullptr;
	void* Connecting_Hide_orig = nullptr;
}

static void Connecting_Show_hook(Il2CppObject* self)
{
	if (config::taskbar_show_progress_on_connecting && TaskbarManager::currentState == TBPF_NOPROGRESS)
	{
		TaskbarManager::SetProgressState(TBPF_INDETERMINATE);
	}

	reinterpret_cast<decltype(Connecting_Show_hook)*>(Connecting_Show_orig)(self);
}

static void Connecting_Hide_hook(Il2CppObject* self)
{
	if (config::taskbar_show_progress_on_connecting && TaskbarManager::currentState == TBPF_INDETERMINATE)
	{
		TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
	}

	reinterpret_cast<decltype(Connecting_Hide_hook)*>(Connecting_Hide_orig)(self);
}

static void InitAddress()
{
	Connecting_Show_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Connecting", "Show", 0);
	Connecting_Hide_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Connecting", "Hide", 0);
}

static void HookMethods()
{
	ADD_HOOK(Connecting_Show, "Gallop.Connecting::Show at %p\n");
	ADD_HOOK(Connecting_Hide, "Gallop.Connecting::Hide at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{

}
