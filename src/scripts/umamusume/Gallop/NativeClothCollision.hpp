#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeClothCollision
	{
		Vector3 Position;
		Vector3 Position2;
		Vector3 Normal;
		int Type;
		bool IsInner;
		float Radius;
		float Distance;
		int ParentWorkIndex;
		int IsEnable;
		int IsCharaCollision;
	};
}
