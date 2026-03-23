#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "LockableBehaviour.hpp"

namespace Gallop
{
	class DialogCommonBase abstract : public LockableBehaviour
	{
	public:
		enum class FormType
		{
			NONE = -1,
			SMALL_NO_BUTTON = 0,
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

		virtual void Close() abstract;

		using LockableBehaviour::LockableBehaviour;
	};
}
