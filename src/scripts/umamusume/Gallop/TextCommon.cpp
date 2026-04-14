#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TextCommon.hpp"

#include "local/local.hpp"

namespace
{
	void* TextCommon_get_TextId_addr = nullptr;

	void* TextCommon_set_FontColor_addr = nullptr;

	void* TextCommon_set_OutlineColor_addr = nullptr;

	void* TextCommon_set_OutlineSize_addr = nullptr;

	void* TextCommon_Awake_addr = nullptr;
	void* TextCommon_Awake_orig = nullptr;

	void* TextCommon_UpdateOutline_addr = nullptr;

	void* TextCommon_SetTextWithCustomTag_addr = nullptr;

	void* TextCommon_ApplyFont_addr = nullptr;
	void* TextCommon_ApplyFont_orig = nullptr;

	Il2CppReflectionType* FontColorType = nullptr;
	Il2CppReflectionType* OutlineSizeType = nullptr;
	Il2CppReflectionType* OutlineColorType = nullptr;
}

static void TextCommon_Awake_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(TextCommon_Awake_hook)*>(TextCommon_Awake_orig)(self);

	auto text = Gallop::TextCommon(self);

	if (config::replace_to_builtin_font && text.lineSpacing() != 1.05f)
	{
		text.fontStyle(UnityEngine::FontStyle::Bold);
		text.fontSize(text.fontSize() - 4);
		text.lineSpacing(1.05f);
	}

	auto textId = text.TextId();
	if (textId)
	{
		if (GetTextIdByName(IL2CPP_STRING("Common0121")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Common0186")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Outgame0028")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Outgame0231")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Outgame0393")) == textId ||
			GetTextIdByName(IL2CPP_STRING("SingleMode0265")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Character0039")) == textId ||
			GetTextIdByName(IL2CPP_STRING("Character0325")) == textId)
		{
			text.horizontalOverflow(UnityEngine::HorizontalWrapMode::Overflow);
			text.verticalOverflow(UnityEngine::VerticalWrapMode::Overflow);
		}
	}

	if (auto str = text.text())
	{
		text.text(local::get_localized_string(str));
	}
}

static void TextCommon_ApplyFont_hook(Il2CppObject* self)
{
	auto text = Gallop::TextCommon(self);
	if (config::replace_to_builtin_font)
	{
		text.AssignDefaultFont();
		return;
	}

	if (config::replace_to_custom_font)
	{
		if (text.font())
		{
			text.font(nullptr);
		}

		auto customFont = GetCustomFont();
		if (customFont)
		{
			text.font(customFont);
		}
	}

	reinterpret_cast<decltype(TextCommon_ApplyFont_hook)*>(TextCommon_ApplyFont_orig)(self);
}

static void InitAddress()
{
	auto TextCommon_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "TextCommon");
	TextCommon_get_TextId_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "get_TextId", 0)->methodPointer;
	TextCommon_set_FontColor_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "set_FontColor", 1)->methodPointer;
	TextCommon_set_OutlineColor_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "set_OutlineColor", 1)->methodPointer;
	TextCommon_set_OutlineSize_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "set_OutlineSize", 1)->methodPointer;
	TextCommon_Awake_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "Awake", 0)->methodPointer;
	TextCommon_ApplyFont_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "ApplyFont", 0)->methodPointer;
	TextCommon_UpdateOutline_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "UpdateOutline", 0)->methodPointer;
	TextCommon_SetTextWithCustomTag_addr = il2cpp_class_get_method_from_name(TextCommon_klass, "SetTextWithCustomTag", 3)->methodPointer;

	FontColorType = GetRuntimeType(ASSEMBLY_NAME, "Gallop", "FontColorType");
	OutlineSizeType = GetRuntimeType(ASSEMBLY_NAME, "Gallop", "OutlineSizeType");
	OutlineColorType = GetRuntimeType(ASSEMBLY_NAME, "Gallop", "OutlineColorType");
}

static void HookMethods()
{
	if (config::replace_to_builtin_font || config::replace_to_custom_font)
	{
		ADD_HOOK(TextCommon_Awake, "Gallop.TextCommon::Awake at %p\n");
		ADD_HOOK(TextCommon_ApplyFont, "Gallop.TextCommon::ApplyFont at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	int TextCommon::TextId()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(TextCommon_get_TextId_addr)(instance);
	}

	void TextCommon::FontColor(int value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_FontColor_addr)(instance, value);
	}

	void TextCommon::FontColor(const Il2CppChar* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_FontColor_addr)(instance, GetEnumValue(ParseEnum(FontColorType, value)));
	}

	void TextCommon::OutlineColor(int value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_OutlineColor_addr)(instance, value);
	}

	void TextCommon::OutlineColor(const Il2CppChar* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_OutlineColor_addr)(instance, GetEnumValue(ParseEnum(OutlineColorType, value)));
	}

	void TextCommon::OutlineSize(int value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_OutlineSize_addr)(instance, value);
	}

	void TextCommon::OutlineSize(const Il2CppChar* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int)>(TextCommon_set_OutlineSize_addr)(instance, GetEnumValue(ParseEnum(OutlineSizeType, value)));
	}

	void TextCommon::Awake()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(TextCommon_Awake_addr)(instance);
	}

	void TextCommon::UpdateOutline()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(TextCommon_UpdateOutline_addr)(instance);
	}

	void TextCommon::SetTextWithCustomTag(Il2CppString* value, float iconScale, int addSpaceNum)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, float, int)>(TextCommon_SetTextWithCustomTag_addr)(instance, value, iconScale, addSpaceNum);
	}
}
