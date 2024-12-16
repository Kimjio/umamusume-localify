#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class Shader : public Object
	{
	public:
		static int PropertyToID(Il2CppString* name);

		using Object::Object;
	};
}