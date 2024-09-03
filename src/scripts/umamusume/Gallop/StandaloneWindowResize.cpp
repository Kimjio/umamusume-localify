#ifdef _MSC_VER

#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StandaloneWindowResize.hpp"

#include "config/config.hpp"

void* get_IsVirt_addr = nullptr;

void* set_IsVirt_addr = nullptr;

static void InitAddress()
{
	get_IsVirt_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "StandaloneWindowResize", "get_IsVirt", 0);
	set_IsVirt_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "StandaloneWindowResize", "set_IsVirt", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	bool StandaloneWindowResize::IsVirt()
	{
		return reinterpret_cast<bool (*)()>(get_IsVirt_addr)();
	}

	void StandaloneWindowResize::IsVirt(bool value)
	{
		reinterpret_cast<void (*)(bool)>(set_IsVirt_addr)(value);
	}
}
#endif
