#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"

namespace UnityEngine
{
	struct Pose
	{
	public:
		Vector3 position;
		Quaternion rotation;
	};
}
