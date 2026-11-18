#include "../../CriMw.CriWare.Runtime.hpp"
#include "../../../ScriptInternal.hpp"
#include "Player.hpp"

#include <unordered_map>

#include "scripts/UnityEngine.CoreModule/UnityEngine/Time.hpp"

#include "scripts/umamusume/Gallop/GameSystem.hpp"

#include "config/config.hpp"

#include "string_utils.hpp"

namespace
{
	void* Player_SetFile_addr = nullptr;
	void* Player_SetFile_orig = nullptr;

	void* Player_EntryFile_addr = nullptr;
	void* Player_EntryFile_orig = nullptr;

	Il2CppMethodPointer Player_Prepare_addr = nullptr;
	void* Player_Prepare_orig = nullptr;

	Il2CppMethodPointer Player_Dispose_addr = nullptr;
	void* Player_Dispose_orig = nullptr;

	Il2CppMethodPointer Player_SetMasterTimerType_addr = nullptr;

	Il2CppMethodPointer Player_SetManualTimerUnit_addr = nullptr;

	Il2CppMethodPointer Player_UpdateWithManualTimeAdvanced_addr = nullptr;

	FieldInfo* Player_playerIdField = nullptr;

	unordered_map<int, Il2CppObject*> players;

	float _manualTimer = 0.f;
	const float MANUAL_INTERVAL = 1.f / 60.f;
}

static void Player_SetFile_hook(int player_id, void* binder, const char* path)
{
	auto fileName = GetFileName(path);

	if (config::replace_assets.find(fileName) != config::replace_assets.end())
	{
		auto& replaceAsset = config::replace_assets.at(fileName);
		reinterpret_cast<decltype(Player_SetFile_hook)*>(Player_SetFile_orig)(player_id, binder, il2cpp_u8(replaceAsset.path).data());
		return;
	}

	reinterpret_cast<decltype(Player_SetFile_hook)*>(Player_SetFile_orig)(player_id, binder, path);
}

static bool Player_EntryFile_hook(int player_id, void* binder, const char* path, bool repeat)
{
	auto fileName = GetFileName(path);

	if (config::replace_assets.find(fileName) != config::replace_assets.end())
	{
		auto& replaceAsset = config::replace_assets.at(fileName);
		return reinterpret_cast<decltype(Player_EntryFile_hook)*>(Player_EntryFile_orig)(player_id, binder, il2cpp_u8(replaceAsset.path).data(), repeat);
	}

	return reinterpret_cast<decltype(Player_EntryFile_hook)*>(Player_EntryFile_orig)(player_id, binder, path, repeat);
}

static void Player_Prepare_hook(Il2CppObject* self)
{
	int id;
	il2cpp_field_get_value(self, Player_playerIdField, &id);
	players.emplace(id, self);
	reinterpret_cast<void (*)(Il2CppObject*, int)>(Player_SetMasterTimerType_addr)(self, 4);
	reinterpret_cast<void (*)(Il2CppObject*, unsigned long, unsigned long)>(Player_SetManualTimerUnit_addr)(self, 1, 60);

	reinterpret_cast<decltype(Player_Prepare_hook)*>(Player_Prepare_orig)(self);
}

static void Player_Dispose_hook(Il2CppObject* self, bool disposing)
{
	int id;
	il2cpp_field_get_value(self, Player_playerIdField, &id);
	if (players.contains(id))
	{
		players.erase(id);
	}
	reinterpret_cast<decltype(Player_Dispose_hook)*>(Player_Dispose_orig)(self, disposing);
}

static void InitAddress()
{
	auto Player_class = il2cpp_symbols::get_class("CriMw.CriWare.Runtime.dll", "CriWare.CriMana", "Player");

	Player_Prepare_addr = il2cpp_symbols::get_method_pointer(Player_class, "Prepare", 0);
	Player_Dispose_addr = il2cpp_symbols::get_method_pointer(Player_class, "Dispose", 1);
	Player_SetMasterTimerType_addr = il2cpp_symbols::get_method_pointer(Player_class, "SetMasterTimerType", 1);
	Player_SetManualTimerUnit_addr = il2cpp_symbols::get_method_pointer(Player_class, "SetManualTimerUnit", 2);
	Player_UpdateWithManualTimeAdvanced_addr = il2cpp_symbols::get_method_pointer(Player_class, "UpdateWithManualTimeAdvanced", 0);

	Player_playerIdField = il2cpp_class_get_field_from_name(Player_class, "playerId");

#ifdef _MSC_VER
	wstring module_name;
	module_name.resize(MAX_PATH);
	module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

	filesystem::path module_path(module_name);

	wstring name = module_path.filename().replace_extension();

	SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());

	auto criware = LoadLibraryExW(L"cri_ware_unity.dll", nullptr, 0);

	SetDllDirectoryW(nullptr);

	Player_SetFile_addr = GetProcAddress(criware, "CRIWARE310ABCEC");
	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = GetProcAddress(criware, "CRIWAREDA7020AB");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = GetProcAddress(criware, "CRIWARE310ABCEC");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = GetProcAddress(criware, "CRIWARE6A5D4549");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = GetProcAddress(criware, "CRIWAREDF8F52BA");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = GetProcAddress(criware, "CRIWARE2896606F");
	}

	Player_EntryFile_addr = GetProcAddress(criware, "CRIWAREB83D23AD");
	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = GetProcAddress(criware, "CRIWAREE7861E0D");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = GetProcAddress(criware, "CRIWARE925FC233");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = GetProcAddress(criware, "CRIWARE6F49ABEA");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = GetProcAddress(criware, "CRIWARE063F9CD6");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = GetProcAddress(criware, "CRIWARE6DFCA962");
	}
#else
	auto criware = dlopen("libcri_ware_unity.so", RTLD_NOW);

	Player_SetFile_addr = dlsym(criware, "CRIWARE310ABCEC");
	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = dlsym(criware, "CRIWAREDA7020AB");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = dlsym(criware, "CRIWARE310ABCEC");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = dlsym(criware, "CRIWARE6A5D4549");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = dlsym(criware, "CRIWAREDF8F52BA");
	}

	if (!Player_SetFile_addr)
	{
		Player_SetFile_addr = dlsym(criware, "CRIWARE2896606F");
	}

	Player_EntryFile_addr = dlsym(criware, "CRIWAREB83D23AD");
	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = dlsym(criware, "CRIWAREE7861E0D");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = dlsym(criware, "CRIWARE925FC233");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = dlsym(criware, "CRIWARE6F49ABEA");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = dlsym(criware, "CRIWARE063F9CD6");
	}

	if (!Player_EntryFile_addr)
	{
		Player_EntryFile_addr = dlsym(criware, "CRIWARE6DFCA962");
	}
#endif
}

static void HookMethods()
{
	if (!config::replace_assets.empty())
	{
		ADD_HOOK(Player_SetFile, "CriWare.CriMana.Player::SetFile at %p\n");
		ADD_HOOK(Player_EntryFile, "CriWare.CriMana.Player::EntryFile at %p\n");
	}

	ADD_HOOK(Player_Prepare, "CriWare.CriMana.Player::Prepare at %p\n");
	ADD_HOOK(Player_Dispose, "CriWare.CriMana.Player::Dispose at %p\n");
}

static void LateInit()
{
	auto GameSystem = Gallop::GameSystem::Instance();
	if (GameSystem)
	{
		auto Func = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "Func`1"),
			GetRuntimeType(il2cpp_defaults.boolean_class));
		auto predicate = CreateDelegateWithClass(Func, GameSystem,
			*[](Il2CppObject* gameSystem)
			{
				_manualTimer += UnityEngine::Time::deltaTime();

				while (_manualTimer >= MANUAL_INTERVAL)
				{
					_manualTimer -= MANUAL_INTERVAL;
					for (const auto& pair : players)
					{
						reinterpret_cast<void (*)(Il2CppObject*)>(Player_UpdateWithManualTimeAdvanced_addr)(pair.second);
					}
				}
				return false;
			}
		);

		auto WaitUntil = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitUntil"));
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(WaitUntil->klass, ".ctor", 1)(WaitUntil, &predicate->delegate);
		GameSystem.StartCoroutineManaged2(WaitUntil);
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
	il2cpp_symbols::late_init_callbacks.emplace_back(LateInit);
}
