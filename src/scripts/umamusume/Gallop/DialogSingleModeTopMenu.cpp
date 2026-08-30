#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogSingleModeTopMenu.hpp"

#include "Screen.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer DialogSingleModeTopMenu_Setup_addr = nullptr;
	void* DialogSingleModeTopMenu_Setup_orig = nullptr;

	Il2CppMethodPointer DialogSingleModeTopMenu_Setup1_addr = nullptr;
	void* DialogSingleModeTopMenu_Setup1_orig = nullptr;

	Il2CppMethodPointer DialogSingleModeTopMenu_SetupButtonPositions_addr = nullptr;
	void* DialogSingleModeTopMenu_SetupButtonPositions_orig = nullptr;
}

static void SetupLandscapeInfoButton(Il2CppObject* self)
{
	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto _infoButtonField = il2cpp_class_get_field_from_name(self->klass, "_infoButton");
		Il2CppObject* _infoButton;
		il2cpp_field_get_value(self, _infoButtonField, &_infoButton);

		UnityEngine::MonoBehaviour(_infoButton).gameObject().SetActive(true);

		bool _isIdleSingleMode = false;

		if (auto _isIdleSingleModeField = il2cpp_class_get_field_from_name(self->klass, "_isIdleSingleMode"))
		{
			il2cpp_field_get_value(self, _isIdleSingleModeField, &_isIdleSingleMode);
		}

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(_infoButton->klass, "SetOnClick", 1)(_infoButton,
			_isIdleSingleMode ?
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_symbols::get_method_pointer<void (*)()>(ASSEMBLY_NAME, "Gallop", "DialogSingleModeStartConfirmEntry", "OpenPlayingIdleSingleMode", 0)();
				})->delegate :
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Gallop::DialogCommon::DispStackType, Gallop::DialogCommonBase::FormType)>(ASSEMBLY_NAME, "Gallop", "DialogSingleModeStartConfirmEntry", "OpenPlayingSingleMode", 2)(Gallop::DialogCommon::DispStackType::DialogOnDialog, Gallop::DialogCommonBase::FormType::WITHOUT_FRAME);
				})->delegate);
	}
}

static void DialogSingleModeTopMenu_Setup_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(DialogSingleModeTopMenu_Setup_hook)*>(DialogSingleModeTopMenu_Setup_orig)(self);

	SetupLandscapeInfoButton(self);
}

static void DialogSingleModeTopMenu_Setup1_hook(Il2CppObject* self, int dialogType)
{
	reinterpret_cast<decltype(DialogSingleModeTopMenu_Setup1_hook)*>(DialogSingleModeTopMenu_Setup1_orig)(self, dialogType);

	SetupLandscapeInfoButton(self);
}

static void DialogSingleModeTopMenu_SetupButtonPositions_hook(Il2CppObject* self)
{
	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		return;
	}

	reinterpret_cast<decltype(DialogSingleModeTopMenu_SetupButtonPositions_hook)*>(DialogSingleModeTopMenu_SetupButtonPositions_orig)(self);
}

static void InitAddress()
{
	auto DialogSingleModeTopMenu_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogSingleModeTopMenu");
	DialogSingleModeTopMenu_Setup_addr = il2cpp_symbols::get_method_pointer(DialogSingleModeTopMenu_klass, "Setup", 0);
	DialogSingleModeTopMenu_Setup1_addr = il2cpp_symbols::get_method_pointer(DialogSingleModeTopMenu_klass, "Setup", 1);
	DialogSingleModeTopMenu_SetupButtonPositions_addr = il2cpp_symbols::get_method_pointer(DialogSingleModeTopMenu_klass, "SetupButtonPositions", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(DialogSingleModeTopMenu_SetupButtonPositions, "Gallop.DialogSingleModeTopMenu::SetupButtonPositions at %p\n");
		ADD_HOOK(DialogSingleModeTopMenu_Setup, "Gallop.DialogSingleModeTopMenu::Setup at %p\n");
		ADD_HOOK(DialogSingleModeTopMenu_Setup1, "Gallop.DialogSingleModeTopMenu::Setup1 at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
