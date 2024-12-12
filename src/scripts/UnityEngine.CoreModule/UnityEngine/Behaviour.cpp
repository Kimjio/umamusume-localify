#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Behaviour.hpp"

void* get_enabled_addr = nullptr;

void* set_enabled_addr = nullptr;

static void InitAddress()
{
	get_enabled_addr = il2cpp_resolve_icall("UnityEngine.Behaviour::get_enabled()");
	set_enabled_addr = il2cpp_resolve_icall("UnityEngine.Behaviour::set_enabled()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	bool Behaviour::enabled()
	{
		return reinterpret_cast<bool (*)(Il2CppObject*)>(get_enabled_addr)(instance);
	}

	void Behaviour::enabled(bool value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, bool)>(set_enabled_addr)(instance, value);
	}
}
