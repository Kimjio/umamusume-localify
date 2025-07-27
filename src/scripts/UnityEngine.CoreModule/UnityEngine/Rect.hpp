#pragma once

namespace UnityEngine
{
	struct Rect
	{
	public:
		float x;
		float y;
		float width;
		float height;

		float xMin() const { return x; }
		float yMin() const { return y; }
		float xMax() const { return x + width; }
		float yMax() const { return y + height; }

		Rect operator*=(float other)
		{
			x = x * other;
			y = y * other;
			width = width * other;
			height = height * other;
			return Rect{ x, y, width, height };
		}

		Rect operator/=(float other)
		{
			x = x / other; 
			y = y / other;
			width = width / other;
			height = height / other;
			return Rect{ x, y, width, height };
		}
	};
}
