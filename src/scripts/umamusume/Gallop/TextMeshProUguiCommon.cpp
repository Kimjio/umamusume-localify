#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TextMeshProUguiCommon.hpp"

#include <cstdint>

#include "scripts/UnityEngine.CoreModule/UnityEngine/Color.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Color32.hpp"
#include "scripts/mscorlib/System/Collections/Generic/Dictionary.hpp"

#include "config/config.hpp"

#include "local/local.hpp"

namespace
{
	void* TextMeshProUguiCommon_Awake_addr = nullptr;
	void* TextMeshProUguiCommon_Awake_orig = nullptr;
}

static void TextMeshProUguiCommon_Awake_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(TextMeshProUguiCommon_Awake_hook)*>(TextMeshProUguiCommon_Awake_orig)(self);
	auto customFont = GetCustomTMPFont();

	if (!customFont)
	{
		return;
	}

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "set_font", 1)->methodPointer(self, customFont);
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(self->klass, "set_enableWordWrapping", 1)->methodPointer(self, false);

	auto customFontMaterialField = il2cpp_class_get_field_from_name(customFont->klass, "material");
	Il2CppObject* customFontMaterial;
	il2cpp_field_get_value(customFont, customFontMaterialField, &customFontMaterial);
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "set_fontSharedMaterial", 1)->methodPointer(self, customFontMaterial);

	auto origOutlineWidth = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(self->klass, "get_outlineWidth", 0)->methodPointer(self);

	auto outlineColorDictField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "ColorPreset"), "OutlineColorDictionary");
	Il2CppObject* outlineColorDict;
	il2cpp_field_static_get_value(outlineColorDictField, &outlineColorDict);
	auto colorEnum = il2cpp_class_get_method_from_name_type<uint32_t(*)(Il2CppObject*)>(self->klass, "get_OutlineColor", 0)->methodPointer(self);

	auto entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "_entries");
	if (!entriesField)
	{
		entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "entries");
	}
	Il2CppArraySize_t<System::Collections::Generic::Dictionary<uint32_t, UnityEngine::Color32>::Entry>* entries;
	il2cpp_field_get_value(outlineColorDict, entriesField, &entries);

	UnityEngine::Color32 color32{};
	for (int i = 0; i < entries->max_length; i++)
	{
		auto entry = entries->vector[i];
		if (entry.key == colorEnum)
		{
			color32 = entry.value;
			break;
		}
	}

	float a = color32.a / static_cast<float>(0xff);
	float b = color32.b / static_cast<float>(0xff);
	float g = color32.g / static_cast<float>(0xff);
	float r = color32.r / static_cast<float>(0xff);

	auto origOutlineColor = UnityEngine::Color{ r, g, b, a };

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, float)>(customFontMaterial->klass, "SetFloat", 2)->methodPointer(customFontMaterial, il2cpp_string_new("_OutlineWidth"), origOutlineWidth);
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, UnityEngine::Color)>(customFontMaterial->klass, "SetColor", 2)->methodPointer(customFontMaterial, il2cpp_string_new("_OutlineColor"), origOutlineColor);
}

static void InitAddress()
{
	TextMeshProUguiCommon_Awake_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "TextMeshProUguiCommon", "Awake", 0);
}

static void HookMethods()
{
	if (config::replace_to_builtin_font || config::replace_to_custom_font)
	{
		ADD_HOOK(TextMeshProUguiCommon_Awake, "Gallop.TextMeshProUguiCommon::Awake at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
