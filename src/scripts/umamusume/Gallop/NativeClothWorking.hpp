#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeClothWorking
	{
		Quaternion InitLocalRotation;
		Quaternion ParentRotation;
		Quaternion FinalRotation;

		Vector3 BoneAxis;
		int : 32;
		Vector3 TargetPosition;
		int : 32;
		Vector3 PrevTargetPosition;
		int : 32;

		Vector3 Force;
		int : 32;
		Vector3 AimVector;
		int : 32;
		Vector3 Diff;
		int : 32;
		Vector3 SelfPosition;
		int : 32;
		Vector3 LimitRotationMin;
		int : 32;
		Vector3 LimitRotationMax;
		int : 32;

		float InitBoneDistance;
		float StiffnessForce;
		float DragForce;
		float CollisionRadius;
		float Gravity;
		float VerticalWindRateSlow;
		float VerticalWindRateFast;
		float HorizontalWindRateSlow;
		float HorizontalWindRateFast;

		ALIGN_TYPE(4) bool CheckCharaCollision;
		ALIGN_TYPE(4) bool IsSkip;
		ALIGN_TYPE(4) bool IsLimit;

		int ActiveCollision;
		short CIndex0;
		short CIndex1;
		short CIndex2;
		short CIndex3;
		short CIndex4;
		short CIndex5;
		short CIndex6;
		short CIndex7;

		float DynamicRatio;
		Quaternion AnimationRotation;
		Vector3 SkirtKneeNormal;
		int : 32;
		Vector3 SkirtNormalPos;
		int : 32;

		ALIGN_TYPE(4) bool IsCheckSkirtKnee;
		float MoveSpringApplyRate;
		int ParentWorkIndex;

		Vector3 ConnectedForce;
		int : 32;

		short CIndex8;
		short CIndex9;
		short CIndex10;
		short CIndex11;
		short CIndex12;
		short CIndex13;
		short CIndex14;
		short CIndex15;
		int IsAddSpring;
	};
}
