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
			return Vector2{.x=x * v, .y=y * v};
		}

		Vector2 operator/(float v) const noexcept
		{
			return Vector2{.x=x / v, .y=y / v};
		}

		Vector2 operator-(float v) const noexcept
		{
			return Vector2{.x=x - v, .y=y - v};
		}

		Vector2 operator-(Vector2 v) const noexcept
		{
			return Vector2{.x=x - v.x, .y=y - v.y};
		}

		[[nodiscard]] float sqrMagnitude() const
		{
			return (x * x) + (y * y);
		}
	};
}
