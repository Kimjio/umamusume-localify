#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class GameObject;
	class Component : public Object
	{
	public:
		GameObject gameObject();

		using Object::Object;
	};
}

#include "GameObject.hpp"
