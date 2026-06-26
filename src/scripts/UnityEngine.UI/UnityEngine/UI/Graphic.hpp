#pragma once
#include "../EventSystems/UIBehaviour.hpp"

namespace UnityEngine::UI
{
	class Graphic : public EventSystems::UIBehaviour
	{
	public:
		using EventSystems::UIBehaviour::UIBehaviour;
	};
}
