#pragma once

namespace UnityEngine
{
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		static Vector3 Slerp(Vector3 a, Vector3 b, float t);

		Vector3 operator*(float d) const noexcept
		{
			return Vector3(x * d, y * d, z * d);
		}

		Vector3 operator/(float d) const noexcept
		{
			return Vector3(x / d, y / d, z / d);
		}
	};
}
