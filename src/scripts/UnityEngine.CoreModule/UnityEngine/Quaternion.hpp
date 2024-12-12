#pragma once

#include "Vector3.hpp"

namespace UnityEngine
{
	struct Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection);
		static Quaternion Inverse(Quaternion rotation);
		static Quaternion Slerp(Quaternion a, Quaternion b, float t);
		static Quaternion Lerp(Quaternion a, Quaternion b, float t);
		static Quaternion Euler(float x, float y, float z);
	};
}
