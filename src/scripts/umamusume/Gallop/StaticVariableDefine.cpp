#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StaticVariableDefine.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

#include "config/config.hpp"

namespace
{
	void* StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_orig = nullptr;

	void* StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_orig = nullptr;

	void* StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_orig = nullptr;

	void* StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_orig = nullptr;

	FieldInfo* StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_BASE_Field = nullptr;
	FieldInfo* StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_BASE_Field = nullptr;
	FieldInfo* StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_BASE_Field = nullptr;
	FieldInfo* StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_BASE_Field = nullptr;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_hook()
{
	UnityEngine::Vector3 value;

	il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_BASE_Field, &value);

	return value;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_hook()
{
	UnityEngine::Vector3 value;

	il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_BASE_Field, &value);

	return value;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_hook()
{
	UnityEngine::Vector3 value;

	il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_BASE_Field, &value);

	return value;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_hook()
{
	UnityEngine::Vector3 value;

	il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_BASE_Field, &value);

	return value;
}

static void InitAddress()
{
	auto StaticVariableDefine_Circle_CircleChat_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StaticVariableDefine/Circle/CircleChat");
	StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_STAMP_ANCHORED_POS_AFTER", 0);
	StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_LOCAL_POS_AFTER", 0);
	StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_ANCHORED_POS_AFTER", 0);
	StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_SIZE_DELTA_POS_AFTER", 0);
	StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "STAMP_ANCHORED_POS_AFTER_BASE");
	StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_LOCAL_POS_AFTER_BASE");
	StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_ANCHORED_POS_AFTER_BASE");
	StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_SIZE_DELTA_POS_AFTER_BASE");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_STAMP_ANCHORED_POS_AFTER at %p\n");
		ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_LOCAL_POS_AFTER at %p\n");
		ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_ANCHORED_POS_AFTER at %p\n");
		ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_SIZE_DELTA_POS_AFTER at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
