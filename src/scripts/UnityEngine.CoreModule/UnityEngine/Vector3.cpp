#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Vector3.hpp"

namespace
{
	void* Vector3_Slerp_Injected_addr = nullptr;
}

static void InitAddress()
{
	Vector3_Slerp_Injected_addr = il2cpp_resolve_icall("UnityEngine.Vector3::Slerp_Injected()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t)
	{
		Vector3 vector3{};
		reinterpret_cast<void (*)(Vector3, Vector3, float, Vector3*)>(Vector3_Slerp_Injected_addr)(a, b, t, &vector3);
		return vector3;
	}
}
