#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GameSystem.hpp"

namespace
{
	void* InitializeGame_addr = nullptr;
	void* InitializeGame1_addr = nullptr;

	void* IsExecutingSoftwareReset_addr = nullptr;

	void* SoftwareReset_addr = nullptr;
}

static void InitAddress()
{
	InitializeGame_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GameSystem", "InitializeGame", 1);
	InitializeGame1_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GameSystem", "InitializeGame", 0);
	IsExecutingSoftwareReset_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GameSystem", "IsExecutingSoftwareReset", 0);
	SoftwareReset_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GameSystem", "SoftwareReset", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	const char* GameSystem::namespaze = "Gallop";
	const char* GameSystem::klassName = "GameSystem";

	Il2CppObject* GameSystem::InitializeGame(Il2CppDelegate* onCompleteInitializeUI)
	{
		if (!InitializeGame_addr)
		{
			return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(InitializeGame1_addr)(instance);
		}
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppDelegate*)>(InitializeGame_addr)(instance, onCompleteInitializeUI);
	}

	bool GameSystem::IsExecutingSoftwareReset()
	{
		return reinterpret_cast<bool (*)()>(IsExecutingSoftwareReset_addr)();
	}

	void GameSystem::SoftwareReset()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(SoftwareReset_addr)(instance);
	}

	GameSystem::SystemState GameSystem::_systemState()
	{
		auto _systemStateField = il2cpp_class_get_field_from_name(instance->klass, "_systemState");
		GameSystem::SystemState _systemState;
		il2cpp_field_get_value(instance, _systemStateField, &_systemState);

		return _systemState;
	}
}
