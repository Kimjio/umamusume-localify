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

		static Color white()
		{
			return { 1.f, 1.f, 1.f, 1.f };
		}

		static Color gray()
		{
			return { .5f, .5f, .5f, 1.f };
		}
	};
}
