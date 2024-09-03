#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Component.hpp"

void* get_gameObject_addr = nullptr;

static void InitAddress()
{
	get_gameObject_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "Component", "get_gameObject", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	GameObject Component::gameObject()
	{
		auto gameObject = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_gameObject_addr)(instance);
		return GameObject(gameObject);
	}
}
