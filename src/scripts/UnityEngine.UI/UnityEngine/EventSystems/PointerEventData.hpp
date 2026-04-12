#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace UnityEngine
{
	namespace EventSystems
	{
		class PointerEventData : public Object
		{
		public:
			enum class InputButton
			{
				Left,
				Right,
				Middle
			};

			using Object::Object;
		};
	}
}
