#include "../../CriMw.CriWare.Runtime.hpp"
#include "../../../ScriptInternal.hpp"
#include "Player.hpp"

#include "config/config.hpp"

#include "string_utils.hpp"

namespace
{
	void* Player_SetFile_addr = nullptr;
	void* Player_SetFile_orig = nullptr;

	void* Player_EntryFile_addr = nullptr;
	void* Player_EntryFile_orig = nullptr;
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

static void InitAddress()
{
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
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
