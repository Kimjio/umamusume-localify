#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "ScreenOrientation.hpp"
#include "Resolution.hpp"

namespace UnityEngine
{
	struct RefreshRate
	{
		uint32_t numerator;
		uint32_t denominator;
	};

	class Screen
	{
	public:
		static int width();

		static int height();

		static bool fullScreen();

		static Resolution currentResolution();

		static void RequestOrientation(ScreenOrientation orientation);
	};
}
