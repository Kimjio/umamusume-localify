#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Material.hpp"

void* HasProperty_addr = nullptr;

void* GetTextureImpl_addr = nullptr;

void* SetTextureImpl_addr = nullptr;

static void InitAddress()
{
	HasProperty_addr = il2cpp_resolve_icall("UnityEngine.Material::HasProperty()");
	GetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::GetTextureImpl()");
	SetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::SetTextureImpl()");
}

static void HookMethods()
{
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
		return reinterpret_cast<bool (*)(Il2CppObject*, int)>(HasProperty_addr)(instance, nameID);
	}

	Il2CppObject* Material::GetTextureImpl(int name)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, int)>(GetTextureImpl_addr)(instance, name);
	}

	void Material::SetTextureImpl(int name, Il2CppObject* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int, Il2CppObject*)>(SetTextureImpl_addr)(instance, name, value);
	}
}
