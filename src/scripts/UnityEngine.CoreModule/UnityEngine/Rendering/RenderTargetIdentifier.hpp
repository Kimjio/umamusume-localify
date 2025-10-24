#pragma once

#include "BuiltinRenderTextureType.hpp"
#include "../CubemapFace.hpp"

namespace UnityEngine
{
	namespace Rendering
	{
		struct RenderTargetIdentifier
		{
		public:
			RenderTargetIdentifier(BuiltinRenderTextureType type)
			{
				m_Type = type;
				m_NameID = -1;
				m_InstanceID = 0;
				m_BufferPointer = nullptr;
				m_MipLevel = 0;
				m_CubeFace = CubemapFace::Unknown;
				m_DepthSlice = 0;
			}

		private:
			BuiltinRenderTextureType m_Type;
			int m_NameID;
			int m_InstanceID;
			void* m_BufferPointer;
			int m_MipLevel;
			CubemapFace m_CubeFace;
			int m_DepthSlice;
		};
	}
}
