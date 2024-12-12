#pragma once

namespace UnityEngine
{
	namespace XR
	{
		enum AvailableTrackingData
		{
			None = 0,
			PositionAvailable = 1,
			RotationAvailable = 2,
			VelocityAvailable = 4,
			AngularVelocityAvailable = 8,
			AccelerationAvailable = 16,
			AngularAccelerationAvailable = 32
		};
	}
}
