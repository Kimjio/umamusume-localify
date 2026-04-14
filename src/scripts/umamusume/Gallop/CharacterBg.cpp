#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CharacterBg.hpp"

#include "config/config.hpp"

namespace
{
	void* CharacterBg_SetupDefaultPosition_addr = nullptr;
	void* CharacterBg_SetupDefaultPosition_orig = nullptr;
}

static void CharacterBg_SetupDefaultPosition_hook(Il2CppObject* self)
{
	auto _skipSetBgDefaultPosField = il2cpp_class_get_field_from_name(self->klass, "_skipSetBgDefaultPos");
	bool _skipSetBgDefaultPos = true;
	il2cpp_field_set_value(self, _skipSetBgDefaultPosField, &_skipSetBgDefaultPos);

	auto _defaultBgPosLocalField = il2cpp_class_get_field_from_name(self->klass, "_defaultBgPosLocal");
	UnityEngine::Vector3 _defaultBgPosLocal{ 0, 0, 20.0f };
	il2cpp_field_set_value(self, _defaultBgPosLocalField, &_defaultBgPosLocal);

	auto _bgRendererField = il2cpp_class_get_field_from_name(self->klass, "_bgRenderer");
	Il2CppObject* _bgRenderer;
	il2cpp_field_get_value(self, _bgRendererField, &_bgRenderer);

	auto bgRendererTransform = UnityEngine::Component(_bgRenderer).transform();
	auto localScale = bgRendererTransform.localScale();

	reinterpret_cast<decltype(CharacterBg_SetupDefaultPosition_hook)*>(CharacterBg_SetupDefaultPosition_orig)(self);

	bgRendererTransform.localScale(localScale);
}

static void InitAddress()
{
	CharacterBg_SetupDefaultPosition_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "CharacterBg", "SetupDefaultPosition", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(CharacterBg_SetupDefaultPosition, "Gallop.CharacterBg::SetupDefaultPosition at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
