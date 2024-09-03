#pragma once

#include "il2cpp/il2cpp-object-internals.h"

namespace UnityEngine
{
	class BeforeRenderHelper
	{
	public:
		struct OrderBlock
		{
		public:
			int order;
			Il2CppDelegate* callback;
		};
	};
}
