#pragma once
#include "Vector2.hpp"

#include <cmath>

namespace UnityEngine
{
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		static float Dot(Vector3 lhs, Vector3 rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		static Vector3 Cross(Vector3 lhs, Vector3 rhs)
		{
			return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		static float Angle(Vector3 from, Vector3 to)
		{
			float num = sqrtf(from.sqrMagnitude() * to.sqrMagnitude());
			bool flag = num < 1E-15f;
			float num2;
			if (flag)
			{
				num2 = 0.0f;
			}
			else
			{
				float num3 = clamp(Dot(from, to) / num, -1.0f, 1.0f);
				num2 = acosf(num3) * 57.29578f;
			}
			return num2;
		}

		static float Distance(Vector3 a, Vector3 b)
		{
			float num = a.x - b.x;
			float num2 = a.y - b.y;
			float num3 = a.z - b.z;
			return sqrtf(num * num + num2 * num2 + num3 * num3);
		}

		static Vector3 Normalize(Vector3 value)
		{
			float num = value.magnitude();
			bool flag = num > 1E-05f;
			Vector3 vector;
			if (flag)
			{
				vector = value / num;
			}
			else
			{
				vector = { 0, 0, 0 };
			}
			return vector;
		}

		static Vector3 Lerp(Vector3 a, Vector3 b, float t)
		{
			t = clamp(t, 0.0f, 1.0f);
			return Vector3(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
		}

		float sqrMagnitude() const
		{
			return x * x + y * y + z * z;
		}

		float magnitude() const
		{
			return sqrtf(sqrMagnitude());
		}

		Vector3 normalized() const
		{
			return Normalize(*this);
		}

		Vector3 operator*(float v) const noexcept
		{
			return Vector3(x * v, y * v, z * v);
		}

		Vector3 operator/(float v) const noexcept
		{
			return Vector3(x / v, y / v, z / v);
		}

		Vector3 operator+(Vector3 v) const noexcept
		{
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		Vector3 operator-(Vector3 v) const noexcept
		{
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		operator Vector2() const
		{
			return Vector2{ x, y };
		}
	};

	inline Vector3 operator*(float v, Vector3 vector) noexcept
	{
		return Vector3(vector.x * v, vector.y * v, vector.z * v);
	}
}
