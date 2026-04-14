#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsSingleModeCommonFooter.hpp"
#include "DialogCommon.hpp"
#include "Screen.hpp"

#include "local/local.hpp"

namespace
{
	void* StoryRaceTextAsset_Load_addr = nullptr;
	void* StoryRaceTextAsset_Load_orig = nullptr;
	
	FieldInfo* StoryRaceTextAsset_textDataField = nullptr;
}

static void StoryRaceTextAsset_Load_hook(Il2CppObject* self)
{
	Il2CppObject* textData;
	il2cpp_field_get_value(self, StoryRaceTextAsset_textDataField, &textData);

	auto enumerator = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(textData->klass, "GetEnumerator", 0)->methodPointer(textData);
	auto get_current = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(enumerator->klass, "get_Current", 0)->methodPointer;
	auto move_next = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(enumerator->klass, "MoveNext", 0)->methodPointer;

	while (move_next(enumerator))
	{
		auto key = get_current(enumerator);
		auto textField = il2cpp_class_get_field_from_name(key->klass, "text");
		Il2CppString* text;
		il2cpp_field_get_value(key, textField, &text);
		il2cpp_field_set_value(key, textField, local::get_localized_string(text));
	}

	reinterpret_cast<decltype(StoryRaceTextAsset_Load_hook)*>(StoryRaceTextAsset_Load_orig)(self);
}

static void InitAddress()
{
	auto StoryRaceTextAsset_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StoryRaceTextAsset");
	StoryRaceTextAsset_Load_addr = il2cpp_symbols::get_method_pointer(StoryRaceTextAsset_klass, "Load", 0);
	StoryRaceTextAsset_textDataField = il2cpp_class_get_field_from_name(StoryRaceTextAsset_klass, "textData");
}

static void HookMethods()
{
	ADD_HOOK(StoryRaceTextAsset_Load, "Gallop.StoryRaceTextAsset.Load at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
