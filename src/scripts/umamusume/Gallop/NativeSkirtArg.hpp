#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeSkirtArg
	{
		Vector3 KneeLPos;
		Vector3 KneeRPos;
		Vector3 AnkleLPos;
		Vector3 AnkleRPos;
		Vector3 CenterPos;
		Vector3 RootPos;
		float KneeColliderRadius;
		float AnkleColliderRadius;
		float InfluenceAngle;
		float InfluenceMaxAngle;
	};
}
