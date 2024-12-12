#pragma once

#include "il2cpp/il2cpp_symbols.hpp"

#include "XRNode.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"

namespace UnityEngine
{
	namespace XR
	{
		class InputTracking
		{
		public:
			static void GetNodeStates_Internal(Il2CppObject* nodeStates);

			static void GetLocalPosition_Injected(XRNode node, Vector3* ret);

			static void GetLocalRotation_Injected(XRNode node, Quaternion* ret);
		};
	}
}
