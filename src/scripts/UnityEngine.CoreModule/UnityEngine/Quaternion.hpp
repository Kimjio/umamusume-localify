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

		static Quaternion AngleAxis(float angle, Vector3 axis);
		static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection);
		static Quaternion Inverse(Quaternion rotation);
		static Quaternion Slerp(Quaternion a, Quaternion b, float t);
		static Quaternion Lerp(Quaternion a, Quaternion b, float t);
		static Quaternion Euler(float x, float y, float z);
		static Quaternion Euler(Vector3 euler);

		Vector3 eulerAngles();

		Vector3 operator*(Vector3 point) const noexcept
		{
			const float num = x * 2.0f;
			const float num2 = y * 2.0f;
			const float num3 = z * 2.0f;
			const float num4 = x * num;
			const float num5 = y * num2;
			const float num6 = z * num3;
			const float num7 = x * num2;
			const float num8 = x * num3;
			const float num9 = y * num3;
			const float num10 = w * num;
			const float num11 = w * num2;
			const float num12 = w * num3;
			Vector3 vector;
			vector.x = ((1.0f - (num5 + num6)) * point.x) + ((num7 - num12) * point.y) + ((num8 + num11) * point.z);
			vector.y = ((num7 + num12) * point.x) + ((1.0f - (num4 + num6)) * point.y) + ((num9 - num10) * point.z);
			vector.z = ((num8 - num11) * point.x) + ((num9 + num10) * point.y) + ((1.0f - (num4 + num5)) * point.z);
			return vector;
		}

		Quaternion operator*(Quaternion rhs) const noexcept
		{
			return Quaternion{.x=(w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y), .y=(w * rhs.y) + (y * rhs.w) + (z * rhs.x) - (x * rhs.z), .z=(w * rhs.z) + (z * rhs.w) + (x * rhs.y) - (y * rhs.x), .w=(w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z)};
		}

	private:
		static Vector3 Internal_ToEulerRad(Quaternion rotation);
		static Vector3 Internal_MakePositive(Vector3 euler)
		{
			const float num = -0.005729578f;
			const float num2 = 360.f + num;
			if (euler.x < num)
			{
				euler.x += 360.f;
			}
			else
			{
				if (euler.x > num2)
				{
					euler.x -= 360.f;
				}
			}
			if (euler.y < num)
			{
				euler.y += 360.f;
			}
			else if (euler.y > num2)
			{
				euler.y -= 360.f;
			}
			if (euler.z < num)
			{
				euler.z += 360.f;
			}
			else if (euler.z > num2)
			{
				euler.z -= 360.f;
			}
			return euler;
		}
	};
}
