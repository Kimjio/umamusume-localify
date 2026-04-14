#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "SingleModeRaceEntryViewController.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

#include "config/config.hpp"

namespace
{
	void* SingleModeRaceEntryViewController_SetupBottomButtons_addr = nullptr;
	void* SingleModeRaceEntryViewController_SetupBottomButtons_orig = nullptr;
}

static void SingleModeRaceEntryViewController_SetupBottomButtons_hook(Il2CppObject* self, uint64_t degree)
{
	reinterpret_cast<decltype(SingleModeRaceEntryViewController_SetupBottomButtons_hook)*>(SingleModeRaceEntryViewController_SetupBottomButtons_orig)(self, degree);

	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto _viewField = il2cpp_class_get_field_from_name(self->klass, "_view");
		Il2CppObject* _view;
		il2cpp_field_get_value(self, _viewField, &_view);

		auto ReserveListButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_view->klass, "get_ReserveListButton", 0)->methodPointer(_view);

		UnityEngine::MonoBehaviour(ReserveListButton).gameObject().SetActive(true);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(ReserveListButton->klass, "SetOnClick", 1)->methodPointer(ReserveListButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickReserveListButton", 0)->methodPointer(self);
				})->delegate);
	}
}

static void InitAddress()
{
	SingleModeRaceEntryViewController_SetupBottomButtons_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "SingleModeRaceEntryViewController", "SetupBottomButtons", 1);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(SingleModeRaceEntryViewController_SetupBottomButtons, "Gallop.SingleModeRaceEntryViewController::SetupBottomButtons at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
