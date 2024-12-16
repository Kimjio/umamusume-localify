#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Shader.hpp"

void* PropertyToID_addr = nullptr;

static void InitAddress()
{
	PropertyToID_addr = il2cpp_resolve_icall("UnityEngine.Shader::PropertyToID()");
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
}
