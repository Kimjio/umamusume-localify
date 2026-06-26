#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "FooterBackButton.hpp"

#include "StaticVariableDefine.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer FooterBackButton_Open_addr = nullptr;
	void* FooterBackButton_Open_orig = nullptr;

	FieldInfo* FooterBackButton__backButtonRect_Field = nullptr;
}

static void FooterBackButton_Open_hook(Il2CppObject* self, float delay, bool isFooterShow)
{
	reinterpret_cast<decltype(FooterBackButton_Open_hook)*>(FooterBackButton_Open_orig)(self, delay, isFooterShow);

	Il2CppObject* _backButtonRect;
	il2cpp_field_get_value(self, FooterBackButton__backButtonRect_Field, &_backButtonRect);
	UnityEngine::RectTransform(_backButtonRect).anchoredPosition(isFooterShow ? Gallop::StaticVariableDefine::Parts::FooterBackButton::FOOTER_ON_POS : Gallop::StaticVariableDefine::Parts::FooterBackButton::FOOTER_OFF_POS);
}

static void InitAddress()
{
	auto FooterBackButton_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "FooterBackButton");
	FooterBackButton_Open_addr = il2cpp_symbols::get_method_pointer(FooterBackButton_klass, "Open", 2);
	FooterBackButton__backButtonRect_Field = il2cpp_class_get_field_from_name(FooterBackButton_klass, "_backButtonRect");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(FooterBackButton_Open, "Gallop.FooterBackButton::Open at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
