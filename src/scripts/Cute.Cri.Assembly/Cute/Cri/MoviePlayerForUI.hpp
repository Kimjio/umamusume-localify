#pragma once
#include "MoviePlayerBase.hpp"

namespace Cute::Cri
{
	class MoviePlayerForUI : public MoviePlayerBase
	{
	public:
		void AdjustScreenSize(UnityEngine::Vector2 dispRectWH, bool isPanScan);

		using MoviePlayerBase::MoviePlayerBase;
	};
}
