#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CharacterBg.hpp"
#include "Screen.hpp"
#include "StaticVariableDefine.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppClass* CircleChat_klass = nullptr;

	Il2CppMethodPointer CircleChat_Initialize_addr = nullptr;
	void* CircleChat_Initialize_orig = nullptr;

	Il2CppMethodPointer CircleChat_Update_addr = nullptr;
	void* CircleChat_Update_orig = nullptr;

	Il2CppMethodPointer CircleChat_UpdatePolling_addr = nullptr;

	Il2CppMethodPointer CircleChat_UpdateItemRequestStatus_addr = nullptr;

	FieldInfo* CircleChat__itemReqBadgeField = nullptr;
}

static void CircleChat_Initialize_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(CircleChat_Initialize_hook)*>(CircleChat_Initialize_orig)(self);

	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		auto _itemRequestButtonField = il2cpp_class_get_field_from_name(self->klass, "_itemRequestButton");
		Il2CppObject* _itemRequestButton;
		il2cpp_field_get_value(self, _itemRequestButtonField, &_itemRequestButton);

		MonoBehaviour(_itemRequestButton).gameObject().SetActive(true);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(_itemRequestButton->klass, "SetOnClick", 1)(_itemRequestButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(self->klass, "OnClickItemRequest", 0)(self);
				})->delegate);

		MonoBehaviour _itemReqBadge = il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*, int, Il2CppObject*)>(ASSEMBLY_NAME, "Gallop", "UIUtil", "CreateNotifyBadgeIconFlash", 3)(MonoBehaviour(_itemRequestButton).transform(), 200, nullptr);
		_itemReqBadge.transform().localPosition(Gallop::StaticVariableDefine::Circle::CircleChat::ITEM_BADGE_POS);

		il2cpp_field_set_value(self, CircleChat__itemReqBadgeField, _itemReqBadge.NativeObject());
	}
}

static void CircleChat_Update_hook(Il2CppObject* self)
{
	if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(CircleChat_UpdatePolling_addr)(self);
		reinterpret_cast<void (*)(Il2CppObject*)>(CircleChat_UpdateItemRequestStatus_addr)(self);
	}
	else
	{
		reinterpret_cast<decltype(CircleChat_Update_hook)*>(CircleChat_Update_orig)(self);
	}
}

static void InitAddress()
{
	CircleChat_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CircleChat");
	CircleChat_Initialize_addr = il2cpp_symbols::get_method_pointer(CircleChat_klass, "Initialize", 0);
	CircleChat_Update_addr = il2cpp_symbols::get_method_pointer(CircleChat_klass, "Update", 0);
	CircleChat_UpdatePolling_addr = il2cpp_symbols::get_method_pointer(CircleChat_klass, "UpdatePolling", 0);
	CircleChat_UpdateItemRequestStatus_addr = il2cpp_symbols::get_method_pointer(CircleChat_klass, "UpdateItemRequestStatus", 0);
	CircleChat__itemReqBadgeField = il2cpp_class_get_field_from_name(CircleChat_klass, "_itemReqBadge");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(CircleChat_Initialize, "Gallop.CircleChat::Initialize at %p\n");
		ADD_HOOK(CircleChat_Update, "Gallop.CircleChat::Update at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
