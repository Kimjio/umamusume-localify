#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Component.hpp"

namespace UnityEngine
{
	class Behaviour : public Component
	{
	public:
		bool enabled();

		void enabled(bool value);

		using Component::Component;
	};
}
