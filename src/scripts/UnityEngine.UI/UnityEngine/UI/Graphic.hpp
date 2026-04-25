#pragma once
#include "../EventSystems/UIBehaviour.hpp"

namespace UnityEngine::UI
{
	class Graphic abstract : public EventSystems::UIBehaviour
	{
	public:
		using EventSystems::UIBehaviour::UIBehaviour;
	};
}
