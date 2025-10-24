#pragma once

namespace Gallop
{
	class DialogCommonBase
	{
	public:
		enum FormType
		{
			SMALL_NO_BUTTON,
			SMALL_ONE_BUTTON,
			SMALL_TWO_BUTTON,
			SMALL_THREE_BUTTON,
			MIDDLE_NO_BUTTON,
			MIDDLE_ONE_BUTTON,
			MIDDLE_TWO_BUTTON,
			MIDDLE_THREE_BUTTON,
			BIG_NO_BUTTON,
			BIG_ONE_BUTTON,
			BIG_TWO_BUTTON,
			BIG_THREE_BUTTON,
			WITHOUT_FRAME
		};

		void Close();
	};
}
