#pragma once

namespace Gallop
{
	class LiveViewController
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
	};
}
