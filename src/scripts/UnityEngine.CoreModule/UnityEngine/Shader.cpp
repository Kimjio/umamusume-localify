#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Shader.hpp"

namespace
{
	void* PropertyToID_addr = nullptr;

	void* GetGlobalTextureImpl_addr = nullptr;

	void* SetGlobalTextureImpl_addr = nullptr;
}

static void InitAddress()
{
	PropertyToID_addr = il2cpp_resolve_icall("UnityEngine.Shader::PropertyToID()");
	GetGlobalTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Shader::GetGlobalTextureImpl()");
	SetGlobalTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Shader::SetGlobalTextureImpl()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	int Shader::PropertyToID(Il2CppString* name)
	{
		return reinterpret_cast<decltype(PropertyToID)*>(PropertyToID_addr)(name);
	}

	Il2CppObject* Shader::GetGlobalTextureImpl(int name)
	{
		return reinterpret_cast<decltype(GetGlobalTextureImpl)*>(GetGlobalTextureImpl_addr)(name);
	}

	void Shader::SetGlobalTextureImpl(int name, Il2CppObject* value)
	{
		reinterpret_cast<decltype(SetGlobalTextureImpl)*>(SetGlobalTextureImpl_addr)(name, value);
	}
}
