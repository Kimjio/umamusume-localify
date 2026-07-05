#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoFunctionSealedBaseForMonoSingleton.hpp"

namespace Gallop
{
	template<class T>
	class MonoSingleton : public MonoFunctionSealedBaseForMonoSingleton
	{
	public:
		static T Instance()
		{
			auto klass = il2cpp_symbols::get_class("umamusume.dll", T::namespaze, T::klassName);

			if (!klass)
			{
				return nullptr;
			}

			// auto instanceField = il2cpp_class_get_field_from_name(klass, "_instance");
			// Il2CppObject* instance;
			// il2cpp_field_static_get_value(instanceField, &instance);
			auto get_Instance = il2cpp_symbols::get_method(klass, "get_Instance", 0);
			auto instance = reinterpret_cast<Il2CppObject * (*)(const MethodInfo*)>(get_Instance->methodPointer)(get_Instance);
			return T(instance);
		}

		using MonoFunctionSealedBaseForMonoSingleton::MonoFunctionSealedBaseForMonoSingleton;
	};
}
