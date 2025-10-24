#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogCommon.hpp"

namespace
{
	void* DialogCommon_Close_addr = nullptr;

	void* DialogCommon_SetContentsRootLocalScale_addr = nullptr;
	void* DialogCommon_SetContentsRootLocalScale_orig = nullptr;

	void* DialogCommon_Data_get_DialogHash_addr = nullptr;

	void* DialogCommon_Data_set_DialogHash_addr = nullptr;

	void* DialogCommon_Data_SetSimpleNoButtonMessage_addr = nullptr;

	void* DialogCommon_Data_SetSimpleOneButtonMessage_addr = nullptr;

	void* DialogCommon_Data_SetSimpleOneButtonMessage2_addr = nullptr;

	void* DialogCommon_Data_SetSimpleTwoButtonMessage_addr = nullptr;

	void* DialogCommon_Data_SetSimpleThreeButtonMessage_addr = nullptr;

	void* DialogCommon_Data_AddOpenCallback_addr = nullptr;

	void* DialogCommon_Data_AddCenterButtonCallback_addr = nullptr;

	void* DialogCommon_Data_AddLeftButtonCallback_addr = nullptr;

	void* DialogCommon_Data_AddRightButtonCallback_addr = nullptr;

	void* DialogCommon_Data_AddBeginCloseCallback_addr = nullptr;

	void* DialogCommon_Data_AddDestroyCallback_addr = nullptr;

	void* DialogCommon_Data_ReleaseDialogResourceAndHash_addr = nullptr;

	FieldInfo* DialogCommon_Data_HeaderSortOrderOffsetField = nullptr;

	FieldInfo* DialogCommon_Data_LeftButtonTextField = nullptr;

	FieldInfo* DialogCommon_Data_RightButtonTextField = nullptr;

	FieldInfo* DialogCommon_Data_RightButtonNoInteractableNotifficationField = nullptr;

	FieldInfo* DialogCommon_Data_CenterButtonTextField = nullptr;

	FieldInfo* DialogCommon_Data_LeftButtonCallBackField = nullptr;

	FieldInfo* DialogCommon_Data_RightButtonCallBackField = nullptr;

	FieldInfo* DialogCommon_Data_CenterButtonCallBackField = nullptr;

	FieldInfo* DialogCommon_Data_DialogTypeField = nullptr;

	FieldInfo* DialogCommon_Data_ObjParentTypeField = nullptr;

	FieldInfo* DialogCommon_Data_DispStackTypeField = nullptr;

	FieldInfo* DialogCommon_Data_DestroyCallBackField = nullptr;

	FieldInfo* DialogCommon_Data_AutoCloseField = nullptr;
}

static void DialogCommon_SetContentsRootLocalScale_hook(Il2CppObject* self, float scale)
{
	// Always set scale to 1.0 to fix dialog size issues
	reinterpret_cast<decltype(DialogCommon_SetContentsRootLocalScale_hook)*>(DialogCommon_SetContentsRootLocalScale_orig)(self, 1.0f);
}

static void InitAddress()
{
	const auto DialogCommon = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommon");
	const auto DialogCommon_Data = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommon/Data");

	DialogCommon_Close_addr = il2cpp_symbols::get_method_pointer(DialogCommon, "Close", 0);
	DialogCommon_SetContentsRootLocalScale_addr = il2cpp_symbols::get_method_pointer(DialogCommon, "SetContentsRootLocalScale", 1);
	DialogCommon_Data_SetSimpleNoButtonMessage_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "SetSimpleNoButtonMessage", 2);
	DialogCommon_Data_SetSimpleOneButtonMessage_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "SetSimpleOneButtonMessage", 5);
	DialogCommon_Data_SetSimpleOneButtonMessage2_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "SetSimpleOneButtonMessage", 4);
	DialogCommon_Data_SetSimpleTwoButtonMessage_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "SetSimpleTwoButtonMessage", 7);
	DialogCommon_Data_SetSimpleThreeButtonMessage_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "SetSimpleThreeButtonMessage", 9);
	DialogCommon_Data_AddOpenCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddOpenCallback", 1);
	DialogCommon_Data_get_DialogHash_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "get_DialogHash", 0);
	DialogCommon_Data_set_DialogHash_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "set_DialogHash", 1);
	DialogCommon_Data_AddCenterButtonCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddCenterButtonCallback", 1);
	DialogCommon_Data_AddLeftButtonCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddLeftButtonCallback", 1);
	DialogCommon_Data_AddRightButtonCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddRightButtonCallback", 1);
	DialogCommon_Data_AddBeginCloseCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddBeginCloseCallback", 1);
	DialogCommon_Data_AddDestroyCallback_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "AddDestroyCallback", 1);
	DialogCommon_Data_ReleaseDialogResourceAndHash_addr = il2cpp_symbols::get_method_pointer(DialogCommon_Data, "ReleaseDialogResourceAndHash", 0);

	DialogCommon_Data_HeaderSortOrderOffsetField = il2cpp_class_get_field_from_name(DialogCommon_Data, "HeaderSortOrderOffset");
	DialogCommon_Data_LeftButtonTextField = il2cpp_class_get_field_from_name(DialogCommon_Data, "LeftButtonText");
	DialogCommon_Data_RightButtonTextField = il2cpp_class_get_field_from_name(DialogCommon_Data, "RightButtonText");
	DialogCommon_Data_RightButtonNoInteractableNotifficationField = il2cpp_class_get_field_from_name(DialogCommon_Data, "RightButtonNoInteractableNotiffication");
	DialogCommon_Data_CenterButtonTextField = il2cpp_class_get_field_from_name(DialogCommon_Data, "CenterButtonText");
	DialogCommon_Data_LeftButtonCallBackField = il2cpp_class_get_field_from_name(DialogCommon_Data, "LeftButtonCallBack");
	DialogCommon_Data_RightButtonCallBackField = il2cpp_class_get_field_from_name(DialogCommon_Data, "RightButtonCallBack");
	DialogCommon_Data_CenterButtonCallBackField = il2cpp_class_get_field_from_name(DialogCommon_Data, "CenterButtonCallBack");
	DialogCommon_Data_DialogTypeField = il2cpp_class_get_field_from_name(DialogCommon_Data, "DialogType");
	DialogCommon_Data_ObjParentTypeField = il2cpp_class_get_field_from_name(DialogCommon_Data, "ObjParentType");
	DialogCommon_Data_DispStackTypeField = il2cpp_class_get_field_from_name(DialogCommon_Data, "DispStackType");
	DialogCommon_Data_DestroyCallBackField = il2cpp_class_get_field_from_name(DialogCommon_Data, "DestroyCallBack");
	DialogCommon_Data_AutoCloseField = il2cpp_class_get_field_from_name(DialogCommon_Data, "AutoClose");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(DialogCommon_SetContentsRootLocalScale, "Gallop.DialogCommon::SetContentsRootLocalScale at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	void DialogCommon::Close()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(DialogCommon_Close_addr)(instance);
	}

	void DialogCommon::SetContentsRootLocalScale(float scale)
	{
		reinterpret_cast<void (*)(Il2CppObject*, float)>(DialogCommon_SetContentsRootLocalScale_addr)(instance, scale);
	}

	DialogCommon::Data::Data() : DialogCommonBaseData(il2cpp_object_new(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommon/Data")))
	{
		il2cpp_runtime_object_init(instance);
	}

	DialogCommon::Data DialogCommon::Data::SetSimpleNoButtonMessage(Il2CppString* headerTextArg, Il2CppString* message)
	{
		reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, Il2CppString*)>(DialogCommon_Data_SetSimpleNoButtonMessage_addr)(instance, headerTextArg, message);
		return *this;
	}

	DialogCommon::Data DialogCommon::Data::SetSimpleOneButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onClickCenterButton, uint64_t closeTextId, DialogCommonBase::FormType dialogFormType)
	{
		reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, Il2CppString*, Il2CppDelegate*, uint64_t, DialogCommonBase::FormType)>(DialogCommon_Data_SetSimpleOneButtonMessage_addr)(instance, headerTextArg, message, onClickCenterButton, closeTextId, dialogFormType);
		return *this;
	}

	DialogCommon::Data DialogCommon::Data::SetSimpleOneButtonMessage(uint64_t headerTextId, Il2CppString* message, Il2CppDelegate* onClose, uint64_t closeTextId)
	{
		reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, uint64_t, Il2CppString*, Il2CppDelegate*, uint64_t)>(DialogCommon_Data_SetSimpleOneButtonMessage2_addr)(instance, headerTextId, message, onClose, closeTextId);
		return *this;
	}

	DialogCommon::Data DialogCommon::Data::SetSimpleTwoButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onRight, uint64_t leftTextId, uint64_t rightTextId, Il2CppDelegate* onLeft, DialogCommonBase::FormType dialogFormType)
	{
		reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, Il2CppString*, Il2CppDelegate*, uint64_t, uint64_t, Il2CppDelegate*, DialogCommonBase::FormType)>(DialogCommon_Data_SetSimpleTwoButtonMessage_addr)(instance, headerTextArg, message, onRight, leftTextId, rightTextId, onLeft, dialogFormType);
		return *this;
	}

	DialogCommon::Data DialogCommon::Data::SetSimpleThreeButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onRight, uint64_t rightTextId, Il2CppDelegate* onCenter, uint64_t centerTextId, Il2CppDelegate* onLeft, uint64_t leftTextId, DialogCommonBase::FormType dialogFormType)
	{
		reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, Il2CppString*, Il2CppDelegate*, uint64_t, Il2CppDelegate*, uint64_t, Il2CppDelegate*, uint64_t, DialogCommonBase::FormType)>(DialogCommon_Data_SetSimpleThreeButtonMessage_addr)(instance, headerTextArg, message, onRight, rightTextId, onCenter, centerTextId, onLeft, leftTextId, dialogFormType);
		return *this;
	}

	uint64_t DialogCommon::Data::DialogHash()
	{
		return reinterpret_cast<uint64_t(*)(Il2CppObject*)>(DialogCommon_Data_get_DialogHash_addr)(instance);
	}

	void DialogCommon::Data::DialogHash(uint64_t value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, uint64_t)>(DialogCommon_Data_set_DialogHash_addr)(instance, value);
	}

	void DialogCommon::Data::AddOpenCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddOpenCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::AddCenterButtonCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddCenterButtonCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::AddLeftButtonCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddLeftButtonCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::AddRightButtonCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddRightButtonCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::AddBeginCloseCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddBeginCloseCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::AddDestroyCallback(Il2CppDelegate* callback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogCommon_Data_AddDestroyCallback_addr)(instance, callback);
	}

	void DialogCommon::Data::ReleaseDialogResourceAndHash()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(DialogCommon_Data_ReleaseDialogResourceAndHash_addr)(instance);
	}

	int DialogCommon::Data::HeaderSortOrderOffset()
	{
		int value;
		il2cpp_field_get_value(instance, DialogCommon_Data_HeaderSortOrderOffsetField, &value);
		return value;
	}

	void DialogCommon::Data::HeaderSortOrderOffset(int value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_HeaderSortOrderOffsetField, &value);
	}

	Il2CppString* DialogCommon::Data::LeftButtonText()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_LeftButtonTextField, &value);
		return value;
	}

	void DialogCommon::Data::LeftButtonText(Il2CppString* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_LeftButtonTextField, value);
	}

	Il2CppString* DialogCommon::Data::RightButtonText()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_RightButtonTextField, &value);
		return value;
	}

	void DialogCommon::Data::RightButtonText(Il2CppString* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_RightButtonTextField, value);
	}

	Il2CppString* DialogCommon::Data::RightButtonNoInteractableNotiffication()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_RightButtonNoInteractableNotifficationField, &value);
		return value;
	}

	void DialogCommon::Data::RightButtonNoInteractableNotiffication(Il2CppString* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_RightButtonNoInteractableNotifficationField, value);
	}

	Il2CppString* DialogCommon::Data::CenterButtonText()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_CenterButtonTextField, &value);
		return value;
	}

	void DialogCommon::Data::CenterButtonText(Il2CppString* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_CenterButtonTextField, value);
	}

	Il2CppDelegate* DialogCommon::Data::LeftButtonCallBack()
	{
		Il2CppDelegate* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_LeftButtonCallBackField, &value);
		return value;
	}

	void DialogCommon::Data::LeftButtonCallBack(Il2CppDelegate* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_LeftButtonCallBackField, value);
	}

	Il2CppDelegate* DialogCommon::Data::RightButtonCallBack()
	{
		Il2CppDelegate* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_RightButtonCallBackField, &value);
		return value;
	}

	void DialogCommon::Data::RightButtonCallBack(Il2CppDelegate* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_RightButtonCallBackField, value);
	}

	Il2CppDelegate* DialogCommon::Data::CenterButtonCallBack()
	{
		Il2CppDelegate* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_CenterButtonCallBackField, &value);
		return value;
	}

	void DialogCommon::Data::CenterButtonCallBack(Il2CppDelegate* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_CenterButtonCallBackField, value);
	}

	DialogCommon::DialogType DialogCommon::Data::DialogType()
	{
		DialogCommon::DialogType value;
		il2cpp_field_get_value(instance, DialogCommon_Data_DialogTypeField, &value);
		return value;
	}

	void DialogCommon::Data::DialogType(DialogCommon::DialogType value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_DialogTypeField, &value);
	}

	DialogCommon::Data::ObjectParentType DialogCommon::Data::ObjParentType()
	{
		DialogCommon::Data::ObjectParentType value;
		il2cpp_field_get_value(instance, DialogCommon_Data_ObjParentTypeField, &value);
		return value;
	}

	void DialogCommon::Data::ObjParentType(DialogCommon::Data::ObjectParentType value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_ObjParentTypeField, &value);
	}

	DialogCommon::DispStackType DialogCommon::Data::DispStackType()
	{
		DialogCommon::DispStackType value;
		il2cpp_field_get_value(instance, DialogCommon_Data_DispStackTypeField, &value);
		return value;
	}

	void DialogCommon::Data::DispStackType(DialogCommon::DispStackType value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_DispStackTypeField, &value);
	}

	Il2CppDelegate* DialogCommon::Data::DestroyCallBack()
	{
		Il2CppDelegate* value;
		il2cpp_field_get_value(instance, DialogCommon_Data_DestroyCallBackField, &value);
		return value;
	}

	void DialogCommon::Data::DestroyCallBack(Il2CppDelegate* value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_DestroyCallBackField, value);
	}

	bool DialogCommon::Data::AutoClose()
	{
		bool value;
		il2cpp_field_get_value(instance, DialogCommon_Data_AutoCloseField, &value);
		return value;
	}

	void DialogCommon::Data::AutoClose(bool value)
	{
		il2cpp_field_set_value(instance, DialogCommon_Data_AutoCloseField, &value);
	}
}
