#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StaticVariableDefine.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_orig = nullptr;

	Il2CppMethodPointer StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_orig = nullptr;

	Il2CppMethodPointer StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_orig = nullptr;

	Il2CppMethodPointer StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_addr = nullptr;
	void* StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_orig = nullptr;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_hook()
{
	return Gallop::StaticVariableDefine::Circle::CircleChat::STAMP_ANCHORED_POS_AFTER_BASE;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_hook()
{
	return Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_LOCAL_POS_AFTER_BASE;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_hook()
{
	return Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_ANCHORED_POS_AFTER_BASE;
}

static UnityEngine::Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_hook()
{
	return Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_SIZE_DELTA_POS_AFTER_BASE;
}

static void InitAddress()
{
	if (auto StaticVariableDefine_Circle_CircleChat_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StaticVariableDefine/Circle/CircleChat"))
	{
		il2cpp_runtime_class_init(StaticVariableDefine_Circle_CircleChat_klass);

		StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_STAMP_ANCHORED_POS_AFTER", 0);
		StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_LOCAL_POS_AFTER", 0);
		StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_ANCHORED_POS_AFTER", 0);
		StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_addr = il2cpp_symbols::get_method_pointer(StaticVariableDefine_Circle_CircleChat_klass, "get_INPUT_SIZE_DELTA_POS_AFTER", 0);

		if (auto StaticVariableDefine_Circle_CircleChat_ITEM_BADGE_POS_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "ITEM_BADGE_POS"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_ITEM_BADGE_POS_Field, &Gallop::StaticVariableDefine::Circle::CircleChat::ITEM_BADGE_POS);
		}

		if (auto StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "STAMP_ANCHORED_POS_AFTER_BASE"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_BASE_Field, &Gallop::StaticVariableDefine::Circle::CircleChat::STAMP_ANCHORED_POS_AFTER_BASE);
		}

		if (auto StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_LOCAL_POS_AFTER_BASE"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_BASE_Field, &Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_LOCAL_POS_AFTER_BASE);
		}

		if (auto StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_ANCHORED_POS_AFTER_BASE"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_BASE_Field, &Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_ANCHORED_POS_AFTER_BASE);
		}

		if (auto StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_BASE_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Circle_CircleChat_klass, "INPUT_SIZE_DELTA_POS_AFTER_BASE"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_BASE_Field, &Gallop::StaticVariableDefine::Circle::CircleChat::INPUT_SIZE_DELTA_POS_AFTER_BASE);
		}
	}

	if (auto StaticVariableDefine_Parts_FooterBackButton_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StaticVariableDefine/Parts/FooterBackButton"))
	{
		il2cpp_runtime_class_init(StaticVariableDefine_Parts_FooterBackButton_klass);

		if (auto StaticVariableDefine_Parts_FooterBackButton_FOOTER_ON_POS_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Parts_FooterBackButton_klass, "FOOTER_ON_POS"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Parts_FooterBackButton_FOOTER_ON_POS_Field, &Gallop::StaticVariableDefine::Parts::FooterBackButton::FOOTER_ON_POS);
		}

		if (auto StaticVariableDefine_Parts_FooterBackButton_FOOTER_OFF_POS_Field = il2cpp_class_get_field_from_name(StaticVariableDefine_Parts_FooterBackButton_klass, "FOOTER_OFF_POS"))
		{
			il2cpp_field_static_get_value(StaticVariableDefine_Parts_FooterBackButton_FOOTER_OFF_POS_Field, &Gallop::StaticVariableDefine::Parts::FooterBackButton::FOOTER_OFF_POS);
		}
	}
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

namespace Gallop
{
	UnityEngine::Vector3 StaticVariableDefine::Circle::CircleChat::ITEM_BADGE_POS;
	UnityEngine::Vector3 StaticVariableDefine::Circle::CircleChat::STAMP_ANCHORED_POS_AFTER_BASE;
	UnityEngine::Vector3 StaticVariableDefine::Circle::CircleChat::INPUT_LOCAL_POS_AFTER_BASE;
	UnityEngine::Vector3 StaticVariableDefine::Circle::CircleChat::INPUT_ANCHORED_POS_AFTER_BASE;
	UnityEngine::Vector3 StaticVariableDefine::Circle::CircleChat::INPUT_SIZE_DELTA_POS_AFTER_BASE;
	UnityEngine::Vector2 StaticVariableDefine::Parts::FooterBackButton::FOOTER_ON_POS;
	UnityEngine::Vector2 StaticVariableDefine::Parts::FooterBackButton::FOOTER_OFF_POS;
}
