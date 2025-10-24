#pragma once

namespace UnityEngine
{
	struct Vector2
	{
	public:
		float x;
		float y;

		Vector2 operator*(float v) const noexcept
		{
			return Vector2(x * v, y * v);
		}

		Vector2 operator/(float v) const noexcept
		{
			return Vector2(x / v, y / v);
		}
	};
}
