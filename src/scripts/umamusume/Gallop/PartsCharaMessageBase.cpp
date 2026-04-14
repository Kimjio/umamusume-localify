#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsCharaMessageBase.hpp"

namespace
{
	void* PartsCharaMessageBase_Awake_addr = nullptr;
	void* PartsCharaMessageBase_Awake_orig = nullptr;
}

static void PartsCharaMessageBase_Awake_hook(Il2CppObject* self)
{
	auto _inlineCharacterNumField = il2cpp_class_get_field_from_name(self->klass, "_inlineCharacterNum");
	int _inlineCharacterNum = 20;
	il2cpp_field_set_value(self, _inlineCharacterNumField, &_inlineCharacterNum);

	reinterpret_cast<decltype(PartsCharaMessageBase_Awake_hook)*>(PartsCharaMessageBase_Awake_orig)(self);
}

static void InitAddress()
{
	PartsCharaMessageBase_Awake_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PartsCharaMessageBase", "Awake", 0);
}

static void HookMethods()
{
	ADD_HOOK(PartsCharaMessageBase_Awake, "Gallop.PartsCharaMessageBase::Awake at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
