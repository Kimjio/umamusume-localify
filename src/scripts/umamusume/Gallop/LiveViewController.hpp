#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	class LiveViewController : UnityEngine::Object
	{
	public:
		enum LiveState
		{
			Play,
			Pause,
			ShowSkipConfirmDialog,
			ShowMenuDialog,
			ShowSettingDialog,
			HideUI
		};

		using Object::Object;
	};
}
