#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GallopUtil.hpp"

namespace
{
	void* GallopUtil_LineHeadWrap_addr = nullptr;
	void* GallopUtil_LineHeadWrap_orig = nullptr;

	void* GallopUtil_StripColorTag_addr = nullptr;

	void* GallopUtil_ConvertSymbolCode_addr = nullptr;

	void* GallopUtil_IsWaveDash_addr = nullptr;

	void* GallopUtil_ConvertInputText_addr = nullptr;
	void* GallopUtil_ConvertInputText_orig = nullptr;

	void* GallopUtil_ConvertInputText2_addr = nullptr;
	void* GallopUtil_ConvertInputText2_orig = nullptr;
}

static Il2CppString* GallopUtil_LineHeadWrap_hook(Il2CppString* str, int lineCharCount)
{
	return reinterpret_cast<decltype(GallopUtil_LineHeadWrap_hook)*>(GallopUtil_LineHeadWrap_orig)(str, lineCharCount + 6);
}

static Il2CppString* GallopUtil_ConvertInputText2_hook(Il2CppString* srcText, bool allowNewLine)
{
	return Gallop::GallopUtil::ConvertInputText(srcText, allowNewLine, false);
}

static void InitAddress()
{
	auto GallopUtil_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "GallopUtil");
	GallopUtil_LineHeadWrap_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "LineHeadWrap", 2);
	GallopUtil_StripColorTag_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "StripColorTag", 1);
	GallopUtil_ConvertSymbolCode_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "ConvertSymbolCode", 1);
	GallopUtil_IsWaveDash_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "IsWaveDash", 1);
	GallopUtil_ConvertInputText_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "ConvertInputText", 3);
	GallopUtil_ConvertInputText2_addr = il2cpp_symbols::get_method_pointer(GallopUtil_klass, "ConvertInputText", 2);
}

static void HookMethods()
{
	ADD_HOOK(GallopUtil_LineHeadWrap, "Gallop.GallopUtil::LineHeadWrap at %p\n");
	auto GallopUtil_ConvertInputText_hook = Gallop::GallopUtil::ConvertInputText;
	ADD_HOOK(GallopUtil_ConvertInputText, "Gallop.GallopUtil::ConvertInputText at %p\n");
	ADD_HOOK(GallopUtil_ConvertInputText2, "Gallop.GallopUtil::ConvertInputText2 at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	Il2CppString* GallopUtil::LineHeadWrap(Il2CppString* str, int lineCharCount)
	{
		if (GallopUtil_LineHeadWrap_orig)
		{
			return reinterpret_cast<Il2CppString * (*)(Il2CppString*, int)>(GallopUtil_LineHeadWrap_orig)(str, lineCharCount);
		}

		return reinterpret_cast<Il2CppString * (*)(Il2CppString*, int)>(GallopUtil_LineHeadWrap_addr)(str, lineCharCount);
	}

	Il2CppString* GallopUtil::StripColorTag(Il2CppString* srcText)
	{
		return reinterpret_cast<Il2CppString * (*)(Il2CppString*)>(GallopUtil_StripColorTag_addr)(srcText);
	}

	Il2CppString* GallopUtil::ConvertSymbolCode(Il2CppString* srcText)
	{
		return reinterpret_cast<Il2CppString * (*)(Il2CppString*)>(GallopUtil_ConvertSymbolCode_addr)(srcText);
	}

	bool GallopUtil::IsWaveDash(Il2CppString* src)
	{
		return reinterpret_cast<bool (*)(Il2CppString*)>(GallopUtil_IsWaveDash_addr)(src);
	}

	Il2CppString* GallopUtil::ConvertInputText(Il2CppString* srcText, bool allowNewLine, bool allowHalfWidth)
	{
		srcText = Gallop::GallopUtil::StripColorTag(srcText);
		auto stringInfo = il2cpp_object_new(il2cpp_symbols::get_class("mscorlib.dll", "System.Globalization", "StringInfo"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(stringInfo->klass, ".ctor", 1)->methodPointer(stringInfo, srcText);

		il2cppstring text = IL2CPP_STRING("");

		int lengthInTextElements = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(stringInfo->klass, "get_LengthInTextElements", 0)->methodPointer(stringInfo);
		for (int i = 0; i < lengthInTextElements; i++)
		{
			auto text2 = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*, int, int)>(stringInfo->klass, "SubstringByTextElements", 2)->methodPointer(stringInfo, i, 1);
			if (allowNewLine && (text2->chars == IL2CPP_STRING("\r"s)) || text2->chars == IL2CPP_STRING("\n"s))
			{
				text += text2->chars;
			}
			else if (Gallop::GallopUtil::IsWaveDash(text2))
			{
				text += IL2CPP_STRING("～");
			}
			else
			{
				if (!allowHalfWidth)
				{
					text2 = Gallop::GallopUtil::ConvertSymbolCode(text2);
				}
				text += text2->chars;
			}
		}

		return il2cpp_string_new16(text.data());
	}
}
