#pragma once
#include "MaskableGraphic.hpp"
#include "scripts/UnityEngine.TextRenderingModule/UnityEngine/HorizontalWrapMode.hpp"
#include "scripts/UnityEngine.TextRenderingModule/UnityEngine/VerticalWrapMode.hpp"
#include "scripts/UnityEngine.TextRenderingModule/UnityEngine/FontStyle.hpp"

namespace UnityEngine
{
	namespace UI
	{
		class Text : public MaskableGraphic
		{
		public:
			Il2CppObject* font();
			void font(Il2CppObject* value);

			virtual Il2CppString* text();
			virtual void text(Il2CppString* value);

			int fontSize();
			void fontSize(int value);

			HorizontalWrapMode horizontalOverflow();
			void horizontalOverflow(HorizontalWrapMode value);

			VerticalWrapMode verticalOverflow();
			void verticalOverflow(VerticalWrapMode value);

			float lineSpacing();
			void lineSpacing(float value);

			FontStyle fontStyle();
			void fontStyle(FontStyle value);

			void AssignDefaultFont();

			using MaskableGraphic::MaskableGraphic;
		};
	}
}