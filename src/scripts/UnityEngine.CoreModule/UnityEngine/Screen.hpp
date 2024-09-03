#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "ScreenOrientation.hpp"
#include "Resolution.hpp"

namespace UnityEngine
{
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
