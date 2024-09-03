#pragma once

#include "il2cpp/il2cpp-object-internals.h"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Color.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"

namespace UnityEngine
{
	struct TextGenerationSettings
	{
	public:
		Il2CppObject* font;
		Color color;
		int fontSize;
		float lineSpacing;
		bool richText;
		float scaleFactor;
		// UnityEngine.FontStyle UnityEngine.TextGenerationSettings::fontStyle
		int fontStyle;
		// UnityEngine.TextAnchor UnityEngine.TextGenerationSettings::textAnchor
		int textAnchor;
		bool alignByGeometry;
		bool resizeTextForBestFit;
		int resizeTextMinSize;
		int resizeTextMaxSize;
		bool updateBounds;
		// UnityEngine.VerticalWrapMode UnityEngine.TextGenerationSettings::verticalOverflow
		int verticalOverflow;
		// UnityEngine.HorizontalWrapMode UnityEngine.TextGenerationSettings::horizontalOverflow
		int horizontalOverflow;
		Vector2 generationExtents;
		Vector2 pivot;
		bool generateOutOfBounds;
	};
}
