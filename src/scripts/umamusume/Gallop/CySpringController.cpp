#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CySpringController.hpp"

#include "config/config.hpp"

namespace
{
	void* CySpringController_set_UpdateMode_addr = nullptr;
	void* CySpringController_set_UpdateMode_orig = nullptr;

	void* CySpringController_get_UpdateMode_addr = nullptr;
	void* CySpringController_get_UpdateMode_orig = nullptr;
}

static void CySpringController_set_UpdateMode_hook(Il2CppObject* self, Gallop::CySpringController::SpringUpdateMode value)
{
	if (config::cyspring_update_mode == -1)
	{
		reinterpret_cast<decltype(CySpringController_set_UpdateMode_hook)*>(CySpringController_set_UpdateMode_orig)(self, value);
		return;
	}

	reinterpret_cast<decltype(CySpringController_set_UpdateMode_hook)*>(CySpringController_set_UpdateMode_orig)(self, static_cast<Gallop::CySpringController::SpringUpdateMode>(config::cyspring_update_mode));
}

static Gallop::CySpringController::SpringUpdateMode CySpringController_get_UpdateMode_hook(Il2CppObject* self)
{
	if (config::cyspring_update_mode == -1)
	{
		return reinterpret_cast<decltype(CySpringController_get_UpdateMode_hook)*>(CySpringController_get_UpdateMode_orig)(self);
	}

	return static_cast<Gallop::CySpringController::SpringUpdateMode>(config::cyspring_update_mode);
}

static void InitAddress()
{
	auto CySpringController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CySpringController");
	auto CySpringController_set_UpdateMode_addr = il2cpp_symbols::get_method_pointer(CySpringController_klass, "set_UpdateMode", 1);
	auto CySpringController_get_UpdateMode_addr = il2cpp_symbols::get_method_pointer(CySpringController_klass, "get_UpdateMode", 0);
}

static void HookMethods()
{
	if (config::cyspring_update_mode != -1)
	{
		ADD_HOOK(CySpringController_set_UpdateMode, "Gallop.CySpringController::set_UpdateMode at %p\n");
		ADD_HOOK(CySpringController_get_UpdateMode, "Gallop.CySpringController::get_UpdateMode at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
