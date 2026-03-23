#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	class LiveTheaterViewController : public UnityEngine::Object
	{
	public:
		enum class Menu
		{
			MusicSelect,
			CharaSelect,
			Max
		};

		using Object::Object;
	};
}
