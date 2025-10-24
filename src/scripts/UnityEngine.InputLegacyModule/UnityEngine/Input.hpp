#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "../../UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace UnityEngine
{
	class Input : public Object
	{
	public:
		static Vector3 mousePosition();

		static bool GetMouseButtonDown(int button);

		static bool GetMouseButton(int button);

		static bool GetMouseButtonUp(int button);

		using Object::Object;
	};
}
