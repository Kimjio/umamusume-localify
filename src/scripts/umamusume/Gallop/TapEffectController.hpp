#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

namespace Gallop
{
	class TapEffectController : public MonoSingleton<TapEffectController>
	{
	public:
		void Update();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
