#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

using namespace std;
using namespace UnityEngine;

namespace Gallop
{
	class TextFontManager : public MonoSingleton<TextFontManager>
	{
	public:
		static Il2CppObject* GetReplacementFontAssetBundle(u16string path);

		static Il2CppObject* GetReplacementFontAsset(Il2CppString* path);

		static Il2CppObject* DefaultFont();
	};
}
