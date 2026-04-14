#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogHomeMenuMain.hpp"

#include "Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* DialogHomeMenuMain_ChangeView_addr = nullptr;
	void* DialogHomeMenuMain_ChangeView_orig = nullptr;

	void* DialogHomeMenuMain_ChangeView2_addr = nullptr;
	void* DialogHomeMenuMain_ChangeView2_orig = nullptr;
}

static void DialogHomeMenuMain_ChangeView_hook(Il2CppObject* self, uint64_t viewId)
{
	reinterpret_cast<decltype(DialogHomeMenuMain_ChangeView_hook)*>(DialogHomeMenuMain_ChangeView_orig)(self, viewId);

	if (!Gallop::Screen::IsLandscapeMode())
	{
		return;
	}

	auto _menuDialogField = il2cpp_class_get_field_from_name(self->klass, "_menuDialog");
	Il2CppObject* _menuDialog;
	il2cpp_field_static_get_value(_menuDialogField, &_menuDialog);

	if (_menuDialog)
	{
		auto dialogCommon = UnityEngine::GameObject(_menuDialog).GetComponent(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogCommon"));
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogManager", "PopDialog", 2)(dialogCommon, nullptr);
		_menuDialog = nullptr;
		il2cpp_field_static_set_value(_menuDialogField, _menuDialog);
	}
}

static void DialogHomeMenuMain_ChangeView2_hook(Il2CppObject* self, uint64_t viewId, Il2CppObject* viewInfo)
{
	reinterpret_cast<decltype(DialogHomeMenuMain_ChangeView2_hook)*>(DialogHomeMenuMain_ChangeView2_orig)(self, viewId, viewInfo);

	if (!Gallop::Screen::IsLandscapeMode())
	{
		return;
	}

	auto _menuDialogField = il2cpp_class_get_field_from_name(self->klass, "_menuDialog");
	Il2CppObject* _menuDialog;
	il2cpp_field_static_get_value(_menuDialogField, &_menuDialog);

	if (_menuDialog)
	{
		auto dialogCommon = UnityEngine::GameObject(_menuDialog).GetComponent(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogCommon"));
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogManager", "PopDialog", 2)(dialogCommon, nullptr);
		_menuDialog = nullptr;
		il2cpp_field_static_set_value(_menuDialogField, _menuDialog);
	}
}

static void InitAddress()
{
	auto DialogHomeMenuMain_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogHomeMenuMain");
	DialogHomeMenuMain_ChangeView_addr = il2cpp_symbols::get_method_pointer(DialogHomeMenuMain_klass, "ChangeView", 1);
	DialogHomeMenuMain_ChangeView2_addr = il2cpp_symbols::get_method_pointer(DialogHomeMenuMain_klass, "ChangeView", 2);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(DialogHomeMenuMain_ChangeView, "Gallop.DialogHomeMenuMain::ChangeView at %p\n");
		ADD_HOOK(DialogHomeMenuMain_ChangeView2, "Gallop.DialogHomeMenuMain::ChangeView2 at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
