#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "RenderTexture.hpp"

namespace
{
	Il2CppClass* RenderTexture_klass = nullptr;

	void* RenderTexture_ctor_RenderTextureDescriptor_addr = nullptr;

	void* RenderTexture_get_width_addr = nullptr;

	void* RenderTexture_set_width_addr = nullptr;

	void* RenderTexture_get_height_addr = nullptr;

	void* RenderTexture_set_height_addr = nullptr;

	void* RenderTexture_Create_addr = nullptr;

	void* RenderTexture_GetTemporary_addr = nullptr;
		
	void* RenderTexture_GetActive_addr = nullptr;
	
	void* RenderTexture_SetActive_addr = nullptr;

	void* RenderTexture_ReleaseTemporary_addr = nullptr;
}

static void InitAddress()
{
	RenderTexture_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine", "RenderTexture");
	RenderTexture_ctor_RenderTextureDescriptor_addr = il2cpp_symbols::get_method_pointer(RenderTexture_klass, ".ctor", 1);
	RenderTexture_get_width_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::get_width()");
	RenderTexture_set_width_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::set_width(System.Int32)");
	RenderTexture_get_height_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::get_height()");
	RenderTexture_set_height_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::set_height(System.Int32)");
	RenderTexture_Create_addr = il2cpp_symbols::get_method_pointer(RenderTexture_klass, "Create", 0);
	RenderTexture_GetTemporary_addr = il2cpp_symbols::get_method_pointer(RenderTexture_klass, "GetTemporary", 2);
	RenderTexture_GetActive_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::GetActive()");
	RenderTexture_SetActive_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::SetActive(UnityEngine.RenderTexture)");
	RenderTexture_ReleaseTemporary_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::ReleaseTemporary(UnityEngine.RenderTexture)");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	RenderTexture::RenderTexture(RenderTextureDescriptor desc) : Object(nullptr)
	{
		instance = il2cpp_object_new(RenderTexture_klass);
		reinterpret_cast<void (*)(Il2CppObject*, RenderTextureDescriptor)>(RenderTexture_ctor_RenderTextureDescriptor_addr)(instance, desc);
	}

	int RenderTexture::width()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(RenderTexture_get_width_addr)(instance);
	}

	void RenderTexture::width(int value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(RenderTexture_set_width_addr)(instance, value);
	}

	int RenderTexture::height()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(RenderTexture_get_height_addr)(instance);
	}

	void RenderTexture::height(int value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(RenderTexture_set_height_addr)(instance, value);
	}

	bool RenderTexture::Create()
	{
		return reinterpret_cast<bool (*)(Il2CppObject*)>(RenderTexture_Create_addr)(instance);
	}

	RenderTexture RenderTexture::GetTemporary(int width, int height)
	{
		return reinterpret_cast<Il2CppObject* (*)(int, int)>(RenderTexture_GetTemporary_addr)(width, height);
	}

	RenderTexture RenderTexture::GetActive()
	{
		return reinterpret_cast<Il2CppObject* (*)()>(RenderTexture_GetActive_addr)();
	}

	void RenderTexture::SetActive(RenderTexture rt)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(RenderTexture_SetActive_addr)(rt);
	}

	void RenderTexture::ReleaseTemporary(RenderTexture temp)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(RenderTexture_ReleaseTemporary_addr)(temp);
	}
}
