#include "../../Cute.Http.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "HttpManager.hpp"

#include "taskbar/TaskbarManager.hpp"

void* HttpManager_Cute_Core_IUpdatable_FastUpdate_orig = nullptr;
void* HttpManager_Cute_Core_IUpdatable_FastUpdate_addr = nullptr;

static void HttpManager_Cute_Core_IUpdatable_FastUpdate_hook(Il2CppObject* self)
{
	auto currentOperationField = il2cpp_class_get_field_from_name(self->klass, "currentOperation");
	Il2CppObject* currentOperation;
	il2cpp_field_get_value(self, currentOperationField, &currentOperation);

	if (currentOperation != nullptr)
	{
		auto timeOutTimeField = il2cpp_class_get_field_from_name(self->klass, "timeOutTime");
		float timeOutTime;
		il2cpp_field_get_value(self, timeOutTimeField, &timeOutTime);

		auto isDone = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(currentOperation->klass, "isDone", 0);
		auto realtimeSinceStartup = il2cpp_resolve_icall_type<float (*)()>("UnityEngine.Time::get_realtimeSinceStartup()")();

		if (!isDone && timeOutTime <= realtimeSinceStartup)
		{
			if (TaskbarManager::currentState == TBPF_NORMAL)
			{
				// Operation timed out
				TaskbarManager::SetProgressState(TBPF_ERROR);
			}
		}
	}

	reinterpret_cast<decltype(HttpManager_Cute_Core_IUpdatable_FastUpdate_hook)*>(HttpManager_Cute_Core_IUpdatable_FastUpdate_orig)(self);
}


static void InitAddress()
{
	HttpManager_Cute_Core_IUpdatable_FastUpdate_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Cute.Http", "HttpManager", "Cute.Core.IUpdatable.FastUpdate", 0);
}

static void HookMethods()
{
	ADD_HOOK(HttpManager_Cute_Core_IUpdatable_FastUpdate, "Cute.Http.HttpManager::IUpdatable.FastUpdate at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Cute
{
	namespace Http
	{
	}
}
