#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogManager.hpp"
#include "Screen.hpp"

namespace
{
	Il2CppMethodPointer PushDialog_addr = nullptr;
	void* PushDialog_orig = nullptr;

	Il2CppMethodPointer PushSystemDialog_addr = nullptr;
}

static Il2CppObject* PushDialog_hook(Il2CppObject* data)
{
	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto dialogData = Gallop::DialogCommon::Data(data);
		if (dialogData.DialogType() >= Gallop::DialogCommon::DialogType::LandscapeModeMenu)
		{
			dialogData.DialogType(Gallop::DialogCommon::DialogType::Game);
		}

		if (dialogData.DispStackType() >= Gallop::DialogCommon::DispStackType::LandscapeModeMenu)
		{
			dialogData.DispStackType(Gallop::DialogCommon::DispStackType::DialogOnDialog);
		
			if (dialogData.FormType() == Gallop::DialogCommonBase::FormType::WITHOUT_FRAME)
			{
				dialogData.FormType(Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
			}
		}

		auto ContentsObject = dialogData.ContentsObject();

		if (ContentsObject)
		{
			if (il2cppstring(UnityEngine::Object::Name(ContentsObject)->chars).starts_with(IL2CPP_STRING("SingleModeLogDialog")))
			{
				dialogData.FormType(Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
			}
		}
	}
	
	auto dialog = reinterpret_cast<decltype(PushDialog_hook)*>(PushDialog_orig)(data);

	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, float)>(dialog->klass, "SetContentsRootLocalScale", 1)(dialog, 1.f);
	}

	return dialog;
}

static void InitAddress()
{
	const auto DialogManager = il2cpp_symbols::get_class(ASSEMBLY_NAME, Gallop::DialogManager::namespaze, Gallop::DialogManager::klassName);
	PushDialog_addr = il2cpp_symbols::get_method_pointer(DialogManager, "PushDialog", 1);
	PushSystemDialog_addr = il2cpp_symbols::get_method_pointer(DialogManager, "PushSystemDialog", 2);
}

static void HookMethods()
{
	ADD_HOOK(PushDialog, "Gallop.DialogManager::PushDialog at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* DialogManager::namespaze = "Gallop";
	const char* DialogManager::klassName = "DialogManager";

	DialogCommon DialogManager::PushDialog(DialogCommon::Data data)
	{
		auto dialog = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(PushDialog_addr)(data);
		return DialogCommon(dialog);
	}

	DialogCommon DialogManager::PushSystemDialog(DialogCommon::Data data, bool isEnableOutsideClick)
	{
		auto dialog = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, bool)>(PushSystemDialog_addr)(data, isEnableOutsideClick);
		return DialogCommon(dialog);
	}
}
