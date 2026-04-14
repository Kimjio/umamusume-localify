#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsSingleModeCommonFooter.hpp"
#include "DialogCommon.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

#include "config/config.hpp"

namespace
{
	void* PartsSingleModeCommonFooter_Setup_addr = nullptr;
	void* PartsSingleModeCommonFooter_Setup_orig = nullptr;
}

static void PartsSingleModeCommonFooter_Setup_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(PartsSingleModeCommonFooter_Setup_hook)*>(PartsSingleModeCommonFooter_Setup_orig)(self);

	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto _logButtonField = il2cpp_class_get_field_from_name(self->klass, "_logButton");
		Il2CppObject* _logButton;
		il2cpp_field_get_value(self, _logButtonField, &_logButton);

		UnityEngine::MonoBehaviour(_logButton).gameObject().SetActive(true);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_logButton->klass, "SetOnClick", 1)->methodPointer(_logButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickLog", 0)->methodPointer(self);
				})->delegate);
	}
}

static void InitAddress()
{
	PartsSingleModeCommonFooter_Setup_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PartsSingleModeCommonFooter", "Setup", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(PartsSingleModeCommonFooter_Setup, "Gallop.PartsSingleModeCommonFooter::Setup at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
