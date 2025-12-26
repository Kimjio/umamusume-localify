#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

using namespace UnityEngine;

namespace Gallop
{
	class ResourceManager : public MonoSingleton<ResourceManager>
	{
		using MonoSingleton::MonoSingleton;
	};
}
