#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogCommonBaseData.hpp"

namespace
{
	FieldInfo* DialogCommonBaseData_FormTypeField = nullptr;

	FieldInfo* DialogCommonBaseData_TitleField = nullptr;

	FieldInfo* DialogCommonBaseData_ContentsObjectField = nullptr;
}

static void InitAddress()
{
	const auto DialogCommonBaseData = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommonBaseData");
	DialogCommonBaseData_FormTypeField = il2cpp_class_get_field_from_name(DialogCommonBaseData, "FormType");
	DialogCommonBaseData_TitleField = il2cpp_class_get_field_from_name(DialogCommonBaseData, "Title");
	DialogCommonBaseData_ContentsObjectField = il2cpp_class_get_field_from_name(DialogCommonBaseData, "ContentsObject");
}


STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	DialogCommonBase::FormType DialogCommonBaseData::FormType()
	{
		DialogCommonBase::FormType value;
		il2cpp_field_get_value(instance, DialogCommonBaseData_FormTypeField, &value);
		return value;
	}

	void DialogCommonBaseData::FormType(DialogCommonBase::FormType value)
	{
		il2cpp_field_set_value(instance, DialogCommonBaseData_FormTypeField, &value);
	}

	Il2CppString* DialogCommonBaseData::Title()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, DialogCommonBaseData_TitleField, &value);
		return value;
	}

	void DialogCommonBaseData::Title(Il2CppString* value)
	{
		il2cpp_field_set_value(instance, DialogCommonBaseData_TitleField, value);
	}

	UnityEngine::GameObject DialogCommonBaseData::ContentsObject()
	{
		Il2CppObject* value;
		il2cpp_field_get_value(instance, DialogCommonBaseData_ContentsObjectField, &value);
		return UnityEngine::GameObject(value);
	}

	void DialogCommonBaseData::ContentsObject(UnityEngine::GameObject value)
	{
		il2cpp_field_set_value(instance, DialogCommonBaseData_ContentsObjectField, value);
	}
}
