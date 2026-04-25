#include "../../../umamusume.hpp"
#include "../../../../ScriptInternal.hpp"
#include "CySpringUpdater.hpp"

#include "config/config.hpp"

namespace
{
	void* CySpringUpdater_set_SpringUpdateMode_addr = nullptr;
	void* CySpringUpdater_set_SpringUpdateMode_orig = nullptr;

	void* CySpringUpdater_get_SpringUpdateMode_addr = nullptr;
	void* CySpringUpdater_get_SpringUpdateMode_orig = nullptr;
}

static void CySpringUpdater_set_SpringUpdateMode_hook(Il2CppObject* self, Gallop::CySpringController::SpringUpdateMode value)
{
	if (config::cyspring_update_mode == -1)
	{
		reinterpret_cast<decltype(CySpringUpdater_set_SpringUpdateMode_hook)*>(CySpringUpdater_set_SpringUpdateMode_orig)(self, value);
		return;
	}

	reinterpret_cast<decltype(CySpringUpdater_set_SpringUpdateMode_hook)*>(CySpringUpdater_set_SpringUpdateMode_orig)(self, static_cast<Gallop::CySpringController::SpringUpdateMode>(config::cyspring_update_mode));
}

static Gallop::CySpringController::SpringUpdateMode CySpringUpdater_get_SpringUpdateMode_hook(Il2CppObject* self)
{
	if (config::cyspring_update_mode == -1)
	{
		return reinterpret_cast<decltype(CySpringUpdater_get_SpringUpdateMode_hook)*>(CySpringUpdater_get_SpringUpdateMode_orig)(self);
	}

	return static_cast<Gallop::CySpringController::SpringUpdateMode>(config::cyspring_update_mode);
}

static void InitAddress()
{
	auto CySpringUpdater_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop.Model.Component", "CySpringUpdater");
	CySpringUpdater_set_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(CySpringUpdater_klass, "set_SpringUpdateMode", 1);
	CySpringUpdater_get_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(CySpringUpdater_klass, "get_SpringUpdateMode", 0);
}

static void HookMethods()
{
	if (config::cyspring_update_mode != -1)
	{
		ADD_HOOK(CySpringUpdater_set_SpringUpdateMode, "Gallop.CySpringUpdater::set_SpringUpdateMode at %p\n");
		ADD_HOOK(CySpringUpdater_get_SpringUpdateMode, "Gallop.CySpringUpdater::get_SpringUpdateMode at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
