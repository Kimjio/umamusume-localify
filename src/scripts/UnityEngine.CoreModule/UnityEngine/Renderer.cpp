#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Renderer.hpp"

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

namespace
{
	void* Renderer_GetMaterial_addr = nullptr;
	void* Renderer_GetMaterial_orig = nullptr;

	void* Renderer_GetSharedMaterial_addr = nullptr;
	void* Renderer_GetSharedMaterial_orig = nullptr;

	void* Renderer_SetMaterial_addr = nullptr;
	void* Renderer_SetMaterial_orig = nullptr;

	void* Renderer_GetMaterialArray_addr = nullptr;
	void* Renderer_GetMaterialArray_orig = nullptr;

	void* Renderer_SetMaterialArray_addr = nullptr;
	void* Renderer_SetMaterialArray_orig = nullptr;

	void* Renderer_GetSharedMaterialArray_addr = nullptr;
	void* Renderer_GetSharedMaterialArray_orig = nullptr;
}

static Il2CppObject* Renderer_GetMaterial_hook(Il2CppObject* self)
{
	auto material = reinterpret_cast<decltype(Renderer_GetMaterial_hook)*>(Renderer_GetMaterial_orig)(self);
	if (material)
	{
		ReplaceMaterialTexture(material);
	}
	return material;
}

static Il2CppArraySize_t<Il2CppObject*>* Renderer_GetMaterialArray_hook(Il2CppObject* self)
{
	auto materials = reinterpret_cast<decltype(Renderer_GetMaterialArray_hook)*>(Renderer_GetMaterialArray_orig)(self);
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
	auto material = reinterpret_cast<decltype(Renderer_GetSharedMaterial_hook)*>(Renderer_GetSharedMaterial_orig)(self);
	if (material)
	{
		ReplaceMaterialTexture(material);
	}
	return material;
}

static Il2CppArraySize_t<Il2CppObject*>* Renderer_GetSharedMaterialArray_hook(Il2CppObject* self)
{
	auto materials = reinterpret_cast<decltype(Renderer_GetSharedMaterialArray_hook)*>(Renderer_GetSharedMaterialArray_orig)(self);
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
	reinterpret_cast<decltype(Renderer_SetMaterial_hook)*>(Renderer_SetMaterial_orig)(self, material);
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
	reinterpret_cast<decltype(Renderer_SetMaterialArray_hook)*>(Renderer_SetMaterialArray_orig)(self, materials, length);
}

static void InitAddress()
{
	Renderer_GetMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetMaterial()");
	Renderer_GetSharedMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetSharedMaterial()");
	Renderer_SetMaterial_addr = il2cpp_resolve_icall("UnityEngine.Renderer::SetMaterial(UnityEngine.Material)");
	Renderer_GetMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetMaterialArray()");
	Renderer_SetMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::SetMaterialArray(UnityEngine.Material[])");
	Renderer_GetSharedMaterialArray_addr = il2cpp_resolve_icall("UnityEngine.Renderer::GetSharedMaterialArray()");
}

static void HookMethods()
{
	if (!config::replace_assetbundle_file_paths.empty())
	{
		ADD_HOOK(Renderer_GetMaterial, "UnityEngine.Renderer::GetMaterial at %p\n");
		ADD_HOOK(Renderer_GetSharedMaterial, "UnityEngine.Renderer::GetSharedMaterial at %p\n");
		ADD_HOOK(Renderer_SetMaterial, "UnityEngine.Renderer::SetMaterial at %p\n");
		ADD_HOOK(Renderer_GetMaterialArray, "UnityEngine.Renderer::GetMaterialArray at %p\n");
		ADD_HOOK(Renderer_SetMaterialArray, "UnityEngine.Renderer::SetMaterialArray at %p\n");
		ADD_HOOK(Renderer_GetSharedMaterialArray, "UnityEngine.Renderer::GetSharedMaterialArray at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
