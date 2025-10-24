#pragma once
#include "Vector2.hpp"

namespace UnityEngine
{
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		static Vector3 Slerp(Vector3 a, Vector3 b, float t);

		Vector3 operator*(float v) const noexcept
		{
			return Vector3(x * v, y * v, z * v);
		}

		Vector3 operator/(float v) const noexcept
		{
			return Vector3(x / v, y / v, z / v);
		}

		operator Vector2() const
		{
			return Vector2{ x, y };
		}
	};
}
