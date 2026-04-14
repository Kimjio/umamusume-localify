#pragma once
#include "MonoSingleton.hpp"

namespace Gallop
{
	class GraphicSettings : public MonoSingleton<GraphicSettings>
	{
	public:
		void Update3DRenderTexture();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
