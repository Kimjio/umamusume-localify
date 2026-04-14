#pragma once
#include "Singleton.hpp"

namespace Gallop
{
	class ModelLoader : public Singleton<ModelLoader>
	{
	public:
		using Singleton::Singleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
