#pragma once
#include "MonoSingleton.hpp"

namespace Gallop
{
	class Certification : public MonoSingleton<Certification>
	{
	public:
		static Il2CppString* dmmViewerId();

		static void dmmViewerId(Il2CppString* value);

		static Il2CppString* dmmOnetimeToken();

		static void dmmOnetimeToken(Il2CppString* value);

		static void initDmmPlatformData();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
