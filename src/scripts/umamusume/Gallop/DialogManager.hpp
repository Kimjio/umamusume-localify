#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "Singleton.hpp"
#include "DialogCommon.hpp"

using namespace UnityEngine;

namespace Gallop
{
	class DialogManager : public Singleton<DialogManager>
	{
	public:
		static DialogCommon PushDialog(DialogCommon::Data data);

		static DialogCommon PushSystemDialog(DialogCommon::Data data, bool isEnableOutsideClick);

		using Singleton::Singleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
