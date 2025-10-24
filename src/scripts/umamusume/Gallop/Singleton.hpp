#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"


namespace Gallop
{
	template<class T>
	class Singleton abstract : public UnityEngine::Object
	{
	public:
		static T Instance()
		{
			auto instanceField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class("umamusume.dll", T::namespaze, T::klassName), "_instance");
			Il2CppObject* instance;
			il2cpp_field_static_get_value(instanceField, &instance);
			return T(instance);
		}

		using Object::Object;
	};
}
