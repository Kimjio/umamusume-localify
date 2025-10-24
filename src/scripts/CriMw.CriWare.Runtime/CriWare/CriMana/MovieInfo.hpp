#pragma once
#include "AudioInfo.hpp"
#include "CodecType.hpp"

#include "il2cpp/il2cpp-object-internals.h"

#include <cstdint>

namespace CriWare
{
	namespace CriMana
	{
		struct MovieInfo : Il2CppObject
		{
		public:
			bool hasAlpha()
			{
				return numAlphaStreams == 1U;
			}

			void hasAlpha(bool value)
			{
				numAlphaStreams = (value ? 1U : 0U);
			}

		private:
			uint32_t _reserved1;

		public:
			uint32_t numAlphaStreams;
			
			uint32_t width;

			uint32_t height;

			uint32_t dispWidth;

			uint32_t dispHeight;

			uint32_t framerateN;

			uint32_t framerateD;

			uint32_t totalFrames;

			CodecType codecType;

			CodecType alphaCodecType;

			uint32_t numAudioStreams;

			AudioInfo* audioPrm[32];

			uint32_t numSubtitleChannels;

			uint32_t maxSubtitleSize;

			uint32_t maxChunkSize;
		};
	}
}
