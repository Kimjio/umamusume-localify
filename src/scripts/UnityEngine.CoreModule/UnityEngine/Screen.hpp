#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "EnabledOrientation.hpp"
#include "ScreenOrientation.hpp"
#include "Resolution.hpp"
#include "Rect.hpp"
#include "FullScreenMode.hpp"

namespace UnityEngine
{
	struct RefreshRate
	{
		uint32_t numerator;
		uint32_t denominator;

        uint32_t value() const
        {
            if (denominator <= 0)
			{
				return numerator;
			}

            return numerator / denominator;
        }
	};

	class Screen
	{
	public:
		static int width();

		static int height();

		static bool fullScreen();

		static Resolution currentResolution();

		static Rect safeArea();

        static void autorotateToPortrait(bool value);

		static void autorotateToPortraitUpsideDown(bool value);

		static void autorotateToLandscapeLeft(bool value);

		static void autorotateToLandscapeRight(bool value);

		static void SetOrientationEnabled(EnabledOrientation orient, bool enabled);

		static void RequestOrientation(ScreenOrientation orientation);

		static void SetResolution_Injected(int width, int height, UnityEngine::FullScreenMode fullscreenMode, RefreshRate* perferredRefreshRate);
	};
}
