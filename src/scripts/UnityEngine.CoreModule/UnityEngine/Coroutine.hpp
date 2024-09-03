#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "YieldInstruction.hpp"

namespace UnityEngine
{
	class Coroutine : public YieldInstruction
	{
	public:
		Coroutine(Il2CppObject* object);

	private:
		Coroutine() {}

		void* m_Ptr;
	};
}
