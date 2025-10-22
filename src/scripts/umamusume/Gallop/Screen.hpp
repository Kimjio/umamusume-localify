#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/ScreenOrientation.hpp"

namespace Gallop
{
	class Screen
	{
	public:
		static int OriginalScreenWidth();

		static void OriginalScreenWidth(int value);

		static int OriginalScreenHeight();

		static void OriginalScreenHeight(int value);

		static int Width();

		static int Height();

		static bool IsLandscapeMode();

		static bool IsSplitWindow();

		static bool IsVertical();

		static Il2CppObject* ChangeScreenOrientation(UnityEngine::ScreenOrientation targetOrientation, bool isForce);

		static Il2CppObject* ChangeScreenOrientationLandscapeAsync();

		static Il2CppObject* ChangeScreenOrientationLandscapeAsyncDefault();

		static Il2CppObject* ChangeScreenOrientationLandscapeAsyncLandscapeMode();

		static Il2CppObject* ChangeScreenOrientationPortraitAsync();

		static Il2CppObject* ChangeScreenOrientationPortraitAsyncDefault();

		static Il2CppObject* ChangeScreenOrientationPortraitAsyncLandscapeMode();
	};
}
