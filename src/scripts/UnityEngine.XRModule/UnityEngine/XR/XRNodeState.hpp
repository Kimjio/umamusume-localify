#pragma once

#include <cstdint>

#include "../../../UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"

#include "XRNode.hpp"
#include "AvailableTrackingData.hpp"

namespace UnityEngine
{
	namespace XR
	{
		struct XRNodeState
		{
		public:
			XRNode m_Type;
			AvailableTrackingData m_AvailableFields;
			Vector3 m_Position;
			Quaternion m_Rotation;
			Vector3 m_Velocity;
			Vector3 m_AngularVelocity;
			Vector3 m_Acceleration;
			Vector3 m_AngularAcceleration;
			int m_Tracked;
			uint32_t m_UniqueID;
		};
	}
}
