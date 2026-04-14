#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop
{
	class CySpringController : public UnityEngine::MonoBehaviour
	{
	public:
		enum class SpringUpdateMode
		{
			ModeNormal,
			Mode60FPS,
			SkipFrame,
			SkipFramePostAlways
		};

		using UnityEngine::MonoBehaviour::MonoBehaviour;
	};
}
