#include "../../UnityEngine.XRModule.hpp"
#include "../../../ScriptInternal.hpp"
#include "InputTracking.hpp"

void* GetNodeStates_Internal_addr = nullptr;
void* GetLocalPosition_Injected_addr = nullptr;
void* GetLocalRotation_Injected_addr = nullptr;

static void InitAddress()
{
	GetNodeStates_Internal_addr = il2cpp_resolve_icall("UnityEngine.XR.InputTracking::GetNodeStates_Internal()");
	GetLocalPosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.XR.InputTracking::GetLocalPosition_Injected()");
	GetLocalRotation_Injected_addr = il2cpp_resolve_icall("UnityEngine.XR.InputTracking::GetLocalRotation_Injected()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	namespace XR
	{
		void InputTracking::GetNodeStates_Internal(Il2CppObject* nodeStates)
		{
			reinterpret_cast<decltype(GetNodeStates_Internal)*>(GetNodeStates_Internal_addr)(nodeStates);
		}

		void InputTracking::GetLocalPosition_Injected(XRNode node, Vector3* ret)
		{
			reinterpret_cast<decltype(GetLocalPosition_Injected)*>(GetLocalPosition_Injected_addr)(node, ret);
		}

		void InputTracking::GetLocalRotation_Injected(XRNode node, Quaternion* ret)
		{
			reinterpret_cast<decltype(GetLocalRotation_Injected)*>(GetLocalRotation_Injected_addr)(node, ret);
		}
	}
}
