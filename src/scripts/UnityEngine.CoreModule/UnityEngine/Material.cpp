#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Material.hpp"

namespace
{
	Il2CppReflectionType* Texture2DClass = nullptr;

	Il2CppMethodPointer Material_HasProperty_addr = nullptr;

	Il2CppMethodPointer Material_GetTextureImpl_addr = nullptr;

	Il2CppMethodPointer Material_SetTextureImpl_addr = nullptr;
}

static Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* self, int nameID)
{
	auto texture = reinterpret_cast<decltype(Material_GetTextureImpl_hook)*>(Material_GetTextureImpl_addr)(self, nameID);
	if (texture && !il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
	{
		auto newTexture = GetReplacementAssets(UnityEngine::Object::Name(texture), Texture2DClass);
		if (newTexture)
		{
			return newTexture;
		}
	}
	return texture;
}

static void Material_SetTextureImpl_hook(Il2CppObject* self, int nameID, Il2CppObject* texture)
{
	if (texture && !il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
	{
		auto newTexture = GetReplacementAssets(UnityEngine::Object::Name(texture), Texture2DClass);
		if (newTexture)
		{
			reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_addr)(self, nameID, newTexture);
			return;
		}
	}
	reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_addr)(self, nameID, texture);
}

static void InitAddress()
{
	Texture2DClass = GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D");
	Material_HasProperty_addr = il2cpp_resolve_icall("UnityEngine.Material::HasProperty");
	Material_GetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::GetTextureImpl");
	Material_SetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::SetTextureImpl");
}

static void HookMethods()
{
	if (!config::replace_assetbundle_file_paths.empty())
	{
		il2cpp_add_internal_call("UnityEngine.Material::GetTextureImpl", reinterpret_cast<Il2CppMethodPointer>(Material_GetTextureImpl_hook));
		il2cpp_add_internal_call("UnityEngine.Material::SetTextureImpl", reinterpret_cast<Il2CppMethodPointer>(Material_SetTextureImpl_hook));
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	bool Material::HasProperty(int nameID)
	{
		return reinterpret_cast<bool (*)(Il2CppObject*, int)>(Material_HasProperty_addr)(instance, nameID);
	}

	Il2CppObject* Material::GetTextureImpl(int name)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, int)>(Material_GetTextureImpl_addr)(instance, name);
	}

	void Material::SetTextureImpl(int name, Il2CppObject* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int, Il2CppObject*)>(Material_SetTextureImpl_addr)(instance, name, value);
	}
}
