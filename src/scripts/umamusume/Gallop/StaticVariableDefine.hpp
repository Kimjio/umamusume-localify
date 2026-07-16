#pragma once

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace Gallop
{
	class StaticVariableDefine
	{
	public:
		class Circle
		{
		public:
			class CircleChat
			{
			public:
				static UnityEngine::Vector3 ITEM_BADGE_POS;
				static UnityEngine::Vector3 STAMP_ANCHORED_POS_AFTER_BASE;
				static UnityEngine::Vector3 INPUT_LOCAL_POS_AFTER_BASE;
				static UnityEngine::Vector3 INPUT_ANCHORED_POS_AFTER_BASE;
				static UnityEngine::Vector3 INPUT_SIZE_DELTA_POS_AFTER_BASE;
			};
		};

		class Parts
		{
		public:
			class FooterBackButton
			{
			public:
				static UnityEngine::Vector2 FOOTER_ON_POS;
				static UnityEngine::Vector2 FOOTER_OFF_POS;
			};
		};
	};
}
