#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Renderer.hpp"

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

namespace
{
	Il2CppMethodPointer Renderer_GetMaterial_addr = nullptr;

	Il2CppMethodPointer Renderer_GetSharedMaterial_addr = nullptr;

	Il2CppMethodPointer Renderer_SetMaterial_addr = nullptr;

	Il2CppMethodPointer Renderer_GetMaterialArray_addr = nullptr;

	Il2CppMethodPointer Renderer_SetMaterialArray_addr = nullptr;

	Il2CppMethodPointer Renderer_GetSharedMaterialArray_addr = nullptr;
}

static Il2CppObject* Renderer_GetMaterial_hook(Il2CppObject* self)
{
	auto material = reinterpret_cast<decltype(Renderer_GetMaterial_hook)*>(Renderer_GetMaterial_addr)(self);
	if (material)
	{
		ReplaceMaterialTexture(material);
	}
	return material;
}

static Il2CppArraySize_t<Il2CppObject*>* Renderer_GetMaterialArray_hook(Il2CppObject* self)
{
	auto materials = reinterpret_cast<decltype(Renderer_GetMaterialArray_hook)*>(Renderer_GetMaterialArray_addr)(self);
	for (int i = 0; i < materials->max_length; i++)
	{
		auto material = materials->vector[i];
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
	}
	return materials;
}

static Il2CppObject* Renderer_GetSharedMaterial_hook(Il2CppObject* self)
{
	auto material = reinterpret_cast<decltype(Renderer_GetSharedMaterial_hook)*>(Renderer_GetSharedMaterial_addr)(self);
	if (material)
	{
		ReplaceMaterialTexture(material);
	}
	return material;
}

static Il2CppArraySize_t<Il2CppObject*>* Renderer_GetSharedMaterialArray_hook(Il2CppObject* self)
{
	auto materials = reinterpret_cast<decltype(Renderer_GetSharedMaterialArray_hook)*>(Renderer_GetSharedMaterialArray_addr)(self);
	for (int i = 0; i < materials->max_length; i++)
	{
		auto material = materials->vector[i];
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
	}
	return materials;
}

static void Renderer_SetMaterial_hook(Il2CppObject* self, Il2CppObject* material)
{
	if (material)
	{
		ReplaceMaterialTexture(material);
	}
	reinterpret_cast<decltype(Renderer_SetMaterial_hook)*>(Renderer_SetMaterial_addr)(self, material);
}

static void Renderer_SetMaterialArray_hook(Il2CppObject* self, Il2CppArraySize* materials, int length)
{
	for (int i = 0; i < materials->max_length; i++)
	{
		auto material = (Il2CppObject*)materials->vector[i];
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
	}
	reinterpret_cast<decltype(Renderer_SetMaterialArray_hook)*>(Renderer_SetMaterialArray_addr)(self, materials, length);
}

static void InitAddress()
{
	Renderer_GetMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetMaterial");
	Renderer_GetSharedMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetSharedMaterial");
	Renderer_SetMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::SetMaterial(UnityEngine.Material)");
	Renderer_GetMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetMaterialArray");
	Renderer_SetMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::SetMaterialArray(UnityEngine.Material[])");
	Renderer_GetSharedMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetSharedMaterialArray");
}

static void HookMethods()
{
	if (!config::replace_assetbundle_file_paths.empty())
	{
		il2cpp_add_internal_call("UnityEngine.Renderer::GetMaterial", reinterpret_cast<Il2CppMethodPointer>(Renderer_GetMaterial_hook));
		il2cpp_add_internal_call("UnityEngine.Renderer::GetSharedMaterial", reinterpret_cast<Il2CppMethodPointer>(Renderer_GetSharedMaterial_hook));
		il2cpp_add_internal_call("UnityEngine.Renderer::SetMaterial", reinterpret_cast<Il2CppMethodPointer>(Renderer_SetMaterial_hook));
		il2cpp_add_internal_call("UnityEngine.Renderer::GetMaterialArray", reinterpret_cast<Il2CppMethodPointer>(Renderer_GetMaterialArray_hook));
		il2cpp_add_internal_call("UnityEngine.Renderer::SetMaterialArray", reinterpret_cast<Il2CppMethodPointer>(Renderer_SetMaterialArray_hook));
		il2cpp_add_internal_call("UnityEngine.Renderer::GetSharedMaterialArray", reinterpret_cast<Il2CppMethodPointer>(Renderer_GetSharedMaterialArray_hook));
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
