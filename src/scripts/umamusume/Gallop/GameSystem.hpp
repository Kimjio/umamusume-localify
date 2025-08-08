#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

namespace Gallop
{
	class GameSystem : public MonoSingleton<GameSystem>
	{
	public:
		enum class SystemState
		{
			Booting,
			Deactive,
			Active,
			SoftwareReset
		};

		static bool IsExecutingSoftwareReset();

		Il2CppObject* InitializeGame(Il2CppDelegate* onCompleteInitializeUI);

		void SoftwareReset();

		SystemState _systemState();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
