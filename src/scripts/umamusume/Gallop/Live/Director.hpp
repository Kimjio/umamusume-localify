#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop::Live
{
	class Director : public UnityEngine::MonoBehaviour
	{
	public:
		enum class DisplayMode
		{
			None,
			Landscape,
			Portrait
		};

		class Live3DSettings : public UnityEngine::Object
		{
		public:
			int ScreenMode();
			void ScreenMode(int value);

			using Object::Object;
		};

		using MonoBehaviour::MonoBehaviour;
	};
}
