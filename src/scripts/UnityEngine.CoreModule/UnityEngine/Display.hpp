#pragma once
#include "Object.hpp"

namespace UnityEngine
{
	class Display : public Object
	{
	public:
		static Display main();

		int renderingWidth();

		int renderingHeight();

		int systemWidth();

		int systemHeight();

		using Object::Object;
	};
}
