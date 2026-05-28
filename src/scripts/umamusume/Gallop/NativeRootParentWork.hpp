#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"

using namespace UnityEngine;

namespace Gallop
{
	struct NativeRootParentWork
	{
		Vector3 WorldPosition;
		int : 32;
		Quaternion WorldRotation;
	};
}
