#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Header.hpp"
#include "DialogCommon.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

#include "config/config.hpp"

namespace
{
	void* Header_Initialize_addr = nullptr;
	void* Header_Initialize_orig = nullptr;
}

static void Header_Initialize_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(Header_Initialize_hook)*>(Header_Initialize_orig)(self);

	if (Gallop::Screen::IsLandscapeMode())
	{
		auto _menuButtonField = il2cpp_class_get_field_from_name(self->klass, "_menuButton");
		Il2CppObject* _menuButton;
		il2cpp_field_get_value(self, _menuButtonField, &_menuButton);

		UnityEngine::MonoBehaviour(_menuButton).gameObject().SetActive(true);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_menuButton->klass, "SetOnClick", 1)->methodPointer(_menuButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*, Gallop::DialogCommon::DispStackType, Gallop::DialogCommonBase::FormType)>(ASSEMBLY_NAME, "Gallop", "DialogHomeMenuMain", "Open", 4)(nullptr, nullptr, Gallop::DialogCommon::DispStackType::DialogOnDialog, Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
				})->delegate
		);
	}
}

static void InitAddress()
{
	Header_Initialize_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Header", "Initialize", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(Header_Initialize, "Gallop.Header::Initialize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
