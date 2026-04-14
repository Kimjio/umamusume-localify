#include "../Plugins.hpp"
#include "../../ScriptInternal.hpp"
#include "AnText.hpp"

#include "local/local.hpp"

namespace
{
	void* AnText__FixData_addr = nullptr;
	void* AnText__FixData_orig = nullptr;

	FieldInfo* AnText__text = nullptr;
}

static void AnText__FixData_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(AnText__FixData_hook)*>(AnText__FixData_orig)(self);

	Il2CppString* str;
	il2cpp_field_get_value(self, AnText__text, &str);
	il2cpp_field_set_value(self, AnText__text, local::get_localized_string(str));
}

static void InitAddress()
{
	auto AnText_klass = il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", "AnText");
	AnText__FixData_addr = il2cpp_symbols::get_method_pointer(AnText_klass, "_FixData", 0);
	AnText__text = il2cpp_class_get_field_from_name(AnText_klass, "_text");
}

static void HookMethods()
{
	ADD_HOOK(AnText__FixData, "AnimateToUnity.AnText::_FixData at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
