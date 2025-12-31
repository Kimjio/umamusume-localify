#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "ResourceManager.hpp"

#include "scripts/umamusume/Gallop/TextFontManager.hpp"

#include "string_utils.hpp"

namespace
{
	void* ResourceManager_Load_addr = nullptr;
	void* ResourceManager_Load_orig = nullptr;

	Il2CppClass* FontClass;
}

static Il2CppObject* ResourceManager_Load_hook(Il2CppObject* self, Il2CppString* path, int hash, bool isLowerCase, const MethodInfo* methodInfo)
{
	auto asset = reinterpret_cast<decltype(ResourceManager_Load_hook)*>(ResourceManager_Load_orig)(self, path, hash, isLowerCase, methodInfo);

	if (asset && asset->klass == FontClass)
	{
		auto font = Gallop::TextFontManager::GetReplacementFontAsset(path);

		if (font)
		{
			return font;
		}
	}

	return asset;
}

static void InitAddress()
{
	if (auto methodInfo = GetGenericMethod(il2cpp_symbols::get_method(ASSEMBLY_NAME, "Gallop", "ResourceManager", "Load", 3), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font")))
	{
		ResourceManager_Load_addr = methodInfo->methodPointer;
	}

	FontClass = il2cpp_symbols::get_class("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font");
}

static void HookMethods()
{
	if (config::replace_to_custom_font)
	{
		ADD_HOOK(ResourceManager_Load, "Gallop.ResourceManager::Load at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{

}
