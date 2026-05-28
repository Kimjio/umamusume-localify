#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeSkirtWorking
	{
		Vector3 SkirtRootPos;
		int : 32;
		Vector3 SkirtInitChildPos;
		int : 32;
		Vector3 SkirtInitNormal;
		int : 32;
		Vector3 RotationAxis;
		int : 32;
		ALIGN_TYPE(4) bool IsCheckRightKnee;
		ALIGN_TYPE(4) bool IsCheckLeftKnee;
		ALIGN_TYPE(4) bool IsCheckRightAnkle;
		ALIGN_TYPE(4) bool IsCheckLeftAnkle;
		float Evaluation;
		float OffsetAngle;
	};
}
