#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class ScriptableObject : public Object
	{
	public:
		static void CreateScriptableObject(ScriptableObject self);
		static ScriptableObject CreateScriptableObjectInstanceFromType(Il2CppObject* runtimeType, bool applyDefaultsAndReset);

		using Object::Object;
	};
}