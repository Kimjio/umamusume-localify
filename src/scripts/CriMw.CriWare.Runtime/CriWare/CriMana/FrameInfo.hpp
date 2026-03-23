#pragma once
#include "AlphaType.hpp"

#include "il2cpp/il2cpp-object-internals.h"

#include <cstdint>

namespace CriWare
{
	namespace CriMana
	{
		struct FrameInfo
		{
		public:
			int frameNo;
			int frameNoPerFile;
			uint32_t width;
			uint32_t height;
			uint32_t dispWidth;
			uint32_t dispHeight;
			uint32_t numImages;
			uint32_t framerateN;
			uint32_t framerateD;
		private:
			uint32_t _reserved1;
		public:
			uint64_t time;
			uint64_t tunit;
			uint32_t cntConcatenatedMovie;
		private:
			AlphaType alphaType;
		public:
			uint32_t cntSkippedFrames;
			uint32_t totalFramesPerFile;
		};
	}
}
