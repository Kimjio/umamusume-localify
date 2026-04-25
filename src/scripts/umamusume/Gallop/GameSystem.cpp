#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GameSystem.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"

namespace
{
	void* GameSystem_InitializeApplication_addr = nullptr;
	void* GameSystem_InitializeApplication_orig = nullptr;

	void* GameSystem_InitializeGame_addr = nullptr;
	void* GameSystem_InitializeGame1_addr = nullptr;

	void* GameSystem_IsExecutingSoftwareReset_addr = nullptr;

	void* GameSystem_SoftwareReset_addr = nullptr;

	FieldInfo* GameSystem__systemStateField = nullptr;
}

static void GameSystem_InitializeApplication_hook()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
		auto persistentDataPathField = il2cpp_class_get_field_from_name(CuteCoreDevice, "persistentDataPath");

		il2cpp_field_static_set_value(persistentDataPathField, UnityEngine::Application::persistentDataPath());
	}

	if (!config::persistent_data_path.empty())
	{
		auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
		auto persistentDataPathField = il2cpp_class_get_field_from_name(CuteCoreDevice, "persistentDataPath");
		if (filesystem::path(config::persistent_data_path.data()).is_relative())
		{
			config::persistent_data_path.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
		}
		if (filesystem::exists(config::persistent_data_path))
		{
			if (filesystem::is_directory(config::persistent_data_path))
			{
				il2cpp_field_static_set_value(persistentDataPathField, il2cpp_string_new16(config::persistent_data_path.data()));
			}
		}
		else if (filesystem::create_directories(config::persistent_data_path))
		{
			il2cpp_field_static_set_value(persistentDataPathField, il2cpp_string_new16(config::persistent_data_path.data()));
		}
	}

	reinterpret_cast<decltype(GameSystem_InitializeApplication_hook)*>(GameSystem_InitializeApplication_orig)();

	il2cpp_symbols::call_late_init_callbacks();
}

static void InitAddress()
{
	auto GameSystem_klas = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "GameSystem");
	GameSystem_InitializeApplication_addr = il2cpp_symbols::get_method_pointer(GameSystem_klas, "InitializeApplication", 0);
	GameSystem_InitializeGame_addr = il2cpp_symbols::get_method_pointer(GameSystem_klas, "InitializeGame", 1);
	GameSystem_InitializeGame1_addr = il2cpp_symbols::get_method_pointer(GameSystem_klas, "InitializeGame", 0);
	GameSystem_IsExecutingSoftwareReset_addr = il2cpp_symbols::get_method_pointer(GameSystem_klas, "IsExecutingSoftwareReset", 0);
	GameSystem_SoftwareReset_addr = il2cpp_symbols::get_method_pointer(GameSystem_klas, "SoftwareReset", 0);
	GameSystem__systemStateField = il2cpp_class_get_field_from_name(GameSystem_klas, "_systemState");
}

static void HookMethods()
{
	ADD_HOOK(GameSystem_InitializeApplication, "Gallop.GameSystem::InitializeApplication at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* GameSystem::namespaze = "Gallop";
	const char* GameSystem::klassName = "GameSystem";

	Il2CppObject* GameSystem::InitializeGame(Il2CppDelegate* onCompleteInitializeUI)
	{
		if (!GameSystem_InitializeGame_addr)
		{
			return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(GameSystem_InitializeGame1_addr)(instance);
		}
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppDelegate*)>(GameSystem_InitializeGame_addr)(instance, onCompleteInitializeUI);
	}

	bool GameSystem::IsExecutingSoftwareReset()
	{
		return reinterpret_cast<bool (*)()>(GameSystem_IsExecutingSoftwareReset_addr)();
	}

	void GameSystem::SoftwareReset()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(GameSystem_SoftwareReset_addr)(instance);
	}

	GameSystem::SystemState GameSystem::_systemState()
	{
		GameSystem::SystemState _systemState;
		il2cpp_field_get_value(instance, GameSystem__systemStateField, &_systemState);

		return _systemState;
	}
}
