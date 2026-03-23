#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "DialogCommon.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop
{
	class DialogInnerBase abstract : public UnityEngine::MonoBehaviour
	{
	public:
		using MonoBehaviour::MonoBehaviour;

	protected:
		DialogCommon::Data CreateDialogData();
	};
}
