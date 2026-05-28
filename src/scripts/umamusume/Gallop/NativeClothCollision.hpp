#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeClothCollision
	{
		Vector3 Position;
		int : 32;
		Vector3 Position2;
		int : 32;
		Vector3 Normal;
		int : 32;
		int Type;
		ALIGN_TYPE(4) bool IsInner;
		float Radius;
		float Distance;
		int ParentWorkIndex;
		int IsEnable;
		int IsCharaCollision;
	};
}
