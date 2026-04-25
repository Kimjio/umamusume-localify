#pragma once
#include "scripts/UnityEngine.UI/UnityEngine/UI/MaskableGraphic.hpp"

namespace Gallop
{
	class TextMeshProUguiCommon : public UnityEngine::UI::MaskableGraphic
	{
	public:
		using UnityEngine::UI::MaskableGraphic::MaskableGraphic;
	};
}