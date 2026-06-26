#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsSingleModeStoryChoiceRewardInfoButton.hpp"

namespace
{
	Il2CppMethodPointer OnClickButton_addr = nullptr;
	void* OnClickButton_orig = nullptr;

	const MethodInfo* DialogSingleModeStoryChoiceReward_PushDialog = nullptr;

	FieldInfo* _eventIdField = nullptr;
	FieldInfo* _choiceParamListField = nullptr;
}

static void OnClickButton_hook(Il2CppObject* self)
{
	int _eventId;
	il2cpp_field_get_value(self, _eventIdField, &_eventId);

	Il2CppObject* _choiceParamList;
	il2cpp_field_get_value(self, _choiceParamListField, &_choiceParamList);

	auto model = il2cpp_object_new(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SingleModeStoryChoiceRewardInfoModel"));
	il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int, Il2CppObject*)>(model->klass, ".ctor", 2)(model, _eventId, _choiceParamList);

	auto onSuccess = CreateDelegate(model, *[](Il2CppObject* model)
		{
			if (DialogSingleModeStoryChoiceReward_PushDialog->parameters_count < 2)
			{
				// legacy
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(DialogSingleModeStoryChoiceReward_PushDialog)(model);
				return;
			}
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(DialogSingleModeStoryChoiceReward_PushDialog)(model, nullptr);
		});
	il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(model->klass, "FetchChoiceRewardInfo", 1)(model, onSuccess);
}

static void InitAddress()
{
	auto klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PartsSingleModeStoryChoiceRewardInfoButton");
	OnClickButton_addr = il2cpp_symbols::get_method_pointer(klass, "OnClickButton", 0);
	_eventIdField = il2cpp_class_get_field_from_name(klass, "_eventId");
	_choiceParamListField = il2cpp_class_get_field_from_name(klass, "_choiceParamList");
	DialogSingleModeStoryChoiceReward_PushDialog = il2cpp_symbols::get_method(ASSEMBLY_NAME, "Gallop", "DialogSingleModeStoryChoiceReward", "PushDialog", IgnoreNumberOfArguments);
}


static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(OnClickButton, "Gallop.PartsSingleModeStoryChoiceRewardInfoButton::OnClickButton at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
