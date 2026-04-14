#include "../../UnityEngine.UI.hpp"
#include "scripts/ScriptInternal.hpp"
#include "Text.hpp"

namespace
{
	void* Text_get_font_addr = nullptr;

	void* Text_set_font_addr = nullptr;
	
	void* Text_get_text_addr = nullptr;
	
	void* Text_set_text_addr = nullptr;

	void* Text_get_fontSize_addr = nullptr;

	void* Text_set_fontSize_addr = nullptr;

	void* Text_get_horizontalOverflow_addr = nullptr;
	
	void* Text_set_horizontalOverflow_addr = nullptr;

	void* Text_get_verticalOverflow_addr = nullptr;

	void* Text_set_verticalOverflow_addr = nullptr;

	void* Text_get_lineSpacing_addr = nullptr;

	void* Text_set_lineSpacing_addr = nullptr;

	void* Text_get_fontStyle_addr = nullptr;

	void* Text_set_fontStyle_addr = nullptr;

	void* Text_AssignDefaultFont_addr = nullptr;
}

static void InitAddress()
{
	auto Text_klass = il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.UI", "Text");
	Text_get_font_addr = il2cpp_class_get_method_from_name(Text_klass, "get_font", 0)->methodPointer;
	Text_set_font_addr = il2cpp_class_get_method_from_name(Text_klass, "set_font", 1)->methodPointer;
	Text_get_text_addr = il2cpp_class_get_method_from_name(Text_klass, "get_text", 0)->methodPointer;
	Text_set_text_addr = il2cpp_class_get_method_from_name(Text_klass, "set_text", 1)->methodPointer;
	Text_get_fontSize_addr = il2cpp_class_get_method_from_name(Text_klass, "get_fontSize", 0)->methodPointer;
	Text_set_fontSize_addr = il2cpp_class_get_method_from_name(Text_klass, "set_fontSize", 1)->methodPointer;
	Text_get_horizontalOverflow_addr = il2cpp_class_get_method_from_name(Text_klass, "get_horizontalOverflow", 0)->methodPointer;
	Text_set_horizontalOverflow_addr = il2cpp_class_get_method_from_name(Text_klass, "set_horizontalOverflow", 1)->methodPointer;
	Text_get_verticalOverflow_addr = il2cpp_class_get_method_from_name(Text_klass, "get_verticalOverflow", 0)->methodPointer;
	Text_set_verticalOverflow_addr = il2cpp_class_get_method_from_name(Text_klass, "set_verticalOverflow", 1)->methodPointer;
	Text_get_lineSpacing_addr = il2cpp_class_get_method_from_name(Text_klass, "get_lineSpacing", 0)->methodPointer;
	Text_set_lineSpacing_addr = il2cpp_class_get_method_from_name(Text_klass, "set_lineSpacing", 1)->methodPointer;
	Text_get_fontStyle_addr = il2cpp_class_get_method_from_name(Text_klass, "get_fontStyle", 0)->methodPointer;
	Text_set_fontStyle_addr = il2cpp_class_get_method_from_name(Text_klass, "set_fontStyle", 1)->methodPointer;
	Text_AssignDefaultFont_addr = il2cpp_class_get_method_from_name(Text_klass, "AssignDefaultFont", 0)->methodPointer;
}

static void HookMethods()
{
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	namespace UI
	{
		Il2CppObject* Text::font()
		{
			return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(Text_get_font_addr)(instance);
		}

		void Text::font(Il2CppObject* value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(Text_set_font_addr)(instance, value);
		}

		Il2CppString* Text::text()
		{
			return reinterpret_cast<Il2CppString * (*)(Il2CppObject*)>(Text_get_text_addr)(instance);
		}

		void Text::text(Il2CppString* value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(Text_set_text_addr)(instance, value);
		}

		int Text::fontSize()
		{
			return reinterpret_cast<int (*)(Il2CppObject*)>(Text_get_fontSize_addr)(instance);
		}

		void Text::fontSize(int value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, int)>(Text_set_fontSize_addr)(instance, value);
		}

		HorizontalWrapMode Text::horizontalOverflow()
		{
			return reinterpret_cast<HorizontalWrapMode (*)(Il2CppObject*)>(Text_get_horizontalOverflow_addr)(instance);
		}

		void Text::horizontalOverflow(HorizontalWrapMode value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, HorizontalWrapMode)>(Text_set_horizontalOverflow_addr)(instance, value);
		}

		VerticalWrapMode Text::verticalOverflow()
		{
			return reinterpret_cast<VerticalWrapMode (*)(Il2CppObject*)>(Text_get_verticalOverflow_addr)(instance);
		}

		void Text::verticalOverflow(VerticalWrapMode value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, VerticalWrapMode)>(Text_set_verticalOverflow_addr)(instance, value);
		}

		float Text::lineSpacing()
		{
			return reinterpret_cast<float (*)(Il2CppObject*)>(Text_get_lineSpacing_addr)(instance);
		}

		void Text::lineSpacing(float value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, float)>(Text_set_lineSpacing_addr)(instance, value);
		}

		FontStyle Text::fontStyle()
		{
			return reinterpret_cast<FontStyle (*)(Il2CppObject*)>(Text_get_fontStyle_addr)(instance);
		}

		void Text::fontStyle(FontStyle value)
		{
			reinterpret_cast<void (*)(Il2CppObject*, FontStyle)>(Text_set_fontStyle_addr)(instance, value);
		}

		void Text::AssignDefaultFont()
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(Text_AssignDefaultFont_addr)(instance);
		}
	}
}
