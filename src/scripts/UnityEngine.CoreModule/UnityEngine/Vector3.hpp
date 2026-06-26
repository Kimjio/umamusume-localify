#pragma once
#include "Vector2.hpp"

#include <algorithm>

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
			return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
		}

		static Vector3 Cross(Vector3 lhs, Vector3 rhs)
		{
			return Vector3{.x=(lhs.y * rhs.z) - (lhs.z * rhs.y), .y=(lhs.z * rhs.x) - (lhs.x * rhs.z), .z=(lhs.x * rhs.y) - (lhs.y * rhs.x)};
		}

		static float Angle(Vector3 from, Vector3 to)
		{
			const float num = sqrtf(from.sqrMagnitude() * to.sqrMagnitude());
			const bool flag = num < 1E-15f;
			float num2;
			if (flag)
			{
				num2 = 0.0f;
			}
			else
			{
				float num3 = std::clamp(Dot(from, to) / num, -1.0f, 1.0f);
				num2 = acosf(num3) * 57.29578f;
			}
			return num2;
		}

		static float Distance(Vector3 a, Vector3 b)
		{
			const float num = a.x - b.x;
			const float num2 = a.y - b.y;
			const float num3 = a.z - b.z;
			return sqrtf((num * num) + (num2 * num2) + (num3 * num3));
		}

		static Vector3 Normalize(Vector3 value)
		{
			const float num = value.magnitude();
			const bool flag = num > 1E-05f;
			Vector3 vector;
			if (flag)
			{
				vector = value / num;
			}
			else
			{
				vector = { .x=0, .y=0, .z=0 };
			}
			return vector;
		}

		static Vector3 Lerp(Vector3 a, Vector3 b, float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			return Vector3{.x=a.x + ((b.x - a.x) * t), .y=a.y + ((b.y - a.y) * t), .z=a.z + ((b.z - a.z) * t)};
		}

		[[nodiscard]] float sqrMagnitude() const
		{
			return (x * x) + (y * y) + (z * z);
		}

		[[nodiscard]] float magnitude() const
		{
			return sqrtf(sqrMagnitude());
		}

		[[nodiscard]] Vector3 normalized() const
		{
			return Normalize(*this);
		}

		Vector3 operator*(float v) const noexcept
		{
			return Vector3{.x=x * v, .y=y * v, .z=z * v};
		}

		Vector3 operator/(float v) const noexcept
		{
			return Vector3{.x=x / v, .y=y / v, .z=z / v};
		}

		Vector3 operator+(Vector3 v) const noexcept
		{
			return Vector3{.x=x + v.x, .y=y + v.y, .z=z + v.z};
		}

		Vector3 operator-(Vector3 v) const noexcept
		{
			return Vector3{.x=x - v.x, .y=y - v.y, .z=z - v.z};
		}

		explicit operator Vector2() const
		{
			return Vector2{ .x=x, .y=y };
		}
	};

	inline Vector3 operator*(float v, Vector3 vector) noexcept
	{
		return Vector3{.x=vector.x * v, .y=vector.y * v, .z=vector.z * v};
	}
}
