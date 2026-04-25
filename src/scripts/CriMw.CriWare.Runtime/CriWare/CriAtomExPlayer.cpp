#include "../CriMw.CriWare.Runtime.hpp"
#include "../../ScriptInternal.hpp"
#include "CriAtomExPlayer.hpp"

#include "scripts/umamusume/Gallop/GallopUtil.hpp"

#include "localify/NotificationManager.hpp"

#include "config/config.hpp"

#include "string_utils.hpp"

namespace
{
	void* criAtomExPlayer_SetCueId_addr = nullptr;
	void* criAtomExPlayer_SetCueId_orig = nullptr;

	void* criAtomExPlayer_Start_addr = nullptr;
	void* criAtomExPlayer_Start_orig = nullptr;

	void* criAtomExPlayer_Stop_addr = nullptr;
	void* criAtomExPlayer_Stop_orig = nullptr;

	void* criAtomExPlayer_StopWithoutReleaseTime_addr = nullptr;
	void* criAtomExPlayer_StopWithoutReleaseTime_orig = nullptr;

	void* criAtomExPlayer_Pause_addr = nullptr;
	void* criAtomExPlayer_Pause_orig = nullptr;
}

static void ShowCaptionByNotification(Il2CppObject* audioManager, Il2CppObject* elem)
{
	if (!audioManager || !elem)
	{
		return;
	}

	auto characterIdField = il2cpp_class_get_field_from_name(elem->klass, "CharacterId");
	auto voiceIdField = il2cpp_class_get_field_from_name(elem->klass, "VoiceId");
	auto textField = il2cpp_class_get_field_from_name(elem->klass, "Text");
	auto cueSheetField = il2cpp_class_get_field_from_name(elem->klass, "CueSheet");
	auto cueIdField = il2cpp_class_get_field_from_name(elem->klass, "CueId");

	if (!characterIdField ||
		!voiceIdField ||
		!textField ||
		!cueSheetField ||
		!cueIdField)
	{
		return;
	}

	int characterId;
	il2cpp_field_get_value(elem, characterIdField, &characterId);

	int voiceId;
	il2cpp_field_get_value(elem, voiceIdField, &voiceId);

	Il2CppString* text;
	il2cpp_field_get_value(elem, textField, &text);

	Il2CppString* cueSheet;
	il2cpp_field_get_value(elem, cueSheetField, &cueSheet);

	int cueId;
	il2cpp_field_get_value(elem, cueIdField, &cueId);

	if (!cueSheet || !text)
	{
		return;
	}

	auto u16Text = il2cppstring(text->chars);
	replaceAll(u16Text, IL2CPP_STRING("\n\n"), IL2CPP_STRING(" "));
	replaceAll(u16Text, IL2CPP_STRING("\n"), IL2CPP_STRING(" "));
	if (il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_home_")) == il2cppstring::npos &&
		il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_tc_")) == il2cppstring::npos &&
		il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_title_")) == il2cppstring::npos &&
		il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_kakao_")) == il2cppstring::npos &&
		il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_gacha_")) == il2cppstring::npos &&
		voiceId != 95001 &&
		(characterId < 9000 || voiceId == 95005 || voiceId == 95006 || voiceId == 70000))
	{

		if (il2cppstring(cueSheet->chars).find(IL2CPP_STRING("_training_")) != il2cppstring::npos && (cueId < 29 || cueId == 39))
		{
			if (!(voiceId >= 2030 && voiceId <= 2037) && voiceId < 93000 && !(cueId == 8 || cueId == 9 || cueId == 12 || cueId == 13))
			{
				if (voiceId == 20025)
				{
					auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));

					if (sceneManager)
					{
						auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);

						if (viewId != 5901)
						{
							return;
						}
					}
					else
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
		}

		float length =
			il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, Il2CppString*, int)>(audioManager->klass, "GetCueLength", 2)->methodPointer(audioManager, cueSheet, cueId);

		Localify::NotificationManager::SetDisplayTime(length);

		Localify::NotificationManager::Show(Gallop::GallopUtil::LineHeadWrap(il2cpp_string_new16(u16Text.data()), config::character_system_text_caption_line_char_count));
	}
}

static void criAtomExPlayer_SetCueId_hook(void* player, void* acb_hn, int id)
{
	if (Localify::NotificationManager::currentAcbHandles.contains(acb_hn))
	{
		Localify::NotificationManager::currentPlayerHandles.insert(player);
	}
	reinterpret_cast<decltype(criAtomExPlayer_SetCueId_hook)*>(criAtomExPlayer_SetCueId_orig)(player, acb_hn, id);
}

static uint32_t criAtomExPlayer_Start_hook(void* player)
{
	if (Localify::NotificationManager::currentPlayerHandles.contains(player) && Localify::NotificationManager::currentElem)
	{
		Localify::NotificationManager::currentPlayerHandle = player;

		auto audioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));

		ShowCaptionByNotification(audioManager, Localify::NotificationManager::currentElem);
	}
	return reinterpret_cast<decltype(criAtomExPlayer_Start_hook)*>(criAtomExPlayer_Start_orig)(player);
}

static void criAtomExPlayer_Stop_hook(void* player)
{
	reinterpret_cast<decltype(criAtomExPlayer_Stop_hook)*>(criAtomExPlayer_Stop_orig)(player);
	if (Localify::NotificationManager::currentPlayerHandles.contains(player) && Localify::NotificationManager::currentPlayerHandle == player)
	{
		Localify::NotificationManager::currentElem = nullptr;
		Localify::NotificationManager::Cleanup();
	}
}

static void criAtomExPlayer_StopWithoutReleaseTime_hook(void* player)
{
	reinterpret_cast<decltype(criAtomExPlayer_StopWithoutReleaseTime_hook)*>(criAtomExPlayer_StopWithoutReleaseTime_orig)(player);
	if (Localify::NotificationManager::currentPlayerHandles.contains(player) && Localify::NotificationManager::currentPlayerHandle == player)
	{
		Localify::NotificationManager::currentElem = nullptr;
		Localify::NotificationManager::Cleanup();
	}
}

static void criAtomExPlayer_Pause_hook(void* player, bool sw)
{
	reinterpret_cast<decltype(criAtomExPlayer_Pause_hook)*>(criAtomExPlayer_Pause_orig)(player, sw);
	if (!sw && Localify::NotificationManager::currentPlayerHandles.contains(player) && Localify::NotificationManager::currentPlayerHandle == player)
	{
		Localify::NotificationManager::currentElem = nullptr;
		Localify::NotificationManager::Cleanup();
	}
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

	criAtomExPlayer_SetCueId_addr = GetProcAddress(criware, "criAtomExPlayer_SetCueId");
	criAtomExPlayer_Start_addr = GetProcAddress(criware, "criAtomExPlayer_Start");
	criAtomExPlayer_Stop_addr = GetProcAddress(criware, "criAtomExPlayer_Stop");
	criAtomExPlayer_StopWithoutReleaseTime_addr = GetProcAddress(criware, "criAtomExPlayer_StopWithoutReleaseTime");
	criAtomExPlayer_Pause_addr = GetProcAddress(criware, "criAtomExPlayer_Pause");
#else
	auto criware = dlopen("libcri_ware_unity.so", RTLD_NOW);

	criAtomExPlayer_SetCueId_addr = dlsym(criware, "criAtomExPlayer_SetCueId");
	criAtomExPlayer_Start_addr = dlsym(criware, "criAtomExPlayer_Start");
	criAtomExPlayer_Stop_addr = dlsym(criware, "criAtomExPlayer_Stop");
	criAtomExPlayer_StopWithoutReleaseTime_addr = dlsym(criware, "criAtomExPlayer_StopWithoutReleaseTime");
	criAtomExPlayer_Pause_addr = dlsym(criware, "criAtomExPlayer_Pause");
#endif
}

static void HookMethods()
{
	if (config::character_system_text_caption)
	{
		ADD_HOOK(criAtomExPlayer_SetCueId, "CriWare.CriAtomExPlayer::criAtomExPlayer_SetCueId");
		ADD_HOOK(criAtomExPlayer_Start, "CriWare.CriAtomExPlayer::criAtomExPlayer_Start");
		ADD_HOOK(criAtomExPlayer_Stop, "CriWare.CriAtomExPlayer::criAtomExPlayer_Stop");
		ADD_HOOK(criAtomExPlayer_StopWithoutReleaseTime, "CriWare.CriAtomExPlayer::criAtomExPlayer_StopWithoutReleaseTime");
		ADD_HOOK(criAtomExPlayer_Pause, "CriWare.CriAtomExPlayer::criAtomExPlayer_Pause");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
