#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	namespace Live
	{
		class Master3dLive : public UnityEngine::Object
		{
		public:
			Master3dLive(int musicId);

			class Live3dData : public UnityEngine::Object
			{
			public:
				int ScreenMode();
				void ScreenMode(int value);

				using Object::Object;
			};

			Live3dData LiveData();

			using Object::Object;
		};
	}
}
