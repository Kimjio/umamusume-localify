#pragma once
#include "Object.hpp"
#include "RenderTextureDescriptor.hpp"

namespace UnityEngine
{
	class RenderTexture : public Object
	{
	public:
		RenderTexture(RenderTextureDescriptor desc);

		int width();

		void width(int value);

		int height();

		void height(int value);

		bool Create();

		static RenderTexture GetTemporary(int width, int height);

		static RenderTexture GetActive();

		static void SetActive(RenderTexture rt);

		static void ReleaseTemporary(RenderTexture temp);

		using Object::Object;
	};
}