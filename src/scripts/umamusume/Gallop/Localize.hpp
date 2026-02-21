#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace Gallop
{
	class Localize : public UnityEngine::Object
	{
	public:
		enum Region
		{
			JP,
		};

		static Il2CppString* Get(uint64_t id);

		static Il2CppString* Get(Il2CppString* id);

		static void Set(Localize::Region region, Il2CppString* id, Il2CppString* value);

		static void DumpAllEntries();

		using Object::Object;
	};
}
