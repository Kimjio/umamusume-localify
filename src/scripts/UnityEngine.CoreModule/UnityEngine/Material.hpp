#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class Material : public Object
	{
	public:
		bool HasProperty(int nameID);
		Il2CppObject* GetTextureImpl(int name);
		void SetTextureImpl(int name, Il2CppObject* value);

		using Object::Object;
	};
}