#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeSkirtArg
	{
		Vector3 KneeLPos;
		int : 32;
		Vector3 KneeRPos;
		int : 32;
		Vector3 AnkleLPos;
		int : 32;
		Vector3 AnkleRPos;
		int : 32;
		Vector3 CenterPos;
		int : 32;
		Vector3 RootPos;
		int : 32;
		float KneeColliderRadius;
		float AnkleColliderRadius;
		float InfluenceAngle;
		float InfluenceMaxAngle;
	};
}
