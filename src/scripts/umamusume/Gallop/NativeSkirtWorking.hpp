#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeSkirtWorking
	{
		Vector3 SkirtRootPos;
		Vector3 SkirtInitChildPos;
		Vector3 SkirtInitNormal;
		Vector3 RotationAxis;
		bool IsCheckRightKnee;
		bool IsCheckLeftKnee;
		bool IsCheckRightAnkle;
		bool IsCheckLeftAnkle;
		float Evaluation;
		float OffsetAngle;
	};
}
