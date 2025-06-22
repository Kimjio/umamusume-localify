#include "../../UnityEngine.UnityWebRequestModule.hpp"
#include "../../../ScriptInternal.hpp"
#include "UnityWebRequest.hpp"

#include "taskbar/TaskbarManager.hpp"

void* UnityWebRequest_get_error_orig = nullptr;
void* UnityWebRequest_get_error_addr = nullptr;

static Il2CppString* UnityWebRequest_get_error_hook(Il2CppObject* self)
{
	auto error = reinterpret_cast<decltype(UnityWebRequest_get_error_hook)*>(UnityWebRequest_get_error_orig)(self);

	if (error != nullptr)
	{
		if (TaskbarManager::currentState == TBPF_NORMAL)
		{
			TaskbarManager::SetProgressState(TBPF_ERROR);
		}
	}

	return error;
}


static void InitAddress()
{
	UnityWebRequest_get_error_addr = il2cpp_symbols::get_method_pointer("UnityEngine.UnityWebRequestModule.dll", "UnityEngine.Networking", "UnityWebRequest", "get_error", 0);
}

static void HookMethods()
{
	ADD_HOOK(UnityWebRequest_get_error, "UnityEngine.Networking.UnityWebRequest::get_error at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	namespace Networking
	{
	}
}
