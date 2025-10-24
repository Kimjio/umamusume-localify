#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Quaternion.hpp"

namespace
{
	void* Quaternion_FromToRotation_Injected_addr = nullptr;

	void* Quaternion_Inverse_Injected_addr = nullptr;

	void* Quaternion_Slerp_Injected_addr = nullptr;

	void* Quaternion_Lerp_Injected_addr = nullptr;

	void* Quaternion_Internal_FromEulerRad_Injected_addr = nullptr;
}

static void InitAddress()
{
	Quaternion_FromToRotation_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::FromToRotation_Injected()");
	Quaternion_Inverse_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Inverse_Injected()");
	Quaternion_Slerp_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Slerp_Injected()");
	Quaternion_Lerp_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Lerp_Injected()");
	Quaternion_Internal_FromEulerRad_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Internal_FromEulerRad_Injected()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Quaternion Quaternion::FromToRotation(Vector3 fromDirection, Vector3 toDirection)
	{
		Quaternion quaternion{};
		reinterpret_cast<void (*)(Vector3, Vector3, Quaternion*)>(Quaternion_FromToRotation_Injected_addr)(fromDirection, toDirection, &quaternion);
		return quaternion;
	}

	Quaternion Quaternion::Inverse(Quaternion rotation)
	{
		Quaternion quaternion{};
		reinterpret_cast<void (*)(Quaternion, Quaternion*)>(Quaternion_Inverse_Injected_addr)(rotation, &quaternion);
		return quaternion;
	}

	Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t)
	{
		Quaternion quaternion{};
		reinterpret_cast<void (*)(Quaternion, Quaternion, float, Quaternion*)>(Quaternion_Slerp_Injected_addr)(a, b, t, &quaternion);
		return quaternion;
	}

	Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t)
	{
		Quaternion quaternion{};
		reinterpret_cast<void (*)(Quaternion, Quaternion, float, Quaternion*)>(Quaternion_Lerp_Injected_addr)(a, b, t, &quaternion);
		return quaternion;
	}

	Quaternion Quaternion::Euler(float x, float y, float z)
	{
		Vector3 vector3{ x, y, z };
		Quaternion quaternion{};
		reinterpret_cast<void (*)(Vector3, Quaternion*)>(Quaternion_Internal_FromEulerRad_Injected_addr)(vector3 * 0.017453292f, &quaternion);
		return quaternion;
	}
}
