#pragma once
#include <cstdint>

namespace UnityEngine
{
	struct Color32
	{
	public:
		union
		{
			int rgba;
			struct
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
		};
	};
}
