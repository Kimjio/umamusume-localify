#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "BootSystem.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer BootSystem_LateUpdate_addr = nullptr;
	void* BootSystem_LateUpdate_orig = nullptr;
}

static void BootSystem_LateUpdate_hook(Il2CppObject* self)
{
	// no-op
}

static void InitAddress()
{
	auto BootSystem_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "BootSystem");
	BootSystem_LateUpdate_addr = il2cpp_symbols::get_method_pointer(BootSystem_klass, "LateUpdate", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(BootSystem_LateUpdate, "Gallop.BootSystem::LateUpdate at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
