#pragma once
#include <cstdint>

namespace CriWare
{
	namespace CriMana
	{
		struct AudioInfo
		{
		public:
			uint32_t samplingRate;

			uint32_t numChannels;

			uint32_t totalSamples;
		};
	}
}
