#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	class GallopUtil : public UnityEngine::Object
	{
	public:
		static Il2CppString* LineHeadWrap(Il2CppString* str, int lineCharCount);

		static Il2CppString* StripColorTag(Il2CppString* srcText);

		static Il2CppString* ConvertSymbolCode(Il2CppString* srcText);

		static bool IsWaveDash(Il2CppString* src);

		static Il2CppString* ConvertInputText(Il2CppString* srcText, bool allowNewLine, bool allowHalfWidth);

		using Object::Object;
	};
}
