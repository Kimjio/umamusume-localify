#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Behaviour.hpp"
#include "Coroutine.hpp"

namespace UnityEngine
{
	class MonoBehaviour : public Behaviour
	{
	public:
		Coroutine StartCoroutineManaged2(Il2CppObject* enumerator);

		void StopCoroutineManaged(Coroutine routine);

		using Behaviour::Behaviour;
	};
}
