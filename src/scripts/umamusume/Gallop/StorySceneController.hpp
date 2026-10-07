#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	class StorySceneController : public UnityEngine::Object
	{
	public:
		void UpdateFlowBackground();

		using Object::Object;
	};
}
