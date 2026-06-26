#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop
{
	class BGManager : public UnityEngine::MonoBehaviour
	{
	public:
		void OnChangeResolutionByGraphicsSettings();

		using MonoBehaviour::MonoBehaviour;
	};
}