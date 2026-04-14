#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CharacterBg.hpp"
#include "Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* CircleChat_Initialize_addr = nullptr;
	void* CircleChat_Initialize_orig = nullptr;
}

static void CircleChat_Initialize_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(CircleChat_Initialize_hook)*>(CircleChat_Initialize_orig)(self);

	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto _itemRequestButtonField = il2cpp_class_get_field_from_name(self->klass, "_itemRequestButton");
		Il2CppObject* _itemRequestButton;
		il2cpp_field_get_value(self, _itemRequestButtonField, &_itemRequestButton);

		UnityEngine::MonoBehaviour(_itemRequestButton).gameObject().SetActive(true);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_itemRequestButton->klass, "SetOnClick", 1)->methodPointer(_itemRequestButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickItemRequest", 0)->methodPointer(self);
				})->delegate);
	}
}

static void InitAddress()
{
	CircleChat_Initialize_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "CircleChat", "Initialize", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(CircleChat_Initialize, "Gallop.CircleChat::Initialize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
