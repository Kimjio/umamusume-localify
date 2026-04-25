#pragma once
#include "Object.hpp"

namespace UnityEngine
{
	class Sprite : public Object
	{
	public:
		Il2CppObject* texture();

		using Object::Object;
	};
}
