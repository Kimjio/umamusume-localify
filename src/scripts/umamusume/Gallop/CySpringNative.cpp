#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CySpringNative.hpp"

#include "FrameRateController.hpp"

#include <vector>

#include "game.hpp"

namespace
{
	Il2CppClass* CySpringNative_klass = nullptr;

	Il2CppMethodPointer CySpringNative_cctor_addr = nullptr;
	void* CySpringNative_cctor_orig = nullptr;

	Il2CppMethodPointer CySpringNative_UpdateForce_addr = nullptr;
	void* CySpringNative_UpdateForce_orig = nullptr;

	void* CySpringNative_UpdateNativeCloth_addr = nullptr;
	void* CySpringNative_UpdateNativeCloth_orig = nullptr;

	void* CySpringNative_NativeClothUpdate_addr = nullptr;
	void* CySpringNative_NativeClothUpdate_orig = nullptr;

	void* CySpringNative_NativeClothSkirtUpdate_addr = nullptr;
	void* CySpringNative_NativeClothSkirtUpdate_orig = nullptr;

	void* CySpringNative_NativeSkirtUpdate_addr = nullptr;
	void* CySpringNative_NativeSkirtUpdate_orig = nullptr;

	FieldInfo* CySpringNative_isNativeField = nullptr;
}

static void CySpringNative_cctor_hook()
{
	reinterpret_cast<decltype(CySpringNative_cctor_hook)*>(CySpringNative_cctor_orig)();
	bool isNative = !config::cyspring_disable_native;
	il2cpp_field_static_set_value(CySpringNative_isNativeField, &isNative);
}

static void CySpringNative_UpdateForce_hook(Gallop::NativeClothWorking* clothWorking, float stiffnessForceRate, float dragForceRate, float gravityRate, Vector3 windPower, float windStrength, Vector3 positionDiff, float frameScale)
{
	if (config::cyspring_update_mode == 1)
	{
		frameScale = config::cyspring_mono_uncap_frame_scale ? min(60.f, Gallop::FrameRateController::Instance().GetLayerFrameRate(Gallop::FrameRateController::FrameRateOverrideLayer::SystemValue) / 2.f) : 60.f;
	}
	reinterpret_cast<decltype(CySpringNative_UpdateForce_hook)*>(CySpringNative_UpdateForce_orig)(clothWorking, stiffnessForceRate, dragForceRate, gravityRate, windPower, windStrength, positionDiff, frameScale);
}

static void _UpdateSkirtNativePluginCalcRotAngle(Gallop::NativeSkirtWorking& working, Vector3 legPos, float radius, float influenceAngle, float influenceMaxAngle, Vector3 centerPos, Vector3 rootPos)
{
	Vector3 vector = working.SkirtRootPos - rootPos;
	Vector3 vector2 = working.SkirtInitChildPos - rootPos;
	Vector3 vector3 = legPos - vector2;
	float num = Vector3::Dot(working.SkirtInitNormal, vector3);
	float num2 = radius + num;
	Vector3 vector4 = legPos - vector;
	float num3 = sqrtf(vector4.sqrMagnitude() - radius * radius);
	float num4 = num3 / vector4.magnitude();
	float num5 = radius / vector4.magnitude();
	float num6 = num3 * num4;
	float num7 = num3 * num5;
	Vector3 normalized = Vector3::Cross(working.RotationAxis, vector4).normalized();
	Vector3 normalized2 = (vector2 - vector).normalized();
	Vector3 vector5 = vector4.normalized() * num6 + normalized * num7;
	Vector3 vector6 = vector + normalized2 * Vector3::Dot(normalized2, vector5) + working.SkirtInitNormal * num2 - vector;
	float num8 = Vector3::Angle(normalized2, vector6);
	if (num2 < 0.f)
	{
		num8 *= -1.f;
		if (working.Evaluation < num8)
		{
			working.Evaluation = num8;
		}
		return;
	}
	Quaternion quaternion = Quaternion::AngleAxis(num8, working.RotationAxis);
	Vector3 vector7 = vector + quaternion * (vector2 - vector);
	Vector3 normalized3 = (legPos - centerPos).normalized();
	float num9 = acosf(Vector3::Dot((vector7 - centerPos).normalized(), normalized3)) * 57.29578f;
	if (influenceMaxAngle < num9)
	{
		if (working.Evaluation < 0.f)
		{
			working.Evaluation = 0.f;
		}
		return;
	}
	float num10 = 1.f;
	if (influenceAngle < num9)
	{
		num10 = 1.f - (num9 - influenceAngle) / (influenceMaxAngle - influenceAngle);
	}
	float num11 = num10 * num8;
	if (working.Evaluation < num11)
	{
		working.Evaluation = num11;
	}
}

static void UpdateSkirtNativePluginOne(Gallop::NativeSkirtWorking& working, Gallop::NativeSkirtArg& arg)
{
	Vector3 vector = arg.CenterPos - arg.RootPos;
	Vector3 vector2 = arg.KneeLPos - arg.RootPos;
	Vector3 vector3 = arg.AnkleLPos - arg.RootPos;
	Vector3 vector4 = arg.KneeRPos - arg.RootPos;
	Vector3 vector5 = arg.AnkleRPos - arg.RootPos;
	if (working.IsCheckLeftKnee)
	{
		_UpdateSkirtNativePluginCalcRotAngle(working, vector2, arg.KneeColliderRadius, arg.InfluenceAngle, arg.InfluenceMaxAngle, vector, arg.RootPos);
	}
	if (working.IsCheckLeftAnkle)
	{
		_UpdateSkirtNativePluginCalcRotAngle(working, vector3, arg.AnkleColliderRadius, arg.InfluenceAngle, arg.InfluenceMaxAngle, vector, arg.RootPos);
	}
	if (working.IsCheckRightKnee)
	{
		_UpdateSkirtNativePluginCalcRotAngle(working, vector4, arg.KneeColliderRadius, arg.InfluenceAngle, arg.InfluenceMaxAngle, vector, arg.RootPos);
	}
	if (working.IsCheckRightAnkle)
	{
		_UpdateSkirtNativePluginCalcRotAngle(working, vector5, arg.AnkleColliderRadius, arg.InfluenceAngle, arg.InfluenceMaxAngle, vector, arg.RootPos);
	}
}

static void UpdateForce(Gallop::NativeClothWorking& clothWorking, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, Vector3 positionDiff, float frameScale)
{
	clothWorking.Force = clothWorking.AimVector * (clothWorking.StiffnessForce * stiffnessForceRate / 1000.f / frameScale);
	clothWorking.Force = clothWorking.Force + positionDiff * (clothWorking.DragForce * dragForceRate / 100.f / frameScale);
	clothWorking.Force.y = clothWorking.Force.y - clothWorking.Gravity * gravityRate / 10000.f / frameScale;
	float num = lerp(clothWorking.VerticalWindRateSlow, clothWorking.VerticalWindRateFast, windStrength);
	float num2 = lerp(clothWorking.HorizontalWindRateSlow, clothWorking.HorizontalWindRateFast, windStrength);
	clothWorking.Force.y = clothWorking.Force.y + windY * num;
	clothWorking.Force.x = clothWorking.Force.x + windX * num2;
	clothWorking.Force.z = clothWorking.Force.z + windZ * num2;
	clothWorking.Force.x = clothWorking.Force.x + clothWorking.ConnectedForce.x / frameScale;
	clothWorking.Force.y = clothWorking.Force.y + clothWorking.ConnectedForce.y / frameScale;
	clothWorking.Force.z = clothWorking.Force.z + clothWorking.ConnectedForce.z / frameScale;
}

static Vector3 GetWorldCollisionPosition(Gallop::NativeClothCollision& collision, Gallop::NativeRootParentWork* parentWorkArray)
{
	return parentWorkArray[collision.ParentWorkIndex].WorldPosition + parentWorkArray[collision.ParentWorkIndex].WorldRotation * collision.Position;
}

static Vector3 GetWorldCollisionPosition2(Gallop::NativeClothCollision& collision, Gallop::NativeRootParentWork* parentWorkArray)
{
	return parentWorkArray[collision.ParentWorkIndex].WorldPosition + parentWorkArray[collision.ParentWorkIndex].WorldRotation * collision.Position2;
}

static float CalcDistance(Vector3 position, Gallop::NativeClothCollision& p)
{
	return Vector3::Dot(position, p.Normal) - p.Distance;
}

static void CollisionDetect(Gallop::NativeClothWorking& clothWorking, Gallop::NativeClothCollision* collisionArray, Gallop::NativeRootParentWork* parentWorkArray, Vector3& finalPosition)
{
	vector<int> array =
	{
		clothWorking.CIndex0,
		clothWorking.CIndex1,
		clothWorking.CIndex2,
		clothWorking.CIndex3,
		clothWorking.CIndex4,
		clothWorking.CIndex5,
		clothWorking.CIndex6,
		clothWorking.CIndex7,
		clothWorking.CIndex8,
		clothWorking.CIndex9,
		clothWorking.CIndex10,
		clothWorking.CIndex11,
		clothWorking.CIndex12,
		clothWorking.CIndex13,
		clothWorking.CIndex14,
		clothWorking.CIndex15
	};

	for (int i = 0; i < clothWorking.ActiveCollision; i++)
	{
		int num = array[i];
		Gallop::NativeClothCollision nativeClothCollision = collisionArray[num];
		if (nativeClothCollision.IsEnable != 0 && (clothWorking.CheckCharaCollision || nativeClothCollision.IsCharaCollision == 0))
		{
			switch (nativeClothCollision.Type)
			{
			case 0:
			{
				Vector3 worldCollisionPosition = GetWorldCollisionPosition(nativeClothCollision, parentWorkArray);
				if (nativeClothCollision.IsInner)
				{
					if (Vector3::Distance(finalPosition, worldCollisionPosition) >= nativeClothCollision.Radius - clothWorking.CollisionRadius)
					{
						Vector3 normalized = (finalPosition - worldCollisionPosition).normalized();
						finalPosition = worldCollisionPosition + normalized * (nativeClothCollision.Radius - clothWorking.CollisionRadius);
						finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
					}
				}
				else if (Vector3::Distance(finalPosition, worldCollisionPosition) <= clothWorking.CollisionRadius + nativeClothCollision.Radius)
				{
					Vector3 normalized2 = (finalPosition - worldCollisionPosition).normalized();
					finalPosition = worldCollisionPosition + normalized2 * (clothWorking.CollisionRadius + nativeClothCollision.Radius);
					finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
				}
				break;
			}
			case 2:
			{
				Vector3 worldCollisionPosition2 = GetWorldCollisionPosition(nativeClothCollision, parentWorkArray);
				Vector3 worldCollisionPosition3 = GetWorldCollisionPosition2(nativeClothCollision, parentWorkArray);
				Vector3 vector = worldCollisionPosition3 - worldCollisionPosition2;
				float magnitude = vector.magnitude();
				vector = vector / magnitude;
				Vector3 vector2 = finalPosition - worldCollisionPosition2;
				float num2 = Vector3::Dot(vector2, vector);
				bool flag = false;
				if (0.f <= num2 && num2 < magnitude)
				{
					Vector3 vector3 = vector * num2;
					Vector3 vector4 = vector2 - vector3;
					float magnitude2 = vector4.magnitude();
					if (magnitude2 < clothWorking.CollisionRadius + nativeClothCollision.Radius)
					{
						finalPosition = worldCollisionPosition2 + vector3 + vector4 * ((clothWorking.CollisionRadius + nativeClothCollision.Radius) / magnitude2);
						finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
						flag = true;
					}
				}
				if (!flag)
				{
					if (Vector3::Distance(finalPosition, worldCollisionPosition2) <= clothWorking.CollisionRadius + nativeClothCollision.Radius)
					{
						Vector3 normalized3 = (finalPosition - worldCollisionPosition2).normalized();
						finalPosition = worldCollisionPosition2 + normalized3 * (clothWorking.CollisionRadius + nativeClothCollision.Radius);
						finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
					}
					else if (Vector3::Distance(finalPosition, worldCollisionPosition3) <= clothWorking.CollisionRadius + nativeClothCollision.Radius)
					{
						Vector3 normalized4 = (finalPosition - worldCollisionPosition3).normalized();
						finalPosition = worldCollisionPosition3 + normalized4 * (clothWorking.CollisionRadius + nativeClothCollision.Radius);
						finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
					}
				}
				break;
			}
			case 3:
			{
				float num3 = CalcDistance(finalPosition, nativeClothCollision);
				if (num3 <= clothWorking.CollisionRadius)
				{
					finalPosition = finalPosition + nativeClothCollision.Normal * (clothWorking.CollisionRadius - num3);
					finalPosition = (finalPosition - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
				}
				break;
			}
			}
		}
	}
}

static void UndoLength(Gallop::NativeClothWorking& clothWorking, Vector3& finalPosition)
{
	Vector3 normalized = (finalPosition - clothWorking.SelfPosition).normalized();
	finalPosition = normalized * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
}

static Vector3 RoundAngle(Vector3 _angle)
{
	Vector3 vector = _angle;
	vector.x = fmod((vector.x + 360.f), 360.f);
	vector.y = fmod((vector.y + 360.f), 360.f);
	vector.z = fmod((vector.z + 360.f), 360.f);
	if (vector.x > 180.f)
	{
		vector.x -= 360.f;
	}
	if (vector.y > 180.f)
	{
		vector.y -= 360.f;
	}
	if (vector.z > 180.f)
	{
		vector.z -= 360.f;
	}
	return vector;
}

static void UpdateNativeClothInternal(Gallop::NativeClothWorking& clothWorking, Gallop::NativeClothCollision* collisionArray, Gallop::NativeRootParentWork* nativeRootParentArray, float stiffnessForceRate, float dragForceRate, float gravityRate,
	float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale = 1.f, bool is60FPS = false, float moveRate = 1.f, float addMoveRate = 1.f, float springRate = 1.f)
{
	if (clothWorking.IsSkip)
	{
		return;
	}

	float num = max(60.f, 1.f * Gallop::FrameRateController::Instance().GetLayerFrameRate(Gallop::FrameRateController::FrameRateOverrideLayer::SystemValue));
	Vector3 vector = clothWorking.PrevTargetPosition - clothWorking.TargetPosition;
	if (!is60FPS)
	{
		vector = vector / 2.f;
		num = 30.f;
	}

	clothWorking.PrevTargetPosition = clothWorking.TargetPosition;
	Quaternion quaternion = clothWorking.ParentRotation * clothWorking.InitLocalRotation;
	clothWorking.AimVector = quaternion * clothWorking.BoneAxis;
	UpdateForce(clothWorking, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, vector, num);
	Vector3 vector2;
	if (is60FPS)
	{
		vector2 = clothWorking.TargetPosition - vector * timescale + clothWorking.Force * timescale;
	}
	else
	{
		vector2 = clothWorking.TargetPosition - 2.f * vector * timescale + 3.f * clothWorking.Force * timescale;
	}
	float num2 = springRate;
	float num3 = moveRate;
	if (clothWorking.IsAddSpring != 0)
	{
		num3 = addMoveRate;
	}
	if (num3 >= 0.f)
	{
		num2 *= lerp(clothWorking.MoveSpringApplyRate, 1.f, num3);
	}
	if (num2 < 1.f)
	{
		vector2 = Vector3::Lerp(clothWorking.AimVector.normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition, vector2, num2);
	}
	UndoLength(clothWorking, vector2);
	if (clothWorking.ActiveCollision > 0 && bCollisionSwitch)
	{
		CollisionDetect(clothWorking, collisionArray, nativeRootParentArray, vector2);
	}
	if (clothWorking.IsCheckSkirtKnee)
	{
		float num4 = Vector3::Dot(vector2 - clothWorking.SkirtNormalPos, clothWorking.SkirtKneeNormal);
		if (num4 < clothWorking.CollisionRadius)
		{
			vector2 = vector2 + clothWorking.SkirtKneeNormal * (clothWorking.CollisionRadius - num4);
			vector2 = (vector2 - clothWorking.SelfPosition).normalized() * clothWorking.InitBoneDistance + clothWorking.SelfPosition;
		}
	}
	clothWorking.Diff = (vector2 - clothWorking.SelfPosition).normalized();
	Quaternion quaternion2 = Quaternion::FromToRotation(clothWorking.AimVector, clothWorking.Diff);
	if (clothWorking.IsLimit)
	{
		Quaternion quaternion3 = quaternion2 * quaternion;
		Quaternion quaternion4 = Quaternion::Inverse(clothWorking.ParentRotation) * quaternion3;
		Vector3 vector3 = (Quaternion::Inverse(clothWorking.InitLocalRotation) * quaternion4).eulerAngles();
		vector3 = RoundAngle(vector3);
		vector3.x = clamp(vector3.x, -clothWorking.LimitRotationMin.x, clothWorking.LimitRotationMax.x);
		vector3.y = clamp(vector3.y, -clothWorking.LimitRotationMin.y, clothWorking.LimitRotationMax.y);
		vector3.z = clamp(vector3.z, -clothWorking.LimitRotationMin.z, clothWorking.LimitRotationMax.z);
		vector3 = RoundAngle(vector3);
		clothWorking.FinalRotation = clothWorking.ParentRotation * (clothWorking.InitLocalRotation * Quaternion::Euler(vector3));
		clothWorking.TargetPosition = clothWorking.SelfPosition + (clothWorking.FinalRotation * clothWorking.BoneAxis).normalized() * clothWorking.InitBoneDistance;
	}
	else
	{
		clothWorking.FinalRotation = quaternion2 * quaternion;
		clothWorking.TargetPosition = vector2;
	}
	if (clothWorking.DynamicRatio < 1.f)
	{
		clothWorking.FinalRotation = Quaternion::Lerp(clothWorking.AnimationRotation, clothWorking.FinalRotation, clothWorking.DynamicRatio);
	}
}

static void CySpringNative_NativeClothUpdate_hook(Gallop::NativeClothWorking* cond, int nCond, Gallop::NativeClothCollision* collisions, Gallop::NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
{
	stiffnessForceRate *= config::cyspring_stiffness_force_rate_scale;
	dragForceRate *= config::cyspring_drag_force_rate_scale;
	moveRate *= config::cyspring_move_rate_scale;
	addMoveRate *= config::cyspring_add_move_rate_scale;

	if (config::cyspring_legacy_behavior)
	{
		cond[0].AnimationRotation = pRootParentWork[cond[0].ParentWorkIndex].WorldRotation * cond[0].AnimationRotation;
		UpdateNativeClothInternal(cond[0], collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
		for (int i = 1; i < nCond; i++)
		{
			cond[i].SelfPosition = cond[i - 1].TargetPosition;
			cond[i].ParentRotation = cond[i - 1].FinalRotation;
			cond[i].AnimationRotation = cond[i - 1].AnimationRotation * cond[i].AnimationRotation;
			UpdateNativeClothInternal(cond[i], collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
		}
	}
	else
	{
		reinterpret_cast<decltype(CySpringNative_NativeClothUpdate_hook)*>(CySpringNative_NativeClothUpdate_orig)(cond, nCond, collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
	}
}

static void CySpringNative_NativeClothSkirtUpdate_hook(Gallop::NativeClothWorking* cond, int nCond, Gallop::NativeClothCollision* collisions, Gallop::NativeSkirtWorking& pWorking, Gallop::NativeSkirtArg& pArg, Gallop::NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
{
	stiffnessForceRate *= config::cyspring_stiffness_force_rate_scale;
	dragForceRate *= config::cyspring_drag_force_rate_scale;
	moveRate *= config::cyspring_move_rate_scale;
	addMoveRate *= config::cyspring_add_move_rate_scale;

	if (config::cyspring_legacy_behavior)
	{
		pWorking.Evaluation = -360.f;
		UpdateSkirtNativePluginOne(pWorking, pArg);
		if (pWorking.Evaluation > -360.f)
		{
			Quaternion quaternion = Quaternion::AngleAxis(pWorking.Evaluation - pWorking.OffsetAngle, pWorking.RotationAxis);
			cond[0].SkirtKneeNormal = quaternion * pWorking.SkirtInitNormal;
			cond[0].SkirtNormalPos = pWorking.SkirtRootPos - pArg.RootPos;
			cond[0].IsCheckSkirtKnee = true;
			for (int j = 1; j < nCond; j++)
			{
				cond[j].SkirtKneeNormal = cond[0].SkirtKneeNormal;
				cond[j].SkirtNormalPos = cond[0].SkirtNormalPos;
				cond[j].IsCheckSkirtKnee = cond[0].IsCheckSkirtKnee;
			}
		}

		cond[0].AnimationRotation = pRootParentWork[cond[0].ParentWorkIndex].WorldRotation * cond[0].AnimationRotation;
		UpdateNativeClothInternal(cond[0], collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
		for (int i = 1; i < nCond; i++)
		{
			cond[i].SelfPosition = cond[i - 1].TargetPosition;
			cond[i].ParentRotation = cond[i - 1].FinalRotation;
			cond[i].AnimationRotation = cond[i - 1].AnimationRotation * cond[i].AnimationRotation;
			UpdateNativeClothInternal(cond[i], collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
		}
	}
	else
	{
		reinterpret_cast<decltype(CySpringNative_NativeClothSkirtUpdate_hook)*>(CySpringNative_NativeClothSkirtUpdate_orig)(cond, nCond, collisions, pWorking, pArg, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, is60FPS, moveRate, addMoveRate, springRate);
	}
}

static void CySpringNative_NativeSkirtUpdate_hook(Gallop::NativeSkirtWorking& pWorking, Gallop::NativeSkirtArg& pArg)
{
	if (config::cyspring_legacy_behavior)
	{
		UpdateSkirtNativePluginOne(pWorking, pArg);
	}
	else
	{
		reinterpret_cast<decltype(CySpringNative_NativeSkirtUpdate_hook)*>(CySpringNative_NativeSkirtUpdate_orig)(pWorking, pArg);
	}
}

static void InitAddress()
{
	CySpringNative_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CySpringNative");
	CySpringNative_cctor_addr = il2cpp_symbols::get_method_pointer(CySpringNative_klass, ".cctor", 0);
	CySpringNative_UpdateForce_addr = il2cpp_symbols::get_method_pointer(CySpringNative_klass, "UpdateForce", 8);
	CySpringNative_isNativeField = il2cpp_class_get_field_from_name(CySpringNative_klass, "isNative");
#ifdef _MSC_VER
	wstring module_name;
	module_name.resize(MAX_PATH);
	module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

	filesystem::path module_path(module_name);

	wstring name = module_path.filename().replace_extension();

	SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());

	auto CySpringPlugin = LoadLibraryExW(L"CySpringPlugin.dll", nullptr, 0);

	SetDllDirectoryW(nullptr);

	CySpringNative_NativeClothUpdate_addr = GetProcAddress(CySpringPlugin, "NativeClothUpdate");
	CySpringNative_NativeClothSkirtUpdate_addr = GetProcAddress(CySpringPlugin, "NativeClothSkirtUpdate");
	CySpringNative_NativeSkirtUpdate_addr = GetProcAddress(CySpringPlugin, "NativeSkirtUpdate");
#else
	if (Game::CurrentGameRegion == Game::Region::ENG)
	{
		CySpringNative_NativeClothUpdate_addr = il2cpp_symbols::get_method_pointer<void*>(CySpringNative_klass, "NativeClothUpdate", -1);
		CySpringNative_NativeClothSkirtUpdate_addr = il2cpp_symbols::get_method_pointer<void*>(CySpringNative_klass, "NativeClothSkirtUpdate", -1);
		CySpringNative_NativeSkirtUpdate_addr = il2cpp_symbols::get_method_pointer<void*>(CySpringNative_klass, "NativeSkirtUpdate", -1);
	}
	else
	{
		auto CySpringPlugin = dlopen("libcyspringandroid.so", RTLD_NOW);

		CySpringNative_NativeClothUpdate_addr = dlsym(CySpringPlugin, "NativeClothUpdate");
		CySpringNative_NativeClothSkirtUpdate_addr = dlsym(CySpringPlugin, "NativeClothSkirtUpdate");
		CySpringNative_NativeSkirtUpdate_addr = dlsym(CySpringPlugin, "NativeSkirtUpdate");
	}
#endif
}

static void HookMethods()
{
	ADD_HOOK(CySpringNative_cctor, "Gallop.CySpringNative::.cctor at %p\n");
	ADD_HOOK(CySpringNative_UpdateForce, "Gallop.CySpringNative::UpdateForce at %p\n");
	ADD_HOOK(CySpringNative_NativeClothUpdate, "Gallop.CySpringNative::NativeClothUpdate at %p\n");
	ADD_HOOK(CySpringNative_NativeClothSkirtUpdate, "Gallop.CySpringNative::NativeClothSkirtUpdate at %p\n");
	ADD_HOOK(CySpringNative_NativeSkirtUpdate, "Gallop.CySpringNative::NativeSkirtUpdate at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	bool CySpringNative::isNative()
	{
		bool value;
		il2cpp_field_static_get_value(CySpringNative_isNativeField, &value);
		return value;
	}

	void CySpringNative::isNative(bool value)
	{
		il2cpp_field_static_set_value(CySpringNative_isNativeField, &value);
	}

	void CySpringNative::NativeClothUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
	{
		reinterpret_cast<decltype(NativeClothUpdate)*>(CySpringNative_NativeClothUpdate_addr)(cond, nCond, collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, true, moveRate, addMoveRate, springRate);
	}

	void CySpringNative::NativeClothSkirtUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeSkirtWorking* pWorking, NativeSkirtArg* pArg, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
	{
		reinterpret_cast<decltype(NativeClothSkirtUpdate)*>(CySpringNative_NativeClothSkirtUpdate_addr)(cond, nCond, collisions, pWorking, pArg, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, true, moveRate, addMoveRate, springRate);
	}

	void CySpringNative::NativeSkirtUpdate(NativeSkirtWorking* pWorking, NativeSkirtArg* pArg)
	{
		reinterpret_cast<decltype(NativeSkirtUpdate)*>(CySpringNative_NativeSkirtUpdate_addr)(pWorking, pArg);
	}
}
