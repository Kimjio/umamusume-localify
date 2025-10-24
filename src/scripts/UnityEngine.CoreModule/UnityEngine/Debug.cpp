#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Debug.hpp"

namespace
{
	void* get_unityLogger = nullptr;

	void* Log_addr = nullptr;

	void* LogError_addr = nullptr;

	void* LogWarning_addr = nullptr;
}

static void InitAddress()
{
	get_unityLogger = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Debug", "get_unityLogger", 0);
	Log_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Debug", "Log", 1);
	LogError_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Debug", "LogError", 1);
	LogWarning_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Debug", "LogWarning", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Il2CppObject* Debug::unityLogger()
	{
		return reinterpret_cast<Il2CppObject * (*)()>(get_unityLogger)();
	}

	void Debug::Log(Il2CppObject* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(Log_addr)(value);
	}

	void Debug::LogError(Il2CppObject* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(LogError_addr)(value);
	}

	void Debug::LogWarning(Il2CppObject* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(LogWarning_addr)(value);
	}
}
