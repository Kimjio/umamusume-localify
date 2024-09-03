#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "MonoBehaviour.hpp"
#include "Coroutine.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"

void* StartCoroutineManaged2_addr = nullptr;
void* StartCoroutineManaged2_orig = nullptr;

static Il2CppObject* StartCoroutineManaged2_hook(Il2CppObject* _this, Il2CppObject* enumerator)
{
	auto coroutine = reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(_this, enumerator);

	if (string(enumerator->klass->name).find("ChangeOrientation") != string::npos)
	{
		auto uiManager = Gallop::UIManager::Instance();
		if (uiManager)
		{
			uiManager.UnlockGameCanvas();
		}
	}

	return coroutine;
}

static void InitAddress()
{
	StartCoroutineManaged2_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StartCoroutineManaged2()");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(StartCoroutineManaged2, "UnityEngine.MonoBehaviour::StartCoroutineManaged2 at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	Coroutine MonoBehaviour::StartCoroutineManaged2(Il2CppObject* enumerator)
	{
		auto object = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(StartCoroutineManaged2_addr)(instance, enumerator);
		return Coroutine(object);
	}
}
