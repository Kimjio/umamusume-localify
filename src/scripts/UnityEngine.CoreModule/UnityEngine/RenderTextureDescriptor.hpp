#pragma once

namespace UnityEngine
{
	struct RenderTextureDescriptor
	{
		int _width;
		int _height;
		int _msaaSamples;
		int _volumeDepth;
		int _mipCount;
		int _graphicsFormat;
		int _stencilFormat;
		int _depthStencilFormat;
		int _dimension;
		int _shadowSamplingMode;
		int _vrUsage;
		int _flags;
		int _memoryless;
	};
}
