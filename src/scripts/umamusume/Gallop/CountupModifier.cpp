#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CountupModifier.hpp"

#include "local/local.hpp"

namespace
{
	Il2CppClass* CountupModifier_klass = nullptr;

	void* CountupModifier_SetValue_addr = nullptr;
	void* CountupModifier_SetValue_orig = nullptr;

	void* CountupModifier_SetUlongValue_addr = nullptr;
	void* CountupModifier_SetUlongValue_orig = nullptr;

	FieldInfo* CountupModifier__prefix = nullptr;
	FieldInfo* CountupModifier__suffix = nullptr;
}

static void UpdateCountupModifierText(Il2CppObject* self)
{
	Il2CppString* prefix;
	il2cpp_field_get_value(self, CountupModifier__prefix, &prefix);

	if (prefix && !il2cppstring(prefix->chars).empty())
	{
		il2cpp_field_set_value(self, CountupModifier__prefix, local::get_localized_string(prefix));
	}

	Il2CppString* suffix;
	il2cpp_field_get_value(self, CountupModifier__suffix, &suffix);

	if (suffix && !il2cppstring(suffix->chars).empty())
	{
		il2cpp_field_set_value(self, CountupModifier__suffix, local::get_localized_string(suffix));
	}
}

static void CountupModifier_SetValue_hook(Il2CppObject* self, double value, bool immediate)
{
	UpdateCountupModifierText(self);
	reinterpret_cast<decltype(CountupModifier_SetValue_hook)*>(CountupModifier_SetValue_orig)(self, value, immediate);
}

static void CountupModifier_SetUlongValue_hook(Il2CppObject* self, uint64_t value, bool immediate)
{
	UpdateCountupModifierText(self);
	reinterpret_cast<decltype(CountupModifier_SetUlongValue_hook)*>(CountupModifier_SetUlongValue_orig)(self, value, immediate);
}

static void InitAddress()
{
	CountupModifier_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CountupModifier");
	CountupModifier__prefix = il2cpp_class_get_field_from_name(CountupModifier_klass, "_prefix");
	CountupModifier__suffix = il2cpp_class_get_field_from_name(CountupModifier_klass, "_suffix");
	CountupModifier_SetValue_addr = il2cpp_symbols::get_method_pointer(CountupModifier_klass, "SetValue", 2);
	CountupModifier_SetUlongValue_addr = il2cpp_symbols::get_method_pointer(CountupModifier_klass, "SetUlongValue", 2);
}

static void HookMethods()
{
	ADD_HOOK(CountupModifier_SetValue, "Gallop.CountupModifier::SetValue at %p\n");
	ADD_HOOK(CountupModifier_SetUlongValue, "Gallop.CountupModifier::SetUlongValue at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
