#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

namespace UnityEngine
{
	class Debug
	{
	public:
		static Il2CppObject* unityLogger();

		static void Log(Il2CppObject* value);

		static void LogError(Il2CppObject* value);

		static void LogWarning(Il2CppObject* value);
	};
}
