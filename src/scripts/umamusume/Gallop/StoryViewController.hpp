#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace Gallop
{
	class StoryViewController : public UnityEngine::Object
	{
	public:
		bool IsSingleModeOrGallery();
		void SetDisplayMode(int displayMode);
		void SetupUIOnChangeOrientation();

		using Object::Object;
	};
}
