#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TapEffectController.hpp"
#include "../../UnityEngine.InputLegacyModule/UnityEngine/Input.hpp"

#include "config/config.hpp"

namespace
{
	void* TapEffectController_Update_addr = nullptr;
}

static void InitAddress()
{
	TapEffectController_Update_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "TapEffectController", "Update", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	const char* TapEffectController::namespaze = "Gallop";
	const char* TapEffectController::klassName = "TapEffectController";

	void TapEffectController::Update()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(TapEffectController_Update_addr)(instance);
	}
}
