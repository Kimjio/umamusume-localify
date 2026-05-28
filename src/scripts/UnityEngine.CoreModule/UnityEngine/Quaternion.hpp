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
			float num = x * 2.0f;
			float num2 = y * 2.0f;
			float num3 = z * 2.0f;
			float num4 = x * num;
			float num5 = y * num2;
			float num6 = z * num3;
			float num7 = x * num2;
			float num8 = x * num3;
			float num9 = y * num3;
			float num10 = w * num;
			float num11 = w * num2;
			float num12 = w * num3;
			Vector3 vector;
			vector.x = (1.0f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
			vector.y = (num7 + num12) * point.x + (1.0f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
			vector.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1.0f - (num4 + num5)) * point.z;
			return vector;
		}

		Quaternion operator*(Quaternion rhs) const noexcept
		{
			return Quaternion(w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y, w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z, w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x, w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
		}

	private:
		static Vector3 Internal_ToEulerRad(Quaternion rotation);
		static Vector3 Internal_MakePositive(Vector3 euler)
		{
			float num = -0.005729578f;
			float num2 = 360.f + num;
			bool flag = euler.x < num;
			if (flag)
			{
				euler.x += 360.f;
			}
			else
			{
				bool flag2 = euler.x > num2;
				if (flag2)
				{
					euler.x -= 360.f;
				}
			}
			bool flag3 = euler.y < num;
			if (flag3)
			{
				euler.y += 360.f;
			}
			else
			{
				bool flag4 = euler.y > num2;
				if (flag4)
				{
					euler.y -= 360.f;
				}
			}
			bool flag5 = euler.z < num;
			if (flag5)
			{
				euler.z += 360.f;
			}
			else
			{
				bool flag6 = euler.z > num2;
				if (flag6)
				{
					euler.z -= 360.f;
				}
			}
			return euler;
		}
	};
}
