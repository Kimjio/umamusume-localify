#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Time.hpp"

namespace
{
	Il2CppMethodPointer Time_get_deltaTime_addr = nullptr;
}

static void InitAddress()
{
	Time_get_deltaTime_addr = il2cpp_resolve_icall("UnityEngine.Time::get_deltaTime");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	float Time::deltaTime()
	{
		return reinterpret_cast<decltype(deltaTime)*>(Time_get_deltaTime_addr)();
	}
}
