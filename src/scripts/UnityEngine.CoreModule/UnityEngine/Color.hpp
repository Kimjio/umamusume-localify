#pragma once

namespace UnityEngine
{
	struct Color
	{
	public:
		float r;
		float g;
		float b;
		float a;

		float grayscale()
		{
			return 0.299f * r + 0.587f * g + 0.114f * b;
		}
	};
}
