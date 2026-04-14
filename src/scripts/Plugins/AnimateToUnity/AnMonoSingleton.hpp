#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace AnimateToUnity
{
	template<class T>
	class AnMonoSingleton abstract : public UnityEngine::MonoBehaviour
	{
	public:
		static T Instance()
		{
			auto klass = il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", T::klassName);

			if (!klass)
			{
				return nullptr;
			}

			auto instanceField = il2cpp_class_get_field_from_name(klass, "_instance");
			Il2CppObject* instance;
			il2cpp_field_static_get_value(instanceField, &instance);
			return T(instance);
		}

		using MonoBehaviour::MonoBehaviour;
	};
}
