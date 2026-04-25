#pragma once
#include "NativeClothWorking.hpp"
#include "NativeClothCollision.hpp"
#include "NativeRootParentWork.hpp"
#include "NativeSkirtWorking.hpp"
#include "NativeSkirtArg.hpp"

namespace Gallop
{
	struct CySpringNative
	{
		static bool isNative();
		static void isNative(bool value);

		static void NativeClothUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate);
		static void NativeClothSkirtUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeSkirtWorking* pWorking, NativeSkirtArg* pArg, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate);
		static void NativeSkirtUpdate(NativeSkirtWorking* pWorking, NativeSkirtArg* pArg);
	};
}
