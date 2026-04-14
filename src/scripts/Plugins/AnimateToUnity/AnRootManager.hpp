#pragma once
#include "AnMonoSingleton.hpp"

namespace AnimateToUnity
{
	class AnRootManager : public AnMonoSingleton<AnRootManager>
	{
	public:
		void ScreenRate(float value);

		using AnMonoSingleton::AnMonoSingleton;

		static const char* klassName;
	};
}
