#include "../../_Cyan.hpp"
#include "../../../ScriptInternal.hpp"
#include "AssetHandle.hpp"

#include "string_utils.hpp"

namespace
{
	void* AssetHandle_LoadAsset_addr = nullptr;
	void* AssetHandle_LoadAsset_orig = nullptr;
}

static Il2CppObject* AssetHandle_LoadAsset_hook(Il2CppObject* self, Il2CppString* name, bool isLowerCase, const MethodInfo* methodInfo)
{
	return reinterpret_cast<decltype(AssetHandle_LoadAsset_hook)*>(AssetHandle_LoadAsset_orig)(self, name, isLowerCase, methodInfo);
}

static void InitAddress()
{
	if (auto methodInfo = GetGenericMethod(il2cpp_symbols::get_method(ASSEMBLY_NAME, "Cyan.Loader", "AssetHandle", "LoadAsset", 2), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font")))
	{
		AssetHandle_LoadAsset_addr = methodInfo->methodPointer;
	}
}

static void HookMethods()
{
	ADD_HOOK(AssetHandle_LoadAsset, "Cyan.Loader.AssetHandle::LoadAsset at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	// il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Cyan
{
	namespace Loader
	{
	}
}
