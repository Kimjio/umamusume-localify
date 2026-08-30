#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogSingleModeMainCharacterDetail.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer DialogSingleModeMainCharacterDetail_GetFormType_addr = nullptr;
	void* DialogSingleModeMainCharacterDetail_GetFormType_orig = nullptr;
}

static Gallop::DialogCommonBase::FormType DialogSingleModeMainCharacterDetail_GetFormType_hook(Il2CppObject* self)
{
	return Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON;
}

static void InitAddress()
{
	auto DialogSingleModeMainCharacterDetail_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogSingleModeMainCharacterDetail");
	DialogSingleModeMainCharacterDetail_GetFormType_addr = il2cpp_symbols::get_method_pointer(DialogSingleModeMainCharacterDetail_klass, "GetFormType", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(DialogSingleModeMainCharacterDetail_GetFormType, "Gallop.DialogSingleModeMainCharacterDetail::GetFormType at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
