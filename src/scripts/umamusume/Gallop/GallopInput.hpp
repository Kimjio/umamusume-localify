#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace Gallop
{
	class GallopInput : public UnityEngine::Object
	{
	public:
		static UnityEngine::Vector3 mousePosition();

		using Object::Object;
	};
}
