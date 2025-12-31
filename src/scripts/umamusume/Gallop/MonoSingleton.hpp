#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoFunctionSealedBaseForMonoSingleton.hpp"

namespace Gallop
{
	template<class T>
	class MonoSingleton abstract : public MonoFunctionSealedBaseForMonoSingleton
	{
	public:
		static T Instance()
		{
			auto instanceField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class("umamusume.dll", T::namespaze, T::klassName), "_instance");
			Il2CppObject* instance;
			il2cpp_field_static_get_value(instanceField, &instance);
			return T(instance);
		}

		using MonoFunctionSealedBaseForMonoSingleton::MonoFunctionSealedBaseForMonoSingleton;
	};
}
