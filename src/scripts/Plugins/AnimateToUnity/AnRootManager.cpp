#include "../Plugins.hpp"
#include "../../ScriptInternal.hpp"
#include "AnRootManager.hpp"

namespace
{
	void* AnRootManager_set_ScreenRate_addr = nullptr;
}

static void InitAddress()
{
	auto AnRootManager_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "AnimateToUnity", "AnRootManager");
	AnRootManager_set_ScreenRate_addr = il2cpp_symbols::get_method_pointer(AnRootManager_klass, "set_ScreenRate", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace AnimateToUnity
{
	const char* AnRootManager::klassName = "AnRootManager";

	void AnRootManager::ScreenRate(float value)
	{
		reinterpret_cast<void(*)(Il2CppObject*, float)>(AnRootManager_set_ScreenRate_addr)(instance, value);
	}
}
