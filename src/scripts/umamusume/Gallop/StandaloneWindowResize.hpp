#pragma once

#ifdef _MSC_VER
#include "il2cpp/il2cpp_symbols.hpp"

namespace Gallop
{
	class StandaloneWindowResize
	{
	public:
		static int windowLastWidth();

		static void windowLastWidth(int value);

		static int windowLastHeight();

		static void windowLastHeight(int value);

		static float AspectRatio();

		static void AspectRatio(float value);

		static bool IsPreventReShape();

		static void IsPreventReShape(bool value);

		static bool IsVirt();

		static void IsVirt(bool value);

		static bool IsEnableWindowHitTest();

		static void IsEnableWindowHitTest(bool value);

		static bool _isWindowSizeChanging();

		static void _isWindowSizeChanging(bool value);

		static bool _isWindowDragging();

		static void _isWindowDragging(bool value);

		static void SaveChangedWidth(float width, float height);

		static void EnableWindowHitTest();

		static void DisableWindowHitTest();

		static void DisableMaximizebox();

		static Il2CppObject* ClearStopFlagAfterWhile();
	};
}
#endif
