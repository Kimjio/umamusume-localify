#pragma once
#include "scripts/UnityEngine.UI/UnityEngine/UI/Text.hpp"

namespace Gallop
{
	class TextCommon : public UnityEngine::UI::Text
	{
	public:
		int TextId();

		void FontColor(int value);

		void FontColor(const Il2CppChar* value);

		void OutlineColor(int value);

		void OutlineColor(const Il2CppChar* value);

		void OutlineSize(int value);

		void OutlineSize(const Il2CppChar* value);

		void Awake();

		void UpdateOutline();

		void SetTextWithCustomTag(Il2CppString* value, float iconScale = 1, int addSpaceNum = 0);

		using UnityEngine::UI::Text::Text;
	};
}