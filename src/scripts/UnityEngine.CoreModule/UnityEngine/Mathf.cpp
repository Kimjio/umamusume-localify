#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Mathf.hpp"

namespace
{
	void* Mathf_Approximately_addr = nullptr;
}

static void InitAddress()
{
	Mathf_Approximately_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Mathf", "Approximately", 2);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	bool Mathf::Approximately(float a, float b)
	{
		return reinterpret_cast<bool (*)(float, float)>(Mathf_Approximately_addr)(a, b);
	}
}
