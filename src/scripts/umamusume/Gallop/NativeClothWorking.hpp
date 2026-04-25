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
		Vector3 TargetPosition;
		Vector3 PrevTargetPosition;
		Vector3 Force;
		Vector3 AimVector;
		Vector3 Diff;
		Vector3 SelfPosition;
		Vector3 LimitRotationMin;
		Vector3 LimitRotationMax;
		float InitBoneDistance;
		float StiffnessForce;
		float DragForce;
		float CollisionRadius;
		float Gravity;
		float VerticalWindRateSlow;
		float VerticalWindRateFast;
		float HorizontalWindRateSlow;
		float HorizontalWindRateFast;
		bool CheckCharaCollision;
		bool IsSkip;
		bool IsLimit;
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
		Vector3 SkirtNormalPos;
		bool IsCheckSkirtKnee;
		float MoveSpringApplyRate;
		int ParentWorkIndex;
		Vector3 ConnectedForce;
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
