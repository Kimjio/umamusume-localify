#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class Shader : public Object
	{
	public:
		static int PropertyToID(Il2CppString* name);

		static Il2CppObject* GetGlobalTextureImpl(int name);

		static void SetGlobalTextureImpl(int name, Il2CppObject* value);

		using Object::Object;
	};
}