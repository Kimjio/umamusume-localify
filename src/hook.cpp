#include "hook.h"

#include <stdinclude.hpp>

#include <Shlobj.h>
#include <ShObjIdl.h>

#include <array>

#include <algorithm>

#include <chrono>

#include <set>
#include <sstream>

#include <Tlhelp32.h>

#include <regex>

#include <psapi.h>
#include <wininet.h>

#include <wrl.h>
#include <wil/com.h>

#include <propkey.h>
#include <propvarutil.h>

#include <fstream>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <windows.ui.notifications.h>
#include <winrt/Windows.Foundation.h>

#include "MINT.h"

#include "config/config.hpp"

#include "notifier/notifier.hpp"

#include "rich_presence.hpp"

#include "il2cpp/il2cpp-tabledefs.h"

#include "il2cpp_dump.h"

#include "discord/core.h"
#include "discord/activity_manager.h"

#include "libcef.h"

#include "settings_text.hpp"

#include "unity/IUnityInterface.h"

#include "openxr/openxr.hpp"

#include "msgpack/msgpack_modify.hpp"
#include "masterdb/masterdb.hpp"

#include "notification/DesktopNotificationManagerCompat.h"

#include "smtc/SystemMediaTransportControlsManager.hpp"

#include "taskbar/TaskbarManager.hpp"

#include "localify/SettingsUI.hpp"
#include "localify/NotificationManager.hpp"

#include "scripts/ScriptInternal.hpp"

#include "scripts/mscorlib/System/Boolean.hpp"
#include "scripts/mscorlib/System/Enum.hpp"
#include "scripts/mscorlib/System/Int32.hpp"
#include "scripts/mscorlib/System/Nullable.hpp"
#include "scripts/mscorlib/System/ValueTuple.hpp"
#include "scripts/mscorlib/System/Collections/Generic/Dictionary.hpp"
#include "scripts/mscorlib/System/Collections/Generic/List.hpp"

#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomEx.hpp"
#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomExPlayback.hpp"
#include "scripts/CriMw.CriWare.Runtime/CriWare/CriMana/MovieInfo.hpp"
#include "scripts/CriMw.CriWare.Runtime/CriWare/CriMana/FrameInfo.hpp"

#include "scripts/Cute.Cri.Assembly/Cute/Cri/AudioPlayback.hpp"
#include "scripts/Cute.Cri.Assembly/Cute/Cri/MoviePlayerHandle.hpp"

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/CastHelper.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Shader.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Material.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector4.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2Int.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Resolution.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ResourcesAPIInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ScreenOrientation.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/SceneManagement/Scene.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rendering/RenderTargetIdentifier.hpp"
#include "scripts/UnityEngine.TextRenderingModule/UnityEngine/TextGenerationSettings.hpp"
#include "scripts/UnityEngine.InputLegacyModule/UnityEngine/Input.hpp"

#include "scripts/umamusume/Gallop/DialogCommonBase.hpp"
#include "scripts/umamusume/Gallop/DialogCommon.hpp"
#include "scripts/umamusume/Gallop/DialogManager.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"
#include "scripts/umamusume/Gallop/LiveViewController.hpp"
#include "scripts/umamusume/Gallop/Localize.hpp"
#include "scripts/umamusume/Gallop/LowResolutionCameraUtil.hpp"
#include "scripts/umamusume/Gallop/RaceCameraManager.hpp"
#include "scripts/umamusume/Gallop/WebViewManager.hpp"
#include "scripts/umamusume/Gallop/TitleViewController.hpp"
#include "scripts/umamusume/Gallop/TextFontManager.hpp"
#include "scripts/umamusume/Gallop/Screen.hpp"
#include "scripts/umamusume/Gallop/GameSystem.hpp"
#ifdef _MSC_VER
#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"
#endif

#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredBool.hpp"
#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredInt.hpp"
#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredLong.hpp"

#include "fpp/fpp.h"

#include "string_utils.hpp"

using namespace std;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace ABI::Windows::Data::Xml::Dom;

namespace
{
	il2cppstring GotoTitleError =
		IL2CPP_STRING("내부적으로 오류가 발생하여 홈으로 이동합니다.\n\n"
			"경우에 따라서 <color=#ff911c><i>타이틀</i></color>로 돌아가거나, \n"
			"게임 <color=#ff911c><i>다시 시작</i></color>이 필요할 수 있습니다.");

	il2cppstring GotoTitleErrorJa =
		IL2CPP_STRING("内部的にエラーが発生し、ホームに移動します。\n\n"
			"場合によっては、<color=#ff911c><i>タイトル</i></color>に戻るか、\n"
			"ゲーム<color=#ff911c><i>再起動</i></color>が必要になる場合がありますあります。");

	void patch_game_assembly();

	void patch_after_criware();

	bool mh_inited = false;

	string module_name;

	void dump_bytes(void* pos)
	{
		if (pos)
		{
			printf("Hex dump of %p\n", pos);

			char* memory = reinterpret_cast<char*>(pos);

			for (int i = 0; i < 0x20; i++)
			{
				if (i > 0 && i % 16 == 0)
					printf("\n");

				char byte = *(memory++);

				printf("%02hhX ", byte);
			}

		}
		printf("\n\n");
	}

	void DecryptManifestDB()
	{
		auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
		Il2CppString* persistentDataPath = il2cpp_class_get_method_from_name_type<Il2CppString * (*)()>(CuteCoreDevice, "GetPersistentDataPath", 0)->methodPointer();

		il2cppstring path = persistentDataPath->chars + il2cppstring(IL2CPP_STRING("/meta"));

		if (!filesystem::exists(path))
		{
			return;
		}

		ifstream file{ filesystem::path(path).string(), ios::binary };
		string magic = string(16, '\0');
		file.read(magic.data(), 16);
		file.close();

		if (!magic.starts_with("SQLite format 3"))
		{
			sqlite3* db;
			sqlite3_open(il2cpp_u8(path).data(), &db);

			sqlite3mc_config(db, "cipher", CODEC_TYPE_CHACHA20);

			vector<char> key;

			if (Game::CurrentGameRegion == Game::Region::JPN)
			{
				key = { 0x6D, 0x5B, 0x65, 0x33, 0x63, 0x36, 0x63, 0x25, 0x54, 0x71, 0x2D, 0x73, 0x50, 0x53, 0x63, 0x38, 0x6D, 0x34, 0x37, 0x7B, 0x35, 0x63, 0x70, 0x23, 0x37, 0x34, 0x53, 0x29, 0x73, 0x43, 0x36, 0x33 };
			}
			else if (Game::CurrentGameRegion == Game::Region::ENG)
			{
				key = { 0x56, 0x63, 0x6B, 0x63, 0x42, 0x72, 0x37, 0x76, 0x65, 0x70, 0x41, 0x62 };
			}

			if (!key.empty())
			{
				sqlite3_key(db, key.data(), key.size());
				sqlite3_rekey(db, nullptr, 0);
			}

			sqlite3_close(db);
		}
	}

	void* InitializeApplication_orig = nullptr;
	void InitializeApplication_hook()
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

		DecryptManifestDB();

		patch_game_assembly();

		reinterpret_cast<decltype(InitializeApplication_hook)*>(InitializeApplication_orig)();
	}

	void* UpdateDispatcher_Initialize_orig = nullptr;
	void UpdateDispatcher_Initialize_hook()
	{
		auto klass = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "UpdateDispatcher");
		auto isQuitField = il2cpp_class_get_field_from_name(klass, "isQuit");
		bool isQuit = false;

		il2cpp_field_static_set_value(isQuitField, &isQuit);
	}

	void* SetResolution_Injected_orig;
	void SetResolution_Injected_hook(int width, int height, int fullscreenMode, UnityEngine::RefreshRate* perferredRefreshRate);

	void init_il2cpp()
	{
		if (config::dump_il2cpp)
		{
			il2cpp_dump();
		}

		il2cpp_symbols::init_defaults();
		il2cpp_symbols::call_init_callbacks();

		auto InitializeApplication = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GameSystem", "InitializeApplication", 0);
		MH_CreateHook(InitializeApplication, InitializeApplication_hook, &InitializeApplication_orig);
		MH_EnableHook(InitializeApplication);

		auto UpdateDispatcher_Initialize_addr = il2cpp_symbols::get_method_pointer("Cute.Core.Assembly.dll", "Cute.Core", "UpdateDispatcher", "Initialize", 0);
		MH_CreateHook(UpdateDispatcher_Initialize_addr, UpdateDispatcher_Initialize_hook, &UpdateDispatcher_Initialize_orig);
		MH_EnableHook(UpdateDispatcher_Initialize_addr);

		il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.SubsystemsModule.dll", "UnityEngine", "SubsystemManager"));
		il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.SubsystemsModule.dll", "UnityEngine.SubsystemsImplementation", "SubsystemDescriptorStore"));
		//il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "BeforeRenderHelper"));

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			KillProcessByName(L"ucldr_Umamusume_KR_loader_x64.exe");
		}

		init_sqlite3();
	}

	void* il2cpp_init_orig = nullptr;
	bool __stdcall il2cpp_init_hook(const char* domain_name)
	{
		const auto result = reinterpret_cast<decltype(il2cpp_init_hook)*>(il2cpp_init_orig)(domain_name);
		il2cpp_symbols::il2cpp_domain = il2cpp_domain_get();
		init_il2cpp();

		return result;
	}

	void* umaFindService_orig = nullptr;
	BOOL __stdcall umaFindService_hook()
	{
		return TRUE;
	}

	set<void*> currentPlayerHandles;
	set<void*> currentAcbHandles;

	void* currentPlayerHandle;

	Il2CppObject* currentElem;



	void ShowCaptionByNotification(Il2CppObject* audioManager, Il2CppObject* elem)
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

			auto LineHeadWrap =
				il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, int)>("umamusume.dll", "Gallop", "GallopUtil",
					"LineHeadWrap", 2);

			float length =
				il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, Il2CppString*, int)>(audioManager->klass, "GetCueLength",
					2)->methodPointer(audioManager, cueSheet, cueId);

			Localify::NotificationManager::SetDisplayTime(length);

			Localify::NotificationManager::Show(LineHeadWrap(il2cpp_string_new16(u16Text.data()), config::character_system_text_caption_line_char_count));
		}
	}

	void* criAtomExPlayer_SetCueId_orig = nullptr;
	void criAtomExPlayer_SetCueId_hook(void* player, void* acb_hn, int id)
	{
		if (currentAcbHandles.contains(acb_hn))
		{
			currentPlayerHandles.insert(player);
		}
		reinterpret_cast<decltype(criAtomExPlayer_SetCueId_hook)*>(criAtomExPlayer_SetCueId_orig)(player, acb_hn, id);
	}

	void* criAtomExPlayer_Start_orig = nullptr;
	UINT criAtomExPlayer_Start_hook(void* player)
	{
		if (currentPlayerHandles.contains(player) && currentElem)
		{
			currentPlayerHandle = player;

			auto audioManager = GetSingletonInstance(
				il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));

			ShowCaptionByNotification(audioManager, currentElem);
		}
		return reinterpret_cast<decltype(criAtomExPlayer_Start_hook)*>(criAtomExPlayer_Start_orig)(player);
	}

	void* criAtomExAcb_GetCueInfoById_orig = nullptr;
	bool criAtomExAcb_GetCueInfoById_hook(void* acb_hn, int id, CriWare::CriAtomEx::CueInfo* info)
	{
		auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoById_hook)*>(criAtomExAcb_GetCueInfoById_orig)(acb_hn, id, info);

		if (!info || !info->name)
		{
			return result;
		}
		const regex r(R"(_(?:9)*(\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
		smatch stringMatch;
		const auto cueSheet = string(info->name);
		const auto cueSheet16 = u8_il2cpp(cueSheet);
		regex_search(cueSheet, stringMatch, r);
		if (!stringMatch.empty())
		{
			auto charaIdStr = stringMatch[1].str();
			if (!stringMatch[2].str().empty() && !stringMatch[3].str().empty())
			{
				auto index = stoi(stringMatch[3].str());

				if (index == 1)
				{
					charaIdStr = stringMatch[2].str();
				}
			}

			Il2CppObject* textList =
				il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int)>("umamusume.dll", "Gallop",
					"MasterCharacterSystemText", "GetByCharaId", 1)(stoi(charaIdStr));
			if (textList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name(textList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* textArr;
				il2cpp_field_get_value(textList, itemsField, &textArr);

				if (textArr)
				{
					for (int i = 0; i < textArr->max_length; i++)
					{
						auto elem = textArr->vector[i];
						if (elem)
						{
							auto elemCueIdField = il2cpp_class_get_field_from_name(elem->klass, "CueId");
							auto elemCueSheetField = il2cpp_class_get_field_from_name(elem->klass, "CueSheet");

							Il2CppString* elemCueSheet;
							il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

							int elemCueId;
							il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

							if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->chars) && info->id == elemCueId) {
								currentAcbHandles.insert(acb_hn);
								currentElem = elem;
								break;
							}
						}
					}
				}
			}
		}

		return result;
	}

	void* criAtomExAcb_GetCueInfoByName_orig = nullptr;
	bool criAtomExAcb_GetCueInfoByName_hook(void* acb_hn, char* name, CriWare::CriAtomEx::CueInfo* info)
	{
		auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoByName_hook)*>(criAtomExAcb_GetCueInfoByName_orig)(acb_hn, name, info);

		if (!info || !info->name)
		{
			return result;
		}

		const regex r(R"((\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
		smatch stringMatch;
		const auto cueSheet = string(info->name);
		const auto cueSheet16 = u8_il2cpp(cueSheet);
		regex_search(cueSheet, stringMatch, r);
		if (!stringMatch.empty())
		{
			auto charaIdStr = stringMatch[1].str();
			if (!stringMatch[2].str().empty() && !stringMatch[3].str().empty())
			{
				auto index = stoi(stringMatch[3].str());

				if (index == 1)
				{
					charaIdStr = stringMatch[2].str();
				}
			}

			Il2CppObject* textList =
				il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int)>("umamusume.dll", "Gallop",
					"MasterCharacterSystemText", "GetByCharaId", 1)(stoi(charaIdStr));
			if (textList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name(textList->klass, "_items");
				Il2CppArraySize* textArr;
				il2cpp_field_get_value(textList, itemsField, &textArr);


				if (textArr)
				{
					for (int i = 0; i < textArr->max_length; i++)
					{
						auto elem = reinterpret_cast<Il2CppObject*>(textArr->vector[i]);
						if (elem)
						{
							auto elemCueIdField = il2cpp_class_get_field_from_name(elem->klass, "CueId");
							auto elemCueSheetField = il2cpp_class_get_field_from_name(elem->klass, "CueSheet");

							Il2CppString* elemCueSheet;
							il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

							int elemCueId;
							il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

							if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->chars) && info->id == elemCueId)
							{
								currentAcbHandles.insert(acb_hn);
								currentElem = elem;
								break;
							}
						}
					}
				}
			}
		}

		return result;
	}

	void* criAtomExAcb_Release_orig = nullptr;
	void criAtomExAcb_Release_hook(void* acb_hn)
	{
		if (currentAcbHandles.contains(acb_hn))
		{
			currentAcbHandles.erase(acb_hn);
		}
		reinterpret_cast<decltype(criAtomExAcb_Release_hook)*>(criAtomExAcb_Release_orig)(acb_hn);
	}

	void* criAtomExPlayer_Stop_orig = nullptr;
	void criAtomExPlayer_Stop_hook(void* player)
	{
		reinterpret_cast<decltype(criAtomExPlayer_Stop_hook)*>(criAtomExPlayer_Stop_orig)(player);
		if (currentPlayerHandles.contains(player) && currentPlayerHandle == player)
		{
			currentElem = nullptr;

			Localify::NotificationManager::Cleanup();
		}
	}

	void* criAtomExPlayer_StopWithoutReleaseTime_orig = nullptr;
	void criAtomExPlayer_StopWithoutReleaseTime_hook(void* player)
	{
		reinterpret_cast<decltype(criAtomExPlayer_StopWithoutReleaseTime_hook)*>(criAtomExPlayer_StopWithoutReleaseTime_orig)(player);
		if (currentPlayerHandles.contains(player) && currentPlayerHandle == player)
		{
			currentElem = nullptr;

			Localify::NotificationManager::Cleanup();
		}
	}

	void* criAtomExPlayer_Pause_orig = nullptr;
	void criAtomExPlayer_Pause_hook(void* player, bool sw)
	{
		reinterpret_cast<decltype(criAtomExPlayer_Pause_hook)*>(criAtomExPlayer_Pause_orig)(player, sw);
		if (!sw && currentPlayerHandles.contains(player) && currentPlayerHandle == player)
		{
			currentElem = nullptr;

			Localify::NotificationManager::Cleanup();
		}
	}

	void* CriMana_SetFileNew_orig = nullptr;
	void CriMana_SetFileNew_hook(int player_id, void* binder, const char* path)
	{
		auto fileName = GetFileName(path);

		if (config::replace_assets.find(fileName) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(fileName);
			reinterpret_cast<decltype(CriMana_SetFileNew_hook)*>(CriMana_SetFileNew_orig)(player_id, binder, il2cpp_u8(replaceAsset.path).data());
			return;
		}

		reinterpret_cast<decltype(CriMana_SetFileNew_hook)*>(CriMana_SetFileNew_orig)(player_id, binder, path);
	}

	void* CriMana_SetFileAppend_orig = nullptr;
	bool CriMana_SetFileAppend_hook(int player_id, void* binder, const char* path, bool repeat)
	{
		auto fileName = GetFileName(path);

		if (config::replace_assets.find(fileName) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(fileName);
			return reinterpret_cast<decltype(CriMana_SetFileAppend_hook)*>(CriMana_SetFileAppend_orig)(player_id, binder, il2cpp_u8(replaceAsset.path).data(), repeat);
		}

		return reinterpret_cast<decltype(CriMana_SetFileAppend_hook)*>(CriMana_SetFileAppend_orig)(player_id, binder, path, repeat);
	}

	void* delete_cookies_orig = nullptr;
	int delete_cookies_hook(
		struct _cef_cookie_manager_t* self,
		const cef_string_t* url,
		const cef_string_t* cookie_name,
		struct _cef_delete_cookies_callback_t* callback)
	{
		// return reinterpret_cast<decltype(delete_cookies_hook)*>(delete_cookies_orig)(self, url, cookie_name, callback);
		return TRUE;
	}

	cef_cookie_manager_t* cefCookieManager;

	void* cef_cookie_manager_get_global_manager_orig = nullptr;
	cef_cookie_manager_t* cef_cookie_manager_get_global_manager_hook(
		struct _cef_completion_callback_t* callback)
	{
		auto manager = reinterpret_cast<decltype(cef_cookie_manager_get_global_manager_hook)*>(cef_cookie_manager_get_global_manager_orig)(callback);

		if (manager)
		{
			cefCookieManager = manager;

			auto addr = manager->delete_cookies;

			MH_CreateHook(addr, delete_cookies_hook, &delete_cookies_orig);
			MH_EnableHook(addr);
		}

		return manager;
	}

	void* load_library_ex_w_orig = nullptr;
	HMODULE
		WINAPI
		load_library_ex_w_hook(
			_In_ LPCWSTR lpLibFileName,
			_Reserved_ HANDLE hFile,
			_In_ DWORD dwFlags
		)
	{
		if (!config::allow_delete_cookie && Game::CurrentGameRegion == Game::Region::KOR)
		{
			if (wstring(lpLibFileName).find(L"KakaoGame.dll") != wstring::npos)
			{
				auto KakaoGame = reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);

				auto cef = GetModuleHandleW(L"libcef.dll");

				const auto cef_cookie_manager_get_global_manager_addr = reinterpret_cast<decltype(cef_cookie_manager_get_global_manager)>(GetProcAddress(cef, "cef_cookie_manager_get_global_manager"));

				MH_CreateHook(cef_cookie_manager_get_global_manager_addr, cef_cookie_manager_get_global_manager_hook, &cef_cookie_manager_get_global_manager_orig);
				MH_EnableHook(cef_cookie_manager_get_global_manager_addr);

				return KakaoGame;
			}
		}

		return reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);
	}

	IUnityInterfaces* unityInterfaces;

	void UnityOpenXR_UnityPluginLoad_hook(IUnityInterfaces* unityInterfaces);

	void* UnityPluginLoad_orig = nullptr;
	void UnityPluginLoad_hook(IUnityInterfaces* unityInterfaces)
	{
		reinterpret_cast<decltype(UnityPluginLoad_hook)*>(UnityPluginLoad_orig)(unityInterfaces);


		::unityInterfaces = unityInterfaces;
		// Unity::OpenXR::InitLibrary(unityInterfaces);

		//Unity::OpenXR::DiagnosticReport::StartReport();
		// Unity::OpenXR::Init();
		// Unity::OpenXR::Start();
	}

	void* UnityOpenXR_UnityPluginLoad_orig = nullptr;
	void UnityOpenXR_UnityPluginLoad_hook(IUnityInterfaces* unityInterfaces)
	{
		reinterpret_cast<decltype(UnityOpenXR_UnityPluginLoad_hook)*>(UnityOpenXR_UnityPluginLoad_orig)(unityInterfaces);

		::unityInterfaces = unityInterfaces;
		// Unity::OpenXR::Init();

		// Unity::OpenXR::InitLibrary(unityInterfaces);

		//Unity::OpenXR::DiagnosticReport::StartReport();
		// Unity::OpenXR::Init();
		// Unity::OpenXR::Start();
	}

	Il2CppObject* (*display_get_main)();

	int (*get_system_width)(Il2CppObject* _this);

	int (*get_system_height)(Il2CppObject* _this);

	void* KGInterfaceBrokerRequest_orig = nullptr;
	LPCWSTR KGInterfaceBrokerRequest_hook(const wchar_t* request)
	{
		auto text = wide_u8(request);

#ifdef EXPERIMENTS
		replaceAll(text, "gameWeb", "googlePlay");
#endif

		if (text.find("Zinny://Support.setViewSize") != string::npos)
		{
			rapidjson::Document doc;
			doc.Parse(text.data());

			if (!doc.HasParseError())
			{
				auto params = doc.GetArray()[2].GetObj();

				auto display = display_get_main();
				auto systemHeight = get_system_height(display);

				auto width = params["width"].GetInt();
				auto height = params["height"].GetInt();

				if (height > systemHeight)
				{
					params["width"].SetInt(static_cast<int>(roundf(width / (16.0f / 9.0f))));
					params["height"].SetInt(width - 100);

					rapidjson::StringBuffer buffer;
					buffer.Clear();
					rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
					doc.Accept(writer);

					text = buffer.GetString();
				}
			}
		}

		auto data = reinterpret_cast<decltype(KGInterfaceBrokerRequest_hook)*>(KGInterfaceBrokerRequest_orig)(u8_wide(text).data());
		return data;
	}

	void* load_library_w_orig = nullptr;
	HMODULE WINAPI load_library_w_hook(LPCWSTR lpLibFileName)
	{
		if (lpLibFileName == L"KakaoGameWin.dll"s)
		{
			auto KakaoGameWin = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);

			MH_DisableHook(LoadLibraryW);
			MH_RemoveHook(LoadLibraryW);

			const auto KGInterfaceBrokerRequest_addr = GetProcAddress(KakaoGameWin, "KGInterfaceBrokerRequest");
			MH_CreateHook(KGInterfaceBrokerRequest_addr, KGInterfaceBrokerRequest_hook, &KGInterfaceBrokerRequest_orig);
			MH_EnableHook(KGInterfaceBrokerRequest_addr);

			return KakaoGameWin;
		}

		if (lpLibFileName == L"UmamusumeKr.dll"s)
		{
			const auto UmamusumeKr = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
			const auto umaFindService_addr = GetProcAddress(UmamusumeKr, "umaFindService");
			MH_CreateHook(umaFindService_addr, umaFindService_hook, &umaFindService_orig);
			MH_EnableHook(umaFindService_addr);
			return UmamusumeKr;
		}

		if (lpLibFileName == L"GameAssembly.dll"s)
		{
			filesystem::path path = filesystem::current_path().append(L"UnityPlayer.dll");
			il2cpp_symbols::load_symbols(path);

			if (!config::external_dlls_path.empty())
			{
				for (int i = 0; i < config::external_dlls_path.size(); i++)
				{
					auto path = config::external_dlls_path[i];
					wcout << L"Loading " << path << L": ";
					auto dll = LoadLibraryW(path.data());
					wcout << dll << endl;
				}
			}

			const auto il2cpp = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
			/*std::ofstream out("./GameAssembly.decrypted.dll", std::ios_base::binary);
			MODULEINFO info;
			if (out && GetModuleInformation(GetCurrentProcess(), il2cpp, &info, sizeof(info)))
			{
				const auto header = static_cast<const IMAGE_DOS_HEADER*>(info.lpBaseOfDll);
				const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>(
					static_cast<const std::byte*>(info.lpBaseOfDll) + header->e_lfanew);
				const auto sections = IMAGE_FIRST_SECTION(ntHeader);

				out.write(static_cast<const char*>(info.lpBaseOfDll),
					reinterpret_cast<const char*>(sections) +
					ntHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER) -
					static_cast<const char*>(info.lpBaseOfDll));

				for (std::size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
				{
					const auto& section = sections[i];

					out.seekp(section.PointerToRawData);
					out.write(static_cast<const char*>(info.lpBaseOfDll) + section.VirtualAddress,
						section.SizeOfRawData);
				}
			}*/
			fpp::ChangeGameAssemblyProtection(il2cpp);

			const auto il2cpp_init_addr = GetProcAddress(il2cpp, il2cpp_fn_name("il2cpp_init").data());
			il2cpp_symbols::init(il2cpp);

			MH_CreateHook(il2cpp_init_addr, il2cpp_init_hook, &il2cpp_init_orig);
			auto result = MH_EnableHook(il2cpp_init_addr);

			if (result != MH_OK)
			{
				cout << "WARN: il2cpp_init Failed: " << MH_StatusToString(result) << " LastError: " << GetLastError() << endl << endl;
			}

			return il2cpp;
		}

		if (lpLibFileName == L"UnityOpenXR.dll"s)
		{
			auto module = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);

			Unity::OpenXR::InitModule(module);
			return module;
		}

		static bool criwareInit = false;

		// GameAssembly.dll code must be loaded and decrypted while loading criware library
		if (lpLibFileName == L"cri_ware_unity.dll"s && !criwareInit)
		{
			criwareInit = true;

			patch_after_criware();

			wstring module_name;
			module_name.resize(MAX_PATH);
			module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

			filesystem::path module_path(module_name);

			wstring name = module_path.filename().replace_extension();

			SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());

			// use original function beacuse we have unhooked that
			auto criware = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);

			if (Game::CurrentGameRegion != Game::Region::KOR)
			{
				MH_DisableHook(LoadLibraryW);
				MH_RemoveHook(LoadLibraryW);
			}

			if (config::character_system_text_caption)
			{
				auto criAtomExPlayer_SetCueId_addr = GetProcAddress(criware, "criAtomExPlayer_SetCueId");

				MH_CreateHook(criAtomExPlayer_SetCueId_addr, criAtomExPlayer_SetCueId_hook, &criAtomExPlayer_SetCueId_orig);
				MH_EnableHook(criAtomExPlayer_SetCueId_addr);

				auto criAtomExPlayer_Start_addr = GetProcAddress(criware, "criAtomExPlayer_Start");

				MH_CreateHook(criAtomExPlayer_Start_addr, criAtomExPlayer_Start_hook, &criAtomExPlayer_Start_orig);
				MH_EnableHook(criAtomExPlayer_Start_addr);

				auto criAtomExAcb_GetCueInfoById_addr = GetProcAddress(criware, "criAtomExAcb_GetCueInfoById");

				MH_CreateHook(criAtomExAcb_GetCueInfoById_addr, criAtomExAcb_GetCueInfoById_hook, &criAtomExAcb_GetCueInfoById_orig);
				MH_EnableHook(criAtomExAcb_GetCueInfoById_addr);

				auto criAtomExAcb_GetCueInfoByName_addr = GetProcAddress(criware, "criAtomExAcb_GetCueInfoByName");

				MH_CreateHook(criAtomExAcb_GetCueInfoByName_addr, criAtomExAcb_GetCueInfoByName_hook, &criAtomExAcb_GetCueInfoByName_orig);
				MH_EnableHook(criAtomExAcb_GetCueInfoByName_addr);

				auto criAtomExAcb_Release_addr = GetProcAddress(criware, "criAtomExAcb_Release");

				MH_CreateHook(criAtomExAcb_Release_addr, criAtomExAcb_Release_hook, &criAtomExAcb_Release_orig);
				MH_EnableHook(criAtomExAcb_Release_addr);

				auto criAtomExPlayer_Stop_addr = GetProcAddress(criware, "criAtomExPlayer_Stop");

				MH_CreateHook(criAtomExPlayer_Stop_addr, criAtomExPlayer_Stop_hook, &criAtomExPlayer_Stop_orig);
				MH_EnableHook(criAtomExPlayer_Stop_addr);

				auto criAtomExPlayer_StopWithoutReleaseTime_addr = GetProcAddress(criware, "criAtomExPlayer_StopWithoutReleaseTime");

				MH_CreateHook(criAtomExPlayer_StopWithoutReleaseTime_addr, criAtomExPlayer_StopWithoutReleaseTime_hook, &criAtomExPlayer_StopWithoutReleaseTime_orig);
				MH_EnableHook(criAtomExPlayer_StopWithoutReleaseTime_addr);

				auto criAtomExPlayer_Pause_addr = GetProcAddress(criware, "criAtomExPlayer_Pause");

				MH_CreateHook(criAtomExPlayer_Pause_addr, criAtomExPlayer_Pause_hook, &criAtomExPlayer_Pause_orig);
				MH_EnableHook(criAtomExPlayer_Pause_addr);
			}

			if (!config::replace_assets.empty())
			{
				auto CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE310ABCEC");
				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWAREDA7020AB");
				}

				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE310ABCEC");
				}

				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE6A5D4549");
				}

				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWAREDF8F52BA");
				}

				MH_CreateHook(CriMana_SetFileNew_addr, CriMana_SetFileNew_hook, &CriMana_SetFileNew_orig);
				MH_EnableHook(CriMana_SetFileNew_addr);

				auto CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWAREB83D23AD");
				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWAREE7861E0D");
				}

				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWARE925FC233");
				}

				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWARE6F49ABEA");
				}

				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWARE063F9CD6");
				}

				MH_CreateHook(CriMana_SetFileAppend_addr, CriMana_SetFileAppend_hook, &CriMana_SetFileAppend_orig);
				MH_EnableHook(CriMana_SetFileAppend_addr);
			}

			return criware;
		}

		return reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
	}

	void* GetProcAddress_orig = nullptr;
	FARPROC WINAPI GetProcAddress_hook(
		_In_ HMODULE hModule,
		_In_ LPCSTR lpProcName
	)
	{
		if (hModule == il2cpp_symbols::module)
		{
			return reinterpret_cast<decltype(GetProcAddress_hook)*>(GetProcAddress_orig)(hModule, il2cpp_fn_name(lpProcName).data());
		}

		return reinterpret_cast<decltype(GetProcAddress_hook)*>(GetProcAddress_orig)(hModule, lpProcName);
	}

	Il2CppObject* GetInt32Instance(int value)
	{
		return il2cpp_value_box(il2cpp_defaults.int32_class, &value);
	}

	void* populate_with_errors_orig = nullptr;
	bool populate_with_errors_hook(Il2CppObject* _this, Il2CppString* str, UnityEngine::TextGenerationSettings* settings, void* context)
	{
		return reinterpret_cast<decltype(populate_with_errors_hook)*>(populate_with_errors_orig) (
			_this, local::get_localized_string(str), settings, context
			);
	}

	void ReplaceTextMeshFont(Il2CppObject* textMesh, Il2CppObject* meshRenderer)
	{
		Il2CppObject* font = GetCustomFont();

		if (!font)
		{
			return;
		}

		Il2CppObject* fontMaterial = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(font->klass, "get_material", 0)->methodPointer(font);
		Il2CppObject* fontTexture = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(fontMaterial->klass, "get_mainTexture", 0)->methodPointer(fontMaterial);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject* _this, Il2CppObject* font)>(textMesh->klass, "set_font", 1)->methodPointer(textMesh, font);
		if (meshRenderer)
		{
			auto get_sharedMaterial = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(meshRenderer->klass, "GetSharedMaterial", 0)->methodPointer;

			Il2CppObject* sharedMaterial = get_sharedMaterial(meshRenderer);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject* _this, Il2CppObject* texture)>(sharedMaterial->klass, "set_mainTexture", 1)->methodPointer(sharedMaterial, fontTexture);
		}
	}

	void* get_preferred_width_orig = nullptr;
	float get_preferred_width_hook(void* _this, Il2CppString* str, UnityEngine::TextGenerationSettings* settings)
	{
		return reinterpret_cast<decltype(get_preferred_width_hook)*>(get_preferred_width_orig) (
			_this, local::get_localized_string(str), settings
			);
	}

	void* an_text_set_material_to_textmesh_orig = nullptr;
	void an_text_set_material_to_textmesh_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(an_text_set_material_to_textmesh_hook)*>(an_text_set_material_to_textmesh_orig)(_this);
		if (!(config::replace_to_custom_font && config::runtime::fontAssets)) return;

		FieldInfo* mainField = il2cpp_class_get_field_from_name(_this->klass, "_mainTextMesh");
		FieldInfo* mainRenderer = il2cpp_class_get_field_from_name(_this->klass, "_mainTextMeshRenderer");

		FieldInfo* outlineField = il2cpp_class_get_field_from_name(_this->klass, "_outlineTextMeshList"); //List<TextMesh>
		FieldInfo* outlineFieldRenderers = il2cpp_class_get_field_from_name(_this->klass, "_outlineTextMeshRendererList"); //List<MeshRenderer>

		FieldInfo* shadowField = il2cpp_class_get_field_from_name(_this->klass, "_shadowTextMesh");
		FieldInfo* shadowFieldRenderer = il2cpp_class_get_field_from_name(_this->klass, "_shadowTextMeshRenderer");

		Il2CppObject* mainMesh;
		Il2CppObject* mainMeshRenderer;

		Il2CppObject* outlineMeshes;
		Il2CppObject* outlineMeshRenderers;

		Il2CppObject* shadowMesh;
		Il2CppObject* shadowMeshRenderer;

		il2cpp_field_get_value(_this, mainField, &mainMesh);
		il2cpp_field_get_value(_this, mainRenderer, &mainMeshRenderer);

		ReplaceTextMeshFont(mainMesh, mainMeshRenderer);

		vector<Il2CppObject*> textMeshies;
		il2cpp_field_get_value(_this, outlineField, &outlineMeshes);
		if (outlineMeshes)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(outlineMeshes->klass, "_items");
			Il2CppArraySize* arr;
			il2cpp_field_get_value(outlineMeshes, itemsField, &arr);
			if (arr)
			{
				for (int i = 0; i < arr->max_length; i++)
				{
					Il2CppObject* mesh = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
					if (!mesh)
					{
						break;
					}
					textMeshies.emplace_back(mesh);
				}
			}
		}

		il2cpp_field_get_value(_this, outlineFieldRenderers, &outlineMeshRenderers);
		if (outlineMeshRenderers)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(outlineMeshRenderers->klass, "_items");
			Il2CppArraySize* arr;
			il2cpp_field_get_value(outlineMeshRenderers, itemsField, &arr);
			if (arr)
			{
				for (int i = 0; i < textMeshies.size(); i++)
				{
					Il2CppObject* meshRenderer = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
					ReplaceTextMeshFont(textMeshies[i], meshRenderer);
				}
			}
		}

		il2cpp_field_get_value(_this, shadowField, &shadowMesh);
		if (shadowMesh)
		{
			il2cpp_field_get_value(_this, shadowFieldRenderer, &shadowMeshRenderer);
			ReplaceTextMeshFont(shadowMesh, shadowMeshRenderer);
		}
	}

	void* an_text_fix_data_orig = nullptr;
	void an_text_fix_data_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(an_text_fix_data_hook)*>(an_text_fix_data_orig)(_this);
		FieldInfo* field = il2cpp_class_get_field_from_name(_this->klass, "_text");
		Il2CppString* str;
		il2cpp_field_get_value(_this, field, &str);
		il2cpp_field_set_value(_this, field, local::get_localized_string(str));
	}

	void* update_orig = nullptr;
	void* update_hook(Il2CppObject* _this, void* updateType, float deltaTime, float independentTime)
	{
		return reinterpret_cast<decltype(update_hook)*>(update_orig)(_this, updateType, deltaTime * config::ui_animation_scale, independentTime * config::ui_animation_scale);
	}

	unordered_map<sqlite3_stmt*, tuple<sqlite3_stmt*, string, bool>> text_queries;
	unordered_map<sqlite3_stmt*, bool> replacement_queries_can_next;

	sqlite3_stmt* select_character_system_text_characterId;
	tuple<sqlite3_stmt*, bool> select_character_system_text_characterId_replacement;
	bool select_character_system_text_characterId_replacement_can_next = true;

	void* sqlite3_prepare_v2_orig = nullptr;
	int sqlite3_prepare_v2_hook(sqlite3* db, const char* zSql, int nBytes, sqlite3_stmt** ppStmt, const char** pzTail)
	{
		auto result = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(db, zSql, nBytes, ppStmt, pzTail);

		string sql = string(zSql, nBytes);

		if (sql.find("text_data") != string::npos ||
			sql.find("race_jikkyo_comment") != string::npos ||
			sql.find("race_jikkyo_message") != string::npos)
		{
			sqlite3_stmt* pStmt = *ppStmt;

			if (MasterDB::replacementMasterDB)
			{
				sqlite3_stmt* stmt;
				auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &stmt, pzTail);
				text_queries.emplace(pStmt, make_tuple(stmt, sql, false));

				if (res != SQLITE_OK)
				{
					cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
				}
			}
		}

		if (sql.find("character_system_text") != string::npos)
		{
			sqlite3_stmt* pStmt = *ppStmt;

			if (sql.find("`voice_id`=?") != string::npos)
			{
				if (MasterDB::replacementMasterDB)
				{
					sqlite3_stmt* stmt;
					auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &stmt, pzTail);
					text_queries.emplace(pStmt, make_tuple(stmt, sql, false));

					if (res != SQLITE_OK)
					{
						cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
					}
				}
			}
			else
			{
				select_character_system_text_characterId = pStmt;


				if (MasterDB::replacementMasterDB)
				{
					auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &get<0>(select_character_system_text_characterId_replacement), pzTail);
					if (res != SQLITE_OK)
					{
						cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
					}
				}
			}
		}

		return result;
	}

	int sqlite3_step_hook(sqlite3_stmt* pStmt)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			if (get<1>(stmt).find("`race_jikkyo_message`;") != string::npos ||
				get<1>(stmt).find("`race_jikkyo_comment`;") != string::npos)
			{
				if (replacement_queries_can_next.find(pStmt) == replacement_queries_can_next.end())
				{
					replacement_queries_can_next.emplace(pStmt, true);
				}
				if (replacement_queries_can_next.at(pStmt))
				{
					get<2>(stmt) = (reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(get<0>(stmt)) == SQLITE_ROW);
				}
			}
			else
			{
				get<2>(stmt) = (reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(get<0>(stmt)) == SQLITE_ROW);
			}
		}

		if (pStmt == select_character_system_text_characterId)
		{
			if (get<0>(select_character_system_text_characterId_replacement))
			{
				auto res = reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(pStmt);

				if (res == SQLITE_ROW)
				{
					int voiceId = sqlite3_column_int(pStmt, 0);

					if (select_character_system_text_characterId_replacement_can_next)
					{
						get<1>(select_character_system_text_characterId_replacement) = (sqlite3_step(get<0>(select_character_system_text_characterId_replacement)) == SQLITE_ROW);
					}

					if (get<1>(select_character_system_text_characterId_replacement))
					{
						select_character_system_text_characterId_replacement_can_next = voiceId == sqlite3_column_int(get<0>(select_character_system_text_characterId_replacement), 0);
					}
				}

				return res;
			}
		}

		return reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(pStmt);
	}

	int sqlite3_reset_hook(sqlite3_stmt* pStmt)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(get<0>(stmt));
			get<2>(stmt) = false;
			replacement_queries_can_next.insert_or_assign(pStmt, true);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(get<0>(select_character_system_text_characterId_replacement));
			get<1>(select_character_system_text_characterId_replacement) = false;
			select_character_system_text_characterId_replacement_can_next = true;
		}

		return reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(pStmt);
	}

	int sqlite3_bind_text_hook(sqlite3_stmt* pStmt, int i, const char* zData, int nData, void (*xDel)(void*))
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(get<0>(stmt), i, zData, nData, xDel);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(get<0>(select_character_system_text_characterId_replacement), i, zData, nData, xDel);
		}

		return reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(pStmt, i, zData, nData, xDel);
	}

	int sqlite3_bind_int_hook(sqlite3_stmt* pStmt, int i, int iValue)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(get<0>(stmt), i, iValue);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(get<0>(select_character_system_text_characterId_replacement), i, iValue);
		}

		return reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(pStmt, i, iValue);
	}

	int sqlite3_bind_int64_hook(sqlite3_stmt* pStmt, int i, sqlite_int64 iValue)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(stmt), i, iValue);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(select_character_system_text_characterId_replacement), i, iValue);
		}

		return reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(pStmt, i, iValue);
	}

	int sqlite3_bind_double_hook(sqlite3_stmt* pStmt, int i, double rValue)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_bind_double_hook)*>(sqlite3_bind_double_orig)(get<0>(stmt), i, rValue);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(select_character_system_text_characterId_replacement), i, rValue);
		}

		return reinterpret_cast<decltype(sqlite3_bind_double_hook)*>(sqlite3_bind_double_orig)(pStmt, i, rValue);
	}

	int sqlite3_finalize_hook(sqlite3_stmt* pStmt)
	{
		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(get<0>(stmt));
			text_queries.erase(pStmt);
		}

		if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
		{
			reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(get<0>(select_character_system_text_characterId_replacement));
			get<0>(select_character_system_text_characterId_replacement) = nullptr;
		}

		return reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(pStmt);
	}

	int sqlite3_key_hook(sqlite3* db, const void* pKey, int nKey)
	{
		// no-op
		return SQLITE_OK;
	}

	const unsigned char* sqlite3_column_text_hook(sqlite3_stmt* pStmt, int i)
	{
		auto result = reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(pStmt, i);

		if (text_queries.contains(pStmt))
		{
			auto& stmt = text_queries.at(pStmt);
			const char* text;
			if (get<2>(stmt))
			{
				text = reinterpret_cast<const char*>(reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(get<0>(stmt), i));
				if (text && !string(text).empty())
				{
					if (get<1>(stmt).find("`race_jikkyo_message`;") != string::npos ||
						get<1>(stmt).find("`race_jikkyo_comment`;") != string::npos)
					{
						int id = sqlite3_column_int(pStmt, 0);
						int id1 = sqlite3_column_int(get<0>(stmt), 0);
						int groupId = sqlite3_column_int(pStmt, 1);
						int groupId1 = sqlite3_column_int(get<0>(stmt), 1);

						if (id == id1 && groupId == groupId1)
						{
							replacement_queries_can_next.insert_or_assign(pStmt, true);
							return reinterpret_cast<const unsigned char*>(text);
						}
						else
						{
							replacement_queries_can_next.insert_or_assign(pStmt, false);
						}
					}
					else
					{
						return reinterpret_cast<const unsigned char*>(text);
					}
				}
			}

			return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
		}

		if (pStmt == select_character_system_text_characterId)
		{
			if (get<1>(select_character_system_text_characterId_replacement) && select_character_system_text_characterId_replacement_can_next)
			{
				return reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(get<0>(select_character_system_text_characterId_replacement), i);
			}

			return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
		}

		return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
	}

	void* CySpringUpdater_set_SpringUpdateMode_orig = nullptr;
	void CySpringUpdater_set_SpringUpdateMode_hook(Il2CppObject* _this, int value)
	{
		reinterpret_cast<decltype(CySpringUpdater_set_SpringUpdateMode_hook)*>(CySpringUpdater_set_SpringUpdateMode_orig)(_this, config::cyspring_update_mode);
	}

	void* CySpringUpdater_get_SpringUpdateMode_orig = nullptr;
	int CySpringUpdater_get_SpringUpdateMode_hook(Il2CppObject* _this)
	{
		return config::cyspring_update_mode;
	}

	void* story_race_textasset_load_orig;
	void story_race_textasset_load_hook(Il2CppObject* _this)
	{
		FieldInfo* textDataField = { il2cpp_class_get_field_from_name(_this->klass, "textData") };
		Il2CppObject* textData;
		il2cpp_field_get_value(_this, textDataField, &textData);

		auto enumerator = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(textData->klass, "GetEnumerator", 0)->methodPointer(textData);
		auto get_current = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(enumerator->klass, "get_Current", 0)->methodPointer;
		auto move_next = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject * _this)>(enumerator->klass, "MoveNext", 0)->methodPointer;

		while (move_next(enumerator))
		{
			auto key = get_current(enumerator);
			FieldInfo* textField = { il2cpp_class_get_field_from_name(key->klass, "text") };
			Il2CppString* text;
			il2cpp_field_get_value(key, textField, &text);
			il2cpp_field_set_value(key, textField, local::get_localized_string(text));
		}

		reinterpret_cast<decltype(story_race_textasset_load_hook)*>(story_race_textasset_load_orig)(_this);
	}

	bool useDefaultFPS = false;

	int (*get_rendering_width)(Il2CppObject* _this);

	int (*get_rendering_height)(Il2CppObject* _this);

	constexpr float ratio_16_9 = 1.7777f;
	constexpr float ratio_9_16 = 0.5625f;
	constexpr float ratio_4_3 = 1.3333f;
	constexpr float ratio_3_4 = 0.75f;

	int last_display_width = 0, last_display_height = 0;
	int last_virt_window_width = 0, last_virt_window_height = 0;
	int last_hriz_window_width = 0, last_hriz_window_height = 0;

	bool fullScreenFl = false;
	bool fullScreenFlOverride = false;

	void (*get_resolution)(UnityEngine::Resolution* buffer);

	void (*set_resolution)(int width, int height, bool fullscreen);

	void get_resolution_stub(UnityEngine::Resolution* r)
	{
		get_resolution(r);

		int width = min(r->height, r->width) * config::aspect_ratio;
		if (r->width > r->height)
			r->width = width;
		else
			r->height = width;
	}

	void* gallop_get_screenheight_orig;
	int gallop_get_screenheight_hook()
	{
		if (!config::freeform_window)
		{
			int w = max(last_display_width, last_display_height), h = min(last_display_width, last_display_height);

			if (Gallop::Screen::IsSplitWindow())
			{
				return h;
			}

			return Gallop::StandaloneWindowResize::IsVirt() ? w : h;
		}

		return UnityEngine::Screen::height();
	}

	void* gallop_get_screenwidth_orig;
	int gallop_get_screenwidth_hook()
	{
		if (!config::freeform_window)
		{
			int w = max(last_display_width, last_display_height), h = min(last_display_width, last_display_height);

			if (Gallop::Screen::IsSplitWindow())
			{
				return h * ratio_3_4;
			}

			return Gallop::StandaloneWindowResize::IsVirt() ? h : w;
		}

		return UnityEngine::Screen::width();
	}

	void* GallopStandaloneInputModule_SetPointerPosition_orig = nullptr;
	void GallopStandaloneInputModule_SetPointerPosition_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(GallopStandaloneInputModule_SetPointerPosition_hook)*>(GallopStandaloneInputModule_SetPointerPosition_orig)(self);
		auto pointerEventDataField = il2cpp_class_get_field_from_name(self->klass, "pointerEventData");
		Il2CppObject* pointerEventData;
		il2cpp_field_get_value(self, pointerEventDataField, &pointerEventData);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(pointerEventData->klass, "set_position", 1)->methodPointer(pointerEventData, (UnityEngine::Input::mousePosition() * (Gallop::Screen::Width() / static_cast<float>(UnityEngine::Screen::width()))));
	}

	void* GallopStandaloneInputModule_ProcessTouchEvents_orig = nullptr;
	bool GallopStandaloneInputModule_ProcessTouchEvents_hook(Il2CppObject* self)
	{
		// cout << "GallopStandaloneInputModule_ProcessTouchEvents_hook" << endl;
		auto isTouchReactField = il2cpp_class_get_field_from_name(self->klass, "isTouchReact");
		bool isTouchReact = true;
		il2cpp_field_static_set_value(isTouchReactField, &isTouchReact);

		auto input = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(self->klass, "get_input", 0)->methodPointer(self);
		auto touchCount = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(input->klass, "get_touchCount", 0)->methodPointer(input);
		for (int i = 0; i < touchCount; i++)
		{
			auto touch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(input->klass, "GetTouch", 1)->methodPointer(input, i);
			auto position = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)(Il2CppObject*)>(touch->klass, "get_position", 0)->methodPointer(touch);
			if (!isnan(position.x) && !isnan(position.y))
			{
				if (!Gallop::Screen::IsSplitWindow())
				{
					auto m_PositionField = il2cpp_class_get_field_from_name(touch->klass, "m_Position");
					position = position * ((float)Gallop::Screen::Width() / (float)UnityEngine::Screen::width());

					il2cpp_field_set_value(touch, m_PositionField, &position);
				}
				auto type = il2cpp_class_get_method_from_name_type<uint64_t(*)(Il2CppObject*)>(touch->klass, "get_type", 0)->methodPointer(touch);
				if (type != 1 /* TouchType.Indirect */)
				{
					bool pressed;
					bool released;
					auto touchPointerEventData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool*, bool*)>(self->klass, "GetTouchPointerEventData", 3)->methodPointer(self, touch, &pressed, &released);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool, bool)>(self->klass, "ProcessTouchPress", 3)->methodPointer(self, touchPointerEventData, pressed, released);
					if (!released)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "ProcessMove", 1)->methodPointer(self, touchPointerEventData);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "ProcessDrag", 1)->methodPointer(self, touchPointerEventData);
					}
					else
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "RemovePointerData", 1)->methodPointer(self, touchPointerEventData);
					}
				}
			}
		}
		isTouchReact = false;
		il2cpp_field_static_set_value(isTouchReactField, &isTouchReact);
		touchCount = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(input->klass, "get_touchCount", 0)->methodPointer(input);
		return touchCount > 0;
	}

	void (*set_scale_factor)(void*, float);

	void* BGManager_CalcBgScale_orig = nullptr;
	float BGManager_CalcBgScale_hook(Il2CppObject* _this, int width, int height, int renderTextureWidth, int renderTextureHeight)
	{
		int width1 = UnityEngine::Screen::width();
		int height1 = UnityEngine::Screen::height();
		float ratio1 = (float)width1 / (float)height1;

		float bgCanvasScalerBaseScale = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, int, int)>(_this->klass, "GetBgCanvasScalerBaseScale", 2)->methodPointer(_this, renderTextureWidth, renderTextureHeight);
		if (Gallop::StandaloneWindowResize::IsVirt())
		{
			return max((float)renderTextureHeight / (float)height, (float)height / (float)renderTextureHeight) * (config::freeform_window ? 2 : 1) / bgCanvasScalerBaseScale;
		}
		return max((float)renderTextureWidth / (float)width, (float)width / (float)renderTextureWidth) * (config::freeform_window ? 2 : 1) / bgCanvasScalerBaseScale * ratio1;
	}

	void SetBGCanvasScalerSize()
	{
		auto bgManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "BGManager", "get_Instance", 0)();
		if (bgManager)
		{
			auto _mainBgField = il2cpp_class_get_field_from_name(bgManager->klass, "_mainBg");
			Il2CppObject* _mainBg;
			il2cpp_field_get_value(bgManager, _mainBgField, &_mainBg);

			if (_mainBg)
			{
				auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainBg->klass, "get_transform", 0)->methodPointer(_mainBg);

				int width = UnityEngine::Screen::width();
				int height = UnityEngine::Screen::height();

				if (width > height)
				{
					auto pos = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(transform->klass, "get_localPosition", 0)->methodPointer(transform);

					if (pos.y == 0)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(transform->klass, "set_localPosition", 1)->methodPointer(transform, UnityEngine::Vector3{ 0, 0, 0 });
					}
				}
			}

			auto _bgCanvasScalerField = il2cpp_class_get_field_from_name(bgManager->klass, "_bgCanvasScaler");
			Il2CppObject* _bgCanvasScaler;
			il2cpp_field_get_value(bgManager, _bgCanvasScalerField, &_bgCanvasScaler);

			if (_bgCanvasScaler)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_bgCanvasScaler->klass, "set_scaleFactor", 1)->methodPointer(_bgCanvasScaler, 1);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_bgCanvasScaler->klass, "set_uiScaleMode", 1)->methodPointer(_bgCanvasScaler, 1);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_bgCanvasScaler->klass, "set_screenMatchMode", 1)->methodPointer(_bgCanvasScaler, 0);
			}
		}
	}

	void ResizeMiniDirector()
	{
		Il2CppArraySize_t<Il2CppObject*>* miniDirectors;
		miniDirectors = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (miniDirectors)
		{
			for (int i = 0; i < miniDirectors->max_length; i++)
			{
				auto obj = miniDirectors->vector[i];

				if (obj)
				{
					auto state = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(obj->klass, "get_State", 0)->methodPointer(obj);

					if (state > 0)
					{
						auto DirectorUI = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_DirectorUI", 0)->methodPointer(obj);
						auto cameraController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_CameraController", 0)->methodPointer(obj);

						if (DirectorUI && cameraController)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(DirectorUI->klass, "ResetTextureSize", 0)->methodPointer(DirectorUI);

							auto TextureResolution = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2Int(*)(Il2CppObject*)>(DirectorUI->klass, "get_TextureResolution", 0)->methodPointer(DirectorUI);

							auto _cameraField = il2cpp_class_get_field_from_name(cameraController->klass, "_camera");
							Il2CppObject* _camera;
							il2cpp_field_get_value(cameraController, _cameraField, &_camera);

							if (_camera)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2Int)>(cameraController->klass, "ResizeRenderTexture", 1)->methodPointer(cameraController, TextureResolution);

								auto _renderTextureField = il2cpp_class_get_field_from_name(cameraController->klass, "_renderTexture");
								Il2CppObject* _renderTexture;
								il2cpp_field_get_value(cameraController, _renderTextureField, &_renderTexture);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(DirectorUI->klass, "SetRenderTexture", 1)->methodPointer(DirectorUI, _renderTexture);
							}
						}
					}
				}
			}
		}
	}

	void* GetLimitSize_orig = nullptr;
	UnityEngine::Vector2 GetLimitSize_hook()
	{
		auto orig = reinterpret_cast<decltype(GetLimitSize_hook)*>(GetLimitSize_orig)();
		UnityEngine::Resolution r;
		get_resolution(&r);
		orig.x = r.width;
		orig.y = r.height;
		return orig;
	}

	bool altEnterPressed = false;

	Il2CppObject* delayTweener;

	void RemakeTextures()
	{
		auto uiManager = Gallop::UIManager::Instance();

		auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
		if (!graphicSettings)
		{
			return;
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(graphicSettings->klass, "Update3DRenderTexture", 0)->methodPointer(graphicSettings);

		Il2CppArraySize_t<Il2CppObject*>* renders;
		renders = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "CutInImageEffectPostRender"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (renders)
		{
			for (int i = 0; i < renders->max_length; i++)
			{
				auto obj = renders->vector[i];

				if (obj)
				{
					auto buffer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_FrameBuffer", 0)->methodPointer(obj);
					if (buffer)
					{
						if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(buffer->klass, "get_ColorBuffer", 0)->methodPointer(buffer))
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
						}
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* cuts;
		cuts = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "LimitBreakCut"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (cuts)
		{
			for (int i = 0; i < cuts->max_length; i++)
			{
				auto obj = cuts->vector[i];

				if (obj)
				{
					auto _frameBufferField = il2cpp_class_get_field_from_name(obj->klass, "_frameBuffer");
					Il2CppObject* _frameBuffer;
					il2cpp_field_get_value(obj, _frameBufferField, &_frameBuffer);

					if (_frameBuffer)
					{
						if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_frameBuffer->klass, "get_ColorBuffer", 0)->methodPointer(_frameBuffer))
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
						}
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* raceEffect;
		raceEffect = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "RaceImageEffect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (raceEffect)
		{
			for (int i = 0; i < raceEffect->max_length; i++)
			{
				auto obj = raceEffect->vector[i];

				if (obj)
				{
					auto get_FrameBuffer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_FrameBuffer", 0);
					if (get_FrameBuffer)
					{
						auto buffer = get_FrameBuffer->methodPointer(obj);
						if (buffer)
						{
							auto _drawPassField = il2cpp_class_get_field_from_name(buffer->klass, "_drawPass");
							uint64_t* _drawPass;
							il2cpp_field_get_value(buffer, _drawPassField, &_drawPass);

							if (!_drawPass)
							{
								uint64_t defPass = 0;
								il2cpp_field_set_value(buffer, _drawPassField, &defPass);
							}


							if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(buffer->klass, "get_ColorBuffer", 0)->methodPointer(buffer))
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}

		/*Il2CppArraySize_t<Il2CppObject*>* storyEffect;
		storyEffect = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "StoryImageEffect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (storyEffect)
		{
			for (int i = 0; i < storyEffect->max_length; i++)
			{
				auto obj = storyEffect->vector[i];

				if (obj)
				{
					auto buffer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_FrameBuffer", 0)->methodPointer(obj);
					if (buffer)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
					}
				}
			}
		}*/

		Il2CppArraySize_t<Il2CppObject*>* lowResCameras;
		lowResCameras = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "LowResolutionCameraBase"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (lowResCameras)
		{
			for (int i = 0; i < lowResCameras->max_length; i++)
			{
				auto obj = lowResCameras->vector[i];

				if (obj)
				{
					auto method = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(obj->klass, "ResizeRenderTextureSize", 0);
					if (method)
					{
						method->methodPointer(obj);
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* liveTheaterCharaSelects;
		liveTheaterCharaSelects = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "LiveTheaterCharaSelect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (liveTheaterCharaSelects)
		{
			for (int i = 0; i < liveTheaterCharaSelects->max_length; i++)
			{
				auto obj = liveTheaterCharaSelects->vector[i];

				if (obj)
				{
					auto _sceneField = il2cpp_class_get_field_from_name(obj->klass, "_scene");
					Il2CppObject* _scene;
					il2cpp_field_get_value(obj, _sceneField, &_scene);

					if (_scene)
					{
						auto camera = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_scene->klass, "GetCamera", 0)->methodPointer(_scene);
						auto texture = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(camera->klass, "get_RenderTexture", 0)->methodPointer(camera);

						auto _formationAllField = il2cpp_class_get_field_from_name(obj->klass, "_formationAll");
						Il2CppObject* _formationAll;
						il2cpp_field_get_value(obj, _formationAllField, &_formationAll);

						if (_formationAll)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_formationAll->klass, "SetRenderTex", 1)->methodPointer(_formationAll, texture);
						}

						auto _formationMainField = il2cpp_class_get_field_from_name(obj->klass, "_formationMain");
						Il2CppObject* _formationMain;
						il2cpp_field_get_value(obj, _formationMainField, &_formationMain);

						if (_formationMain)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_formationMain->klass, "SetRenderTex", 1)->methodPointer(_formationMain, texture);
						}

						// TODO: reposition
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* miniDirectors;
		miniDirectors = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

		if (miniDirectors && miniDirectors->max_length)
		{

			if (delayTweener)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(delayTweener, true);
			}

			auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), uiManager, *([](Il2CppObject* _this)
				{
					ResizeMiniDirector();
					delayTweener = nullptr;
				}));

			// Delay 50ms
			delayTweener = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, &callback->delegate, true);
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "FanRaidViewController"s)
			{
				auto _fanRaidTopSequenceField = il2cpp_class_get_field_from_name(controller->klass, "_fanRaidTopSequence");
				Il2CppObject* _fanRaidTopSequence;
				il2cpp_field_get_value(controller, _fanRaidTopSequenceField, &_fanRaidTopSequence);

				if (_fanRaidTopSequence)
				{
					auto _frameBufferField = il2cpp_class_get_field_from_name(_fanRaidTopSequence->klass, "_frameBuffer");
					Il2CppObject* _frameBuffer;
					il2cpp_field_get_value(_fanRaidTopSequence, _frameBufferField, &_frameBuffer);

					if (_frameBuffer)
					{
						if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_frameBuffer->klass, "get_ColorBuffer", 0)->methodPointer(_frameBuffer))
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
						}
					}
				}
			}

			if (controller->klass->name == "GachaMainViewController"s)
			{
				auto _contextField = il2cpp_class_get_field_from_name(controller->klass, "_context");
				Il2CppObject* _context;
				il2cpp_field_get_value(controller, _contextField, &_context);

				if (_context)
				{
					auto FrameBufferField = il2cpp_class_get_field_from_name(_context->klass, "FrameBuffer");
					Il2CppObject* FrameBuffer;
					il2cpp_field_get_value(_context, FrameBufferField, &FrameBuffer);

					if (FrameBuffer)
					{
						if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(FrameBuffer->klass, "get_ColorBuffer", 0)->methodPointer(FrameBuffer))
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(FrameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(FrameBuffer);
						}
					}
				}
			}

			if (controller->klass->name == "SingleModeSuccessionCutViewController"s ||
				controller->klass->name == "EpisodeMainUnlockRaceCutinViewController"s ||
				controller->klass->name == "SingleModeSuccessionEventViewController"s)
			{
				auto _resultField = il2cpp_class_get_field_from_name(controller->klass, "_result");
				Il2CppObject* _result;
				il2cpp_field_get_value(controller, _resultField, &_result);

				if (_result)
				{
					auto _resultCameraField = il2cpp_class_get_field_from_name(_result->klass, "_resultCamera");
					Il2CppObject* _resultCamera;
					il2cpp_field_get_value(_result, _resultCameraField, &_resultCamera);

					if (_resultCamera)
					{
						auto texture = uiManager.UITexture();
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_resultCamera->klass, "set_targetTexture", 1)->methodPointer(_resultCamera, texture);
					}
				}
			}

			if (string(controller->klass->name).ends_with("PaddockViewController"))
			{
				auto _frameBufferField = il2cpp_class_get_field_from_name(controller->klass, "_frameBuffer");
				Il2CppObject* _frameBuffer;
				il2cpp_field_get_value(controller, _frameBufferField, &_frameBuffer);

				if (_frameBuffer)
				{
					if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_frameBuffer->klass, "get_ColorBuffer", 0)->methodPointer(_frameBuffer))
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
					}
				}
			}

			if (config::freeform_window)
			{
				if (string(controller->klass->name).ends_with("LiveViewController"))
				{
					auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
					auto _fullPortraitRootField = il2cpp_class_get_field_from_name(view->klass, "_fullPortraitRoot");

					if (_fullPortraitRootField)
					{
						Il2CppObject* _fullPortraitRoot;
						il2cpp_field_get_value(view, _fullPortraitRootField, &_fullPortraitRoot);

						if (_fullPortraitRoot)
						{
							UnityEngine::GameObject(_fullPortraitRoot).SetActive(false);
		}
					}
				}
			}
		}

		auto storyManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryManager"));
		if (storyManager)
		{
			auto storySceneController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "StoryManager", "get_StorySceneController", IgnoreNumberOfArguments)();
			if (storySceneController)
			{
				auto DisplayMode = il2cpp_class_get_method_from_name_type<uint64_t(*)(Il2CppObject*)>(storySceneController->klass, "get_DisplayMode", 0)->methodPointer(storySceneController);

				auto storyViewController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "StoryManager", "get_ViewController", 0)(storyManager);

				auto IsSingleModeOrGallery = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(storyViewController->klass, "get_IsSingleModeOrGallery", 0)->methodPointer(storyViewController);

				auto scene = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(storySceneController->klass, "GetSceneBase", 0)->methodPointer(storySceneController);

				if (!IsSingleModeOrGallery)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, uint64_t)>(storyViewController->klass, "SetDisplayMode", 1)->methodPointer(storyViewController, DisplayMode);
				}
				else
				{
					if (DisplayMode == 1)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(storySceneController->klass, "SetDisplayAreaPortrait", 0)->methodPointer(storySceneController);
					}
					else
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(storySceneController->klass, "SetDisplayAreaFullScreen", 0)->methodPointer(storySceneController);
					}

					uint64_t drawDirection = 6;

					if (DisplayMode == 0 || DisplayMode == 3)
					{
						drawDirection = 0;
					}
					else if (DisplayMode == 1)
					{
						drawDirection = 7;
					}

					auto _lowResolutionCameraListField = il2cpp_class_get_field_from_name(storySceneController->klass, "_lowResolutionCameraList");
					Il2CppObject* _lowResolutionCameraList;
					il2cpp_field_get_value(storySceneController, _lowResolutionCameraListField, &_lowResolutionCameraList);

					if (_lowResolutionCameraList)
					{
						FieldInfo* _itemsField = il2cpp_class_get_field_from_name(_lowResolutionCameraList->klass, "_items");
						Il2CppArraySize_t<Il2CppObject*>* _items;
						il2cpp_field_get_value(_lowResolutionCameraList, _itemsField, &_items);

						for (int i = 0; i < _items->max_length; i++)
						{
							auto lowResolutionCamera = _items->vector[i];

							if (lowResolutionCamera)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, uint64_t)>(lowResolutionCamera->klass, "ChangeDirection", 1)->methodPointer(lowResolutionCamera, drawDirection);
							}
						}
					}

					auto FrameBufferDisplayMode = il2cpp_symbols::get_method_pointer<uint64_t(*)(uint64_t)>("umamusume.dll", "Gallop", "LowResolutionCameraUtil", "GetDrawPass", 1)(DisplayMode);

					auto FrameBuffer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(storySceneController->klass, "get_FrameBuffer", 0)->methodPointer(storySceneController);

					if (il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(FrameBuffer->klass, "get_ColorBuffer", 0)->methodPointer(FrameBuffer))
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, uint64_t)>(FrameBuffer->klass, "RemakeRenderTexture", 1)->methodPointer(FrameBuffer, FrameBufferDisplayMode);
					}

					// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, uint64_t)>(storySceneController->klass, "UpdateFovFactor", 1)->methodPointer(storySceneController, DisplayMode);

					auto FullScreenImageRendererField = il2cpp_class_get_field_from_name(scene->klass, "FullScreenImageRenderer");
					Il2CppObject* FullScreenImageRenderer;
					il2cpp_field_get_value(scene, FullScreenImageRendererField, &FullScreenImageRenderer);

					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(FullScreenImageRenderer->klass, "ForceRender", 0)->methodPointer(FullScreenImageRenderer);
				}

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(storyViewController->klass, "SetupUIOnChangeOrientation", 0)->methodPointer(storyViewController);
			}
		}
	}

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* _this, UnityEngine::Vector2 dispRectWH, bool isPanScan);

	void ResizeMoviePlayer()
	{
		auto movieManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "get_Instance", IgnoreNumberOfArguments)();

		if (movieManager)
		{
			auto playerDicField = il2cpp_class_get_field_from_name(movieManager->klass, "playerDic");
			Il2CppObject* playerDic;
			il2cpp_field_get_value(movieManager, playerDicField, &playerDic);

			if (playerDic)
			{
				auto entriesField = il2cpp_class_get_field_from_name(playerDic->klass, "_entries");
				if (!entriesField)
				{
					entriesField = il2cpp_class_get_field_from_name(playerDic->klass, "entries");
				}

				Il2CppArraySize_t<System::Collections::Generic::Dictionary<Cute::Cri::MoviePlayerHandle, Il2CppObject*>::Entry>* entries;
				il2cpp_field_get_value(playerDic, entriesField, &entries);

				if (entries)
				{
					for (int i = 0; i < entries->max_length; i++)
					{
						auto entry = entries->vector[i];

						auto player = entry.value;

						if (player)
						{
							auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(player->klass, "get_gameObject", 0)->methodPointer(player);

							if (gameObject)
							{
								auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gameObject->klass, "get_transform", 0)->methodPointer(gameObject);

								if (transform)
								{
									auto parent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_parent", 0)->methodPointer(transform);

									if (parent)
									{
										auto parentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parent->klass, "get_gameObject", 0)->methodPointer(parent);
										auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize * (*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(parentGameObject->klass, "GetComponentsInternal", 6)->methodPointer;

										if (UnityEngine::Object::Name(parent)->chars == il2cppstring(IL2CPP_STRING("MainCanvas")))
										{
											if (auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryMovieView"))
											{
												auto array1 = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(klass)), true, true, false, false, nullptr);

												if (array1)
												{
													if (array1->max_length > 0)
													{
														auto fullPlayer = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryFullMoviePlayer"));
														auto _handleField = il2cpp_class_get_field_from_name(fullPlayer->klass, "_handle");
														il2cpp_field_set_value(fullPlayer, _handleField, &entry.key);

														il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(fullPlayer->klass, "AdjustMovieSize", 1)->methodPointer(fullPlayer, Gallop::StandaloneWindowResize::IsVirt() ? 0 : 1);

														return;
													}
												}
											}

											auto array2 = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
												"umamusume.dll", "Gallop", "StoryView")), true, true, false, false, nullptr);

											if (array2)
											{
												if (array2->max_length > 0)
												{
													auto controller = GetCurrentViewController();

													auto _wipeControllerField = il2cpp_class_get_field_from_name(controller->klass, "_wipeController");
													Il2CppObject* _wipeController;
													il2cpp_field_get_value(controller, _wipeControllerField, &_wipeController);

													if (_wipeController)
													{
														auto _moviePlayerField = il2cpp_class_get_field_from_name(_wipeController->klass, "_moviePlayer");
														Il2CppObject* _moviePlayer;
														il2cpp_field_get_value(_wipeController, _moviePlayerField, &_moviePlayer);

														if (_moviePlayer)
														{
															auto StoryTimelineController = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineController");
															auto CurrentDisplayModeField = il2cpp_class_get_field_from_name(StoryTimelineController->klass, "CurrentDisplayMode");
															int CurrentDisplayMode;
															il2cpp_field_static_get_value(CurrentDisplayModeField, &CurrentDisplayMode);

															if (CurrentDisplayMode == 3 && !Gallop::StandaloneWindowResize::IsVirt())
															{
																int tmpMode = 2;
																il2cpp_field_static_get_value(CurrentDisplayModeField, &tmpMode);
															}

															il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_moviePlayer->klass, "AdjustScreenSize", 0)->methodPointer(_moviePlayer);

															il2cpp_field_static_set_value(CurrentDisplayModeField, &CurrentDisplayMode);
														}
													}
													return;
												}
											}

											auto newSize = il2cpp_symbols::get_method_pointer<UnityEngine::Vector2(*)()>("umamusume.dll", "Gallop", "MovieScreenSizeHelper", "GetMovieTargetCanvasSize", IgnoreNumberOfArguments)();

											auto criPlayer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(player->klass, "get_Player", 0)->methodPointer(player);

											if (criPlayer)
											{
												auto status = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(criPlayer->klass, "get_status", 0)->methodPointer(criPlayer);
												if (status == 5)
												{
													MoviePlayerForUI_AdjustScreenSize_hook(player, newSize, true);
												}
											}

										}
										else if (parent->klass->name == "RectTransform"s)
										{
											auto parentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parent->klass, "get_gameObject", 0)->methodPointer(parent);
											auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize * (*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(parentGameObject->klass, "GetComponentsInternal", 6)->methodPointer;

											auto array = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
												"umamusume.dll", "Gallop", "PartsEpisodeList")), true, true, false, true, nullptr);

											if (array)
											{
												for (int j = 0; j < array->max_length; j++)
												{
													auto obj =
														il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(array, j);
													if (!obj) continue;

													auto newSize = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)(Il2CppObject*)>(obj->klass, "CalcMovieRectSize", 0)->methodPointer(obj);

													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(parent->klass, "set_sizeDelta", 1)->methodPointer(parent, newSize);

													auto criPlayer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(player->klass, "get_Player", 0)->methodPointer(player);

													if (criPlayer)
													{
														auto status = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(criPlayer->klass, "get_status", 0)->methodPointer(criPlayer);
														if (status == 5)
														{
															MoviePlayerForUI_AdjustScreenSize_hook(player, newSize, true);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void WaitForEndOfFrame(void (*fn)())
	{
		try
		{
			// auto uiManager = Gallop::UIManager::Instance();
			auto gameSystem = Gallop::GameSystem::Instance();
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(gameSystem, CreateDelegateStatic(fn));
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "WaitForEndOfFrame error: " << e.ex->message->chars << endl;
			PrintStackTrace();
		}
	}

	void WaitForEndOfFrame(Il2CppObject* target, void (*fn)(Il2CppObject* _this))
	{
		try
		{
			// auto uiManager = Gallop::UIManager::Instance();
			auto delegate = &CreateUnityAction(target, fn)->delegate;
			auto gameSystem = Gallop::GameSystem::Instance();
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(gameSystem, delegate);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "WaitForEndOfFrame error: " << e.ex->message->chars << endl;
			PrintStackTrace();
		}
	}

	void ResizeWebView()
	{
		if (Cute::Core::WebViewManager::webviewController)
		{
			BOOL isWebViewVisible;
			Cute::Core::WebViewManager::webviewController->get_IsVisible(&isWebViewVisible);

			if (isWebViewVisible)
			{
				auto dialog = Gallop::WebViewManager::Instance()._currentWebViewDialog();

				auto _dataField = il2cpp_class_get_field_from_name(dialog->klass, "_data");
				Il2CppObject* _data;
				il2cpp_field_get_value(dialog, _dataField, &_data);

				UnityEngine::Rect WebViewRectOffset;
				auto WebViewRectOffsetField = il2cpp_class_get_field_from_name(_data->klass, "WebViewRectOffset");
				il2cpp_field_get_value(_data, WebViewRectOffsetField, &WebViewRectOffset);
				Gallop::WebViewManager::Instance().SetMargin(WebViewRectOffset);
			}
		}
	}

	void ResizeWindow(HWND _hWnd, int _updateWidth, int _updateHeight)
	{
		if (_updateWidth < 72 || _updateHeight < 72)
		{
			return;
		}

		static HWND hWnd;
		static int updateWidth;
		static int updateHeight;

		hWnd = _hWnd;
		updateWidth = _updateWidth;
		updateHeight = _updateHeight;

		WaitForEndOfFrame(*[]()
			{
				WaitForEndOfFrame(*[]()
					{
						auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

						auto lastWidthField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastWidth");
						auto lastHeightField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastHeight");

						RECT windowRect;
						GetWindowRect(hWnd, &windowRect);
						int windowWidth = windowRect.right - windowRect.left;
						int windowHeight = windowRect.bottom - windowRect.top;

						RECT clientRect;
						GetClientRect(hWnd, &clientRect);

						int frameWidth = windowWidth - clientRect.right;
						int frameHeight = windowHeight - clientRect.bottom;

						int contentWidth = updateWidth - frameWidth;
						int contentHeight = updateHeight - frameHeight;

						auto ratio = static_cast<float>(contentWidth) / static_cast<float>(contentHeight);

						if (contentWidth < contentHeight)
						{
							last_display_width = contentWidth;
							last_display_height = contentHeight;

							last_virt_window_width = contentWidth;
							last_virt_window_height = contentHeight;
						}
						else
						{
							last_display_width = contentWidth;
							last_display_height = contentHeight;

							last_hriz_window_width = contentWidth;
							last_hriz_window_height = contentHeight;
						}

						auto lastWidth = updateWidth;
						auto lastHeight = updateHeight;

						il2cpp_field_static_set_value(lastWidthField, &lastWidth);
						il2cpp_field_static_set_value(lastHeightField, &lastHeight);

						il2cpp_class_get_method_from_name_type<void (*)(float, float)>(StandaloneWindowResize, "SaveChangedWidth", 2)->methodPointer(lastWidth, lastHeight);

						auto _aspectRatioField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "_aspectRatio");
						float _aspectRatio = contentWidth / contentHeight;
						il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);

						il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
						il2cpp_class_get_method_from_name_type<void (*)()>(StandaloneWindowResize, "DisableWindowHitTest", IgnoreNumberOfArguments)->methodPointer();

						bool isPortrait = contentWidth < contentHeight;

						Gallop::StandaloneWindowResize::IsVirt(isPortrait);

						auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

						auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

						auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

						auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenWidth");

						auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenHeight");


						int unityWidth = UnityEngine::Screen::width();
						int unityHeight = UnityEngine::Screen::height();

						if (unityWidth < unityHeight)
						{
							il2cpp_field_static_set_value(NUMBER1080_Field, &contentWidth);
							il2cpp_field_static_set_value(NUMBER1920_Field, &contentHeight);
							il2cpp_field_static_set_value(_originalScreenHeight_Field, &contentWidth);
							il2cpp_field_static_set_value(_originalScreenWidth_Field, &contentHeight);
						}
						else
						{
							il2cpp_field_static_set_value(NUMBER1920_Field, &contentWidth);
							il2cpp_field_static_set_value(NUMBER1080_Field, &contentHeight);
							il2cpp_field_static_set_value(_originalScreenWidth_Field, &contentWidth);
							il2cpp_field_static_set_value(_originalScreenHeight_Field, &contentHeight);
						}

						auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));

						auto uiManager = Gallop::UIManager::Instance();

						if (uiManager)
						{
							//auto loadingCanvas = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(uiManager, "get_LoadingCanvas", IgnoreNumberOfArguments)->methodPointer();
							//if (loadingCanvas)
							//{
							//	auto canvas = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadingCanvas->klass, "get_Canvas", 0)->methodPointer(loadingCanvas);

							//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(canvas->klass, "set_sortingLayerName", 1)->methodPointer(canvas, il2cpp_string_new("Default"));
							//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvas->klass, "set_sortingOrder", 1)->methodPointer(canvas, 0);
							//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(canvas->klass, "set_overrideSorting", 1)->methodPointer(canvas, true);
							//}

							//auto nowLoading = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "NowLoading", "get_Instance", IgnoreNumberOfArguments)();
							//if (nowLoading)
							//{
							//	auto _bgImageField = il2cpp_class_get_field_from_name(nowLoading->klass, "_bgImage");
							//	Il2CppObject* _bgImage;
							//	il2cpp_field_get_value(nowLoading, _bgImageField, &_bgImage);

							//	if (_bgImage)
							//	{
							//		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_bgImage->klass, "get_gameObject", 0)->methodPointer(_bgImage);
							//		if (gameObject)
							//		{
							//			// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);
							//		}
							//	}

							//	auto _backCanvasGroupField = il2cpp_class_get_field_from_name(nowLoading->klass, "_backCanvasGroup");
							//	Il2CppObject* _backCanvasGroup;
							//	il2cpp_field_get_value(nowLoading, _backCanvasGroupField, &_backCanvasGroup);

							//	if (_backCanvasGroup)
							//	{
							//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_backCanvasGroup->klass, "set_alpha", 1)->methodPointer(_backCanvasGroup, 1);
							//	}

							//	auto ids = il2cpp_symbols::get_method_pointer<Il2CppArraySize * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "GetSortingLayerIDsInternal", IgnoreNumberOfArguments)();
							//	for (int i = 0; i < ids->max_length; i++)
							//	{
							//		auto id = reinterpret_cast<int>(ids->vector[i]);
							//		cout << "Layer Id: " << id << endl;
							//		cout << "Layer name: " << wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "IDToName", 1)(id)->chars) << endl;
							//	}

							//	auto _activeHorseShoeParticleField = il2cpp_class_get_field_from_name(nowLoading->klass, isPortrait ? "_horseShoeParticleVertical" : "_horseShoeParticleHorizontal");
							//	Il2CppObject* _activeHorseShoeParticle;
							//	il2cpp_field_get_value(nowLoading, _activeHorseShoeParticleField, &_activeHorseShoeParticle);

							//	if (_activeHorseShoeParticle)
							//	{
							//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_activeHorseShoeParticle->klass, "SetActive", 1)->methodPointer(_activeHorseShoeParticle, true);
							//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_activeHorseShoeParticle->klass, "SetParticleAlpha", 1)->methodPointer(_activeHorseShoeParticle, 1);
							//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, int)>(_activeHorseShoeParticle->klass, "SetLayer", 2)->methodPointer(_activeHorseShoeParticle, il2cpp_string_new("Default"), 0);

							//		auto _particleRendererField = il2cpp_class_get_field_from_name(_activeHorseShoeParticle->klass, "_particleRenderer");
							//		Il2CppObject* _particleRenderer;
							//		il2cpp_field_get_value(_activeHorseShoeParticle, _particleRendererField, &_particleRenderer);

							//		if (_particleRenderer)
							//		{
							//			reinterpret_cast<void (*)(Il2CppObject*, int)>(il2cpp_resolve_icall("UnityEngine.Renderer::set_rendererPriority(System.Int32)"))(_particleRenderer, 0);
							//		}
							//	}

							//	auto _activeHorseShoeParticle1Field = il2cpp_class_get_field_from_name(nowLoading->klass, isPortrait ? "_horseShoeParticleHorizontal" : "_horseShoeParticleVertical");
							//	Il2CppObject* _activeHorseShoeParticle1;
							//	il2cpp_field_get_value(nowLoading, _activeHorseShoeParticle1Field, &_activeHorseShoeParticle1);

							//	if (_activeHorseShoeParticle1)
							//	{
							//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_activeHorseShoeParticle1->klass, "SetActive", 1)->methodPointer(_activeHorseShoeParticle1, false);
							//	}
							//}

							/*auto _bgCameraSettingsField = il2cpp_class_get_field_from_name(GallopScreen, "_bgCameraSettings");

							Il2CppObject* _bgCameraSettings;
							il2cpp_symbols::get_method_pointer<void (*)(bool, Il2CppObject**)>("umamusume.dll", "Gallop", "Screen", "InitializeChangeScaleForPC", 2)(isPortrait, &_bgCameraSettings);

							il2cpp_field_static_set_value(_bgCameraSettingsField, &_bgCameraSettings);*/
							/*auto _bgCameraField = il2cpp_class_get_field_from_name(uiManager, "_bgCamera");
							Il2CppObject* _bgCamera;

							il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
								il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());*/

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Disable", 0)->methodPointer(tapEffectController);

							uiManager.SetCameraSizeByOrientation(UnityEngine::ScreenOrientation::Portrait);
						}

						auto anRootManager = GetSingletonInstance(il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", "AnRootManager"));

						if (anRootManager)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(anRootManager->klass, "set_ScreenRate", 1)->methodPointer(anRootManager, _aspectRatio);
						}

						// il2cpp_class_get_method_from_name_type<void (*)()>(GallopScreen, "UpdateForPC", IgnoreNumberOfArguments)->methodPointer();

						if (uiManager)
						{
							// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(uiManager, "ChangeResizeUIForPC", 2)->methodPointer(uiManager, lastWidth, lastHeight);

							// AutoRotation
							// il2cpp_symbols::get_method_pointer<void (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "set_orientation", 1)(5);

							auto gameObject = uiManager.gameObject();

							auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gameObject, "get_transform", 0)->methodPointer(gameObject);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(transform->klass, "set_localScale", 1)->methodPointer(transform, UnityEngine::Vector3{ 1, 1, 1 });

							// auto _bgCameraSettingsField = il2cpp_class_get_field_from_name(GallopScreen, "_bgCameraSettings");

							// Il2CppObject* _bgCameraSettings;
							// il2cpp_field_static_get_value(_bgCameraSettingsField, &_bgCameraSettings);

							// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject**)>(uiManager, "EndOrientation", 1)->methodPointer(uiManager, &_bgCameraSettings);

							// il2cpp_field_static_set_value(_bgCameraSettingsField, &_bgCameraSettings);

							if (tapEffectController)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);
							}

							Il2CppArraySize_t<Il2CppObject*>* canvasScalerList;
							canvasScalerList = UnityEngine::Object::FindObjectsByType(
								GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
							// auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(uiManager, "GetCanvasScalerList", 0)->methodPointer(uiManager);

							for (int i = 0; i < canvasScalerList->max_length; i++)
							{
								auto canvasScaler = canvasScalerList->vector[i];
								if (canvasScaler)
								{
									auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasScaler->klass, "get_gameObject", 0)->methodPointer(canvasScaler);

									bool keepActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(gameObject->klass, "get_activeSelf", 0)->methodPointer(gameObject);

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

									/*if (isPortrait)
									{
										float scale = min(config::freeform_ui_scale_portrait, max(1.0f, contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, UnityEngine::Vector2{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
									}
									else
									{
										float scale = min(config::freeform_ui_scale_landscape, max(1.0f, contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, UnityEngine::Vector2{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
									}*/

									// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvasScaler->klass, "set_uiScaleMode", 1)->methodPointer(canvasScaler, 0);

									// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvasScaler->klass, "set_screenMatchMode", 1)->methodPointer(canvasScaler, 0);

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, keepActive);

									auto scaleMode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(canvasScaler->klass, "get_uiScaleMode", 0)->methodPointer(canvasScaler);

									if (scaleMode == 1)
									{
										if (isPortrait)
										{
											float scale = min(config::freeform_ui_scale_portrait, max(1.0f, contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, UnityEngine::Vector2{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
										}
										else
										{
											float scale = min(config::freeform_ui_scale_landscape, max(1.0f, contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, UnityEngine::Vector2{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
										}
									}

									if (scaleMode == 0)
									{
										if (isPortrait)
										{
											float scale = min(config::freeform_ui_scale_portrait, max(1.0f, contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
										}
										else
										{
											float scale = min(config::freeform_ui_scale_landscape, max(1.0f, contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
										}
									}
								}
							}

							SetBGCanvasScalerSize();
						}

						static int _contentWidth;
						static int _contentHeight;
						_contentWidth = contentWidth;
						_contentHeight = contentHeight;

						WaitForEndOfFrame(*[]()
							{
								auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

								auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));

								bool isPortrait = _contentWidth < _contentHeight;

								auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

								auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

								auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

								auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenWidth");

								auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenHeight");

								auto uiManager = Gallop::UIManager::Instance();

								if (uiManager)
								{
									uiManager.AdjustSafeArea();
									Il2CppObject* _bgManager = uiManager._bgManager();
									if (_bgManager)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_bgManager->klass, "OnChangeResolutionByGraphicsSettings", 0)->methodPointer(_bgManager);
									}

									uiManager.CheckUIToFrameBufferBlitInstance();
									uiManager.ReleaseRenderTexture();

									auto renderTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture"));
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int, int)>(renderTexture->klass, ".ctor", 3)->methodPointer(renderTexture, _contentWidth, _contentHeight, 24);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderTexture->klass, "set_autoGenerateMips", 1)->methodPointer(renderTexture, false);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderTexture->klass, "set_useMipMap", 1)->methodPointer(renderTexture, false);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderTexture->klass, "set_antiAliasing", 1)->methodPointer(renderTexture, 1);

									uiManager.UITexture(renderTexture);

									if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(renderTexture->klass, "Create", 0)->methodPointer(renderTexture))
									{
										uiManager.ReleaseRenderTexture();
									}

									Il2CppObject* _uiToFrameBufferRenderCameraData = uiManager._uiToFrameBufferRenderCameraData();

									if (_uiToFrameBufferRenderCameraData)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_uiToFrameBufferRenderCameraData->klass, "set_ScreenTexture", 1)->methodPointer(_uiToFrameBufferRenderCameraData, renderTexture);
									}

									Il2CppObject* _uiCommandBuffer = uiManager._uiCommandBuffer();

									if (_uiCommandBuffer)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, UnityEngine::Rendering::RenderTargetIdentifier, Il2CppObject*)>(_uiCommandBuffer->klass, "Blit", 3)->methodPointer(_uiCommandBuffer, renderTexture, UnityEngine::Rendering::RenderTargetIdentifier(UnityEngine::Rendering::BuiltinRenderTextureType::CurrentActive), uiManager._blitToFrameMaterial());
									}

									Il2CppObject* _uiCamera = uiManager._uiCamera();
									Il2CppObject* _bgCamera = uiManager._bgCamera();

									Il2CppObject* _noImageEffectUICamera = uiManager._noImageEffectUICamera();
									Il2CppObject* _uiToFrameBufferBlitCamera = uiManager._uiToFrameBufferBlitCamera();

									if (_uiCamera)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_uiCamera->klass, "set_targetTexture", 1)->methodPointer(_uiCamera, renderTexture);
									}

									if (_bgCamera)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_bgCamera->klass, "set_targetTexture", 1)->methodPointer(_bgCamera, renderTexture);
									}

									if (_noImageEffectUICamera)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_noImageEffectUICamera->klass, "set_targetTexture", 1)->methodPointer(_noImageEffectUICamera, renderTexture);
									}

									if (_uiToFrameBufferBlitCamera)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_uiToFrameBufferBlitCamera->klass, "set_enabled", 1)->methodPointer(_uiToFrameBufferBlitCamera, true);
									}
								}

								RemakeTextures();

								ResizeWebView();

								auto raceCameraManager = Gallop::RaceCameraManager::Instance();
								if (raceCameraManager)
								{
									raceCameraManager.SetupOrientation(isPortrait ? Gallop::LowResolutionCameraUtil::DrawDirection::Portrait : Gallop::LowResolutionCameraUtil::DrawDirection::Landscape);
								}

								auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
								if (director)
								{
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(director->klass, "SetupOrientation", 1)->methodPointer(director, isPortrait ? 2 : 1);

									auto ChampionsTextControllerField = il2cpp_class_get_field_from_name(director->klass, "ChampionsTextController");
									Il2CppObject* ChampionsTextController;
									il2cpp_field_get_value(director, ChampionsTextControllerField, &ChampionsTextController);

									if (ChampionsTextController)
									{
										auto _flashPlayerField = il2cpp_class_get_field_from_name(ChampionsTextController->klass, "_flashPlayer");
										Il2CppObject* _flashPlayer;
										il2cpp_field_get_value(ChampionsTextController, _flashPlayerField, &_flashPlayer);

										auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

										float scale = 1.0f;

										if (_contentWidth < _contentHeight)
										{
											scale = min(config::freeform_ui_scale_portrait, max(1.0f, _contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
										}
										else
										{
											scale = min(config::freeform_ui_scale_landscape, max(1.0f, _contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
										}

										float availableWidth = static_cast<float>(_contentWidth) / scale;
										float availableHeight = static_cast<float>(_contentHeight) / scale;

										float width = ratio_16_9 * availableHeight;
										float height = availableHeight;

										if (width > availableWidth)
										{
											width = availableWidth;
											height = width / ratio_16_9;
										}

										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ width, height });
									}


									auto liveFlashController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(director->klass, "get_LiveFlashController", 0)->methodPointer(director);

									if (liveFlashController)
									{
										auto _flashPlayerField = il2cpp_class_get_field_from_name(liveFlashController->klass, "_flashPlayer");

										if (_flashPlayerField)
										{
											Il2CppObject* _flashPlayer;
											il2cpp_field_get_value(liveFlashController, _flashPlayerField, &_flashPlayer);

											auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

											float scale = 1.0f;

											if (_contentWidth < _contentHeight)
											{
												scale = min(config::freeform_ui_scale_portrait, max(1.0f, _contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
											}
											else
											{
												scale = min(config::freeform_ui_scale_landscape, max(1.0f, _contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
											}

											float availableWidth = static_cast<float>(_contentWidth) / scale;
											float availableHeight = static_cast<float>(_contentHeight) / scale;

											float width = ratio_16_9 * availableHeight;
											float height = availableHeight;

											if (width > availableWidth)
											{
												width = availableWidth;
												height = width / ratio_16_9;
											}

											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ width, height });
										}
									}
								}

								if (tapEffectController)
								{
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "RefreshAll", 0)->methodPointer(tapEffectController);
								}

								if (uiManager)
								{
									uiManager.AdjustMissionClearContentsRootRect();
									uiManager.AdjustSafeAreaToAnnounceRect();

									/*Il2CppObject* _bgCamera = uiManager._bgCamera();

									if (_bgCamera)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
											il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());
									}*/
								}

								if (isPortrait)
								{
									il2cpp_field_static_set_value(NUMBER1080_Field, &_contentWidth);
									il2cpp_field_static_set_value(NUMBER1920_Field, &_contentHeight);
									il2cpp_field_static_set_value(_originalScreenHeight_Field, &_contentWidth);
									il2cpp_field_static_set_value(_originalScreenWidth_Field, &_contentHeight);
								}
								else
								{
									il2cpp_field_static_set_value(NUMBER1920_Field, &_contentWidth);
									il2cpp_field_static_set_value(NUMBER1080_Field, &_contentHeight);
									il2cpp_field_static_set_value(_originalScreenWidth_Field, &_contentWidth);
									il2cpp_field_static_set_value(_originalScreenHeight_Field, &_contentHeight);
								}

								il2cpp_class_get_method_from_name_type<void (*)()>(StandaloneWindowResize, "EnableWindowHitTest", IgnoreNumberOfArguments)->methodPointer();
								// il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(false);

							});
					});
			});


		long style = GetWindowLongW(hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
	}

	bool isRequestChangeResolution = false;

	bool IsChangingResolution()
	{
		bool IsRunChangeView = false;
		bool IsRunChangeScene = false;

		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
		if (sceneManager)
		{
			IsRunChangeView = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(sceneManager->klass, "get_IsRunChangeView", 0)->methodPointer(sceneManager);
			IsRunChangeScene = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(sceneManager->klass, "get_IsRunChangeScene", 0)->methodPointer(sceneManager);
		}

		auto uiManager = Gallop::UIManager::Instance();
		if (uiManager)
		{
			Il2CppObject* _noImageEffectCanvas = uiManager._noImageEffectCanvas();

			if (_noImageEffectCanvas)
			{
				auto camera = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_noImageEffectCanvas->klass, "get_worldCamera", 0)->methodPointer(_noImageEffectCanvas);
				if (camera)
				{
					return IsRunChangeView || IsRunChangeScene || il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(camera->klass, "get_enabled", 0)->methodPointer(camera);
				}
			}
		}

		return false;
	}

	void SetResolution_Injected_hook(int width, int height, int fullscreenMode, UnityEngine::RefreshRate* perferredRefreshRate)
	{
		if (width < 72)
		{
			if (Gallop::StandaloneWindowResize::IsVirt())
			{
				width = 72;
			}
			else
			{
				width = 128;
			}
		}

		if (height < 72)
		{
			if (Gallop::StandaloneWindowResize::IsVirt())
			{
				height = 128;
			}
			else
			{
				height = 72;
			}
		}

		if (config::freeform_window)
		{
			auto hWnd = GetHWND();

			long style = GetWindowLongW(hWnd, GWL_STYLE);
			style |= WS_MAXIMIZEBOX;
			SetWindowLongPtrW(hWnd, GWL_STYLE, style);

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

			isRequestChangeResolution = true;
			return;
		}

		UnityEngine::Resolution r;
		get_resolution(&r);

		bool reqVirt = width < height;

		bool unlockSize = config::unlock_size || config::freeform_window;

		if (Gallop::StandaloneWindowResize::IsVirt() && fullScreenFl)
		{
			fullScreenFl = false;
			fullScreenFlOverride = false;
			if (unlockSize)
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
			}
			else
			{
				r.height *= 0.825f;
				r.width = r.height * config::runtime::ratioVertical;
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(r.width, r.height, 3, perferredRefreshRate);
			}
			return;
		}

		auto display = display_get_main();

		if (reqVirt && (get_rendering_width(display) > get_rendering_height(display)))
		{
			fullScreenFl = false;
			fullScreenFlOverride = false;
			if (unlockSize)
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
			}
			else
			{
				r.height *= 0.825f;
				r.width = r.height * config::runtime::ratioVertical;
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(r.width, r.height, 3, perferredRefreshRate);
			}
			return;
		}

		bool need_fullscreen = false;

		if (config::auto_fullscreen)
		{
			auto ratio = static_cast<float>(r.width) / static_cast<float>(r.height);
			ratio *= 1000;
			ratio = roundf(ratio) / 1000;

			if (Gallop::StandaloneWindowResize::IsVirt() && ratio == ratio_9_16)
			{
				need_fullscreen = true;
			}
			else if (!Gallop::StandaloneWindowResize::IsVirt() && ratio == ratio_16_9)
			{
				need_fullscreen = true;
			}
		}

		if (!fullScreenFl && unlockSize)
		{
			if (!(get_rendering_width(display) > get_rendering_height(display)))
			{
				last_virt_window_width = get_rendering_width(display);
				last_virt_window_height = get_rendering_height(display);
				if (need_fullscreen && (!last_hriz_window_width || !last_hriz_window_height))
				{
					last_hriz_window_width = r.width - 400;
					last_hriz_window_height = last_hriz_window_width * config::runtime::ratioHorizontal;
				}
			}
			else
			{
				last_hriz_window_width = get_rendering_width(display);
				last_hriz_window_height = get_rendering_height(display);
			}
		}

		if (!fullScreenFlOverride)
		{
			fullScreenFl = need_fullscreen;
		}

		if (!reqVirt && !fullScreenFl && last_hriz_window_width && last_hriz_window_height && unlockSize)
		{
			width = last_hriz_window_width;
			height = last_hriz_window_height;
		}

		if (unlockSize)
		{
			auto ratio = static_cast<float>(width) / static_cast<float>(height);
			ratio *= 1000;
			ratio = roundf(ratio) / 1000;

			auto round_ratio_vertical = config::runtime::ratioVertical * 1000;
			round_ratio_vertical = roundf(round_ratio_vertical) / 1000;

			auto round_ratio_horizontal = config::runtime::ratioHorizontal * 1000;
			round_ratio_horizontal = roundf(round_ratio_horizontal) / 1000;

			if (reqVirt && ratio != round_ratio_vertical)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				if (last_virt_window_width > last_virt_window_height)
				{
					auto display = display_get_main();
					if (config::initial_width < config::initial_height)
					{
						last_virt_window_height = get_system_width(display) - 400;
						last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;

						if (last_virt_window_height >= get_system_height(display))
						{
							last_virt_window_height = get_system_height(display) - 400;
							last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;
						}
					}
					else
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;

						if (last_virt_window_height >= get_system_height(display))
						{
							last_virt_window_height = get_system_height(display) - 400;
							last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;
						}
					}
				}

				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
				return;

			}

			if (!reqVirt && ratio != round_ratio_horizontal)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				if (last_hriz_window_width < last_hriz_window_height)
				{
					auto display = display_get_main();
					if (config::initial_width < config::initial_height)
					{
						last_hriz_window_width = get_system_height(display) - 400;
						last_hriz_window_height = last_hriz_window_width / config::runtime::ratioHorizontal;

						if (last_hriz_window_height >= get_system_height(display))
						{
							last_hriz_window_height = get_system_height(display) - 400;
							last_hriz_window_width = last_hriz_window_height * config::runtime::ratioHorizontal;
						}
					}
					else
					{
						last_hriz_window_width = get_system_width(display) - 400;
						last_hriz_window_height = last_hriz_window_width / config::runtime::ratioHorizontal;

						if (last_hriz_window_height >= get_system_height(display))
						{
							last_hriz_window_height = get_system_height(display) - 400;
							last_hriz_window_width = last_hriz_window_height * config::runtime::ratioHorizontal;
						}
					}
				}

				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_hriz_window_width, last_hriz_window_height, 3, perferredRefreshRate);
				return;
			}
		}

		reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(
			fullScreenFl ? r.width : width, fullScreenFl ? r.height : height, fullScreenFl ? 1 : 3, perferredRefreshRate);
	}

	static bool isExitOpened = false;

	void ShowExitDialog()
	{
		if (isExitOpened)
		{
			return;
		}

		auto dialogData = Gallop::DialogCommon::Data();
		dialogData.SetSimpleTwoButtonMessage(
			Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Title0040"))),
			Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Title0041"))),
			CreateDelegateStatic(*[]()
				{
					isExitOpened = false;
					UnityEngine::Application::Exit(0);
				}),
			GetTextIdByName(IL2CPP_STRING("Common0004")),
			GetTextIdByName(IL2CPP_STRING("Common0003")),
			CreateDelegateStatic(*[]()
				{
					isExitOpened = false;
				})
		);

		isExitOpened = true;

		Gallop::DialogManager::PushSystemDialog(dialogData, true);
	}

	void PressButton(Il2CppObject* button)
	{
		auto get_gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0);
		auto pointerDownHandler = il2cpp_symbols::get_method_pointer<Il2CppDelegate * (*)()>("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "get_pointerDownHandler", IgnoreNumberOfArguments)();
		auto pointerUpHandler = il2cpp_symbols::get_method_pointer<Il2CppDelegate * (*)()>("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "get_pointerUpHandler", IgnoreNumberOfArguments)();
		auto pointerClickHandler = il2cpp_symbols::get_method_pointer<Il2CppDelegate * (*)()>("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "get_pointerClickHandler", IgnoreNumberOfArguments)();

		auto eventSystem = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("UnityEngine.UI.dll", "UnityEngine.EventSystems", "EventSystem", "get_current", IgnoreNumberOfArguments)();
		auto eventData = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.EventSystems", "PointerEventData"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(eventData->klass, ".ctor", 1)->methodPointer(eventData, eventSystem);

		do
		{
			auto ExecuteEvents_Execute_Method = GetGenericMethod(il2cpp_symbols::get_method("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "Execute", 3),
				GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.EventSystems", "IPointerDownHandler"));

			void** params = new void* [3];
			params[0] = get_gameObject(button);
			params[1] = eventData;
			params[2] = pointerDownHandler;

			Il2CppException* exception;

			il2cpp_runtime_invoke_type<bool>(ExecuteEvents_Execute_Method, nullptr, params, &exception);
			delete[] params;
		} while (false);

		do
		{
			auto ExecuteEvents_Execute_Method = GetGenericMethod(il2cpp_symbols::get_method("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "Execute", 3),
				GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.EventSystems", "IPointerUpHandler"));

			void** params = new void* [3];
			params[0] = get_gameObject(button);
			params[1] = eventData;
			params[2] = pointerUpHandler;

			Il2CppException* exception;

			il2cpp_runtime_invoke_type<bool>(ExecuteEvents_Execute_Method, nullptr, params, &exception);
			delete[] params;
		} while (false);

		do
		{
			auto ExecuteEvents_Execute_Method = GetGenericMethod(il2cpp_symbols::get_method("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents", "Execute", 3),
				GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.EventSystems", "IPointerClickHandler"));

			void** params = new void* [3];
			params[0] = get_gameObject(button);
			params[1] = eventData;
			params[2] = pointerClickHandler;

			Il2CppException* exception;

			il2cpp_runtime_invoke_type<bool>(ExecuteEvents_Execute_Method, nullptr, params, &exception);
			delete[] params;
		} while (false);
	}

	bool PressDialogButton(WPARAM wParam)
	{
		if (!(wParam == VK_SPACE || wParam == VK_RETURN))
		{
			return false;
		}

		bool isExistDialog = il2cpp_symbols::get_method_pointer<bool (*)()>(
			"umamusume.dll", "Gallop", "DialogManager", "get_IsExistDialog", IgnoreNumberOfArguments)();

		if (!isExistDialog)
		{
			return false;
		}

		auto dialog = GetFrontDialog();

		if (!dialog)
		{
			return false;
		}

		auto _dataField = il2cpp_class_get_field_from_name(dialog->klass, "_data");
		Il2CppObject* _data;
		il2cpp_field_get_value(dialog, _dataField, &_data);

		if (!_data)
		{
			return false;
		}

		auto FormTypeField = il2cpp_class_get_field_from_name(_data->klass, "FormType");

		Gallop::DialogCommonBase::FormType type = Gallop::DialogCommonBase::FormType::WITHOUT_FRAME;
		il2cpp_field_get_value(_data, FormTypeField, &type);

		bool isOneButton = type == Gallop::DialogCommonBase::FormType::SMALL_ONE_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_ONE_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON;
		bool isTwoButton = type == Gallop::DialogCommonBase::FormType::SMALL_TWO_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_TWO_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON;
		bool isThreeButton = type == Gallop::DialogCommonBase::FormType::SMALL_THREE_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_THREE_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_THREE_BUTTON;

		auto _currentDialogObjField = il2cpp_class_get_field_from_name(dialog->klass, "_currentDialogObj");

		if (!_currentDialogObjField)
		{
			return false;
		}

		Il2CppObject* _currentDialogObj;
		il2cpp_field_get_value(dialog, _currentDialogObjField, &_currentDialogObj);

		if (!_currentDialogObj)
		{
			return false;
		}

		// auto leftButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObj->klass, "get_LeftButton", 0)->methodPointer(_currentDialogObj);
		auto centerButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObj->klass, "get_CenterButton", 0)->methodPointer(_currentDialogObj);
		auto rightButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObj->klass, "get_RightButton", 0)->methodPointer(_currentDialogObj);

		if (isOneButton)
		{
			if (wParam == VK_RETURN || wParam == VK_SPACE)
			{
				PressButton(centerButton);
				return true;
			}
		}

		if (isTwoButton)
		{
			if (wParam == VK_RETURN || wParam == VK_SPACE)
			{
				PressButton(rightButton);
				return true;
			}
		}

		if (isThreeButton)
		{
			if (wParam == VK_RETURN)
			{
				PressButton(rightButton);
				return true;
			}

			if (wParam == VK_SPACE)
			{
				PressButton(centerButton);
				return true;
			}
		}

		return false;
	}

	bool PressSingleModeButton(WPARAM wParam)
	{
		if (!(wParam == 'Q' || wParam == 'W' || wParam == 'E' ||
			wParam == 'A' || wParam == 'S' || wParam == 'D' || wParam == 'F'))
		{
			return false;
		}

		if (GetFrontDialog())
		{
			return false;
		}

		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));

		if (!sceneManager)
		{
			return false;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "SingleModeMainViewController"s)
			{
				auto IsStoryActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(controller->klass, "get_IsStoryActive", 0)->methodPointer(controller);

				if (IsStoryActive)
				{
					return false;
				}

				auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
				if (view)
				{
					auto stablesPanel = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_StablesPanel", 0)->methodPointer(view);
					if (stablesPanel)
					{
						auto get_gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0);
						auto get_activeSelf = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_activeSelf", 0);

						auto panelIsActive = get_activeSelf(get_gameObject(stablesPanel));

						if (!panelIsActive)
						{
							return false;
						}

						auto _nextMonthButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_nextMonthButton");
						Il2CppObject* _nextMonthButton;
						il2cpp_field_get_value(stablesPanel, _nextMonthButtonField, &_nextMonthButton);

						auto _skillGetButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_skillGetButton");
						Il2CppObject* _skillGetButton;
						il2cpp_field_get_value(stablesPanel, _skillGetButtonField, &_skillGetButton);

						auto _outingSummerButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_outingSummerButton");
						Il2CppObject* _outingSummerButton;
						il2cpp_field_get_value(stablesPanel, _outingSummerButtonField, &_outingSummerButton);

						auto _trainingButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_trainingButton");
						Il2CppObject* _trainingButton;
						il2cpp_field_get_value(stablesPanel, _trainingButtonField, &_trainingButton);

						auto _outingButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_outingButton");
						Il2CppObject* _outingButton;
						il2cpp_field_get_value(stablesPanel, _outingButtonField, &_outingButton);

						Il2CppObject* outingButton = nullptr;
						if (_outingButton)
						{
							outingButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_outingButton->klass, "get_Button", 0)->methodPointer(_outingButton);
						}

						auto _hospitalButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_hospitalButton");
						Il2CppObject* _hospitalButton;
						il2cpp_field_get_value(stablesPanel, _hospitalButtonField, &_hospitalButton);

						Il2CppObject* hospitalButton = nullptr;
						if (_hospitalButton)
						{
							hospitalButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_hospitalButton->klass, "get_Button", 0)->methodPointer(_hospitalButton);
						}

						auto _raceRegistButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_raceRegistButton");
						Il2CppObject* _raceRegistButton;
						il2cpp_field_get_value(stablesPanel, _raceRegistButtonField, &_raceRegistButton);

						Il2CppObject* raceRegistButton = nullptr;
						if (_raceRegistButton)
						{
							raceRegistButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_raceRegistButton->klass, "get_Button", 0)->methodPointer(_raceRegistButton);
						}

						auto _targetRaceSkillGetButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_targetRaceSkillGetButton");
						Il2CppObject* _targetRaceSkillGetButton;
						il2cpp_field_get_value(stablesPanel, _targetRaceSkillGetButtonField, &_targetRaceSkillGetButton);

						Il2CppObject* targetRaceSkillGetButton = nullptr;
						if (_targetRaceSkillGetButton)
						{
							targetRaceSkillGetButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_targetRaceSkillGetButton->klass, "get_Button", 0)->methodPointer(_targetRaceSkillGetButton);
						}

						auto _targetRaceRegistButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_targetRaceRegistButton");
						Il2CppObject* _targetRaceRegistButton;
						il2cpp_field_get_value(stablesPanel, _targetRaceRegistButtonField, &_targetRaceRegistButton);

						auto _addonScenarioButtonField = il2cpp_class_get_field_from_name(stablesPanel->klass, "_addonScenarioButton");
						Il2CppObject* _addonScenarioButton;
						il2cpp_field_get_value(stablesPanel, _addonScenarioButtonField, &_addonScenarioButton);

						Il2CppObject* addonScenarioButton = nullptr;
						if (_addonScenarioButton)
						{
							addonScenarioButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_addonScenarioButton->klass, "get_Button", 0)->methodPointer(_addonScenarioButton);
						}

						auto model = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(stablesPanel->klass, "get_Model", 0)->methodPointer(stablesPanel);
						auto isOnlyRaceEntry = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(model->klass, "get_IsOnlyRaceEntry", 0)->methodPointer(model);

						if (isOnlyRaceEntry)
						{
							if (wParam == 'Q')
							{
								PressButton(targetRaceSkillGetButton);
								return true;
							}

							if (wParam == 'W')
							{
								PressButton(_targetRaceRegistButton);
								return true;
							}

							if (wParam == 'E' && addonScenarioButton)
							{
								PressButton(addonScenarioButton);
								return true;
							}
						}
						else
						{
							if (wParam == 'Q')
							{
								PressButton(_nextMonthButton);
								return true;
							}

							if (wParam == 'W')
							{
								PressButton(_trainingButton);
								return true;
							}

							if (wParam == 'E')
							{
								PressButton(_skillGetButton);
								return true;
							}

							if (wParam == 'A')
							{
								PressButton(hospitalButton);
								return true;
							}

							if (wParam == 'S')
							{
								auto isActive = get_activeSelf(get_gameObject(outingButton));

								if (isActive)
								{
									PressButton(outingButton);
								}
								else
								{
									PressButton(_outingSummerButton);
								}
								return true;
							}

							if (wParam == 'D' && addonScenarioButton)
							{
								PressButton(addonScenarioButton);
								return true;
							}

							if (wParam == 'F')
							{
								PressButton(raceRegistButton);
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}

	bool PressSingleModeRaceEntry(WPARAM wParam)
	{
		if (!(wParam == VK_RETURN || wParam == VK_SPACE || wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN))
		{
			return false;
		}

		if (GetFrontDialog())
		{
			return false;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "SingleModeRaceEntryViewController"s)
			{
				auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
				if (view)
				{
					auto get_gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0);
					auto get_activeSelf = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_activeSelf", 0);

					if (wParam == VK_RETURN || wParam == VK_SPACE)
					{
						auto entryButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_EntryButton", 0)->methodPointer(view);
						auto entryIsActive = get_activeSelf(get_gameObject(entryButton));

						auto reserveButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_ReserveButton", 0)->methodPointer(view);
						auto reserveIsActive = get_activeSelf(get_gameObject(reserveButton));

						auto cancelReserveButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_CancelReserveButton", 0)->methodPointer(view);
						auto cancelReserveIsActive = get_activeSelf(get_gameObject(cancelReserveButton));

						if (entryIsActive)
						{
							PressButton(entryButton);
							return true;
						}

						if (reserveIsActive)
						{
							PressButton(reserveButton);
							return true;
						}

						if (cancelReserveIsActive)
						{
							PressButton(cancelReserveButton);
							return true;
						}
					}

					bool shiftKeyDown = GetKeyState(VK_SHIFT) < 0;

					if (wParam == VK_LEFT || wParam == VK_RIGHT)
					{
						auto leftArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_LeftArrowButton", 0)->methodPointer(view);

						Il2CppObject* leftSkipArrowButton = nullptr;

						auto get_LeftSkipArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_LeftSkipArrowButton", 0);
						if (get_LeftSkipArrowButton)
						{
							leftSkipArrowButton = get_LeftSkipArrowButton->methodPointer(view);
						}

						auto rightArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_RightArrowButton", 0)->methodPointer(view);

						Il2CppObject* rightSkipArrowButton = nullptr;

						auto get_RightSkipArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_RightSkipArrowButton", 0);
						if (get_RightSkipArrowButton)
						{
							rightSkipArrowButton = get_RightSkipArrowButton->methodPointer(view);
						}

						if (wParam == VK_LEFT)
						{
							if (shiftKeyDown && leftSkipArrowButton)
							{
								PressButton(leftSkipArrowButton);
							}
							else
							{
								PressButton(leftArrowButton);
							}

							return true;
						}

						if (wParam == VK_RIGHT)
						{
							if (shiftKeyDown && rightSkipArrowButton)
							{
								PressButton(rightSkipArrowButton);
							}
							else
							{
								PressButton(rightArrowButton);
							}

							return true;
						}
					}
				}

				if (wParam == VK_UP || wParam == VK_DOWN)
				{
					auto _itemListField = il2cpp_class_get_field_from_name(controller->klass, "_itemList");
					Il2CppObject* _itemList;
					il2cpp_field_get_value(controller, _itemListField, &_itemList);

					if (_itemList)
					{
						FieldInfo* itemsField = il2cpp_class_get_field_from_name(_itemList->klass, "_items");
						Il2CppArraySize_t<Il2CppObject*>* arr;
						il2cpp_field_get_value(_itemList, itemsField, &arr);
					}
				}
			}
		}

		return false;
	}

	bool PressSingleModePaddock(WPARAM wParam)
	{
		if (!(wParam == VK_RETURN || wParam == VK_SPACE))
		{
			return false;
		}

		if (GetFrontDialog())
		{
			return false;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "SingleModePaddockViewController"s)
			{
				auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
				if (view)
				{
					auto holder = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_PaddockContentsHolder", 0)->methodPointer(view);

					if (holder)
					{
						auto get_gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0);
						auto get_activeSelf = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_activeSelf", 0);

						auto raceStartButtonCenter = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_RaceStartButtonCenter", 0)->methodPointer(holder);
						auto raceStartButtonCenterIsActive = get_activeSelf(get_gameObject(raceStartButtonCenter));

						auto raceSkipButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_RaceSkipButton", 0)->methodPointer(holder);

						auto raceStartButtonRight = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_RaceStartButtonRight", 0)->methodPointer(holder);

						if (raceStartButtonCenterIsActive)
						{
							PressButton(raceStartButtonCenter);
							return true;
						}
						else if (wParam == VK_SPACE)
						{
							PressButton(raceSkipButton);
							return true;
						}
						else if (wParam == VK_RETURN)
						{
							PressButton(raceStartButtonRight);
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	bool isNumKeyDown = false;

	bool PressSingleModeLiveSelect(WPARAM wParam)
	{
		bool keydownNumber = 0 < (wParam - 48) && (wParam - 48) <= 3;
		if (!keydownNumber)
		{
			return false;
		}

		if (isNumKeyDown)
		{
			return false;
		}

		if (GetFrontDialog())
		{
			return false;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "SingleModeScenarioLiveSelectViewController"s)
			{
				isNumKeyDown = true;

				int choiceIndex = (wParam - 48) - 1;

				auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
				if (view)
				{
					auto treeList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_TreeList", 0)->methodPointer(view);
					if (treeList)
					{
						auto _treeArrayField = il2cpp_class_get_field_from_name(treeList->klass, "_treeArray");
						Il2CppArraySize_t<Il2CppObject*>* _treeArray;
						il2cpp_field_get_value(treeList, _treeArrayField, &_treeArray);

						if (_treeArray)
						{
							auto treeItem = _treeArray->vector[choiceIndex];
							if (treeItem)
							{
								auto _buttonField = il2cpp_class_get_field_from_name(treeItem->klass, "_button");
								Il2CppObject* _button;

								il2cpp_field_get_value(treeItem, _buttonField, &_button);

								if (_button)
								{
									PressButton(_button);
									return true;
								}
							}
						}
					}
				}
			}
		}

		return false;
	}

	bool StepTrainingItem(WPARAM wParam)
	{
		if (Game::CurrentGameStore == Game::Store::Steam)
		{
			return false;
		}

		bool keydownNumber = 0 < (wParam - 48) && (wParam - 48) <= 9;
		if (!(wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_RETURN || keydownNumber))
		{
			return false;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			if (controller->klass->name == "SingleModeMainViewController"s)
			{
				auto IsStoryActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(controller->klass, "get_IsStoryActive", 0)->methodPointer(controller);

				if (IsStoryActive)
				{
					return false;
				}

				auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
				if (view)
				{
					auto trainingController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_TrainingController", 0)->methodPointer(controller);
					if (il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(trainingController->klass, "get_IsInTraining", 0)->methodPointer(trainingController))
					{
						auto footer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_TrainingFooter", 0)->methodPointer(view);
						if (footer)
						{
							auto _itemsField = il2cpp_class_get_field_from_name(footer->klass, "_items");
							Il2CppObject* _items;
							il2cpp_field_get_value(footer, _itemsField, &_items);

							int count = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(_items->klass, "get_Count", 0)->methodPointer(_items);

							FieldInfo* itemsField = il2cpp_class_get_field_from_name(_items->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* arr;
							il2cpp_field_get_value(_items, itemsField, &arr);

							auto selectedItem = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footer->klass, "get_SelectItem", 0)->methodPointer(footer);
							auto selectedMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footer->klass, "get_SelectedMenu", 0)->methodPointer(footer);

							if (wParam == VK_RETURN)
							{
								auto enabledObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredBool(*)(Il2CppObject*)>(selectedMenu->klass, "get_IsEnable", 0)->methodPointer(selectedMenu);
								auto enabled = enabledObscured.GetDecrypted();

								if (enabled)
								{
									auto _onClickEnableField = il2cpp_class_get_field_from_name(footer->klass, "_onClickEnable");
									bool _onClickEnable = true;
									il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);

									auto button = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(selectedItem->klass, "get_Button", 0)->methodPointer(selectedItem);

									auto delegate = GetButtonCommonOnClickDelegate(button);
									reinterpret_cast<void (*)(Il2CppObject*)>(delegate->method_ptr)(delegate->target);
									return true;
								}
							}
							else if (keydownNumber)
							{
								if (isNumKeyDown)
								{
									return false;
								}

								int number = wParam - 48;

								if (number > count) {
									number = count;
								}

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name(footer->klass, "_preSelectedMenu");
								Il2CppObject* footerItem = arr->vector[number - 1];

								auto trainingMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footerItem->klass, "get_TrainingMenu", 0)->methodPointer(footerItem);
								il2cpp_field_set_value(footer, _preSelectedMenuField, trainingMenu);

								isNumKeyDown = true;

								if (selectedMenu == trainingMenu)
								{
									auto enabledObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredBool(*)(Il2CppObject*)>(selectedMenu->klass, "get_IsEnable", 0)->methodPointer(selectedMenu);
									auto enabled = enabledObscured.GetDecrypted();

									if (enabled)
									{
										auto _onClickEnableField = il2cpp_class_get_field_from_name(footer->klass, "_onClickEnable");
										bool _onClickEnable = true;
										il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);


										auto button = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(selectedItem->klass, "get_Button", 0)->methodPointer(selectedItem);

										auto delegate = GetButtonCommonOnClickDelegate(button);
										reinterpret_cast<void (*)(Il2CppObject*)>(delegate->method_ptr)(delegate->target);
									}
									return true;
								}

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, bool, bool)>(footer->klass, "Select", 4)->methodPointer(footer, footerItem, trainingMenu, false, false);
								return true;
							}
							else
							{
								int selectedIndex = 0;

								for (int i = 0; i < count; i++)
								{
									if (arr->vector[i] == selectedItem)
									{
										selectedIndex = i;
										break;
									}
								}

								if (wParam == VK_LEFT)
								{
									selectedIndex--;
									if (selectedIndex < 0)
									{
										selectedIndex = count - 1;
									}
								}

								if (wParam == VK_RIGHT)
								{
									selectedIndex++;
									if (selectedIndex >= count)
									{
										selectedIndex = 0;
									}
								}

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name(footer->klass, "_preSelectedMenu");
								Il2CppObject* footerItem = arr->vector[selectedIndex];

								auto trainingMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footerItem->klass, "get_TrainingMenu", 0)->methodPointer(footerItem);
								il2cpp_field_set_value(footer, _preSelectedMenuField, trainingMenu);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, bool, bool)>(footer->klass, "Select", 4)->methodPointer(footer, footerItem, trainingMenu, false, false);
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}

	bool SelectStoryChoice(WPARAM wParam)
	{
		bool keydownNumber = 0 < (wParam - 48) && (wParam - 48) <= 9;
		if (!keydownNumber)
		{
			return false;
		}

		if (isNumKeyDown)
		{
			return false;
		}

		auto choiceController = GetSingletonInstanceByMethod(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryChoiceController"));
		if (!choiceController)
		{
			return false;
		}

		bool isWaitSelect = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(choiceController->klass, "get_IsWaitSelect", 0)->methodPointer(choiceController);
		if (!isWaitSelect)
		{
			return false;
		}

		isNumKeyDown = true;

		int choiceIndex = (wParam - 48) - 1;

		int maxCount = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(choiceController->klass, "get_MaxCount", 0)->methodPointer(choiceController);
		if (choiceIndex > maxCount - 1)
		{
			return false;
		}

		auto _choiceButtonListField = il2cpp_class_get_field_from_name(choiceController->klass, "_choiceButtonList");
		Il2CppObject* _choiceButtonList;

		il2cpp_field_get_value(choiceController, _choiceButtonListField, &_choiceButtonList);

		if (!_choiceButtonList)
		{
			return false;
		}

		auto choiceButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(_choiceButtonList->klass, "get_Item", 1)->methodPointer(_choiceButtonList, choiceIndex);
		auto _buttonField = il2cpp_class_get_field_from_name(choiceButton->klass, "_button");
		Il2CppObject* _button;

		il2cpp_field_get_value(choiceButton, _buttonField, &_button);

		if (_button)
		{
			PressButton(_button);
			return true;
		}

		return false;
	}

	bool SelectRaceStoryChoice(WPARAM wParam)
	{
		bool keydownNumber = 0 < (wParam - 48) && (wParam - 48) <= 9;
		if (!keydownNumber)
		{
			return false;
		}

		if (isNumKeyDown)
		{
			return false;
		}

		auto storyReplay = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "RaceManagerStoryReplay"));
		if (!storyReplay)
		{
			return false;
		}

		isNumKeyDown = true;

		int choiceIndex = (wParam - 48) - 1;

		auto _choiceControllerField = il2cpp_class_get_field_from_name(storyReplay->klass, "_choiceController");
		Il2CppObject* _choiceController;
		il2cpp_field_get_value(storyReplay, _choiceControllerField, &_choiceController);

		if (!_choiceController)
		{
			return false;
		}

		auto _dialogTypeField = il2cpp_class_get_field_from_name(_choiceController->klass, "_dialogType");
		int _dialogType;
		il2cpp_field_get_value(_choiceController, _dialogTypeField, &_dialogType);

		if (_dialogType == 0)
		{
			return false;
		}

		auto _choicesArrayField = il2cpp_class_get_field_from_name(_choiceController->klass, "_choicesArray");
		Il2CppArraySize_t<Il2CppObject*>* _choicesArray;
		il2cpp_field_get_value(_choiceController, _choicesArrayField, &_choicesArray);

		if (!_choicesArray)
		{
			return false;
		}

		if (choiceIndex > _choicesArray->max_length - 1)
		{
			return false;
		}

		auto choiceParam = _choicesArray->vector[choiceIndex];
		auto ButtonField = il2cpp_class_get_field_from_name(choiceParam->klass, "Button");
		Il2CppObject* Button;

		il2cpp_field_get_value(choiceParam, ButtonField, &Button);

		if (!Button)
		{
			return false;
		}

		auto _buttonField = il2cpp_class_get_field_from_name(Button->klass, "_button");
		Il2CppObject* _button;

		il2cpp_field_get_value(Button, _buttonField, &_button);

		if (_button)
		{
			PressButton(_button);
			return true;
		}

		return false;
	}

	static bool IsMovingLivePlayback = false;

	void MoveLivePlayback(float value)
	{
		if (IsMovingLivePlayback)
		{
			return;
		}

		IsMovingLivePlayback = true;

		auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
		if (director)
		{
			bool isPauseLive = il2cpp_class_get_method_from_name_type<bool (*)()>(director->klass, "IsPauseLive", 0)->methodPointer();

			auto _liveCurrentTimeField = il2cpp_class_get_field_from_name(director->klass, "_liveCurrentTime");
			il2cpp_field_set_value(director, _liveCurrentTimeField, &value);

			auto LiveTimeController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(director->klass, "get_LiveTimeController", 0)->methodPointer(director);

			if (!isPauseLive)
			{
				// prevent voice desync
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(LiveTimeController->klass, "PauseLive", 0)->methodPointer(LiveTimeController);
			}

			auto _elapsedTimeField = il2cpp_class_get_field_from_name(LiveTimeController->klass, "_elapsedTime");
			il2cpp_field_set_value(LiveTimeController, _elapsedTimeField, &value);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(LiveTimeController->klass, "set_CurrentTime", 1)->methodPointer(LiveTimeController, value);

			auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));
			auto CriAudioManager = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AudioManager->klass, "get_CriAudioManager", 0)->methodPointer(AudioManager);

			auto audioCtrlDictField = il2cpp_class_get_field_from_name(CriAudioManager->klass, "audioCtrlDict");
			Il2CppObject* audioCtrlDict;
			il2cpp_field_get_value(CriAudioManager, audioCtrlDictField, &audioCtrlDict);

			auto _songPlaybackField = il2cpp_class_get_field_from_name(AudioManager->klass, "_songPlayback");
			Cute::Cri::AudioPlayback _songPlayback;
			il2cpp_field_get_value(AudioManager, _songPlaybackField, &_songPlayback);

			auto _songCharaPlaybacksField = il2cpp_class_get_field_from_name(AudioManager->klass, "_songCharaPlaybacks");
			Il2CppArraySize_t<Cute::Cri::AudioPlayback>* _songCharaPlaybacks;
			il2cpp_field_get_value(AudioManager, _songCharaPlaybacksField, &_songCharaPlaybacks);

			void** params = new void* [1];
			params[0] = &_songPlayback.soundGroup;

			Il2CppException* exception;

			auto audioCtrl = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(audioCtrlDict->klass, "get_Item", 1), audioCtrlDict, params, &exception);

			delete[] params;

			if (exception)
			{
				wcout << exception->message->chars << endl;
			}

			auto poolField = il2cpp_class_get_field_from_name(audioCtrl->klass, "pool");
			Il2CppObject* pool;
			il2cpp_field_get_value(audioCtrl, poolField, &pool);

			if (pool)
			{
				FieldInfo* sourceListField = il2cpp_class_get_field_from_name(pool->klass, "sourceList");
				Il2CppObject* sourceList;
				il2cpp_field_get_value(pool, sourceListField, &sourceList);

				FieldInfo* itemsField = il2cpp_class_get_field_from_name(sourceList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* sources;
				il2cpp_field_get_value(sourceList, itemsField, &sources);

				Il2CppObject* cuteAudioSource = nullptr;

				for (int i = 0; i < sources->max_length; i++)
				{
					auto obj = sources->vector[i];

					if (obj)
					{
						auto isSame = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, Cute::Cri::AudioPlayback)>(obj->klass, "IsSamePlaybackId", 1)->methodPointer(obj, _songPlayback);
						if (isSame)
						{
							cuteAudioSource = obj;
							break;
						}
					}
				}

				if (cuteAudioSource)
				{
					auto sourceListField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "sourceList");
					Il2CppObject* sourceList;
					il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

					auto usingIndexField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "usingIndex");
					int usingIndex;
					il2cpp_field_get_value(cuteAudioSource, usingIndexField, &usingIndex);

					params = new void* [1];
					params[0] = &usingIndex;

					auto AtomSource = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(sourceList->klass, "get_Item", 1), sourceList, params, &exception);

					delete[] params;

					if (exception)
					{
						wcout << exception->message->chars << endl;
					}

					auto player = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AtomSource->klass, "get_player", 0)->methodPointer(AtomSource);
					il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "StopWithoutReleaseTime", 0)->methodPointer(player);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, long)>(player->klass, "SetStartTime", 1)->methodPointer(player, static_cast<long>(roundf(value * 1000.0f)));
					auto playback = il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "Start", 0)->methodPointer(player);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(player->klass, "Update", 1)->methodPointer(player, playback);

					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);

					_songPlayback.criAtomExPlayback = playback;
					il2cpp_field_set_value(AudioManager, _songPlaybackField, &_songPlayback);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(AtomSource->klass, "set_Playback", 1)->methodPointer(AtomSource, playback);
				}
			}


			if (_songCharaPlaybacks)
			{
				for (int i = 0; i < _songCharaPlaybacks->max_length; i++)
				{
					auto charaPlayback = _songCharaPlaybacks->vector[i];

					void** params = new void* [1];
					params[0] = &charaPlayback.soundGroup;

					Il2CppException* exception;

					auto audioCtrl = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(audioCtrlDict->klass, "get_Item", 1), audioCtrlDict, params, &exception);

					delete[] params;

					if (exception)
					{
						wcout << exception->message->chars << endl;
						continue;
					}

					auto poolField = il2cpp_class_get_field_from_name(audioCtrl->klass, "pool");
					Il2CppObject* pool;
					il2cpp_field_get_value(audioCtrl, poolField, &pool);

					if (pool)
					{
						FieldInfo* sourceListField = il2cpp_class_get_field_from_name(pool->klass, "sourceList");
						Il2CppObject* sourceList;
						il2cpp_field_get_value(pool, sourceListField, &sourceList);

						FieldInfo* itemsField = il2cpp_class_get_field_from_name(sourceList->klass, "_items");
						Il2CppArraySize_t<Il2CppObject*>* sources;
						il2cpp_field_get_value(sourceList, itemsField, &sources);

						Il2CppObject* cuteAudioSource = nullptr;

						for (int i = 0; i < sources->max_length; i++)
						{
							auto obj = sources->vector[i];

							if (obj)
							{
								auto isSame = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, Cute::Cri::AudioPlayback)>(obj->klass, "IsSamePlaybackId", 1)->methodPointer(obj, charaPlayback);
								if (isSame)
								{
									cuteAudioSource = obj;
									break;
								}
							}
						}

						if (cuteAudioSource)
						{
							auto sourceListField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "sourceList");
							Il2CppObject* sourceList;
							il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

							auto usingIndexField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "usingIndex");
							int usingIndex;
							il2cpp_field_get_value(cuteAudioSource, usingIndexField, &usingIndex);

							params = new void* [1];
							params[0] = &usingIndex;

							auto AtomSource = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(sourceList->klass, "get_Item", 1), sourceList, params, &exception);

							delete[] params;

							if (exception)
							{
								wcout << exception->message->chars << endl;
								continue;
							}

							auto player = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AtomSource->klass, "get_player", 0)->methodPointer(AtomSource);
							il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "StopWithoutReleaseTime", 0)->methodPointer(player);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, long)>(player->klass, "SetStartTime", 1)->methodPointer(player, static_cast<long>(roundf(value * 1000.0f)));
							auto playback = il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "Start", 0)->methodPointer(player);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(player->klass, "Update", 1)->methodPointer(player, playback);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);

							charaPlayback.criAtomExPlayback = playback;

							il2cpp_array_setref_type_memmove(_songCharaPlaybacks, Cute::Cri::AudioPlayback, i, &playback);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(AtomSource->klass, "set_Playback", 1)->methodPointer(AtomSource, playback);
						}
					}
				}
			}

			if (!isPauseLive)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(LiveTimeController->klass, "ResumeLive", 0)->methodPointer(LiveTimeController);
			}
		}

		IsMovingLivePlayback = false;
	}

	bool ControlLiveTime(WPARAM wParam)
	{
		if (wParam == VK_LEFT || wParam == VK_RIGHT)
		{
			auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
			if (director)
			{
				auto LiveCurrentTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveCurrentTime", 0)->methodPointer(director);

				bool shiftKeyDown = GetKeyState(VK_SHIFT) < 0;
				bool controlKeyDown = GetKeyState(VK_CONTROL) < 0;

				if (wParam == VK_LEFT)
				{
					if (controlKeyDown)
					{
						LiveCurrentTime -= shiftKeyDown ? 1.0f : 0.5f;
					}
					else
					{
						LiveCurrentTime -= shiftKeyDown ? 0.01f : 0.1f;
					}
				}

				if (wParam == VK_RIGHT)
				{
					if (controlKeyDown)
					{
						LiveCurrentTime += shiftKeyDown ? 1.0f : 0.5f;
					}
					else
					{
						LiveCurrentTime += shiftKeyDown ? 0.01f : 0.1f;
					}
				}

				MoveLivePlayback(LiveCurrentTime);

				return true;
			}
		}

		if (wParam == VK_SPACE)
		{
			auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
			if (director)
			{
				bool isPauseLive = il2cpp_class_get_method_from_name_type<bool (*)()>(director->klass, "IsPauseLive", 0)->methodPointer();

				auto controller = GetCurrentViewController();
				if (controller)
				{
					if (isPauseLive)
					{
						il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(controller->klass, "ResumeLive", 0)->methodPointer(controller);
					}
					else
					{
						il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(controller->klass, "PauseLive", 0)->methodPointer(controller);
					}
				}

				return true;
			}
		}

		return false;
	}


	static vector<int8_t> KONAMI_COMMAND{ VK_UP, VK_UP, VK_DOWN, VK_DOWN, VK_LEFT, VK_RIGHT, VK_LEFT, VK_RIGHT, 'B', 'A' };

	static int konamiCommandIndex = 0;

	static bool isKonamiOpened = false;

	bool CheckKonamiCmd(WPARAM wParam)
	{
		if (!config::runtime::allowStart)
		{
			if (isKonamiOpened)
			{
				return false;
			}

			if (wParam == KONAMI_COMMAND[konamiCommandIndex])
			{
				if (konamiCommandIndex == KONAMI_COMMAND.size() - 1)
				{
					konamiCommandIndex = 0;

					auto dialogData = Gallop::DialogCommon::Data();
					dialogData.SetSimpleTwoButtonMessage(
						Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Title0002"))),
						Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Title0023"))),
						CreateDelegateStatic(*[]()
							{
								wstringstream subKeyStream;

								subKeyStream << L"Software";
								subKeyStream << L"\\" << UnityEngine::Application::companyName()->chars;
								subKeyStream << L"\\" << UnityEngine::Application::productName()->chars;

								DWORD data = 1;
								HKEY hKey;
								RegCreateKeyExW(HKEY_CURRENT_USER, subKeyStream.str().data(), 0, nullptr, 0, KEY_WRITE, 0, &hKey, nullptr);
								RegSetValueExW(hKey, L"AgreeOwnYourRisk", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&data), sizeof(data));
								RegCloseKey(hKey);

								auto dialogData = Gallop::DialogCommon::Data();
								dialogData.SetSimpleOneButtonMessage(GetTextIdByName(IL2CPP_STRING("AccoutDataLink0061")), Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0309"))), nullptr, GetTextIdByName(IL2CPP_STRING("Common0185")));

								auto onDestroy = CreateDelegateStatic(*[]()
									{
										UnityEngine::Application::Exit(0);
									});

								dialogData.AddDestroyCallback(onDestroy);
								Gallop::DialogManager::PushDialog(dialogData);
							}),
						GetTextIdByName(IL2CPP_STRING("Common0309")),
						GetTextIdByName(IL2CPP_STRING("Common0150")),
						CreateDelegateStatic(*[]()
							{
								isKonamiOpened = false;
							})
					);

					isKonamiOpened = true;

					Gallop::DialogManager::PushSystemDialog(dialogData, true);
				}
				else
				{
					konamiCommandIndex++;
				}
				return true;
			}
			else
			{
				konamiCommandIndex = 0;
			}
		}

		return false;
	}

	static int SetListMusicId = 0;

	// Short: 0
	// Game: 1
	static int musicType = 0;

	void StartCoroutinePlaySetList(Il2CppObject* dialogJukeboxRequestSong, Il2CppObject* JukeboxBgmSelector, Il2CppObject* itemInfoList)
	{
		auto ValueTuple3Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`3"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
		System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*> tuple = { dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList };

		auto boxedTuple = il2cpp_value_box(ValueTuple3Class, &tuple);
		auto WaitWhile = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(WaitWhile->klass, ".ctor", 1)->methodPointer(WaitWhile,
			CreateDelegate(
				boxedTuple,
				*[](Il2CppObject* self)
				{
					auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*>*>(self);
					auto dialogJukeboxRequestSong = tuple.Item1;
					auto JukeboxBgmSelector = tuple.Item2;
					auto itemInfoList = tuple.Item3;

					auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));
					auto bgmPlayback = il2cpp_class_get_method_from_name_type<Cute::Cri::AudioPlayback(*)(Il2CppObject*)>(AudioManager->klass, "get_BgmPlayback", 0)->methodPointer(AudioManager);

					auto req = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(JukeboxBgmSelector->klass, "GetCurrentSetListRequestData", 0)->methodPointer(JukeboxBgmSelector);

					if (!req)
					{
						return false;
					}

					auto CueName = bgmPlayback.cueName;
					if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, Il2CppString*)>(JukeboxBgmSelector->klass, "IsCueNameInSetList", 1)->methodPointer(JukeboxBgmSelector, CueName))
					{
						return false;
					}

					auto tempData = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TempData"));
					auto get_JukeboxSetListPlayingInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(tempData->klass, "get_JukeboxSetListPlayingInfo", 0);
					if (get_JukeboxSetListPlayingInfo)
					{
						auto JukeboxSetListPlayingInfo = get_JukeboxSetListPlayingInfo->methodPointer(tempData);

						auto MusicData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(JukeboxSetListPlayingInfo->klass, "GetMasterSetListMusicData", 0)->methodPointer(JukeboxSetListPlayingInfo);
						auto PlayLengthField = il2cpp_class_get_field_from_name(MusicData->klass, "PlayLength");
						int PlayLength;
						il2cpp_field_get_value(MusicData, PlayLengthField, &PlayLength);

						auto bgmTime = il2cpp_symbols::get_method_pointer<long (*)(uint32_t)>("CriMw.CriWare.Runtime.dll", "CriWare", "CriAtomExPlayback", "criAtomExPlayback_GetTime", 1)(bgmPlayback.criAtomExPlayback.id);

						auto nextMusicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(JukeboxSetListPlayingInfo->klass, "GetNextMusicId", 0)->methodPointer(JukeboxSetListPlayingInfo);
						if (PlayLength <= bgmTime && SetListMusicId != nextMusicId)
						{
							SetListMusicId = nextMusicId;

							auto callback = &CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"),
								self,
								*[](Il2CppObject* self)
								{
									auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*>*>(self);
									auto dialogJukeboxRequestSong = tuple.Item1;
									auto itemInfoList = tuple.Item3;

									auto _songListField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_songList");
									Il2CppObject* _songList;
									il2cpp_field_get_value(dialogJukeboxRequestSong, _songListField, &_songList);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_songList->klass, "HideSelectUi", 0)->methodPointer(_songList);

									auto _loopScrollField = il2cpp_class_get_field_from_name(_songList->klass, "_loopScroll");
									Il2CppObject* _loopScroll;
									il2cpp_field_get_value(_songList, _loopScrollField, &_loopScroll);

									auto ItemList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_loopScroll->klass, "get_ItemList", 0)->methodPointer(_loopScroll);

									auto loopItemsField = il2cpp_class_get_field_from_name(ItemList->klass, "_items");
									Il2CppArraySize_t<Il2CppObject*>* loopItemArray;
									il2cpp_field_get_value(ItemList, loopItemsField, &loopItemArray);

									if (loopItemArray)
									{
										for (int i = 0; i < loopItemArray->max_length; i++)
										{
											auto item = loopItemArray->vector[i];
											if (item)
											{
												auto _infoListField = il2cpp_class_get_field_from_name(item->klass, "_infoList");
												Il2CppObject* _infoList;
												il2cpp_field_get_value(item, _infoListField, &_infoList);

												if (_infoList)
												{
													auto itemsField = il2cpp_class_get_field_from_name(_infoList->klass, "_items");
													Il2CppArraySize_t<Il2CppObject*>* itemArray;
													il2cpp_field_get_value(_infoList, itemsField, &itemArray);

													if (itemArray)
													{
														for (int j = 0; j < itemArray->max_length; j++)
														{
															auto info = itemArray->vector[j];
															if (info)
															{
																auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_Id", 0)->methodPointer(info);

																if (Id == SetListMusicId)
																{
																	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(item->klass, "OnClickItem", 1)->methodPointer(item, j);
																}
															}
														}
													}
												}
											}
										}
									}

									auto itemsField = il2cpp_class_get_field_from_name(itemInfoList->klass, "_items");
									Il2CppArraySize_t<Il2CppObject*>* itemArray;
									il2cpp_field_get_value(itemInfoList, itemsField, &itemArray);

									if (itemArray)
									{
										for (int i = 0; i < itemArray->max_length; i++)
										{
											auto info = itemArray->vector[i];
											if (info)
											{
												auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_Id", 0)->methodPointer(info);
												auto IsSelect = Id == SetListMusicId;
												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(info->klass, "set_IsSelect", 1)->methodPointer(info, IsSelect);

												if (IsSelect)
												{
													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_songList->klass, "set_SelectedInfo", 1)->methodPointer(_songList, info);
												}
											}
										}
									}

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(dialogJukeboxRequestSong->klass, "SetupMusicInfo", 1)->methodPointer(dialogJukeboxRequestSong, SetListMusicId);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(dialogJukeboxRequestSong->klass, "SelectBgm", 1)->methodPointer(dialogJukeboxRequestSong, SetListMusicId);
								}
							)->delegate;

							il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(3, callback, true);
						}

						auto IsPlayingField = il2cpp_class_get_field_from_name(JukeboxSetListPlayingInfo->klass, "IsPlaying");
						bool IsPlaying = false;
						il2cpp_field_get_value(JukeboxSetListPlayingInfo, IsPlayingField, &IsPlaying);

						return IsPlaying;
					}
					return false;
				}
			)
		);

		UnityEngine::MonoBehaviour(dialogJukeboxRequestSong).StartCoroutineManaged2(WaitWhile);
	}

	void OpenMusicBoxSetList(Il2CppObject* dialogJukeboxRequestSong, Il2CppObject* JukeboxBgmSelector, Il2CppObject* itemInfoList)
	{
		System::Nullable<int> setListId = {};
		auto tempData = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TempData"));
		auto JukeboxSetListPlayingInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(tempData->klass, "get_JukeboxSetListPlayingInfo", 0)->methodPointer(tempData);

		auto IsPlayingField = il2cpp_class_get_field_from_name(JukeboxSetListPlayingInfo->klass, "IsPlaying");
		bool IsPlaying = false;
		il2cpp_field_get_value(JukeboxSetListPlayingInfo, IsPlayingField, &IsPlaying);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(dialogJukeboxRequestSong->klass, "SetupTrialListeningButton", 1)->methodPointer(dialogJukeboxRequestSong, IsPlaying);

		if (IsPlaying)
		{
			auto SetListIdField = il2cpp_class_get_field_from_name(JukeboxSetListPlayingInfo->klass, "SetListId");
			int SetListId = 0;
			il2cpp_field_get_value(JukeboxSetListPlayingInfo, SetListIdField, &SetListId);
			setListId.Value(SetListId);
		}

		auto ValueTuple3Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`3"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
		System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*> tuple = { dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList };

		auto boxedTuple = il2cpp_value_box(ValueTuple3Class, &tuple);

		auto DialogJukeboxSetListModel = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogJukeboxSetListModel"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*, System::Nullable<int>)>(DialogJukeboxSetListModel->klass, ".ctor", 2)->methodPointer(DialogJukeboxSetListModel,
			CreateDelegate(boxedTuple,
				*[](Il2CppObject* self, int setListId, int setListIndex, bool isRepeat)
				{
					auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*>*>(self);
					auto dialogJukeboxRequestSong = tuple.Item1;
					auto JukeboxBgmSelector = tuple.Item2;
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int, bool, Il2CppDelegate*)>(JukeboxBgmSelector->klass, "RequestSetList", 4)->methodPointer(JukeboxBgmSelector, setListId, setListIndex, isRepeat,
						CreateDelegate(self, *[](Il2CppObject* self)
							{
								auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*>*>(self);
								auto dialogJukeboxRequestSong = tuple.Item1;
								auto JukeboxBgmSelector = tuple.Item2;
								auto itemInfoList = tuple.Item3;

								StartCoroutinePlaySetList(dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList);

								auto tempData = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TempData"));
								auto JukeboxSetListPlayingInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(tempData->klass, "get_JukeboxSetListPlayingInfo", 0)->methodPointer(tempData);

								SetListMusicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(JukeboxSetListPlayingInfo->klass, "GetMusicId", 0)->methodPointer(JukeboxSetListPlayingInfo);

								auto _songListField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_songList");
								Il2CppObject* _songList;
								il2cpp_field_get_value(dialogJukeboxRequestSong, _songListField, &_songList);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_songList->klass, "HideSelectUi", 0)->methodPointer(_songList);

								auto _loopScrollField = il2cpp_class_get_field_from_name(_songList->klass, "_loopScroll");
								Il2CppObject* _loopScroll;
								il2cpp_field_get_value(_songList, _loopScrollField, &_loopScroll);

								auto ItemList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_loopScroll->klass, "get_ItemList", 0)->methodPointer(_loopScroll);

								auto loopItemsField = il2cpp_class_get_field_from_name(ItemList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* loopItemArray;
								il2cpp_field_get_value(ItemList, loopItemsField, &loopItemArray);

								if (loopItemArray)
								{
									for (int i = 0; i < loopItemArray->max_length; i++)
									{
										auto item = loopItemArray->vector[i];
										if (item)
										{
											auto _infoListField = il2cpp_class_get_field_from_name(item->klass, "_infoList");
											Il2CppObject* _infoList;
											il2cpp_field_get_value(item, _infoListField, &_infoList);

											if (_infoList)
											{
												auto itemsField = il2cpp_class_get_field_from_name(_infoList->klass, "_items");
												Il2CppArraySize_t<Il2CppObject*>* itemArray;
												il2cpp_field_get_value(_infoList, itemsField, &itemArray);

												if (itemArray)
												{
													for (int j = 0; j < itemArray->max_length; j++)
													{
														auto info = itemArray->vector[j];
														if (info)
														{
															auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_Id", 0)->methodPointer(info);

															if (Id == SetListMusicId)
															{
																il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(item->klass, "OnClickItem", 1)->methodPointer(item, j);
															}
														}
													}
												}
											}
										}
									}
								}

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(dialogJukeboxRequestSong->klass, "SetupMusicInfo", 1)->methodPointer(dialogJukeboxRequestSong, SetListMusicId);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(dialogJukeboxRequestSong->klass, "SelectBgm", 1)->methodPointer(dialogJukeboxRequestSong, SetListMusicId);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(dialogJukeboxRequestSong->klass, "SetupTrialListeningButton", 1)->methodPointer(dialogJukeboxRequestSong, true);

								auto _isEnableTrialListeningField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_isEnableTrialListening");
								bool _isEnableTrialListening = true;
								il2cpp_field_set_value(dialogJukeboxRequestSong, _isEnableTrialListeningField, &_isEnableTrialListening);

								Gallop::DialogCommon(GetFrontDialog()).Close();
								Gallop::DialogCommon(GetFrontDialog()).Close();

								auto itemsField = il2cpp_class_get_field_from_name(itemInfoList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* itemArray;
								il2cpp_field_get_value(itemInfoList, itemsField, &itemArray);

								if (itemArray)
								{
									for (int i = 0; i < itemArray->max_length; i++)
									{
										auto info = itemArray->vector[i];
										if (info)
										{
											auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_Id", 0)->methodPointer(info);
											auto IsSelect = Id == SetListMusicId;
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(info->klass, "set_IsSelect", 1)->methodPointer(info, IsSelect);

											if (IsSelect)
											{
												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_songList->klass, "set_SelectedInfo", 1)->methodPointer(_songList, info);
											}
										}
									}
								}
							}
						));
				}
			), setListId);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("umamusume.dll", "Gallop", "DialogJukeboxSetList", "Open", 2)(DialogJukeboxSetListModel, false);
	}

	static GameObject* jukeboxObject;
	void OpenMusicBox()
	{
		if (!Gallop::Screen::IsLandscapeMode())
		{
			return;
		}

		auto JukeboxHomeTopUIClass = GetRuntimeType("umamusume.dll", "Gallop", "JukeboxHomeTopUI");

		if (!jukeboxObject)
		{
			jukeboxObject = new GameObject();
			UnityEngine::Object::DontDestroyOnLoad(*jukeboxObject);
		}

		auto jukeboxUi = jukeboxObject->GetComponent(JukeboxHomeTopUIClass);

		if (!jukeboxUi)
		{
			jukeboxUi = jukeboxObject->AddComponent(JukeboxHomeTopUIClass);

			auto _isLandscapeUIField = il2cpp_class_get_field_from_name(jukeboxUi->klass, "_isLandscapeUI");

			if (!_isLandscapeUIField)
			{
				_isLandscapeUIField = il2cpp_class_get_field_from_name(jukeboxUi->klass, "IsSteamUI");
				if (!_isLandscapeUIField)
				{
					return;
				}
			}

			bool _isLandscapeUI = true;
			il2cpp_field_set_value(jukeboxUi, _isLandscapeUIField, &_isLandscapeUI);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(jukeboxUi->klass, "InitializeJukeboxBgmSelector", 0)->methodPointer(jukeboxUi);

			auto JukeboxBgmSelector = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(jukeboxUi->klass, "get_JukeboxBgmSelector", 0)->methodPointer(jukeboxUi);

			if (JukeboxBgmSelector)
			{
				auto IsLandscapeUIField = il2cpp_class_get_field_from_name(JukeboxBgmSelector->klass, "IsLandscapeUI");

				if (!IsLandscapeUIField)
				{
					IsLandscapeUIField = il2cpp_class_get_field_from_name(JukeboxBgmSelector->klass, "IsSteamUI");
					if (!IsLandscapeUIField)
					{
						return;
					}
				}

				bool IsLandscapeUI = true;
				il2cpp_field_set_value(JukeboxBgmSelector, IsLandscapeUIField, &IsLandscapeUI);
			}
		}

		try
		{
			auto JukeboxBgmSelector = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(jukeboxUi->klass, "get_JukeboxBgmSelector", 0)->methodPointer(jukeboxUi);
			if (JukeboxBgmSelector)
			{
				auto serverTimeStamp = il2cpp_symbols::get_method_pointer<int64_t(*)()>("umamusume.dll", "Gallop", "TimeUtil", "GetServerTimeStamp", 0)();
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int64_t)>(JukeboxBgmSelector->klass, "InitializeBgmList", 1)->methodPointer(JukeboxBgmSelector, serverTimeStamp);
				auto JukeboxBgmDic = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(JukeboxBgmSelector->klass, "get_JukeboxBgmDic", 0)->methodPointer(JukeboxBgmSelector);
				auto get_Keys = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, const MethodInfo*)>(JukeboxBgmDic->klass, "get_Keys", 0);
				auto Keys = get_Keys->methodPointer(JukeboxBgmDic, reinterpret_cast<const MethodInfo*>(get_Keys));
				auto get_Count = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*, const MethodInfo*)>(Keys->klass, "get_Count", 0);
				auto bgmList = il2cpp_object_new(GetGenericClass(GetRuntimeType("mscorlib.dll", "System.Collections.Generic", "List`1"), GetRuntimeType(il2cpp_defaults.int32_class)));
				void* iter = nullptr;
				while (const MethodInfo* method = il2cpp_class_get_methods(bgmList->klass, &iter))
				{
					if (method->parameters_count && method->name == ".ctor"s && method->parameters[0]->type == Il2CppTypeEnum::IL2CPP_TYPE_GENERICINST)
					{
						reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*, const MethodInfo*)>(method->methodPointer)(bgmList, Keys, method);
						break;
					}
				}

				auto LoadAndInstantiatePrefab = il2cpp_symbols::get_method("umamusume.dll", "Gallop", "DialogInnerBase", "LoadAndInstantiatePrefab", 1);
				auto LoadAndInstantiatePrefabGeneric = GetGenericMethod(LoadAndInstantiatePrefab, GetRuntimeType("umamusume.dll", "Gallop", "DialogJukeboxRequestSong"));
				auto dialogJukeboxRequestSong = reinterpret_cast<Il2CppObject * (*)(Il2CppString*, const MethodInfo*)>(LoadAndInstantiatePrefabGeneric->methodPointer)(il2cpp_string_new("UI/Parts/Home/Jukebox/DialogJukeboxRequestSong"), LoadAndInstantiatePrefabGeneric);

				Gallop::DialogCommon::Data data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialogJukeboxRequestSong->klass, "CreateDialogData", 0)->methodPointer(dialogJukeboxRequestSong);
				data.Title(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame424008"))));
				auto dialogHash = data.DialogHash();

				auto _isSplitWindowField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_isSplitWindow");
				bool _isSplitWindow = true;
				il2cpp_field_set_value(dialogJukeboxRequestSong, _isSplitWindowField, &_isSplitWindow);

				data.RightButtonText(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Home400101"))));

				auto _dialogDataField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_dialogData");
				il2cpp_field_set_value(dialogJukeboxRequestSong, _dialogDataField, data);

				auto ValueTuple2Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`2"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
				System::ValueTuple<Il2CppObject*, Il2CppObject*> tuple2 = { dialogJukeboxRequestSong, JukeboxBgmSelector };

				auto boxed2 = il2cpp_value_box(ValueTuple2Class, &tuple2);

				auto _startTrialListening = CreateDelegate(
					boxed2,
					*[](Il2CppObject* self, int musicId, bool isTrialListening, Il2CppDelegate* onComplete)
					{
						if (isTrialListening)
						{
							return;
						}

						auto tuple = *il2cpp_object_unbox_type<System::ValueTuple<Il2CppObject*, Il2CppObject*>*>(self);
						auto dialogJukeboxRequestSong = tuple.Item1;
						auto JukeboxBgmSelector = tuple.Item2;

						auto _isSelectShortField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_isSelectShort");
						bool _isSelectShort;
						il2cpp_field_get_value(dialogJukeboxRequestSong, _isSelectShortField, &_isSelectShort);

						musicType = _isSelectShort ? 0 : 1;

						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, bool, Il2CppDelegate*)>(JukeboxBgmSelector->klass, "RequestSong", 3)->methodPointer(JukeboxBgmSelector, musicId, _isSelectShort, nullptr);
					}
				);

				auto _startTrialListeningField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_startTrialListening");
				il2cpp_field_set_value(dialogJukeboxRequestSong, _startTrialListeningField, _startTrialListening);

				auto _resetBgm = CreateDelegate(
					boxed2,
					*[](Il2CppObject* self, bool forceRestart)
					{
						auto tuple = *il2cpp_object_unbox_type<System::ValueTuple<Il2CppObject*, Il2CppObject*>*>(self);
						auto JukeboxBgmSelector = tuple.Item2;
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, bool, Il2CppDelegate*)>(JukeboxBgmSelector->klass, "RequestSong", 3)->methodPointer(JukeboxBgmSelector, 0, false, nullptr);

					}
				);

				auto _resetBgmField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_resetBgm");
				il2cpp_field_set_value(dialogJukeboxRequestSong, _resetBgmField, _resetBgm);

				int musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(JukeboxBgmSelector->klass, "get_CurrentBgmMusicId", 0)->methodPointer(JukeboxBgmSelector);
				auto tempData = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TempData"));

				bool IsPlaying = false;

				auto get_JukeboxSetListPlayingInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(tempData->klass, "get_JukeboxSetListPlayingInfo", 0);
				if (get_JukeboxSetListPlayingInfo)
				{
					auto JukeboxSetListPlayingInfo = get_JukeboxSetListPlayingInfo->methodPointer(tempData);

					auto IsPlayingField = il2cpp_class_get_field_from_name(JukeboxSetListPlayingInfo->klass, "IsPlaying");
					il2cpp_field_get_value(JukeboxSetListPlayingInfo, IsPlayingField, &IsPlaying);

					if (IsPlaying)
					{
						musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(JukeboxSetListPlayingInfo->klass, "GetMusicId", 0)->methodPointer(JukeboxSetListPlayingInfo);
					}
				}

				if (musicId)
				{
					auto _isEnableTrialListeningField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_isEnableTrialListening");
					bool _isEnableTrialListening = true;
					il2cpp_field_set_value(dialogJukeboxRequestSong, _isEnableTrialListeningField, &_isEnableTrialListening);

					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(dialogJukeboxRequestSong->klass, "SetupTrialListeningButton", 1)->methodPointer(dialogJukeboxRequestSong, true);
				}

				auto Contains = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, int, const MethodInfo*)>(bgmList->klass, "Contains", 1);
				bool containsMusic = Contains->methodPointer(bgmList, musicId, reinterpret_cast<const MethodInfo*>(Contains));

				int num = containsMusic ? musicId : 1006;

				auto LeftButtonCallBack = CreateDelegate(
					dialogJukeboxRequestSong,
					*[](Il2CppObject* self, Il2CppObject* dialogCommon)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(dialogCommon->klass, "Close", 0)->methodPointer(dialogCommon);
					});
				data.LeftButtonCallBack(LeftButtonCallBack);

				auto itemInfoList = il2cpp_object_new(GetGenericClass(GetRuntimeType("mscorlib.dll", "System.Collections.Generic", "List`1"), GetRuntimeType("umamusume.dll", "Gallop", "SelectListItem/Info")));
				il2cpp_runtime_object_init(itemInfoList);

				auto Add = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, const MethodInfo*)>(itemInfoList->klass, "Add", 1);

				{
					auto itemsField = il2cpp_class_get_field_from_name(bgmList->klass, "_items");
					Il2CppArraySize_t<int>* itemArray;
					il2cpp_field_get_value(bgmList, itemsField, &itemArray);

					if (itemArray)
					{
						for (int i = 0; i < itemArray->max_length; i++)
						{
							auto bgmId = itemArray->vector[i];

							auto itemInfo = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SelectListItem/Info"));

							auto getPathFunc = CreateDelegate(dialogJukeboxRequestSong, *[](Il2CppObject* self, int id)
								{
									auto GetJacketPath = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, uint64_t, bool)>("umamusume.dll", "Gallop", "ResourcePath", "GetJacketPath", 3);
									if (GetJacketPath)
									{
										return GetJacketPath(id, 0, true);
									}
									return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, uint64_t)>("umamusume.dll", "Gallop", "ResourcePath", "GetJacketPath", 2)(id, 0);
								}
							);

							auto ctor = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, Il2CppDelegate*, UnityEngine::Vector2, bool, bool, float, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, bool)>(itemInfo->klass, ".ctor", 10);

							if (ctor)
							{
								ctor->methodPointer
								(itemInfo, bgmId,
									getPathFunc,
									{ 228.0f, 228.0f }, num == bgmId, true, 24.0f, nullptr,
									CreateDelegate(dialogJukeboxRequestSong, *[](Il2CppObject* self, Il2CppObject* selectInfo, Il2CppObject* dialog)
										{
											auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(selectInfo->klass, "get_Id", 0)->methodPointer(selectInfo);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(self->klass, "SetupMusicInfo", 1)->methodPointer(self, Id);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(self->klass, "SelectBgm", 1)->methodPointer(self, Id);
										}
									), nullptr, true
								);
							}
							else
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, Il2CppDelegate*, UnityEngine::Vector2, bool, bool, float, Il2CppDelegate*, Il2CppDelegate*, bool)>(itemInfo->klass, ".ctor", 9)->methodPointer
								(itemInfo, bgmId,
									getPathFunc,
									{ 228.0f, 228.0f }, num == bgmId, true, 24.0f, nullptr,
									CreateDelegate(dialogJukeboxRequestSong, *[](Il2CppObject* self, Il2CppObject* selectInfo)
										{
											auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(selectInfo->klass, "get_Id", 0)->methodPointer(selectInfo);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(self->klass, "SetupMusicInfo", 1)->methodPointer(self, Id);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(self->klass, "SelectBgm", 1)->methodPointer(self, Id);
										}
									), true
								);
							}

							Add->methodPointer(itemInfoList, itemInfo, reinterpret_cast<const MethodInfo*>(Add));
						}
					}
				}

				auto _songListField = il2cpp_class_get_field_from_name(dialogJukeboxRequestSong->klass, "_songList");
				Il2CppObject* _songList;
				il2cpp_field_get_value(dialogJukeboxRequestSong, _songListField, &_songList);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, int)>(_songList->klass, "Create", 2)->methodPointer(_songList, itemInfoList, static_cast<int>(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "ResourceManager/ResourceHash"), IL2CPP_STRING("ViewHashBegin")))));

				auto ValueTuple3Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`3"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
				System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*> tuple = { dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList };

				auto boxed = il2cpp_value_box(ValueTuple3Class, &tuple);

				auto RightButtonCallBack = CreateDelegate(
					boxed,
					*[](Il2CppObject* self, Il2CppObject* dialogCommon)
					{
						if (!il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogJukeboxSetListModel"))
						{
							Gallop::UIManager::Instance().ShowNotification(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Error0025"))));
							return;
						}
						auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, Il2CppObject*>*>(self);
						auto dialogJukeboxRequestSong = tuple.Item1;
						auto JukeboxBgmSelector = tuple.Item2;
						auto itemInfoList = tuple.Item3;
						OpenMusicBoxSetList(dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList);
					}
				);
				data.RightButtonCallBack(RightButtonCallBack);

				il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, int, int, uint64_t)>(dialogJukeboxRequestSong->klass, "Setup", 3)->methodPointer(dialogJukeboxRequestSong, num, musicType, dialogHash);

				/*data.DestroyCallBack(
					&CreateUnityActionStatic(*[](void*)
						{
							auto WaitReleaseUnusedAssets = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "DialogJukeboxRequestSong", "WaitReleaseUnusedAssets", 0)();
							UnityEngine::MonoBehaviour(GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UpdateDispatcher"))).StartCoroutineManaged2(WaitReleaseUnusedAssets);
						}
					)->delegate
				);*/

				if (IsPlaying)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(dialogJukeboxRequestSong->klass, "SetupTrialListeningButton", 1)->methodPointer(dialogJukeboxRequestSong, true);
					StartCoroutinePlaySetList(dialogJukeboxRequestSong, JukeboxBgmSelector, itemInfoList);
				}

				Gallop::DialogManager::PushDialog(data);
			}
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << e.ex->message->chars << endl;
		}
	}

	bool isPortraitBeforeFullscreen = false;

	WNDPROC oldWndProcPtr = nullptr;

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_XBUTTONDOWN && GET_KEYSTATE_WPARAM(wParam) == MK_XBUTTON1 ||
			uMsg == WM_RBUTTONDOWN)
		{
			auto backKeyInputManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "BackKeyInputManager"));
			auto ExecuteBackKeyAction = il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(backKeyInputManager->klass, "ExecuteBackKeyAction", 0);

			if (ExecuteBackKeyAction)
			{
				ExecuteBackKeyAction->methodPointer(backKeyInputManager);
			}
			else
			{
				array<INPUT, 2> inputs = {
					INPUT
					{
						INPUT_KEYBOARD,
						.ki = KEYBDINPUT{ VK_ESCAPE }
					},
					INPUT
					{
						INPUT_KEYBOARD,
						.ki = KEYBDINPUT{ VK_ESCAPE, .dwFlags = KEYEVENTF_KEYUP }
					}
				};

				SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
			}

			return TRUE;
		}

		bool isLandscape = Gallop::Screen::IsLandscapeMode();

		if (uMsg == WM_KEYDOWN)
		{
			//if (wParam == VK_F2)
			//{
			//	MH_CreateHook(il2cpp_init, il2cpp_init_hook, &il2cpp_init_orig);
			//	auto result = MH_EnableHook(il2cpp_init);

			//	if (result != MH_OK)
			//	{
			//		cout << "WARN: il2cpp_init Failed: " << MH_StatusToString(result) << " LastError: " << GetLastError() << endl << endl;
			//	}

			//	// SOMETING TEST
			//	auto details = UnityEngine::Object::FindObjectsByType(GetRuntimeType("umamusume.dll", "Gallop", "DialogTrainedCharacterDetail"),
			//		UnityEngine::FindObjectsInactive::Exclude, UnityEngine::FindObjectsSortMode::None);

			//	if (details->max_length)
			//	{
			//		auto detail = details->vector[0];
			//		cout << detail->klass->name << endl;

			//		/*auto _scrollRootRectField = il2cpp_class_get_field_from_name(detail->klass, "_scrollRootRect");
			//		Il2CppObject* _scrollRootRect;
			//		il2cpp_field_get_value(detail, _scrollRootRectField, &_scrollRootRect);

			//		auto _successionToggleRootField = il2cpp_class_get_field_from_name(detail->klass, "_successionToggleRoot");
			//		Il2CppObject* _successionToggleRoot;
			//		il2cpp_field_get_value(detail, _successionToggleRootField, &_successionToggleRoot);

			//		auto transform = static_cast<UnityEngine::RectTransform>(UnityEngine::GameObject(_successionToggleRoot).transform());

			//		auto rect = transform.rect();
			//		auto sizeDelta = transform.sizeDelta();

			//		cout << "RECT: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << endl;
			//		cout << "sizeDelta: " << sizeDelta.x << " " << sizeDelta.y << endl;*/

			//		// UnityEngine::RectTransform(_scrollRootRect).sizeDelta({ 0, rect.height });

			//		/*auto dialogCommon = GetFrontDialog();
			//		auto _currentDialogObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialogCommon->klass, "get_CurrentDialogObj", 0)->methodPointer(dialogCommon);
			//		auto rootRectTransform = UnityEngine::RectTransform(il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObject->klass, "get_BaseRectTransform", 0)->methodPointer(_currentDialogObject));

			//		rootRectTransform.sizeDelta({ 0, rootRectTransform.rect().height + rect.height });*/


			//		auto Big = UnityEngine::GameObject::Find(il2cpp_string_new("Big"));

			//		if (Big)
			//		{
			//			auto cloned = UnityEngine::GameObject(Object_Internal_CloneSingle_hook(Big));

			//			auto transform = static_cast<UnityEngine::RectTransform>(cloned.transform());

			//			auto rect = transform.rect();
			//			auto sizeDelta = transform.sizeDelta();

			//			cout << "RECT: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << endl;
			//			cout << "sizeDelta: " << sizeDelta.x << " " << sizeDelta.y << endl;
			//			transform.sizeDelta({ sizeDelta.x, 5000 });
			//		}
			//	}
			//}

			if (PressDialogButton(wParam))
			{
				return TRUE;
			}

			if (PressSingleModeButton(wParam))
			{
				return TRUE;
			}

			if (PressSingleModeRaceEntry(wParam))
			{
				return TRUE;
			}

			if (PressSingleModePaddock(wParam))
			{
				return TRUE;
			}

			if (PressSingleModeLiveSelect(wParam))
			{
				return TRUE;
			}

			if (StepTrainingItem(wParam))
			{
				return TRUE;
			}

			if (SelectStoryChoice(wParam))
			{
				return TRUE;
			}

			if (SelectRaceStoryChoice(wParam))
			{
				return TRUE;
			}

			if (ControlLiveTime(wParam))
			{
				return TRUE;
			}

			if (CheckKonamiCmd(wParam))
			{
				return TRUE;
			}
		}

		if (uMsg == WM_KEYUP)
		{
			if ((0 < (wParam - 48) && (wParam - 48) <= 9))
			{
				isNumKeyDown = false;
				return TRUE;
			}
		}

		if (uMsg == WM_SYSKEYDOWN)
		{
			bool altDown = (lParam & (static_cast<long long>(1) << 29)) != 0;
			if (config::freeform_window)
			{
				if ((config::auto_fullscreen || config::unlock_size || config::freeform_window) &&
					wParam == VK_RETURN &&
					altDown &&
					!altEnterPressed)
				{
					altEnterPressed = true;

					UnityEngine::Resolution r;
					get_resolution(&r);

					auto display = display_get_main();

					auto rendering_ratio = static_cast<float>(get_rendering_width(display)) / static_cast<float>(get_rendering_height(display));
					rendering_ratio *= 1000;
					rendering_ratio = roundf(rendering_ratio) / 1000;

					auto system_ratio = static_cast<float>(get_system_width(display)) / static_cast<float>(get_system_height(display));
					system_ratio *= 1000;
					system_ratio = roundf(system_ratio) / 1000;

					if ((!Gallop::StandaloneWindowResize::IsVirt() && rendering_ratio == system_ratio) ||
						config::freeform_window)
					{
						if (!fullScreenFlOverride)
						{
							fullScreenFlOverride = true;
						}
						fullScreenFl = !fullScreenFl;
						if (config::unlock_size || config::freeform_window)
						{
							if (!fullScreenFl)
							{
								if (isPortraitBeforeFullscreen)
								{
									r.width = last_virt_window_width;
									r.height = last_virt_window_height;
									if (r.width > r.height)
									{
										r.width = last_virt_window_height;
										r.height = last_virt_window_width;
									}
								}
								else
								{
									r.width = last_hriz_window_height;
									r.height = last_hriz_window_width;
									if (r.width < r.height)
									{
										r.width = last_hriz_window_width;
										r.height = last_hriz_window_height;
									}
								}
							}
							else
							{
								RECT windowRect;
								GetClientRect(hWnd, &windowRect);
								if (get_rendering_width(display) > get_rendering_height(display))
								{
									isPortraitBeforeFullscreen = false;
									last_hriz_window_width = windowRect.right - windowRect.left;
									last_hriz_window_height = windowRect.bottom - windowRect.top;
								}
								else
								{
									isPortraitBeforeFullscreen = true;
									last_virt_window_width = windowRect.right - windowRect.left;
									last_virt_window_height = windowRect.bottom - windowRect.top;
								}
							}
						}
						else if (!fullScreenFl)
						{
							r.width *= 0.825f;
							r.height = r.width / config::runtime::ratioHorizontal;
						}

						auto refreshRate = RefreshRate{ 0, 1 };
						reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(r.width, r.height, fullScreenFl ? 1 : 3, &refreshRate);
					}
					return TRUE;

				}
			}

			if (config::max_fps > -1 && wParam == 'F' && altDown)
			{
				config::runtime::useDefaultFPS = !config::runtime::useDefaultFPS;
				UnityEngine::Application::targetFrameRate(30);
				return TRUE;
			}

			if (config::freeform_window && wParam == 'J' && altDown)
			{
				OpenMusicBox();
				return TRUE;
			}
		}

		if (config::cyspring_update_mode != -1)
		{
			bool altDown = (lParam & (static_cast<long long>(1) << 29)) != 0;
			if (altDown)
			{
				switch (wParam)
				{
				case '1':
					config::cyspring_update_mode = 0;
					break;
				case '2':
					config::cyspring_update_mode = 1;
					break;
				case '3':
					config::cyspring_update_mode = 2;
					break;
				case '4':
					config::cyspring_update_mode = 3;
					break;
				}
			}
		}

		if (uMsg == WM_SYSCOMMAND && !config::freeform_window)
		{
			if (wParam == SC_MAXIMIZE)
			{
				il2cpp_symbols::get_method_pointer<void (*)()>("umamusume.dll", "Gallop", "StandaloneWindowResize", "DisableMaximizebox", IgnoreNumberOfArguments)();
				return TRUE;
			}
		}


		if (config::auto_fullscreen || config::unlock_size || config::freeform_window)
		{
			if (uMsg == WM_SYSKEYUP)
			{
				if (wParam == VK_RETURN && (lParam & (static_cast<long long>(1) << 29)) != 0)
				{
					altEnterPressed = false;
					return TRUE;
				}
			}
		}

		if (uMsg == WM_NCHITTEST)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			if (!il2cpp_class_get_method_from_name_type<bool (*)()>(StandaloneWindowResize, "get_IsEnableWindowHitTest", IgnoreNumberOfArguments)->methodPointer())
			{
				return FALSE;
			}
		}

		if (uMsg == WM_ENTERSIZEMOVE)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowSizeChangingField = il2cpp_class_get_field_from_name(StandaloneWindowResize->klass, "_isWindowSizeChanging");
			bool _isWindowSizeChanging = true;
			il2cpp_field_static_set_value(_isWindowSizeChangingField, &_isWindowSizeChanging);
		}

		if (uMsg == WM_MOVING)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowDraggingField = il2cpp_class_get_field_from_name(StandaloneWindowResize->klass, "_isWindowDragging");
			bool _isWindowDragging = true;
			il2cpp_field_static_set_value(_isWindowDraggingField, &_isWindowDragging);
		}

		if (uMsg == WM_SIZE)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			switch (wParam)
			{
			case SIZE_RESTORED:
			{
				auto uiManager = Gallop::UIManager::Instance();
				if (uiManager)
				{
					uiManager.StartCoroutineManaged2(il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(StandaloneWindowResize->klass, "ClearStopFlagAfterWhile", 0)->methodPointer());
				}
				break;
			}
			case SIZE_MINIMIZED:
			{
				auto _isWindowDraggingField = il2cpp_class_get_field_from_name(StandaloneWindowResize->klass, "_isWindowDragging");
				bool _isWindowDragging = true;
				il2cpp_field_static_set_value(_isWindowDraggingField, &_isWindowDragging);
				break;
			}
			}
		}

		if (uMsg == WM_EXITSIZEMOVE)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowDraggingField = il2cpp_class_get_field_from_name(StandaloneWindowResize->klass, "_isWindowDragging");
			auto _isWindowSizeChangingField = il2cpp_class_get_field_from_name(StandaloneWindowResize->klass, "_isWindowSizeChanging");
			bool _isWindowSizeChanging = false;
			il2cpp_field_static_set_value(_isWindowDraggingField, &_isWindowSizeChanging);
			il2cpp_field_static_set_value(_isWindowSizeChangingField, &_isWindowSizeChanging);

			if (config::freeform_window)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;
				ResizeWindow(hWnd, windowWidth, windowHeight);
			}

			ResizeWebView();
		}

		if ((uMsg == WM_EXITSIZEMOVE || uMsg == WM_SIZE) && config::character_system_text_caption)
		{
			WaitForEndOfFrame(*[]() {
				auto callback = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
					{
						auto sliderX = Localify::UIParts::GetOptionSlider("character_system_text_caption_position_x");
						auto sliderY = Localify::UIParts::GetOptionSlider("character_system_text_caption_position_y");

						if (sliderX && sliderY)
						{
							Localify::NotificationManager::SetPosition(Localify::UIParts::GetOptionSliderValue(sliderX) / 10, Localify::UIParts::GetOptionSliderValue(sliderY) / 10);
						}
						else
						{
							Localify::NotificationManager::SetPosition(config::character_system_text_caption_position_x, config::character_system_text_caption_position_y);
						}
					}))->delegate;

				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.01, callback, true);
				});
		}

		if (uMsg == WM_SIZE && config::freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			auto lastWidthField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastHeight");

			int lastWidth, lastHeight;

			il2cpp_field_static_get_value(lastWidthField, &lastWidth);
			il2cpp_field_static_get_value(lastHeightField, &lastHeight);

			if (lastWidth != LOWORD(lParam) || lastHeight != HIWORD(lParam))
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;
				ResizeWindow(hWnd, windowWidth, windowHeight);
				ResizeWebView();

				if (oldWndProcPtr)
				{
					return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
				}

				return TRUE;
			}
		}

		if (uMsg == WM_SIZING && config::freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto lastWidthField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastHeight");

			int lastWidth, lastHeight;

			il2cpp_field_static_get_value(lastWidthField, &lastWidth);
			il2cpp_field_static_get_value(lastHeightField, &lastHeight);

			RECT* rect = reinterpret_cast<RECT*>(lParam);

			if (lastWidth != rect->right - rect->left || lastHeight != rect->bottom - rect->top)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;

				RECT clientRect;
				GetClientRect(hWnd, &clientRect);

				int frameWidth = windowWidth - clientRect.right;
				int frameHeight = windowHeight - clientRect.bottom;

				int contentWidth = rect->right - rect->left - frameWidth;
				int contentHeight = rect->bottom - rect->top - frameHeight;

				bool resizeLeft = wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_LEFT || wParam == WMSZ_TOPLEFT;
				bool resizeTop = wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT;

				if (resizeLeft)
				{
					rect->left = rect->right - (contentWidth + frameWidth);
				}
				else
				{
					rect->right = rect->left + (contentWidth + frameWidth);
				}

				if (resizeTop)
				{
					rect->top = rect->bottom - (contentHeight + frameHeight);
				}
				else
				{
					rect->bottom = rect->top + (contentHeight + frameHeight);
				}

				lastWidth = rect->right - rect->left;
				lastHeight = rect->bottom - rect->top;

				il2cpp_field_static_set_value(lastWidthField, &lastWidth);
				il2cpp_field_static_set_value(lastHeightField, &lastHeight);

				int width = UnityEngine::Screen::width();
				int height = UnityEngine::Screen::height();

				il2cpp_class_get_method_from_name_type<void (*)(float, float)>(StandaloneWindowResize, "SaveChangedWidth", 2)->methodPointer(width, height);

				auto _aspectRatioField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "_aspectRatio");
				float _aspectRatio = contentWidth / contentHeight;
				il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);

				auto uiManager = Gallop::UIManager::Instance();
				uiManager.ChangeResolution();

				ResizeWebView();
			}

			if (oldWndProcPtr)
			{
				return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
			}

			return TRUE;
		}

		if (uMsg == WM_SIZING && !config::freeform_window)
		{
			RECT* rect = reinterpret_cast<RECT*>(lParam);

			float ratio = (Gallop::StandaloneWindowResize::IsVirt() && !isLandscape) ? config::runtime::ratioVertical : config::runtime::ratioHorizontal;

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			if (Gallop::StandaloneWindowResize::IsVirt() && ratio != (9.0 / 16.0))
			{
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
			}

			if (!Gallop::StandaloneWindowResize::IsVirt() && ratio != (16.0 / 9.0))
			{
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
			}

			RECT windowRect;
			GetWindowRect(hWnd, &windowRect);
			int windowWidth = windowRect.right - windowRect.left,
				windowHeight = windowRect.bottom - windowRect.top;

			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			int clientWidth = (clientRect.right - clientRect.left),
				clientHeight = (clientRect.bottom - clientRect.top);
			auto lastWidthField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "windowLastHeight");

			auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Disable", 0)->methodPointer(tapEffectController);

			int width = rect->right - rect->left;
			int height = rect->bottom - rect->top;

			il2cpp_field_static_set_value(lastWidthField, &width);
			il2cpp_field_static_set_value(lastHeightField, &height);

			int borderWidth = windowWidth - clientWidth;
			int borderHeight = windowHeight - clientHeight;

			rect->right -= borderWidth;
			rect->bottom -= borderHeight;

			switch (wParam)
			{
			case WMSZ_LEFT:
			case WMSZ_LEFT + WMSZ_BOTTOM:
				rect->left = rect->right - width;
				rect->bottom = rect->top + roundf(width / ratio);
				break;
			case WMSZ_LEFT + WMSZ_TOP:
				rect->left = rect->right - width;
				rect->top = rect->bottom - roundf(width / ratio);
				break;
			case WMSZ_RIGHT:
			case WMSZ_RIGHT + WMSZ_BOTTOM:
				rect->right = rect->left + width;
				rect->bottom = rect->top + roundf(width / ratio);
				break;
			case WMSZ_RIGHT + WMSZ_TOP:
				rect->right = rect->left + width;
				rect->top = rect->bottom - roundf(width / ratio);
				break;
			case WMSZ_TOP:
				rect->top = rect->bottom - height;
				rect->right = rect->left + roundf(height * ratio);
				break;
			case WMSZ_BOTTOM:
				rect->bottom = rect->top + height;
				rect->right = rect->left + roundf(height * ratio);
				break;
			}

			if (width > height)
			{
				last_hriz_window_width = rect->right - rect->left;
				last_hriz_window_height = rect->bottom - rect->top;
			}
			else
			{
				last_virt_window_width = rect->right - rect->left;
				last_virt_window_height = rect->bottom - rect->top;
			}

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);

			rect->right += borderWidth;
			rect->bottom += borderHeight;

			width = rect->right - rect->left;
			height = rect->bottom - rect->top;

			il2cpp_class_get_method_from_name_type<void (*)(float, float)>(StandaloneWindowResize, "SaveChangedWidth", 2)->methodPointer(width, height);

			auto _aspectRatioField = il2cpp_class_get_field_from_name(StandaloneWindowResize, "_aspectRatio");
			float _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);

			auto uiManager = Gallop::UIManager::Instance();
			if (uiManager)
			{
				bool isVirt = width < height;
				if (config::unlock_size || isLandscape)
				{
					uiManager.ChangeResizeUIForPC(isVirt ? min(last_display_width, last_display_height) : max(last_display_width, last_display_height),
						isVirt ? max(last_display_width, last_display_height) : min(last_display_width, last_display_height));
				}
				else
				{
					uiManager.ChangeResizeUIForPC(isVirt ? 1080 : 1920, isVirt ? 1920 : 1080);
				}
			}

			auto anRootManager = GetSingletonInstance(il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", "AnRootManager"));

			if (anRootManager)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(anRootManager->klass, "set_ScreenRate", 1)->methodPointer(anRootManager, _aspectRatio);
			}

			il2cpp_symbols::get_method_pointer<void (*)()>(
				"umamusume.dll", "Gallop",
				"StandaloneWindowResize", "DisableMaximizebox", IgnoreNumberOfArguments
			)();

			uiManager.ChangeResolution();

			ResizeWebView();

			if (oldWndProcPtr)
			{
				return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
			}

			return TRUE;
		}

		if (uMsg == WM_CLOSE)
		{
			if (isExitOpened)
			{
				UnityEngine::Application::Exit(0);
				return TRUE;
			}

			ShowExitDialog();
			return TRUE;
		}

		if (oldWndProcPtr)
		{
			return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	void* StandaloneWindowResize_DisableMaximizebox_orig = nullptr;

	void StandaloneWindowResize_DisableMaximizebox_hook()
	{
		auto hWnd = GetHWND();

		long style = GetWindowLongW(hWnd, GWL_STYLE);
		if (Gallop::Screen::IsLandscapeMode() && !config::freeform_window)
		{
			style &= ~WS_MAXIMIZEBOX;
		}
		else
		{
			style |= WS_MAXIMIZEBOX;
		}
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
	}

	void* StandaloneWindowResize_ReshapeAspectRatio_orig = nullptr;

	void StandaloneWindowResize_ReshapeAspectRatio_hook()
	{

	}

	void* StandaloneWindowResize_KeepAspectRatio_orig = nullptr;

	void StandaloneWindowResize_KeepAspectRatio_hook(float currentWidth, float currentHeight)
	{
		auto hWnd = GetHWND();

		long style = GetWindowLongW(hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
	}

	void* GallopInput_mousePosition_orig = nullptr;

	UnityEngine::Vector3 GallopInput_mousePosition_hook()
	{
		auto position = il2cpp_symbols::get_method_pointer<UnityEngine::Vector3(*)()>("UnityEngine.InputLegacyModule.dll", "UnityEngine", "Input", "get_mousePosition", IgnoreNumberOfArguments)();

		if (!config::freeform_window)
		{
			if (Gallop::Screen::IsSplitWindow())
			{
				return position * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
			}

			return position * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
		}
		return position;
	}

	vector<Il2CppObject*> frameBuffers;

	void* GallopFrameBuffer_Initialize_orig = nullptr;

	void GallopFrameBuffer_Initialize_hook(Il2CppObject* _this, Il2CppObject* parent)
	{
		auto value = find(frameBuffers.begin(), frameBuffers.end(), _this);
		if (value == frameBuffers.end())
		{
			frameBuffers.emplace_back(_this);
		}

		reinterpret_cast<decltype(GallopFrameBuffer_Initialize_hook)*>(GallopFrameBuffer_Initialize_orig)(_this, parent);
	}

	void* GallopFrameBuffer_Initialize2_orig = nullptr;

	void GallopFrameBuffer_Initialize2_hook(Il2CppObject* _this, Il2CppObject* parent, bool copyUIFrameBuffer)
	{
		auto value = find(frameBuffers.begin(), frameBuffers.end(), _this);
		if (value == frameBuffers.end())
		{
			frameBuffers.emplace_back(_this);
		}

		reinterpret_cast<decltype(GallopFrameBuffer_Initialize2_hook)*>(GallopFrameBuffer_Initialize2_orig)(_this, parent, copyUIFrameBuffer);
	}

	void* GallopFrameBuffer_Release_orig = nullptr;

	void GallopFrameBuffer_Release_hook(Il2CppObject* _this)
	{
		auto value = find(frameBuffers.begin(), frameBuffers.end(), _this);
		if (value != frameBuffers.end())
		{
			frameBuffers.erase(value);
		}

		reinterpret_cast<decltype(GallopFrameBuffer_Release_hook)*>(GallopFrameBuffer_Release_orig)(_this);
	}

	void* GallopFrameBuffer_ResizeRenderTexture_orig = nullptr;

	void GallopFrameBuffer_ResizeRenderTexture_hook()
	{
		for (auto buffer : frameBuffers)
		{
			if (buffer && il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(buffer->klass, "get_ColorBuffer", 0)->methodPointer(buffer))
			{
				auto _drawPathField = il2cpp_class_get_field_from_name(buffer->klass, "_drawPass");
				Il2CppObject* _drawPath;
				il2cpp_field_get_value(buffer, _drawPathField, &_drawPath);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 1)->methodPointer(buffer, _drawPath);
			}
		}
	}

	void (*text_assign_font)(void*);
	void (*text_set_font)(void*, Il2CppObject*);
	Il2CppObject* (*text_get_font)(void*);
	int (*text_get_size)(void*);
	void (*text_set_size)(void*, int);
	float (*text_get_linespacing)(void*);
	void (*text_set_style)(void*, int);
	void (*text_set_linespacing)(void*, float);
	Il2CppString* (*text_get_text)(void*);
	void (*text_set_text)(void*, Il2CppString*);
	void (*text_set_horizontalOverflow)(void*, int);
	void (*text_set_verticalOverflow)(void*, int);
	int (*textcommon_get_TextId)(void*);

	void* on_populate_orig = nullptr;
	void on_populate_hook(Il2CppObject* _this, void* toFill)
	{
		if (config::replace_to_builtin_font && text_get_linespacing(_this) != 1.05f)
		{
			text_set_style(_this, 1);
			text_set_size(_this, text_get_size(_this) - 4);
			text_set_linespacing(_this, 1.05f);
		}
		if (config::replace_to_custom_font)
		{
			auto font = text_get_font(_this);
			Il2CppString* name = UnityEngine::Object::Name(font);
			if (config::font_asset_name.find(name->chars) == il2cppstring::npos)
			{
				auto customFont = GetCustomFont();
				if (customFont)
				{
					text_set_font(_this, customFont);
				}
			}
		}
		auto textId = textcommon_get_TextId(_this);
		if (textId)
		{
			if (GetTextIdByName(IL2CPP_STRING("Common0121")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Common0186")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Outgame0028")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Outgame0231")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Outgame0393")) == textId ||
				GetTextIdByName(IL2CPP_STRING("SingleMode0265")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Character0039")) == textId ||
				GetTextIdByName(IL2CPP_STRING("Character0325")) == textId)
			{
				text_set_horizontalOverflow(_this, 1);
				text_set_verticalOverflow(_this, 1);
			}

		}
		return reinterpret_cast<decltype(on_populate_hook)*>(on_populate_orig)(_this, toFill);
	}

	void* textcommon_awake_orig = nullptr;
	void textcommon_awake_hook(Il2CppObject* _this)
	{
		if (config::replace_to_builtin_font)
		{
			text_assign_font(_this);
		}
		if (config::replace_to_custom_font)
		{
			if (text_get_font(_this))
			{
				text_set_font(_this, nullptr);
			}

			auto customFont = GetCustomFont();
			if (customFont)
			{
				text_set_font(_this, customFont);
			}
		}
		text_set_text(_this, local::get_localized_string(text_get_text(_this)));
		reinterpret_cast<decltype(textcommon_awake_hook)*>(textcommon_awake_orig)(_this);
	}

	void* textcommon_SetTextWithLineHeadWrap_orig = nullptr;
	void textcommon_SetTextWithLineHeadWrap_hook(Il2CppObject* _this, Il2CppString* str, int maxCharacter)
	{
		reinterpret_cast<decltype(textcommon_SetTextWithLineHeadWrap_hook)*>(textcommon_SetTextWithLineHeadWrap_orig)(_this, str, maxCharacter * 2);
	}

	void* textcommon_SetTextWithLineHeadWrapWithColorTag_orig = nullptr;
	void textcommon_SetTextWithLineHeadWrapWithColorTag_hook(Il2CppObject* _this, Il2CppString* str, int maxCharacter)
	{
		reinterpret_cast<decltype(textcommon_SetTextWithLineHeadWrapWithColorTag_hook)*>(textcommon_SetTextWithLineHeadWrapWithColorTag_orig)(_this, str, maxCharacter * 2);
	}

	void* textcommon_SetSystemTextWithLineHeadWrap_orig = nullptr;
	void textcommon_SetSystemTextWithLineHeadWrap_hook(Il2CppObject* _this, Il2CppObject* systemText, int maxCharacter)
	{
		reinterpret_cast<decltype(textcommon_SetSystemTextWithLineHeadWrap_hook)*>(textcommon_SetSystemTextWithLineHeadWrap_orig)(_this, systemText, maxCharacter * 2);
	}


	void* TMP_Settings_get_instance_orig = nullptr;

	Il2CppObject* TMP_Settings_get_instance_hook()
	{
		auto tmpSettings = reinterpret_cast<decltype(TMP_Settings_get_instance_hook)*>(TMP_Settings_get_instance_orig)();
		auto fontAssetField = il2cpp_class_get_field_from_name(tmpSettings->klass,
			"m_defaultFontAsset");
		il2cpp_field_set_value(tmpSettings, fontAssetField, GetCustomTMPFont());
		return tmpSettings;
	}

	void* TextMeshProUguiCommon_Awake_orig = nullptr;
	void TextMeshProUguiCommon_Awake_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(TextMeshProUguiCommon_Awake_hook)*>(TextMeshProUguiCommon_Awake_orig)(_this);
		auto customFont = GetCustomTMPFont();

		if (!customFont)
		{
			return;
		}

		auto customFontMaterialField = il2cpp_class_get_field_from_name(customFont->klass, "material");
		Il2CppObject* customFontMaterial;
		il2cpp_field_get_value(customFont, customFontMaterialField, &customFontMaterial);

		auto SetFloat = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, float)>(customFontMaterial->klass, "SetFloat", 2)->methodPointer;
		auto SetColor = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, UnityEngine::Color)>(customFontMaterial->klass, "SetColor", 2)->methodPointer;

		auto origOutlineWidth = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(_this->klass, "get_outlineWidth", 0)->methodPointer(_this);

		auto outlineColorDictField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ColorPreset"), "OutlineColorDictionary");
		Il2CppObject* outlineColorDict;
		il2cpp_field_static_get_value(outlineColorDictField, &outlineColorDict);
		auto colorEnum = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(_this->klass, "get_OutlineColor", 0)->methodPointer(_this);

		auto entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "_entries");
		if (!entriesField)
		{
			entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "entries");
		}
		Il2CppArraySize* entries;
		il2cpp_field_get_value(outlineColorDict, entriesField, &entries);

		auto colorType = GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType");

		auto color32 = 0xFFFFFFFF;
		for (int i = 0; i < entries->max_length; i++)
		{
			auto entry = reinterpret_cast<uint64_t>(entries->vector[i]);
			auto color = (entry & 0xFFFFFFFF00000000) >> 32;
			auto key = entry & 0xFFFFFFFF;
			if (key == colorEnum && (color != 0xFFFFFFFF && color != 0))
			{
				color32 = color;
				break;
			}
			auto enumName = GetEnumName(colorType, colorEnum)->chars;
			if (enumName == il2cppstring(IL2CPP_STRING("White")) || enumName == il2cppstring(IL2CPP_STRING("Black")))
			{
				color32 = color;
				break;
			}
		}

		float a = ((color32 & 0xFF000000) >> 24) / static_cast<float>(0xff);
		float b = ((color32 & 0xFF0000) >> 16) / static_cast<float>(0xff);
		float g = ((color32 & 0xFF00) >> 8) / static_cast<float>(0xff);
		float r = (color32 & 0xFF) / static_cast<float>(0xff);
		auto origOutlineColor = UnityEngine::Color{ r, g, b, a };

		SetFloat(customFontMaterial, il2cpp_string_new("_OutlineWidth"), origOutlineWidth);
		SetColor(customFontMaterial, il2cpp_string_new("_OutlineColor"), origOutlineColor);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_this->klass, "set_fontStyle", 1)->methodPointer(_this, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_this->klass, "set_font", 1)->methodPointer(_this, customFont);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_this->klass, "set_enableWordWrapping", 1)->methodPointer(_this, false);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool, bool)>(_this->klass, "ForceMeshUpdate", 2)->methodPointer(_this, true, false);
	}

	void* load_zekken_composite_resource_orig = nullptr;
	void load_zekken_composite_resource_hook(Il2CppObject* _this)
	{
		if (config::replace_to_custom_font && config::runtime::fontAssets)
		{
			auto font = GetCustomFont();
			if (font)
			{
				FieldInfo* zekkenFontField = il2cpp_class_get_field_from_name(_this->klass, "_fontZekken");
				il2cpp_field_set_value(_this, zekkenFontField, font);
			}
		}
		reinterpret_cast<decltype(load_zekken_composite_resource_hook)*>(load_zekken_composite_resource_orig)(_this);
	}

	void* get_modified_string_orig = nullptr;
	Il2CppString* get_modified_string_hook(Il2CppString* text, Il2CppObject* input, bool allowNewLine)
	{
		if (!allowNewLine)
		{
			auto str = il2cppstring(text->chars);
			replaceAll(str, IL2CPP_STRING("\n"), IL2CPP_STRING(""));
			return il2cpp_string_new16(str.data());
		}
		return text;
	}

	void* set_vSyncCount_orig = nullptr;
	void set_vSyncCount_hook(int level)
	{
		reinterpret_cast<decltype(set_vSyncCount_hook)*>(set_vSyncCount_orig)(config::vsync_count);
	}

	void* set_anti_aliasing_orig = nullptr;
	void set_anti_aliasing_hook(int level)
	{
		if (config::anti_aliasing < 0)
		{
			reinterpret_cast<decltype(set_anti_aliasing_hook)*>(set_anti_aliasing_orig)(level);
			return;
		}

		reinterpret_cast<decltype(set_anti_aliasing_hook)*>(set_anti_aliasing_orig)(config::anti_aliasing);
	}

	void* set_anisotropicFiltering_orig = nullptr;
	void set_anisotropicFiltering_hook(int mode)
	{
		reinterpret_cast<decltype(set_anisotropicFiltering_hook)*>(set_anisotropicFiltering_orig)(config::anisotropic_filtering);
	}

	void* apply_graphics_quality_orig = nullptr;
	void apply_graphics_quality_hook(Il2CppObject* _this, int quality, bool force)
	{
		reinterpret_cast<decltype(apply_graphics_quality_hook)*>(apply_graphics_quality_orig)(_this, config::graphics_quality, true);
	}

	void* GraphicSettings_Get3DAntiAliasingLevel_orig = nullptr;
	int GraphicSettings_Get3DAntiAliasingLevel_hook(Il2CppObject* self, bool allowMSAA)
	{
		if (config::anti_aliasing < 0)
		{
			return reinterpret_cast<decltype(GraphicSettings_Get3DAntiAliasingLevel_hook)*>(GraphicSettings_Get3DAntiAliasingLevel_orig)(self, allowMSAA);
		}

		if (!allowMSAA)
		{
			return 1;
		}

		return config::anti_aliasing;
	}

	void* GraphicSettings_GetVirtualResolution_orig = nullptr;
	UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution_hook(Il2CppObject* _this)
	{
		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
			int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

			/*auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			if (width < height)
			{
				return UnityEngine::Vector2Int{ number1080, number1920 };
			}

			return UnityEngine::Vector2Int{ number1920, number1080 };*/

			if (Gallop::Screen::IsSplitWindow())
			{
				width = height * ratio_3_4;
			}

			return UnityEngine::Vector2Int{ width, height };
		}

		return reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution_hook)*>(GraphicSettings_GetVirtualResolution_orig)(_this);
	}

	void* GraphicSettings_GetVirtualResolution3D_orig = nullptr;
	UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution3D_hook(Il2CppObject* _this, bool isForcedWideAspect)
	{
		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
			int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

			/*auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			number1920 *= config::resolution_3d_scale;
			number1080 *= config::resolution_3d_scale;

			if (width < height)
			{
				return UnityEngine::Vector2Int{ number1080, number1920 };
			}

			return UnityEngine::Vector2Int{ number1920, number1080 };*/

			if (Gallop::Screen::IsSplitWindow())
			{
				width = height * ratio_3_4;
			}

			return UnityEngine::Vector2Int{ width, height };
		}

		auto resolution = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution3D_hook)*>(GraphicSettings_GetVirtualResolution3D_orig)(_this, isForcedWideAspect);
		if (config::unlock_size)
		{
			UnityEngine::Resolution res;
			get_resolution(&res);
			if (resolution.m_X > resolution.m_Y)
			{
				resolution.m_X = res.width;
				resolution.m_Y = res.height;
			}
			else
			{
				resolution.m_X = res.height;
				resolution.m_Y = res.width;
			}
		}
		resolution.m_X *= config::resolution_3d_scale;
		resolution.m_Y *= config::resolution_3d_scale;
		return resolution;
	}


	void* GraphicSettings_GetVirtualResolution3D2_orig = nullptr;
	UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution3D2_hook(Il2CppObject* _this, UnityEngine::Vector2Int resolution, bool isForcedWideAspect)
	{
		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
			int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

			if (Gallop::Screen::IsSplitWindow())
			{
				width = height * ratio_3_4;
			}

			return UnityEngine::Vector2Int{ width, height };
		}

		auto resolution2 = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution3D2_hook)*>(GraphicSettings_GetVirtualResolution3D2_orig)(_this, resolution, isForcedWideAspect);
		if (config::unlock_size)
		{
			UnityEngine::Resolution res;
			get_resolution(&res);
			if (resolution2.m_X > resolution2.m_Y)
			{
				resolution2.m_X = res.width;
				resolution2.m_Y = res.height;
			}
			else
			{
				resolution2.m_X = res.height;
				resolution2.m_Y = res.width;
			}
		}
		resolution2.m_X *= config::resolution_3d_scale;
		resolution2.m_Y *= config::resolution_3d_scale;
		return resolution2;
	}

	void* Renderer_get_material_orig = nullptr;

	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* _this, int nameID);
	Il2CppObject* Renderer_get_material_hook(Il2CppObject* _this);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_materials_hook(Il2CppObject* _this);
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* _this);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_sharedMaterials_hook(Il2CppObject* _this);

	void ReplaceMaterialTexture(Il2CppObject* material);

	void ReplaceRendererTexture(Il2CppObject* renderer)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(renderer))
		{
			return;
		}

		auto count = il2cpp_resolve_icall_type<int (*)(Il2CppObject*)>("UnityEngine.Renderer::GetMaterialCount()")(renderer);

		if (count > 0)
		{
			Il2CppArraySize_t<Il2CppObject*>* materials = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Material"), count);
			il2cpp_resolve_icall_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>("UnityEngine.Renderer::CopySharedMaterialArray()")(renderer, materials);

			for (int i = 0; i < materials->max_length; i++)
			{
				auto material = materials->vector[i];
				if (material)
				{
					ReplaceMaterialTexture(material);
				}
			}
		}
	}

	void ReplaceMaterialTexture(Il2CppObject* material)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(material))
		{
			return;
		}

		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_DirtTex"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_EmissiveTex"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_EnvMap"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_MainTex"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_MaskColorTex"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_OptionMaskMap"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_ToonMap"));
		ReplaceMaterialTextureProperty(material, il2cpp_string_new("_TripleMaskMap"));
	}

	void ReplaceAssetHolderTextures(Il2CppObject* holder)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(holder))
		{
			return;
		}

		auto objectList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_ObjectList", 0)->methodPointer(holder);

		FieldInfo* itemsField = il2cpp_class_get_field_from_name(objectList->klass, "_items");
		Il2CppArraySize_t<Il2CppObject*>* arr;
		il2cpp_field_get_value(objectList, itemsField, &arr);

		for (int i = 0; i < arr->max_length; i++)
		{
			auto pair = arr->vector[i];

			if (!pair)
			{
				continue;
			}

			auto field = il2cpp_class_get_field_from_name(pair->klass, "Value");
			Il2CppObject* obj;
			il2cpp_field_get_value(pair, field, &obj);

			if (obj)
			{
				// cout << "AssetHolder: " << i << " " << obj->klass->name << " " << wide_u8(UnityEngine::Object::Name(obj)->chars) << endl;
				if (obj->klass->name == "GameObject"s && UnityEngine::Object::IsNativeObjectAlive(obj))
				{
					// auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppType*)>(component->klass, "GetComponent", 1)->methodPointer;
					auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize * (*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(obj->klass, "GetComponentsInternal", 6)->methodPointer;

					auto array = getComponents(obj, reinterpret_cast<Il2CppType*>(GetRuntimeType(
						"UnityEngine.CoreModule.dll", "UnityEngine", "Object")), true, true, true, false, nullptr);

					if (array)
					{
						for (int j = 0; j < array->max_length; j++)
						{
							auto obj =
								il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(array, j);
							if (!obj)
							{
								continue;
							}

							/*if (obj && obj->klass && obj->klass->name != "Transform"s)
							{
								cout << "AssetHolder GameObject -> " << obj->klass->name << endl;
							}*/
							if (string(obj->klass->name).find("MeshRenderer") != string::npos)
							{
								// ReplaceRendererTexture(obj);
							}
						}
					}
				}
				if (obj->klass->name == "Texture2D"s)
				{
					auto uobject_name = UnityEngine::Object::Name(obj);
					if (!il2cppstring(uobject_name->chars).empty())
					{
						auto newTexture = GetReplacementAssets(
							uobject_name,
							GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
						if (newTexture)
						{
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
								(newTexture, 32);
							il2cpp_field_set_value(pair, field, newTexture);
						}
					}
				}
				if (obj->klass->name == "Material"s)
				{
					ReplaceMaterialTexture(obj);
				}
			}
		}
	}

	void ReplaceRawImageTexture(Il2CppObject* rawImage)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(rawImage))
		{
			return;
		}

		auto textureField = il2cpp_class_get_field_from_name(rawImage->klass, "m_Texture");
		Il2CppObject* texture;
		il2cpp_field_get_value(rawImage, textureField, &texture);

		if (texture)
		{
			auto uobject_name = UnityEngine::Object::Name(texture);
			if (uobject_name)
			{
				auto nameU8 = il2cpp_u8(uobject_name->chars);
				if (!nameU8.empty())
				{
					do
					{
						stringstream pathStream(nameU8);
						string segment;
						vector<string> split;
						while (getline(pathStream, segment, '/'))
						{
							split.emplace_back(segment);
						}
						auto& textureName = split.back();
						if (!textureName.empty())
						{
							auto texture2D = GetReplacementAssets(il2cpp_string_new(split.back().data()),
								GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
							if (texture2D)
							{
								il2cpp_field_set_value(rawImage, textureField, texture2D);
							}
						}
					} while (false);
				}
			}
		}
	}

	void ReplaceAnimateToUnityTextures(Il2CppObject* anRoot)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(anRoot))
		{
			return;
		}

		auto meshParameterGroup = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(anRoot->klass, "get_MeshParameterGroup", 0)->methodPointer(anRoot);

		if (meshParameterGroup)
		{
			auto meshParameterList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(meshParameterGroup->klass, "get_MeshParameterList", 0)->methodPointer(meshParameterGroup);

			if (meshParameterList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name(meshParameterList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* items;
				il2cpp_field_get_value(meshParameterList, itemsField, &items);

				if (items)
				{
					for (int i = 0; i < items->max_length; i++)
					{
						auto parameter = items->vector[i];

						if (parameter)
						{
							auto meshParameterGroupList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parameter->klass, "get_MeshParameterGroupList", 0)->methodPointer(parameter);

							if (meshParameterGroupList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name(meshParameterGroupList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(meshParameterGroupList, items1Field, &items1);

								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto group = items1->vector[j];

										if (group)
										{
											auto textureSetColor = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(group->klass, "get_TextureSetColor", 0)->methodPointer(group);

											if (textureSetColor)
											{
												auto uobject_name = UnityEngine::Object::Name(textureSetColor);
												if (!il2cppstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
													if (newTexture)
													{
														il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(group->klass, "set_TextureSetColor", 1)->methodPointer(group, newTexture);
													}
												}
											}

											auto textureSetAlpha = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(group->klass, "get_TextureSetAlpha", 0)->methodPointer(group);

											if (textureSetAlpha)
											{
												auto uobject_name = UnityEngine::Object::Name(textureSetAlpha);
												if (!il2cppstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
													if (newTexture)
													{
														il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(group->klass, "set_TextureSetAlpha", 1)->methodPointer(group, newTexture);
													}
												}
											}
										}
									}
								}
							}

							auto customMeshInfoParameterList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parameter->klass, "get_CustomMeshInfoParameterList", 0)->methodPointer(parameter);

							if (customMeshInfoParameterList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name(customMeshInfoParameterList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(customMeshInfoParameterList, items1Field, &items1);

								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto customParameter = items1->vector[j];

										if (customParameter)
										{
											auto textureColor = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(customParameter->klass, "get_TextureColor", 0)->methodPointer(customParameter);

											if (textureColor)
											{
												auto uobject_name = UnityEngine::Object::Name(textureColor);
												if (!il2cppstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
													if (newTexture)
													{
														il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(customParameter->klass, "set_TextureColor", 1)->methodPointer(customParameter, newTexture);
													}
												}
											}

											auto textureAlpha = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(customParameter->klass, "get_TextureAlpha", 0)->methodPointer(customParameter);

											if (textureAlpha)
											{
												auto uobject_name = UnityEngine::Object::Name(textureAlpha);
												if (!il2cppstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
													if (newTexture)
													{
														il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(customParameter->klass, "set_TextureAlpha", 1)->methodPointer(customParameter, newTexture);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void ReplaceGameObjectTextures(Il2CppObject* gameObject, bool isChild = false);

	void ReplaceTransformTextures(Il2CppObject* transform)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(transform))
		{
			return;
		}

		auto childCount = il2cpp_class_get_method_from_name_type<int(*)(Il2CppObject*)>(transform->klass, "get_childCount", 0)->methodPointer(transform);

		if (!childCount)
		{
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_gameObject", 0)->methodPointer(transform);
			ReplaceGameObjectTextures(gameObject, true);
		}

		for (int i = 0; i < childCount; i++)
		{
			auto child = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(transform->klass, "GetChild", 1)->methodPointer(transform, i);

			// auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(child->klass, "get_gameObject", 0)->methodPointer(child);

			// cout << "Transform gameObject " << wide_u8(UnityEngine::Object::Name(gameObject)->chars) << endl;

			ReplaceTransformTextures(child);
		}
	}

	void ReplaceCutInTimelineControllerTextures(Il2CppObject* controller)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(controller))
		{
			return;
		}

		auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_Data", 0)->methodPointer(controller);

		if (data)
		{
			auto worksheetList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(data->klass, "GetWorkSheetList", 0)->methodPointer(data);

			if (worksheetList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name(worksheetList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* items;
				il2cpp_field_get_value(worksheetList, itemsField, &items);

				if (items)
				{
					for (int i = 0; i < items->max_length; i++)
					{
						auto worksheet = items->vector[i];

						if (worksheet)
						{
							auto effectListField = il2cpp_class_get_field_from_name(worksheet->klass, "_effectList");
							Il2CppObject* effectList;
							il2cpp_field_get_value(worksheet, effectListField, &effectList);

							if (effectList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name(effectList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(effectList, items1Field, &items1);


								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto effectData = items1->vector[j];

										if (effectData)
										{
											auto keysField = il2cpp_class_get_field_from_name(effectData->klass, "_keys");
											Il2CppObject* keys;
											il2cpp_field_get_value(effectData, keysField, &keys);

											if (keys)
											{
												auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(keys->klass, "get_Prefab", 0)->methodPointer(keys);

												if (gameObject)
												{
													ReplaceGameObjectTextures(gameObject);
												}
											}
										}
									}
								}
							}

							auto bgListField = il2cpp_class_get_field_from_name(worksheet->klass, "_bgList");
							Il2CppObject* bgList;
							il2cpp_field_get_value(worksheet, bgListField, &bgList);

							if (bgList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name(bgList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(bgList, items1Field, &items1);


								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto effectData = items1->vector[j];

										if (effectData)
										{
											auto keysField = il2cpp_class_get_field_from_name(effectData->klass, "_keys");
											Il2CppObject* keys;
											il2cpp_field_get_value(effectData, keysField, &keys);

											if (keys)
											{
												auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(keys->klass, "get_Prefab", 0)->methodPointer(keys);

												if (gameObject)
												{
													ReplaceGameObjectTextures(gameObject);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void ReplaceGameObjectTextures(Il2CppObject* gameObject, bool isChild)
	{
		auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(gameObject->klass, "GetComponent", 1)->methodPointer;
		auto getComponents =
			il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppReflectionType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;

		auto array = getComponents(gameObject, GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "Object"), true, true, true, false, nullptr);

		if (array)
		{
			for (int j = 0; j < array->max_length; j++)
			{
				auto obj =
					il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(array, j);
				if (!obj) continue;
				/*if (obj->klass && obj->klass->name != "Transform"s)
				{
					cout << "GameObject -> " << obj->klass->name << endl;
				}*/

				/*if ("AssetHolder"s == obj->klass->name)
				{
					ReplaceAssetHolderTextures(obj);
				}*/

				if (string(obj->klass->name).find("MeshRenderer") != string::npos)
				{
					ReplaceRendererTexture(obj);
				}
			}
		}

		auto rawImages = getComponents(gameObject, GetRuntimeType("umamusume.dll", "Gallop", "RawImageCommon"),
			true, true, true, false, nullptr);

		if (rawImages && rawImages->max_length)
		{
			for (int i = 0; i < rawImages->max_length; i++)
			{
				auto rawImage = rawImages->vector[i];
				if (rawImage)
				{
					ReplaceRawImageTexture(rawImage);
				}
			}
		}

		auto anRoots = getComponents(gameObject, GetRuntimeType("Plugins.dll", "AnimateToUnity", "AnRoot"),
			true, true, true, false, nullptr);

		if (anRoots && anRoots->max_length)
		{
			for (int i = 0; i < anRoots->max_length; i++)
			{
				auto anRoot = anRoots->vector[i];
				if (anRoot)
				{
					ReplaceAnimateToUnityTextures(anRoot);
				}
			}
		}

		auto cutInTimelineControllers = getComponents(gameObject, GetRuntimeType("umamusume.dll", "Gallop.CutIn.Cutt", "CutInTimelineController"),
			true, true, true, false, nullptr);

		if (cutInTimelineControllers && cutInTimelineControllers->max_length)
		{
			for (int i = 0; i < cutInTimelineControllers->max_length; i++)
			{
				auto cutInTimelineController = cutInTimelineControllers->vector[i];
				if (cutInTimelineController)
				{
					ReplaceCutInTimelineControllerTextures(cutInTimelineController);
				}
			}
		}

		if (!isChild)
		{
			auto transforms = getComponents(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Transform"),
				true, true, true, false, nullptr);

			if (transforms && transforms->max_length)
			{
				for (int i = 0; i < transforms->max_length; i++)
				{
					auto transform = transforms->vector[i];
					if (transform)
					{
						ReplaceTransformTextures(transform);
					}
				}
			}
		}

		auto assetholder = getComponent(gameObject, GetRuntimeType("umamusume.dll", "Gallop", "AssetHolder"));

		if (assetholder)
		{
			ReplaceAssetHolderTextures(assetholder);
		}
	}

	void ReplaceFontTexture(Il2CppObject* font)
	{
		if (!UnityEngine::Object::IsNativeObjectAlive(font))
		{
			return;
		}

		auto material = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(font->klass, "get_material", 0)->methodPointer(font);

		if (material)
		{
			ReplaceMaterialTexture(material);
		}
	}

	void InitOptionLayout(Il2CppObject* parentRectTransform)
	{
		Localify::UIParts::AddToLayout(parentRectTransform,
			{
				Localify::UIParts::GetOptionItemTitle(LocalifySettings::GetText("settings_title")),
				Localify::UIParts::GetOptionItemButton("open_settings", LocalifySettings::GetText("open_settings")),
			}
		);
	}

	void SetupOptionLayout()
	{
		Localify::UIParts::SetOptionItemButtonAction("open_settings", *([](Il2CppObject*)
			{
				Localify::SettingsUI::OpenSettings();
			}));
	}

	void SetupLiveOptionLayout()
	{
		Localify::UIParts::SetOptionItemButtonAction("open_settings", *([](Il2CppObject*)
			{
				Localify::SettingsUI::OpenLiveSettings();
			}));
	}

	bool UpdateViewerIdText()
	{
		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			return true;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);

			auto ViewerIdTextField = il2cpp_class_get_field_from_name(view->klass, "ViewerIdText");
			Il2CppObject* ViewerIdText;
			il2cpp_field_get_value(view, ViewerIdTextField, &ViewerIdText);

			auto CopyIdButtonField = il2cpp_class_get_field_from_name(view->klass, "CopyIdButton");
			Il2CppObject* CopyIdButton;
			il2cpp_field_get_value(view, CopyIdButtonField, &CopyIdButton);

			if (!GetButtonCommonOnClickDelegate(CopyIdButton))
			{
				return false;
			}

			auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

			auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
			auto viewerIdStringObscured = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workUserData->klass, "get_ViewerIdString", 0)->methodPointer(workUserData);

			auto viewerId = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(viewerIdStringObscured->klass, "InternalDecrypt", 0)->methodPointer(viewerIdStringObscured);

			if (viewerId)
			{
				auto text = text_get_text(ViewerIdText);
				auto newText = text->chars + il2cppstring(IL2CPP_STRING("\n(")) + viewerId->chars + IL2CPP_STRING(")");
				text_set_text(ViewerIdText, il2cpp_string_new16(newText.data()));
			}

			auto fn = *[](void*)
				{
					auto viewerIdCopyFn = *[](void*)
						{
							auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

							auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
							auto viewerIdObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredLong(*)(Il2CppObject*)>(workUserData->klass, "get_ViewerId", 0)->methodPointer(workUserData);
							auto viewerId = viewerIdObscured.GetDecrypted();

							il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("UnityEngine.IMGUIModule.dll", "UnityEngine", "GUIUtility", "set_systemCopyBuffer", 1)(il2cpp_string_new(to_string(viewerId).data()));

							auto dialogData = Gallop::DialogCommon::Data();
							dialogData.SetSimpleOneButtonMessage(GetTextIdByName(IL2CPP_STRING("Outgame0031")), il2cpp_string_new16(IL2CPP_STRING("트레이너 ID를 복사했습니다.")), nullptr, GetTextIdByName(IL2CPP_STRING("Common0007")));

							Gallop::DialogManager::PushDialog(dialogData);
						};

					auto playerIdCopyFn = *[](void*)
						{
							auto controller = GetCurrentViewController();
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "OnClickCopyIdButton", 0)->methodPointer(controller);
						};

					auto dialogData = Gallop::DialogCommon::Data();
					dialogData.SetSimpleTwoButtonMessage(
						Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0031"))),
						il2cpp_string_new("트레이너 ID를 복사하시겠습니까?"),
						CreateDelegateStatic(viewerIdCopyFn),
						GetTextIdByName(IL2CPP_STRING("Outgame0002")),
						GetTextIdByName(IL2CPP_STRING("Common0003")),
						CreateDelegateStatic(playerIdCopyFn)
					);

					Gallop::DialogManager::PushDialog(dialogData);
				};

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(CopyIdButton->klass, "SetOnClick", 1)->methodPointer(CopyIdButton, &CreateUnityActionStatic(fn)->delegate);
			return true;
		}
		return false;
	}

	void* Object_Internal_CloneSingleWithParent_orig = nullptr;
	Il2CppObject* Object_Internal_CloneSingleWithParent_hook(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		auto cloned = reinterpret_cast<decltype(Object_Internal_CloneSingleWithParent_hook)*>(Object_Internal_CloneSingleWithParent_orig)(data, parent, worldPositionStays);

		if (Game::CurrentGameRegion == Game::Region::KOR && il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("ProfileTopView")) != wstring::npos)
		{
			static Il2CppDelegate* updateViewerIdText;
			updateViewerIdText = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
				{
					if (!UpdateViewerIdText())
					{
						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateViewerIdText, true);
					}
				}))->delegate;

			// Delay 50ms
			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateViewerIdText, true);
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("DialogOptionHome")) != wstring::npos)
		{
			auto dialog = UnityEngine::GameObject(cloned).GetComponent(GetRuntimeType("umamusume.dll", "Gallop", "DialogOptionHome"));

			auto _optionPageBasicSettingField = il2cpp_class_get_field_from_name(dialog->klass, "_optionPageBasicSetting");
			Il2CppObject* _optionPageBasicSetting;
			il2cpp_field_get_value(dialog, _optionPageBasicSettingField, &_optionPageBasicSetting);

			auto PartsOptionPageBasicSetting = UnityEngine::MonoBehaviour(_optionPageBasicSetting).gameObject();
			PartsOptionPageBasicSetting.SetActive(true);

			auto rectTransformArray = PartsOptionPageBasicSetting.GetComponentsInChildren(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"), false);

			for (int j = 0; j < rectTransformArray->max_length; j++)
					{
				auto rectTransform = rectTransformArray->vector[j];
				if (rectTransform && UnityEngine::Object::Name(rectTransform)->chars == il2cppstring(IL2CPP_STRING("Content")))
						{
					InitOptionLayout(rectTransform);
							break;
						}
					}

			SetupOptionLayout();
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("DialogOptionLiveTheater")) != wstring::npos)
		{
			auto rectTransformArray = UnityEngine::GameObject(cloned).GetComponentsInChildren(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"), false);

			for (int i = 0; i < rectTransformArray->max_length; i++)
			{
				auto rectTransform = rectTransformArray->vector[i];

				if (rectTransform && UnityEngine::Object::Name(rectTransform)->chars == il2cppstring(IL2CPP_STRING("Content")))
				{
					InitOptionLayout(rectTransform);
					break;
				}
			}

			SetupLiveOptionLayout();
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("CharacterHomeTopUI")) != wstring::npos)
		{
			auto CharacterHomeTopUI = UnityEngine::GameObject(cloned).GetComponent(GetRuntimeType("umamusume.dll", "Gallop", "CharacterHomeTopUI"));

			if (CharacterHomeTopUI)
			{
				auto _cardRootButtonField = il2cpp_class_get_field_from_name(CharacterHomeTopUI->klass, "_cardRootButton");
				Il2CppObject* _cardRootButton;
				il2cpp_field_get_value(CharacterHomeTopUI, _cardRootButtonField, &_cardRootButton);

				if (_cardRootButton)
				{
					auto targetText = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_cardRootButton->klass, "get_TargetText", 0)->methodPointer(_cardRootButton);

					if (targetText)
					{
						text_set_horizontalOverflow(targetText, 1);
						text_set_verticalOverflow(targetText, 1);
					}
				}
			}
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("LiveView")) != wstring::npos)
		{
			auto gameObject = UnityEngine::GameObject(cloned);
			auto contentsRoot = gameObject.transform().Find(il2cpp_string_new(config::live_slider_always_show ? "ContentsRoot" : "ContentsRoot/MenuRoot"));
			auto slider = Localify::UIParts::GetSlider("live_slider", 0, 0, 180, false,
				*[](Il2CppObject* sliderCommon)
				{
					auto value = Localify::UIParts::GetOptionSliderValue("live_slider");

					MoveLivePlayback(value);
				});
			auto sliderTransform = static_cast<UnityEngine::RectTransform>(slider.transform());
			sliderTransform.anchoredPosition({ 0, 28 });
			sliderTransform.anchorMax({ 1, 0 });
			sliderTransform.anchorMin({ 0, 0 });
			sliderTransform.pivot({ 0.2, 0.5 });
			sliderTransform.sizeDelta({ -520, 24 });
			sliderTransform.SetParent(contentsRoot, false);
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("RaceResultList")) != wstring::npos)
		{
			auto raceInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "RaceManager", "get_RaceInfo", 0)();

			if (raceInfo)
			{
				auto raceType = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_RaceType", 0)->methodPointer(raceInfo);

				if (raceType == 6 || raceType == 7)
				{
					auto raceInstanceId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_RaceInstanceId", 0)->methodPointer(raceInfo);
					auto grade = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_Grade", 0)->methodPointer(raceInfo);

					auto musicId = MasterDB::GetSingleModeRaceLiveMusicId(raceInstanceId, grade);

					auto playerHorseIndex = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_PlayerHorseIndex", 0)->methodPointer(raceInfo);
					auto raceHorse = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*)>(raceInfo->klass, "get_RaceHorse", 0)->methodPointer(raceInfo);
					auto horseData = raceHorse->vector[playerHorseIndex];

					auto charaIdField = il2cpp_class_get_field_from_name(horseData->klass, "charaId");

					int charaId;
					il2cpp_field_get_value(horseData, charaIdField, &charaId);

					if (MasterDB::HasLivePermission(musicId, charaId))
					{
						auto gameObject = UnityEngine::GameObject(cloned);
						auto raceResultList = gameObject.GetComponent(GetRuntimeType("umamusume.dll", "Gallop", "RaceResultList"));
						auto _singleModeLiveButtonField = il2cpp_class_get_field_from_name(raceResultList->klass, "_singleModeLiveButton");
						Il2CppObject* _singleModeLiveButton;
						il2cpp_field_get_value(raceResultList, _singleModeLiveButtonField, &_singleModeLiveButton);

						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "PartsLiveTheaterVoiceIcon", "FindOrCreate", 1)(MonoBehaviour(_singleModeLiveButton).gameObject().transform());
					}
				}
			}
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("LiveChampionsTextController")) != wstring::npos)
		{
			auto updateScreenReferenceSize = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), cloned, *([](Il2CppObject* _this)
				{
					auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
					if (director)
					{
						auto ChampionsTextControllerField = il2cpp_class_get_field_from_name(director->klass, "ChampionsTextController");
						Il2CppObject* ChampionsTextController;
						il2cpp_field_get_value(director, ChampionsTextControllerField, &ChampionsTextController);

						if (ChampionsTextController)
						{
							auto _flashPlayerField = il2cpp_class_get_field_from_name(ChampionsTextController->klass, "_flashPlayer");
							Il2CppObject* _flashPlayer;
							il2cpp_field_get_value(ChampionsTextController, _flashPlayerField, &_flashPlayer);

							auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

							int unityWidth = UnityEngine::Screen::width();

							int unityHeight = UnityEngine::Screen::height();

							auto _flashCanvasScalerField = il2cpp_class_get_field_from_name(ChampionsTextController->klass, "_flashCanvasScaler");
							Il2CppObject* _flashCanvasScaler;
							il2cpp_field_get_value(ChampionsTextController, _flashCanvasScalerField, &_flashCanvasScaler);

							float scale = 1.0f;

							if (unityWidth < unityHeight)
							{
								scale = min(config::freeform_ui_scale_portrait, max(1.0f, unityHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
							}
							else
							{
								scale = min(config::freeform_ui_scale_landscape, max(1.0f, unityWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
							}

							float availableWidth = static_cast<float>(unityWidth) / scale;
							float availableHeight = static_cast<float>(unityHeight) / scale;

							float width = ratio_16_9 * availableHeight;
							float height = availableHeight;

							if (width > availableWidth)
							{
								width = availableWidth;
								height = width / ratio_16_9;
							}

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(_flashCanvasScaler->klass, "set_referenceResolution", 1)->methodPointer(_flashCanvasScaler, UnityEngine::Vector2{ width, height });
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ width, height });
						}

						auto liveFlashController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(director->klass, "get_LiveFlashController", 0)->methodPointer(director);

						if (liveFlashController)
						{
							auto _flashPlayerField = il2cpp_class_get_field_from_name(liveFlashController->klass, "_flashPlayer");

							if (_flashPlayerField)
							{
								Il2CppObject* _flashPlayer;
								il2cpp_field_get_value(liveFlashController, _flashPlayerField, &_flashPlayer);

								auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

								int unityWidth = UnityEngine::Screen::width();

								int unityHeight = UnityEngine::Screen::height();

								float scale = 1.0f;

								if (unityWidth < unityHeight)
								{
									scale = min(config::freeform_ui_scale_portrait, max(1.0f, unityHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
								}
								else
								{
									scale = min(config::freeform_ui_scale_landscape, max(1.0f, unityWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
								}

								float availableWidth = static_cast<float>(unityWidth) / scale;
								float availableHeight = static_cast<float>(unityHeight) / scale;

								float width = ratio_16_9 * availableHeight;
								float height = availableHeight;

								if (width > availableWidth)
								{
									width = availableWidth;
									height = width / ratio_16_9;
								}

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ width, height });
							}
						}
					}
				}));

			// Delay 50ms
			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, &updateScreenReferenceSize->delegate, true);
		}

		return cloned;
	}

	void* Sprite_get_texture_orig = nullptr;
	Il2CppObject* Sprite_get_texture_hook(Il2CppObject* _this)
	{
		auto texture2D = reinterpret_cast<decltype(Sprite_get_texture_hook)*>(Sprite_get_texture_orig)(_this);
		auto uobject_name = UnityEngine::Object::Name(texture2D);
		if (!il2cppstring(uobject_name->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_name,
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				return newTexture;
			}
		}
		return texture2D;
	}

	Il2CppObject* Renderer_get_material_hook(Il2CppObject* _this)
	{
		auto material = reinterpret_cast<decltype(Renderer_get_material_hook)*>(Renderer_get_material_orig)(_this);
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		return material;
	}

	void* Renderer_get_materials_orig = nullptr;
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_materials_hook(Il2CppObject* _this)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_materials_hook)*>(Renderer_get_materials_orig)(_this);
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
		return materials;
	}

	void* Renderer_get_sharedMaterial_orig = nullptr;
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* _this)
	{
		auto material = reinterpret_cast<decltype(Renderer_get_sharedMaterial_hook)*>(Renderer_get_sharedMaterial_orig)(_this);
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		return material;
	}

	void* Renderer_get_sharedMaterials_orig = nullptr;
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_sharedMaterials_hook(Il2CppObject* _this)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_sharedMaterials_hook)*>(Renderer_get_sharedMaterials_orig)(_this);
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
		return materials;
	}

	void* Renderer_set_material_orig = nullptr;
	void Renderer_set_material_hook(Il2CppObject* _this, Il2CppObject* material)
	{
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		reinterpret_cast<decltype(Renderer_set_material_hook)*>(Renderer_set_material_orig)(_this, material);
	}

	void* Renderer_set_materials_orig = nullptr;
	void Renderer_set_materials_hook(Il2CppObject* _this, Il2CppArraySize* materials)
	{
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = (Il2CppObject*)materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
		reinterpret_cast<decltype(Renderer_set_materials_hook)*>(Renderer_set_materials_orig)(_this, materials);
	}

	void* Material_SetTextureI4_orig = nullptr;
	void Material_SetTextureI4_hook(Il2CppObject* _this, int nameID, Il2CppObject* texture)
	{
		if (texture && !il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				reinterpret_cast<decltype(Material_SetTextureI4_hook)*>(Material_SetTextureI4_orig)(_this, nameID, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(Material_SetTextureI4_hook)*>(Material_SetTextureI4_orig)(_this, nameID, texture);
	}

	void* Material_GetTextureImpl_orig = nullptr;
	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* _this, int nameID)
	{
		auto texture = reinterpret_cast<decltype(Material_GetTextureImpl_hook)*>(Material_GetTextureImpl_orig)(_this, nameID);
		if (texture && !il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				return newTexture;
			}
		}
		return texture;
	}

	void* Material_SetTextureImpl_orig = nullptr;
	void Material_SetTextureImpl_hook(Il2CppObject* _this, int nameID, Il2CppObject* texture)
	{
		if (texture && !il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_orig)(_this, nameID, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_orig)(_this, nameID, texture);
	}

	void* CharaPropRendererAccessor_SetTexture_orig = nullptr;
	void CharaPropRendererAccessor_SetTexture_hook(Il2CppObject* _this, Il2CppObject* texture)
	{
		if (!il2cppstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				reinterpret_cast<decltype(CharaPropRendererAccessor_SetTexture_hook)*>(CharaPropRendererAccessor_SetTexture_orig)(_this, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(CharaPropRendererAccessor_SetTexture_hook)*>(CharaPropRendererAccessor_SetTexture_orig)(_this, texture);
	}

	void* GameObject_GetComponent_orig = nullptr;
	Il2CppObject* GameObject_GetComponent_hook(Il2CppObject* _this, const Il2CppType* type)
	{
		auto component = reinterpret_cast<decltype(GameObject_GetComponent_hook)*>(GameObject_GetComponent_orig)(_this, type);
		if (component)
		{
			// cout << "Component: " << component->klass->name << endl;
			if ("AssetHolder"s == component->klass->name)
			{
				ReplaceAssetHolderTextures(component);
			}
		}
		return component;
	}

	void* GameObject_GetComponentFastPath_orig = nullptr;
	void GameObject_GetComponentFastPath_hook(Il2CppObject* _this, Il2CppObject* type, uintptr_t oneFurtherThanResultValue)
	{
		/*auto name = il2cpp_class_get_method_from_name_type<Il2CppString* (*)(Il2CppObject*)>(type->klass, "get_FullName", 0)->methodPointer(type);
		cout << "GameObject_GetComponentFastPath " << wide_u8(name->chars) << endl;*/
		reinterpret_cast<decltype(GameObject_GetComponentFastPath_hook)*>(GameObject_GetComponentFastPath_orig)(_this, type, oneFurtherThanResultValue);
		auto helper = UnityEngine::CastHelper{};
		int objSize = sizeof(helper.obj);
		memmove(&helper, reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), sizeof(UnityEngine::CastHelper<>));
		if (helper.obj)
		{
			// cout << "Helper " << helper.obj->klass->name << endl;
			if (string(helper.obj->klass->name).find("MeshRenderer") != string::npos)
			{
				// ReplaceRendererTexture(helper.obj);
			}
			if (string(helper.obj->klass->name).find("RawImage") != string::npos)
			{
				ReplaceRawImageTexture(helper.obj);
			}
			if (helper.obj->klass->name == "ImageCommon"s)
			{
				auto material = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(helper.obj->klass, "get_material", 0)->methodPointer(helper.obj);
				if (material)
				{
					ReplaceMaterialTexture(material);
				}
			}
			if (helper.obj->klass->name == "AssetHolder"s)
			{
				ReplaceAssetHolderTextures(helper.obj);
			}
			memmove(reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), &helper, sizeof(UnityEngine::CastHelper<>));
		}
	}

	void* Screen_set_orientation_orig = nullptr;

	void Screen_set_orientation_hook(UnityEngine::ScreenOrientation orientation)
	{
		if (!config::freeform_window)
		{
			reinterpret_cast<decltype(Screen_set_orientation_hook)*>(Screen_set_orientation_orig)(
				orientation);
		}
	}

	void* DeviceOrientationGuide_Show_orig = nullptr;

	void DeviceOrientationGuide_Show_hook(Il2CppObject* _this, bool isTargetOrientationPortrait,
		int target)
	{
		if (!config::freeform_window)
		{
			reinterpret_cast<decltype(DeviceOrientationGuide_Show_hook)*>(DeviceOrientationGuide_Show_orig)(
				_this, isTargetOrientationPortrait, target);
		}
	}

	void* NowLoading_Show_orig = nullptr;

	void NowLoading_Show_hook(Il2CppObject* _this, int type, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType, Il2CppObject* customInEffect, Il2CppObject* customLoopEffect, Il2CppObject* customOutEffect, int charaId)
	{
		// NowLoadingOrientation
		if (type == 2 && (config::freeform_window || !config::ui_loading_show_orientation_guide))
		{
			// NowLoadingTips
			type = 0;
		}
		if (!config::hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Show_hook)*>(NowLoading_Show_orig)(
				_this,
				type,
				onComplete, overrideDuration, easeType,
				customInEffect, customLoopEffect, customOutEffect, charaId);
		}
		if (onComplete && config::hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* NowLoading_Show3_orig = nullptr;

	void NowLoading_Show3_hook(Il2CppObject* _this, int type, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration)
	{
		// NowLoadingOrientation
		if (type == 2 && (config::freeform_window || !config::ui_loading_show_orientation_guide))
		{
			// NowLoadingTips
			type = 0;
		}
		if (!config::hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Show3_hook)*>(NowLoading_Show3_orig)(
				_this,
				type,
				onComplete, overrideDuration);
		}
		if (onComplete && config::hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* NowLoading_Hide_orig = nullptr;

	void NowLoading_Hide_hook(Il2CppObject* _this, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType, Il2CppDelegate* onUnloadCustomEffectResourcesComplete)
	{
		if (!config::hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Hide_hook)*>(NowLoading_Hide_orig)(_this, onComplete, overrideDuration, easeType, onUnloadCustomEffectResourcesComplete);
		}
		if (onComplete && config::hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* NowLoading_Hide1_orig = nullptr;

	void NowLoading_Hide1_hook(Il2CppObject* _this, Il2CppDelegate* onComplete)
	{
		if (!config::hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Hide1_hook)*>(NowLoading_Hide1_orig)(_this, onComplete);
		}
		if (onComplete && config::hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* WaitDeviceOrientation_orig = nullptr;

	Il2CppObject* WaitDeviceOrientation_hook(UnityEngine::ScreenOrientation targetOrientation)
	{
		if (!config::freeform_window)
		{
			return reinterpret_cast<decltype(WaitDeviceOrientation_hook)*>(WaitDeviceOrientation_orig)(
				targetOrientation);
		}

		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}

	void* SplashViewController_KakaoStart_orig;
	void SplashViewController_KakaoStart_hook(Il2CppObject* _this)
	{
		// no-op
	}

	CriWare::CriMana::MovieInfo* (*MoviePlayerBase_get_MovieInfo)(Il2CppObject* _this);
	CriWare::CriMana::MovieInfo* (*MovieManager_GetMovieInfo)(Il2CppObject* _this, Cute::Cri::MoviePlayerHandle playerHandle);

	void* MoviePlayerForUI_AdjustScreenSize_orig = nullptr;

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* self, UnityEngine::Vector2 dispRectWH, bool isPanScan)
	{
		auto movieInfo = MoviePlayerBase_get_MovieInfo(self);
		if (!movieInfo)
		{
			return;
		}

		if (movieInfo->width < movieInfo->height && !Gallop::StandaloneWindowResize::IsVirt())
		{
			auto ratio1 = static_cast<float>(movieInfo->width) / static_cast<float>(movieInfo->height);
			dispRectWH.x = dispRectWH.y * ratio1;
		}

		reinterpret_cast<decltype(MoviePlayerForUI_AdjustScreenSize_hook)*>(MoviePlayerForUI_AdjustScreenSize_orig)(self, dispRectWH, isPanScan);
	}

	void* FrameRateController_GetLayerFrameRate_orig = nullptr;
	int FrameRateController_GetLayerFrameRate_hook(Il2CppObject* _this, int layer)
	{
		return UnityEngine::Application::targetFrameRate();
	}

	void* FrameRateController_OverrideByNormalFrameRate_orig = nullptr;
	void FrameRateController_OverrideByNormalFrameRate_hook(Il2CppObject* _this, int layer)
	{
		// FrameRateOverrideLayer.SystemValue
		reinterpret_cast<decltype(FrameRateController_OverrideByNormalFrameRate_hook)*>(FrameRateController_OverrideByNormalFrameRate_orig)(_this, 0);
	}

	void* FrameRateController_OverrideByMaxFrameRate_orig = nullptr;
	void FrameRateController_OverrideByMaxFrameRate_hook(Il2CppObject* _this, int layer)
	{
		// FrameRateOverrideLayer.SystemValue
		reinterpret_cast<decltype(FrameRateController_OverrideByMaxFrameRate_hook)*>(FrameRateController_OverrideByMaxFrameRate_orig)(_this, 0);
	}

	void* FrameRateController_ResetOverride_orig = nullptr;
	void FrameRateController_ResetOverride_hook(Il2CppObject* _this, int layer)
	{
		// FrameRateOverrideLayer.SystemValue
		reinterpret_cast<decltype(FrameRateController_ResetOverride_hook)*>(FrameRateController_ResetOverride_orig)(_this, 0);
	}

	void* FrameRateController_ReflectionFrameRate_orig = nullptr;
	void FrameRateController_ReflectionFrameRate_hook(Il2CppObject* _this)
	{
		UnityEngine::Application::targetFrameRate(30);
	}

	Il2CppDelegate* updateFriendSearchButton = nullptr;

	bool UpdateFriendSearchButton()
	{
		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			return true;
		}

		auto controller = GetCurrentViewController();

		if (controller)
		{
			auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);

			Il2CppObject* ButtonIDSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_ButtonIDSearch", 0)->methodPointer(view);

			if (!GetButtonCommonOnClickDelegate(ButtonIDSearch))
			{
				return false;
			}

			auto fn = *[](void*)
				{
					auto viewerIdSearchFn = *[](void*)
						{
							auto fn = *[](void*, Il2CppString* text)
								{
									auto dialog = GetFrontDialog();
									auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

									auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
									Il2CppObject* ContentsObject;
									il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

									auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

									auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
									Il2CppObject* _inputField;
									il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

									auto text1 = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

									string searchId = il2cpp_u8(text1->chars);

									auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

									auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
									auto viewerIdObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredLong(*)(Il2CppObject*)>(workUserData->klass, "get_ViewerId", 0)->methodPointer(workUserData);
									auto viewerId = viewerIdObscured.GetDecrypted();

									if (searchId == to_string(viewerId))
									{
										il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FriendViewController", "PushErrorSearchOwn", IgnoreNumberOfArguments)();
										return;
									}

									auto onModifyFriendCloseFn = *[]()
										{
											auto controller = GetCurrentViewController();
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "UpdateCurrentTabView", 0)->methodPointer(controller);
										};

									il2cpp_symbols::get_method_pointer<void (*)(int64_t, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppObject*, bool, Il2CppDelegate*, bool, Il2CppDelegate*)>("umamusume.dll", "Gallop", "DialogTrainerInfo", "PushDialog", 10)
										(stoll(searchId.data()), CreateDelegateStatic(onModifyFriendCloseFn), nullptr, nullptr, nullptr, nullptr, false, nullptr, false, nullptr);
								};
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "DialogIDSearch", "PushDialog", 1)(CreateDelegateStatic(fn));

							auto dialog = GetFrontDialog();
							auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

							auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
							Il2CppObject* ContentsObject;
							il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

							auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

							auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
							Il2CppObject* _inputField;
							il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

							auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_inputField->klass, "get_onValueChanged", 0)->methodPointer(_inputField);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(onValueChanged->klass, "RemoveAllListeners", 0)->methodPointer(onValueChanged);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_inputField->klass, "Start", 0)->methodPointer(_inputField);


							auto valueChangeFn = *[](Il2CppObject* obj)
								{
									auto dialog = GetFrontDialog();
									auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

									auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
									Il2CppObject* ContentsObject;
									il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

									auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

									auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
									Il2CppObject* _inputField;
									il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

									auto value = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

									Il2CppObject* rightButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, uint64_t)>(dialog->klass, "GetButtonObj", 1)->methodPointer(dialog, 2);

									bool valid = false;

									if (value && value->length >= 9)
									{
										try
										{
											stoll(il2cpp_u8(value->chars).data());
											valid = true;
										}
										catch (invalid_argument e)
										{
										}
									}

									if (rightButton)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(rightButton->klass, "set_interactable", 1)->methodPointer(rightButton, valid);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(rightButton->klass, "SetNotificationMessage", 1)->methodPointer(rightButton, valid ? il2cpp_string_new("") : Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Friend0013"))));
									}
								};

							auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.string_class));

							void** params = new void* [1];
							params[0] = CreateDelegateWithClass(delegateClass, _inputField, valueChangeFn);

							Il2CppException* exception;

							il2cpp_runtime_invoke_type<void>(il2cpp_class_get_method_from_name(onValueChanged->klass, "AddListener", 1), onValueChanged, params, &exception);

							auto _myIdTextField = il2cpp_class_get_field_from_name(idSearch->klass, "_myIdText");
							Il2CppObject* _myIdText;
							il2cpp_field_get_value(idSearch, _myIdTextField, &_myIdText);

							auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

							auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
							auto viewerIdStringObscured = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workUserData->klass, "get_ViewerIdString", 0)->methodPointer(workUserData);

							auto viewerId = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(viewerIdStringObscured->klass, "InternalDecrypt", 0)->methodPointer(viewerIdStringObscured);

							text_set_text(_myIdText, viewerId);
						};

					auto playerIdSearchFn = *[](void*)
						{
							auto controller = GetCurrentViewController();
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "OnClickIDSearch", 0)->methodPointer(controller);
						};

					auto dialogData = Gallop::DialogCommon::Data();
					dialogData.SetSimpleTwoButtonMessage(
						Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Friend0039"))),
						il2cpp_string_new("트레이너 ID로 검색하시겠습니까?"),
						CreateDelegateStatic(viewerIdSearchFn),
						GetTextIdByName(IL2CPP_STRING("Outgame0002")),
						GetTextIdByName(IL2CPP_STRING("Common0003")),
						CreateDelegateStatic(playerIdSearchFn)
					);

					Gallop::DialogManager::PushDialog(dialogData);
				};

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(ButtonIDSearch->klass, "SetOnClick", 1)->methodPointer(ButtonIDSearch, &CreateUnityActionStatic(fn)->delegate);
			return true;

		}
		return false;
	}

	void* DialogCircleItemDonate_Initialize_orig = nullptr;

	void DialogCircleItemDonate_Initialize_hook(Il2CppObject* _this, Il2CppObject* dialog, Il2CppObject* itemRequestInfo)
	{
		reinterpret_cast<decltype(DialogCircleItemDonate_Initialize_hook)*>(DialogCircleItemDonate_Initialize_orig)(
			_this, dialog, itemRequestInfo);
		auto donateCountField = il2cpp_class_get_field_from_name(_this->klass, "_donateCount");
		il2cpp_field_set_value(_this, donateCountField,
			GetInt32Instance(
				il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(
					_this->klass, "CalcDonateItemMax",
					0)->methodPointer
					(_this)));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "ValidateDonateItemCount",
			0)->methodPointer
			(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "ApplyDonateItemCountText",
			0)->methodPointer
			(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "OnClickPlusButton",
			0)->methodPointer
			(_this);
	}

	void* Input_get_mousePosition_Injected_orig = nullptr;

	void Input_get_mousePosition_Injected_hook(UnityEngine::Vector3* out)
	{
		reinterpret_cast<decltype(Input_get_mousePosition_Injected_hook)*>(Input_get_mousePosition_Injected_orig)(out);
	}

	void* Certification_initDmmPlatformData_orig = nullptr;

	void Certification_initDmmPlatformData_hook()
	{
		reinterpret_cast<decltype(Certification_initDmmPlatformData_hook)*>(Certification_initDmmPlatformData_orig)();

		if (!Gallop::TitleViewController::viewerId.empty() && !Gallop::TitleViewController::onetimeToken.empty())
		{
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmViewerId", 1)(il2cpp_string_new(Gallop::TitleViewController::viewerId.data()));
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmOnetimeToken", 1)(il2cpp_string_new(Gallop::TitleViewController::onetimeToken.data()));
		}
	}

	void* LiveTheaterCharaSelect_CheckSwapChara_orig = nullptr;

	void LiveTheaterCharaSelect_CheckSwapChara_hook(Il2CppObject* self, int index, int oldCharaId, int oldDressId, int oldDressColorId, int oldDressId2, int oldDressColorId2, int newCharaId)
	{

	}

	void DumpMsgPackFile(const string& file_path, const char* buffer, const size_t len) {
		auto parent_path = filesystem::path(file_path).parent_path();
		if (!filesystem::exists(parent_path)) {
			filesystem::create_directories(parent_path);
		}
		ofstream file{ file_path, ios::binary };
		file.write(buffer, static_cast<int>(len));
		file.flush();
		file.close();
	}

	int64_t current_time() {
		auto ms = chrono::duration_cast<chrono::milliseconds>(
			chrono::system_clock::now().time_since_epoch());
		return ms.count();
	}

	void* UploadHandlerRaw_Create_orig = nullptr;

	void* UploadHandlerRaw_Create_hook(Il2CppObject* self, const char* data, int dataLength)
	{
		try
		{
			if (config::msgpack_notifier && config::msgpack_notifier_request)
			{
				notifier::notify_request(string(data, dataLength));
			}

			if (config::dump_msgpack && config::dump_msgpack_request)
			{
				string out_path =
					"msgpack_dump/"s.append(to_string(current_time())).append("Q.msgpack");

				DumpMsgPackFile(out_path, data, dataLength);
			}

			MsgPackData::ReadRequest(data, dataLength);

#ifdef EXPERIMENTS
			if (config::unlock_live_chara)
			{
				auto modified = MsgPackModify::ModifyRequest(data, dataLength);

				if (!modified.empty())
				{
					data = new char[modified.size()];
					memcpy(const_cast<char*>(data), modified.data(), modified.size());
				}
			}
#endif
		}
		catch (...)
		{
		}

		return reinterpret_cast<decltype(UploadHandlerRaw_Create_hook)*>(UploadHandlerRaw_Create_orig)(self, data, dataLength);
	}

	void* DownloadHandler_InternalGetByteArray_orig = nullptr;

	const char* DownloadHandler_InternalGetByteArray_hook(Il2CppObject* self, int* length)
	{
		auto data = reinterpret_cast<decltype(DownloadHandler_InternalGetByteArray_hook)*>(DownloadHandler_InternalGetByteArray_orig)(self, length);

		try
		{
			if (config::msgpack_notifier)
			{
				notifier::notify_response(string(data, *length));
			}

			if (config::dump_msgpack)
			{
				string out_path =
					"msgpack_dump/"s.append(to_string(current_time())).append("R.msgpack");

				DumpMsgPackFile(out_path, data, *length);
			}

			MsgPackData::ReadResponse(data, *length);

#ifdef EXPERIMENTS
			if (config::unlock_live_chara)
			{
				auto modified = MsgPackModify::ModifyResponse(data, *length);

				if (!modified.empty())
				{
					data = new char[modified.size()];
					memcpy(const_cast<char*>(data), modified.data(), modified.size());
				}
			}
#endif
		}
		catch (...)
		{
		}

		return data;
	}

	void* HttpHelper_CompressRequest_orig = nullptr;

	Il2CppArraySize_t<int8_t>* HttpHelper_CompressRequest_hook(Il2CppArraySize_t<int8_t>* data)
	{
		auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

		try
		{
			if (config::msgpack_notifier && config::msgpack_notifier_request)
			{
				notifier::notify_request(string(buf, data->max_length));
			}

			if (config::dump_msgpack && config::dump_msgpack_request)
			{
				string out_path =
					"msgpack_dump/"s.append(to_string(current_time())).append("Q.msgpack");

				DumpMsgPackFile(out_path, buf, data->max_length);
			}

			MsgPackData::ReadRequest(buf, data->max_length);

#ifdef EXPERIMENTS
			if (config::unlock_live_chara)
			{
				auto modified = MsgPackModify::ModifyRequest(buf, data->max_length);

				if (!modified.empty())
				{
					data = il2cpp_array_new_type<int8_t>(il2cpp_defaults.byte_class, modified.size());

					char* buf1 = reinterpret_cast<char*>(data) + kIl2CppSizeOfArray;
					memcpy(buf1, modified.data(), modified.size());
				}
			}
#endif
		}
		catch (...)
		{
		}

		return reinterpret_cast<decltype(HttpHelper_CompressRequest_hook)*>(HttpHelper_CompressRequest_orig)(data);
	}

	void* HttpHelper_DecompressResponse_orig = nullptr;

	Il2CppArraySize_t<int8_t>* HttpHelper_DecompressResponse_hook(Il2CppArraySize_t<int8_t>* compressed)
	{
		auto data = reinterpret_cast<decltype(HttpHelper_DecompressResponse_hook)*>(HttpHelper_DecompressResponse_orig)(compressed);

		try
		{
			auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

			if (config::msgpack_notifier)
			{
				notifier::notify_response(string(buf, data->max_length));
			}

			if (config::dump_msgpack)
			{
				string out_path =
					"msgpack_dump/"s.append(to_string(current_time())).append("R.msgpack");

				DumpMsgPackFile(out_path, buf, data->max_length);
			}

			MsgPackData::ReadResponse(buf, data->max_length);

#ifdef EXPERIMENTS
			if (config::unlock_live_chara)
			{
				auto modified = MsgPackModify::ModifyResponse(buf, data->max_length);

				if (!modified.empty())
				{
					data = il2cpp_array_new_type<int8_t>(il2cpp_defaults.byte_class, modified.size());

					char* buf1 = reinterpret_cast<char*>(data) + kIl2CppSizeOfArray;
					memcpy(buf1, modified.data(), modified.size());
				}
			}
#endif
		}
		catch (...)
		{
		}

		return data;
	}

	Il2CppObject* GetRaceManager()
	{
		return GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "RaceManager"));
	}

	Il2CppObject* GetRaceHorseManager()
	{
		auto instance = GetRaceManager();
		if (!instance)
		{
			return nullptr;
		}

		auto _horseManagerField = il2cpp_class_get_field_from_name(instance->klass, "_horseManager");
		Il2CppObject* _horseManager;
		il2cpp_field_get_value(instance, _horseManagerField, &_horseManager);

		return _horseManager;
	}

	il2cppstring GetRaceState()
	{
		auto instance = GetRaceManager();
		if (!instance)
		{
			return IL2CPP_STRING("");
		}

		auto state = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(instance->klass, "get_State", 0)->methodPointer(instance);
		auto stateName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "RaceDefine/RaceState"), state);
		return stateName->chars;
	}

	il2cppstring GetRaceCameraMode()
	{
		auto instance = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "RaceCameraManager"));
		if (!instance)
		{
			return IL2CPP_STRING("");
		}

		auto mode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(instance->klass, "GetCameraMode", 0)->methodPointer(instance);
		auto modeName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "RaceCameraManager/CameraMode"), mode);
		return modeName->chars;
	}

	bool IsRace() {
		auto stateName = GetRaceState();
		if (!stateName.empty())
		{
			return IL2CPP_STRING("Race") == stateName;
		}
		return false;
	}

	discord::Core* discord;

	uint64_t startTime;

	void StartTickFrame();

	void TickDiscord()
	{
		if (discord)
		{
			auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
			if (director)
			{
				float currentTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveCurrentTime", 0)->methodPointer(director);
				float totalTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveTotalTime", 0)->methodPointer(director);
				string name;

				auto titleField = il2cpp_class_get_field_from_name(director->klass, "TitleController");
				Il2CppObject* title;
				il2cpp_field_get_value(director, titleField, &title);
				if (title)
				{
					auto textField = il2cpp_class_get_field_from_name(title->klass, "_titleText");
					Il2CppObject* text;
					il2cpp_field_get_value(title, textField, &text);
					if (text)
					{
						name = il2cpp_u8(text_get_text(text)->chars);
						if (name.empty())
						{
							name = "Live (unknown)";
						}
					}
				}

				auto songId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(director->klass, "GetPlaySongId", 0)->methodPointer(director);

				if (discord)
				{
					discord::Activity activity{};
					activity.SetDetails(name.data());
					activity.GetAssets().SetLargeImage("jacket_"s.append(to_string(songId)).data());
					activity.GetAssets().SetLargeText(name.data());
					activity.GetAssets().SetSmallImage("umamusume");
					activity.SetType(discord::ActivityType::Watching);
					activity.GetTimestamps().SetStart(
						(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - (static_cast<int64_t>(totalTime * 1000))));
					activity.GetTimestamps().SetEnd(
						(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - (static_cast<int64_t>(totalTime * 1000))) + (static_cast<int64_t>(currentTime * 1000)));
					discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
				}
			}
			else
			{
				auto uiManager = Gallop::UIManager::Instance();
				auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
				if (uiManager && sceneManager)
				{
					string detail;

					auto CommonHeaderTitle = uiManager.CommonHeaderTitle();

					if (CommonHeaderTitle)
					{
						auto _cacheTextField = il2cpp_class_get_field_from_name(CommonHeaderTitle->klass, "_cacheText");
						Il2CppString* _cacheText;
						il2cpp_field_get_value(CommonHeaderTitle, _cacheTextField, &_cacheText);

						if (_cacheText)
						{
							detail = il2cpp_u8(_cacheText->chars);
						}
					}

					if (detail.empty())
					{
						auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);
						auto viewName = il2cpp_u8(GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "SceneDefine/ViewId"), viewId)->chars);
						detail = GetViewName(viewName);

						if (detail.empty())
						{
							// detail = GetSceneName()
						}
					}

					if (discord && !detail.empty())
					{
						discord::Activity activity{};
						activity.GetAssets().SetLargeImage("umamusume");
						activity.SetDetails(detail.data());
						activity.GetTimestamps().SetStart(startTime);
						activity.GetTimestamps().SetEnd(0);
						activity.SetType(discord::ActivityType::Playing);
						discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
					}
				}
			}

			if (discord)
			{
				discord->RunCallbacks();
			}
		}
	}

	void TickFrame()
	{
		try
		{
			if (config::unlock_size || config::freeform_window)
			{
				SetBGCanvasScalerSize();
			}

			if (config::freeform_window)
			{
				ResizeMoviePlayer();
			}

			if (config::discord_rich_presence)
			{
				TickDiscord();
			}

			auto active = il2cpp_symbols::get_method_pointer<UnityEngine::SceneManagement::Scene(*)()>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager", "GetActiveScene", IgnoreNumberOfArguments)();

			auto handleName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "Scene", "GetNameInternal", 1)(active.handle);

			if (handleName)
			{
				il2cppstring sceneName = handleName->chars;

				if (sceneName == IL2CPP_STRING("Live"))
				{
					auto controller = GetCurrentViewController();

					if (controller && controller->klass->name == "LiveViewController"s)
					{
						auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
						if (director)
						{
							auto LiveCurrentTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveCurrentTime", 0)->methodPointer(director);
							auto LiveTotalTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveTotalTime", 0)->methodPointer(director);

							auto sliderCommon = Localify::UIParts::GetOptionSlider("live_slider");

							auto textCommon = Localify::UIParts::GetTextCommon("live_slider");

							if (textCommon)
							{
								auto timeMin = static_cast<int>(LiveCurrentTime / 60);
								auto timeSec = static_cast<int>(fmodf(LiveCurrentTime, 60));

								auto timeMinIl2Cpp = u8_il2cpp(to_string(timeMin));
								auto timeSecIl2Cpp = u8_il2cpp(to_string(timeSec));

								il2cppstringstream str;
								str << setw(2) << setfill(IL2CPP_STRING('0')) << timeSecIl2Cpp;

								Localify::UIParts::SetTextCommonText(textCommon, (timeMinIl2Cpp + IL2CPP_STRING(":") + str.str()).data());
							}

							auto textCommonTotal = Localify::UIParts::GetTextCommon("live_slider_total");

							if (textCommonTotal)
							{
								auto timeMin = static_cast<int>(LiveTotalTime / 60);
								auto timeSec = static_cast<int>(fmodf(LiveTotalTime, 60));

								auto timeMinIl2Cpp = u8_il2cpp(to_string(timeMin));
								auto timeSecIl2Cpp = u8_il2cpp(to_string(timeSec));

								il2cppstringstream str;
								str << setw(2) << setfill(IL2CPP_STRING('0')) << timeSecIl2Cpp;

								Localify::UIParts::SetTextCommonText(textCommonTotal, (timeMinIl2Cpp + IL2CPP_STRING(":") + str.str()).data());
							}

							if (config::live_playback_loop)
							{
								if (LiveCurrentTime >= LiveTotalTime - 0.1f)
								{
									LiveCurrentTime = 0;
									MoveLivePlayback(LiveCurrentTime);
								}
							}

							try
							{
								if (sliderCommon)
								{
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_maxValue", 1)->methodPointer(sliderCommon, LiveTotalTime);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "SetValueWithoutNotify", 1)->methodPointer(sliderCommon, LiveCurrentTime);
								}
							}
							catch (const Il2CppExceptionWrapper& e)
							{
								cout << e.ex->klass->name << ": ";
								wcout << e.ex->message << endl;
							}
						}
					}
				}

				if (SystemMediaTransportControlsManager::instance.IsEnabled())
				{
					if (sceneName == IL2CPP_STRING("Home"))
					{
						bool hasSetList = false;

						auto hubViewController = GetCurrentHubViewChildController();

						if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
						{
							auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);
							if (topUi)
							{
								auto get_TempSetListPlayingData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(topUi->klass, "get_TempSetListPlayingData", 0);
								if (get_TempSetListPlayingData)
								{
									auto data = get_TempSetListPlayingData->methodPointer(topUi);

									auto IsPlayingField = il2cpp_class_get_field_from_name(data->klass, "IsPlaying");
									bool IsPlaying;
									il2cpp_field_get_value(data, IsPlayingField, &IsPlaying);

									hasSetList = IsPlaying;

									if (IsPlaying)
									{
										SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Playing);

										auto SetListIndexField = il2cpp_class_get_field_from_name(data->klass, "SetListIndex");
										int SetListIndex;
										il2cpp_field_get_value(data, SetListIndexField, &SetListIndex);

										int MusicListCount = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(data->klass, "GetMusicListCount", 0)->methodPointer(data);

										SystemMediaTransportControlsManager::instance.IsPreviousEnabled(SetListIndex > 0);
										SystemMediaTransportControlsManager::instance.IsNextEnabled(SetListIndex < MusicListCount - 1);

										auto musicData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(data->klass, "GetMasterSetListMusicData", 0)->methodPointer(data);

										auto MusicIdField = il2cpp_class_get_field_from_name(musicData->klass, "MusicId");
										int MusicId;
										il2cpp_field_get_value(musicData, MusicIdField, &MusicId);

										SystemMediaTransportControlsManager::UpdateMetadata(MusicId);
									}
								}
							}
						}

						if (!hasSetList)
						{
							auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

							auto workJukeboxData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_JukeboxData", 0)->methodPointer(workDataManager);
							auto currentBgmMusicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(workJukeboxData->klass, "GetCurrentBgmMusicId", 0)->methodPointer(workJukeboxData);

							SystemMediaTransportControlsManager::UpdateMetadata(currentBgmMusicId);

							if (currentBgmMusicId)
							{
								SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Playing);
							}
							else
							{
								SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Paused);
							}
						}
					}

					if (sceneName == IL2CPP_STRING("Live"))
					{
						auto controller = GetCurrentViewController();

						if (controller && controller->klass->name == "LiveViewController"s)
						{
							auto _stateField = il2cpp_class_get_field_from_name(controller->klass, "_state");
							Gallop::LiveViewController::LiveState state;
							il2cpp_field_get_value(controller, _stateField, &state);

							if (state == Gallop::LiveViewController::LiveState::Play)
							{
								SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Playing);
							}
							else
							{
								SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Paused);
							}
						}
					}
				}
			}

			StartTickFrame();
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "TickFrame error: " << e.ex->message->chars << endl;
		}
	}

	void StartTickFrame()
	{
		static auto tickFrameDelegate = CreateDelegateStatic(TickFrame);

		try
		{
			auto GameSystem = Gallop::GameSystem::Instance();
			if (GameSystem)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(GameSystem, tickFrameDelegate);
			}
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "StartTickFrame error: " << e.ex->message->chars << endl;
		}
	}

	LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
	HHOOK hCBTHook = SetWindowsHookExW(WH_CBT, CBTProc, nullptr, GetCurrentThreadId());

	LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HCBT_MINMAX)
		{
			if (lParam != SW_RESTORE)
			{
				if (UnityEngine::Screen::fullScreen())
				{
					return TRUE;
				}
			}
		}

		return CallNextHookEx(hCBTHook, nCode, wParam, lParam);
	}

	void adjust_size()
	{
		thread([]()
			{
				auto tr = il2cpp_thread_attach(il2cpp_domain_get());

				UnityEngine::Resolution r;
				get_resolution_stub(&r);

				auto target_height = r.height - 100;

				set_resolution(target_height * config::runtime::ratioVertical, target_height, false);

				il2cpp_thread_detach(tr);
			}).detach();
	}

	void* load_scene_internal_orig = nullptr;
	void* load_scene_internal_hook(Il2CppString* sceneName, int sceneBuildIndex, void* parameters, bool mustCompleteNextFrame)
	{
		printf("%ls\n", sceneName->chars);
		return reinterpret_cast<decltype(load_scene_internal_hook)*>(load_scene_internal_orig)(sceneName, sceneBuildIndex, parameters, mustCompleteNextFrame);
	}

	void* CollectRaidBgCutinHelper_InstantiateTimeline_orig = nullptr;
	bool CollectRaidBgCutinHelper_InstantiateTimeline_hook(Il2CppObject* self, Il2CppString* path, Il2CppObject* parent, Il2CppObject* renderTexture, float fovFactor)
	{
		if (renderTexture)
		{
			auto width = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(renderTexture->klass, "get_width", 0)->methodPointer(renderTexture);
			auto height = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(renderTexture->klass, "get_height", 0)->methodPointer(renderTexture);

			auto ratio = static_cast<float>(width) / static_cast<float>(height);

			auto newWidth = UnityEngine::Screen::height();
			auto newHeight = static_cast<int>(static_cast<float>(newWidth) / ratio);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderTexture->klass, "set_width", 1)->methodPointer(renderTexture, newWidth);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderTexture->klass, "set_height", 1)->methodPointer(renderTexture, newHeight);
		}

		return reinterpret_cast<decltype(CollectRaidBgCutinHelper_InstantiateTimeline_hook)*>(CollectRaidBgCutinHelper_InstantiateTimeline_orig)(self, path, parent, renderTexture, fovFactor);
	}

	void* Header_Initialize_orig = nullptr;
	void Header_Initialize_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(Header_Initialize_hook)*>(Header_Initialize_orig)(self);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _menuButtonField = il2cpp_class_get_field_from_name(self->klass, "_menuButton");
			Il2CppObject* _menuButton;
			il2cpp_field_get_value(self, _menuButtonField, &_menuButton);

			UnityEngine::MonoBehaviour(_menuButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_menuButton->klass, "SetOnClick", 1)->methodPointer(_menuButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*, Gallop::DialogCommon::DispStackType, Gallop::DialogCommonBase::FormType)>("umamusume.dll", "Gallop", "DialogHomeMenuMain", "Open", 4)(nullptr, nullptr, Gallop::DialogCommon::DispStackType::DialogOnDialog, Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
					})->delegate);
		}
	}

	void* DialogHomeMenuMain_ChangeView_orig = nullptr;
	void DialogHomeMenuMain_ChangeView_hook(Il2CppObject* self, uint64_t viewId)
	{
		reinterpret_cast<decltype(DialogHomeMenuMain_ChangeView_hook)*>(DialogHomeMenuMain_ChangeView_orig)(self, viewId);

		if (!Gallop::Screen::IsLandscapeMode())
		{
			return;
		}

		auto _menuDialogField = il2cpp_class_get_field_from_name(self->klass, "_menuDialog");
		Il2CppObject* _menuDialog;
		il2cpp_field_static_get_value(_menuDialogField, &_menuDialog);

		if (_menuDialog)
		{
			auto dialogCommon = UnityEngine::GameObject(_menuDialog).GetComponent(GetRuntimeType("umamusume.dll", "Gallop", "DialogCommon"));
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "DialogManager", "PopDialog", 2)(dialogCommon, nullptr);
			_menuDialog = nullptr;
			il2cpp_field_static_set_value(_menuDialogField, _menuDialog);
		}
	}

	void* DialogHomeMenuMain_ChangeView2_orig = nullptr;
	void DialogHomeMenuMain_ChangeView2_hook(Il2CppObject* self, uint64_t viewId, Il2CppObject* viewInfo)
	{
		reinterpret_cast<decltype(DialogHomeMenuMain_ChangeView2_hook)*>(DialogHomeMenuMain_ChangeView2_orig)(self, viewId, viewInfo);

		if (!Gallop::Screen::IsLandscapeMode())
		{
			return;
		}

		auto _menuDialogField = il2cpp_class_get_field_from_name(self->klass, "_menuDialog");
		Il2CppObject* _menuDialog;
		il2cpp_field_static_get_value(_menuDialogField, &_menuDialog);

		if (_menuDialog)
		{
			auto dialogCommon = UnityEngine::GameObject(_menuDialog).GetComponent(GetRuntimeType("umamusume.dll", "Gallop", "DialogCommon"));
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "DialogManager", "PopDialog", 2)(dialogCommon, nullptr);
			_menuDialog = nullptr;
			il2cpp_field_static_set_value(_menuDialogField, _menuDialog);
		}
	}

	void* PartsSingleModeCommonFooter_Setup_orig = nullptr;
	void PartsSingleModeCommonFooter_Setup_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(PartsSingleModeCommonFooter_Setup_hook)*>(PartsSingleModeCommonFooter_Setup_orig)(self);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _logButtonField = il2cpp_class_get_field_from_name(self->klass, "_logButton");
			Il2CppObject* _logButton;
			il2cpp_field_get_value(self, _logButtonField, &_logButton);

			UnityEngine::MonoBehaviour(_logButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_logButton->klass, "SetOnClick", 1)->methodPointer(_logButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickLog", 0)->methodPointer(self);
					})->delegate);
		}
	}

	void* DialogSingleModeTopMenu_Setup_orig = nullptr;
	void DialogSingleModeTopMenu_Setup_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(DialogSingleModeTopMenu_Setup_hook)*>(DialogSingleModeTopMenu_Setup_orig)(self);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _infoButtonField = il2cpp_class_get_field_from_name(self->klass, "_infoButton");
			Il2CppObject* _infoButton;
			il2cpp_field_get_value(self, _infoButtonField, &_infoButton);

			UnityEngine::MonoBehaviour(_infoButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_infoButton->klass, "SetOnClick", 1)->methodPointer(_infoButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Gallop::DialogCommon::DispStackType, Gallop::DialogCommonBase::FormType)>("umamusume.dll", "Gallop", "DialogSingleModeStartConfirmEntry", "OpenPlayingSingleMode", 2)(Gallop::DialogCommon::DispStackType::DialogOnDialog, Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
					})->delegate);
		}
	}

	void* DialogSingleModeTopMenu_Setup1_orig = nullptr;
	void DialogSingleModeTopMenu_Setup1_hook(Il2CppObject* self, uint64_t dialogType)
	{
		reinterpret_cast<decltype(DialogSingleModeTopMenu_Setup1_hook)*>(DialogSingleModeTopMenu_Setup1_orig)(self, dialogType);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _infoButtonField = il2cpp_class_get_field_from_name(self->klass, "_infoButton");
			Il2CppObject* _infoButton;
			il2cpp_field_get_value(self, _infoButtonField, &_infoButton);

			UnityEngine::MonoBehaviour(_infoButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_infoButton->klass, "SetOnClick", 1)->methodPointer(_infoButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Gallop::DialogCommon::DispStackType, Gallop::DialogCommonBase::FormType)>("umamusume.dll", "Gallop", "DialogSingleModeStartConfirmEntry", "OpenPlayingSingleMode", 2)(Gallop::DialogCommon::DispStackType::DialogOnDialog, Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);
					})->delegate);
		}
	}

	void* DialogSingleModeTopMenu_SetupButtonPositions_orig = nullptr;
	void DialogSingleModeTopMenu_SetupButtonPositions_hook(Il2CppObject* self)
	{
		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			return;
		}

		reinterpret_cast<decltype(DialogSingleModeTopMenu_SetupButtonPositions_hook)*>(DialogSingleModeTopMenu_SetupButtonPositions_orig)(self);
	}

	void* SingleModeRaceEntryViewController_SetupBottomButtons_orig = nullptr;
	void SingleModeRaceEntryViewController_SetupBottomButtons_hook(Il2CppObject* self, uint64_t degree)
	{
		reinterpret_cast<decltype(SingleModeRaceEntryViewController_SetupBottomButtons_hook)*>(SingleModeRaceEntryViewController_SetupBottomButtons_orig)(self, degree);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _viewField = il2cpp_class_get_field_from_name(self->klass, "_view");
			Il2CppObject* _view;
			il2cpp_field_get_value(self, _viewField, &_view);

			auto ReserveListButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_view->klass, "get_ReserveListButton", 0)->methodPointer(_view);

			UnityEngine::MonoBehaviour(ReserveListButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(ReserveListButton->klass, "SetOnClick", 1)->methodPointer(ReserveListButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickReserveListButton", 0)->methodPointer(self);
					})->delegate);
		}
	}

	void* StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_orig = nullptr;
	Vector3 StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_hook()
	{
		auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat");
		auto field = il2cpp_class_get_field_from_name(klass, "STAMP_ANCHORED_POS_AFTER_BASE");
		Vector3 value;

		il2cpp_field_static_get_value(field, &value);

		return value;
	}

	void* StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_orig = nullptr;
	Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_hook()
	{
		auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat");
		auto field = il2cpp_class_get_field_from_name(klass, "INPUT_LOCAL_POS_AFTER_BASE");
		Vector3 value;

		il2cpp_field_static_get_value(field, &value);

		return value;
	}

	void* StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_orig = nullptr;
	Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_hook()
	{
		auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat");
		auto field = il2cpp_class_get_field_from_name(klass, "INPUT_ANCHORED_POS_AFTER_BASE");
		Vector3 value;

		il2cpp_field_static_get_value(field, &value);

		return value;
	}

	void* StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_orig = nullptr;
	Vector3 StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_hook()
	{
		auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat");
		auto field = il2cpp_class_get_field_from_name(klass, "INPUT_SIZE_DELTA_POS_AFTER_BASE");
		Vector3 value;

		il2cpp_field_static_get_value(field, &value);

		return value;
	}

	void* ResourcePath_GetPreInTitleMoviePath_orig = nullptr;
	Il2CppString* ResourcePath_GetPreInTitleMoviePath_hook()
	{
		auto GameDefine = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GameDefine");
		auto PREIN_TITLE_OP_VERSION_Field = il2cpp_class_get_field_from_name(GameDefine, "PREIN_TITLE_OP_VERSION");
		uint64_t PREIN_TITLE_OP_VERSION;
		il2cpp_field_static_get_value(PREIN_TITLE_OP_VERSION_Field, &PREIN_TITLE_OP_VERSION);

		auto ResourcePath = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ResourcePath");

		auto _stringBuilderField = il2cpp_class_get_field_from_name(ResourcePath, "_stringBuilder");
		Il2CppObject* _stringBuilder;
		il2cpp_field_static_get_value(_stringBuilderField, &_stringBuilder);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

		auto boxedOpVersion = il2cpp_value_box(il2cpp_defaults.int32_class, &PREIN_TITLE_OP_VERSION);

		Il2CppString* titleMovieFormat = nullptr;

		auto TITLE_MOVIE_FORMAT_DEFAULT_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT_DEFAULT");

		if (TITLE_MOVIE_FORMAT_DEFAULT_Field)
		{
			Il2CppString* TITLE_MOVIE_FORMAT_DEFAULT;
			il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_DEFAULT_Field, &TITLE_MOVIE_FORMAT_DEFAULT);

			auto TITLE_MOVIE_FORMAT_LANDSCAPE_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT_LANDSCAPE");
			Il2CppString* TITLE_MOVIE_FORMAT_LANDSCAPE;
			il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_LANDSCAPE_Field, &TITLE_MOVIE_FORMAT_LANDSCAPE);

			if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
			{
				titleMovieFormat = TITLE_MOVIE_FORMAT_LANDSCAPE;
			}
			else
			{
				titleMovieFormat = TITLE_MOVIE_FORMAT_DEFAULT;
			}
		}
		else
		{
			auto TITLE_MOVIE_FORMAT_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT");
			Il2CppString* TITLE_MOVIE_FORMAT;
			il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_Field, &TITLE_MOVIE_FORMAT);

			titleMovieFormat = TITLE_MOVIE_FORMAT;

			if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
			{
				auto streamingAssetsPath = UnityEngine::Application::streamingAssetsPath()->chars;

				il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);
				auto titleMovie = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder)->chars;
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

				if (filesystem::exists(streamingAssetsPath + il2cppstring(IL2CPP_STRING("/PreinResource/Movie/m/")) + titleMovie + IL2CPP_STRING("_landscape.usm")))
				{
					titleMovieFormat = il2cpp_string_new("Title/title_movie_{0:D2}_landscape");
				}
				else
				{
					titleMovieFormat = TITLE_MOVIE_FORMAT;
				}
			}
			else
			{
				titleMovieFormat = TITLE_MOVIE_FORMAT;
			}
		}

		il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);

		return il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder);
	}

	void* ResourcePath_GetLatestAssetBundleTitleMoviePath_orig = nullptr;
	Il2CppString* ResourcePath_GetLatestAssetBundleTitleMoviePath_hook()
	{
		auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SaveDataManager"));
		auto SaveLoader = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)->methodPointer(SaveDataManager);
		int NewOpProgress = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(SaveLoader->klass, "get_NewOpProgress", 0)->methodPointer(SaveLoader);

		auto ResourcePath = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ResourcePath");

		auto _stringBuilderField = il2cpp_class_get_field_from_name(ResourcePath, "_stringBuilder");
		Il2CppObject* _stringBuilder;
		il2cpp_field_static_get_value(_stringBuilderField, &_stringBuilder);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

		auto boxedOpVersion = il2cpp_value_box(il2cpp_defaults.int32_class, &NewOpProgress);

		Il2CppString* titleMovieFormat = nullptr;

		auto ASSET_BUNDLE_TITLE_MOVIE_FORMAT_Field = il2cpp_class_get_field_from_name(ResourcePath, "ASSET_BUNDLE_TITLE_MOVIE_FORMAT");
		Il2CppString* ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
		il2cpp_field_static_get_value(ASSET_BUNDLE_TITLE_MOVIE_FORMAT_Field, &ASSET_BUNDLE_TITLE_MOVIE_FORMAT);

		titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;

		if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
		{
			auto streamingAssetsPath = UnityEngine::Application::streamingAssetsPath()->chars;

			il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);
			auto titleMovie = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder)->chars;
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

			if (filesystem::exists(streamingAssetsPath + il2cppstring(IL2CPP_STRING("/PreinResource/Movie/m/")) + titleMovie + IL2CPP_STRING("_landscape.usm")))
			{
				titleMovieFormat = il2cpp_string_new("Title/title_movie_{0:D2}_landscape");
			}
			else
			{
				titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
			}
		}
		else
		{
			titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
		}

		il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);

		return il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder);
	}

	void* CircleChat_Initialize_orig = nullptr;
	void CircleChat_Initialize_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(CircleChat_Initialize_hook)*>(CircleChat_Initialize_orig)(self);

		if (config::freeform_window && Gallop::Screen::IsLandscapeMode())
		{
			auto _itemRequestButtonField = il2cpp_class_get_field_from_name(self->klass, "_itemRequestButton");
			Il2CppObject* _itemRequestButton;
			il2cpp_field_get_value(self, _itemRequestButtonField, &_itemRequestButton);

			UnityEngine::MonoBehaviour(_itemRequestButton).gameObject().SetActive(true);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_itemRequestButton->klass, "SetOnClick", 1)->methodPointer(_itemRequestButton,
				&CreateUnityAction(self, *[](Il2CppObject* self)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickItemRequest", 0)->methodPointer(self);
					})->delegate);
		}
	}


	void* LiveTempData_ctor_orig = nullptr;
	void LiveTempData_ctor_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(LiveTempData_ctor_hook)*>(LiveTempData_ctor_orig)(self);

		auto _screenModeDicField = il2cpp_class_get_field_from_name(self->klass, "_cachedLiveScreenModeDic");
		Il2CppObject* _screenModeDic;
		il2cpp_field_get_value(self, _screenModeDicField, &_screenModeDic);

		auto entriesField = il2cpp_class_get_field_from_name(_screenModeDic->klass, "_entries");
		if (!entriesField)
		{
			entriesField = il2cpp_class_get_field_from_name(_screenModeDic->klass, "entries");
		}

		Il2CppArraySize_t<System::Collections::Generic::Dictionary<int, int>::Entry>* entries;
		il2cpp_field_get_value(_screenModeDic, entriesField, &entries);

		vector<int> musicIds;

		if (entries)
		{
			for (int i = 0; i < entries->max_length; i++)
			{
				auto entry = entries->vector[i];

				if (entry.key)
				{
					musicIds.emplace_back(entry.key);
				}
			}
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_screenModeDic->klass, "Clear", 0)->methodPointer(_screenModeDic);

		for (auto musicId : musicIds)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(_screenModeDic->klass, "Add", 2)->methodPointer(_screenModeDic, musicId, 0);
		}
	}

	void patch_game_assembly()
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			auto binData = il2cpp_symbols::get_class("uncheatercsd.dll", "Uncheater", "SystemBins64");
			auto binArrayField = il2cpp_class_get_field_from_name(binData->klass, "UNCHEATER_DATA");
			auto array = il2cpp_array_new(il2cpp_defaults.object_class, 128);

			auto byteArray = il2cpp_array_new(il2cpp_defaults.byte_class, 0);

			il2cpp_array_setref(array, 8, byteArray);
			il2cpp_array_setref(array, 9, byteArray);

			il2cpp_field_static_set_value(binArrayField, array);
		}

		if (!mh_inited)
		{
			return;
		}

		printf("Trying to patch GameAssembly.dll...\n");

#pragma region HOOK_MACRO
#define ADD_HOOK(_name_, _fmt_) \
	auto _name_##_offset = reinterpret_cast<void*>(_name_##_addr); \
	\
	printf(_fmt_, _name_##_offset); \
	dump_bytes(_name_##_offset); \
	\
	auto _name_##_create_result = MH_CreateHook(_name_##_offset, _name_##_hook, &_name_##_orig); \
	if (_name_##_create_result != MH_OK) \
	{\
		cout << "WARN: " << #_name_ << " Create Failed: " << MH_StatusToString(_name_##_create_result) << endl; \
	}\
	auto _name_##_result = MH_EnableHook(_name_##_offset); \
	if (_name_##_result != MH_OK) \
	{\
		cout << "WARN: " << #_name_ << " Failed: " << MH_StatusToString(_name_##_result) << " LastError: " << GetLastError() << endl << endl; \
		_name_##_orig = _name_##_addr; \
	}
#pragma endregion
#pragma region HOOK_ADDRESSES
		auto populate_with_errors_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.TextRenderingModule.dll",
			"UnityEngine", "TextGenerator",
			"PopulateWithErrors", 3
		);

		auto get_preferred_width_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.TextRenderingModule.dll",
			"UnityEngine", "TextGenerator",
			"GetPreferredWidth", 2
		);

		auto localizeextension_text_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "LocalizeExtention", "Text", 1
		);

		auto update_addr = il2cpp_symbols::get_method_pointer(
			"DOTween.dll", "DG.Tweening.Core", "TweenManager", "Update", 3
		);

		auto CySpringUpdater_set_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop.Model.Component",
			"CySpringUpdater", "set_SpringUpdateMode", 1
		);

		auto CySpringUpdater_get_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop.Model.Component",
			"CySpringUpdater", "get_SpringUpdateMode", 0
		);

		auto StandaloneWindowResize_DisableMaximizebox_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "DisableMaximizebox", IgnoreNumberOfArguments
		);

		auto StandaloneWindowResize_ReshapeAspectRatio_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "ReshapeAspectRatio", IgnoreNumberOfArguments
		);

		auto StandaloneWindowResize_KeepAspectRatio_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "KeepAspectRatio", 2
		);

		auto GallopInput_mousePosition_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopInput", "mousePosition", 0
		);

		auto GallopFrameBuffer_Initialize_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopFrameBuffer", "Initialize", 1
		);

		auto GallopFrameBuffer_Initialize2_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopFrameBuffer", "Initialize", 2
		);

		auto GallopFrameBuffer_Release_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopFrameBuffer", "Release", 0
		);

		auto GallopFrameBuffer_ResizeRenderTexture_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopFrameBuffer", "ResizeRenderTexture", 0
		);

		display_get_main = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Display", "get_main", IgnoreNumberOfArguments);

		get_system_width = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Display", "get_systemWidth", 0);

		get_system_height = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Display", "get_systemHeight", 0);

		get_rendering_width = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Display", "get_renderingWidth", 0);

		get_rendering_height = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Display", "get_renderingHeight", 0);

		get_resolution = reinterpret_cast<decltype(get_resolution)>(il2cpp_resolve_icall("UnityEngine.Screen::get_currentResolution_Injected(UnityEngine.Resolution&)"));

		auto story_race_textasset_load_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryRaceTextAsset", "Load", 0);

		auto get_modified_string_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GetModifiedString", IgnoreNumberOfArguments);

		set_scale_factor = il2cpp_symbols::get_method_pointer<void(*)(void*, float)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"CanvasScaler", "set_scaleFactor", 1
		);

		auto on_populate_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextCommon", "OnPopulateMesh", 1
		);

		auto textcommon_awake_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextCommon", "Awake", 0
		);

		auto TMP_Settings_get_instance_addr = il2cpp_symbols::get_method_pointer(
			"Unity.TextMeshPro.dll", "TMPro", "TMP_Settings", "get_instance", IgnoreNumberOfArguments);

		auto TextMeshProUguiCommon_Awake_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextMeshProUguiCommon", "Awake", 0
		);

		auto textcommon_SetSystemTextWithLineHeadWrap_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextCommon", "SetSystemTextWithLineHeadWrap", 2
		);

		auto textcommon_SetTextWithLineHeadWrapWithColorTag_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextCommon", "SetTextWithLineHeadWrapWithColorTag", 2
		);

		auto textcommon_SetTextWithLineHeadWrap_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"TextCommon", "SetTextWithLineHeadWrap", 2
		);

		textcommon_get_TextId = il2cpp_symbols::get_method_pointer<int (*)(void*)>(
			"umamusume.dll", "Gallop",
			"TextCommon", "get_TextId", 0
		);

		text_get_text = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(void*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "get_text", 0
		);
		text_set_text = il2cpp_symbols::get_method_pointer<void (*)(void*, Il2CppString*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_text", 1
		);

		text_assign_font = il2cpp_symbols::get_method_pointer<void(*)(void*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "AssignDefaultFont", 0
		);

		text_set_font = il2cpp_symbols::get_method_pointer<void (*)(void*, Il2CppObject*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_font", 1
		);

		text_get_font = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(void*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "get_font", 0
		);

		text_get_size = il2cpp_symbols::get_method_pointer<int(*)(void*)>(
			"umamusume.dll", "Gallop",
			"TextCommon", "get_FontSize", 0
		);

		text_set_size = il2cpp_symbols::get_method_pointer<void(*)(void*, int)>(
			"umamusume.dll", "Gallop",
			"TextCommon", "set_FontSize", 1
		);

		text_get_linespacing = il2cpp_symbols::get_method_pointer<float(*)(void*)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "get_lineSpacing", 0
		);

		text_set_style = il2cpp_symbols::get_method_pointer<void(*)(void*, int)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_fontStyle", 1
		);

		text_set_linespacing = il2cpp_symbols::get_method_pointer<void(*)(void*, float)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_lineSpacing", 1
		);

		text_set_horizontalOverflow = il2cpp_symbols::get_method_pointer<void(*)(void*, int)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_horizontalOverflow", 1
		);

		text_set_verticalOverflow = il2cpp_symbols::get_method_pointer<void(*)(void*, int)>(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"Text", "set_verticalOverflow", 1
		);

		set_resolution = il2cpp_symbols::get_method_pointer<void (*)(int, int, bool)>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Screen", "SetResolution", 3
		);

		auto an_text_fix_data_addr = il2cpp_symbols::get_method_pointer("Plugins.dll", "AnimateToUnity", "AnText", "_FixData", 0);

		auto an_text_set_material_to_textmesh_addr = il2cpp_symbols::get_method_pointer("Plugins.dll", "AnimateToUnity", "AnText", "_SetMaterialToTextMesh", 0);

		auto load_zekken_composite_resource_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "ModelLoader", "LoadZekkenCompositeResourceInternal", 0);

		auto set_anti_aliasing_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_antiAliasing(System.Int32)");

		auto set_vSyncCount_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_vSyncCount()");

		auto set_anisotropicFiltering_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_anisotropicFiltering(UnityEngine.AnisotropicFiltering)");

		auto apply_graphics_quality_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "ApplyGraphicsQuality", 2);

		auto GraphicSettings_Get3DAntiAliasingLevel_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "Get3DAntiAliasingLevel", 1);

		auto GraphicSettings_GetVirtualResolution3D_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "GetVirtualResolution3D", 1);

		auto GraphicSettings_GetVirtualResolution3D2_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "GetVirtualResolution3D", 2);

		auto GraphicSettings_GetVirtualResolution_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "GetVirtualResolution", 0);

		auto NowLoading_Show_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "NowLoading", "Show", 8);

		auto NowLoading_Hide_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "NowLoading", "Hide", 4);

		MoviePlayerBase_get_MovieInfo = il2cpp_symbols::get_method_pointer<CriWare::CriMana::MovieInfo * (*)(Il2CppObject*)>(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerBase", "get_MovieInfo", 0);

		MovieManager_GetMovieInfo = il2cpp_symbols::get_method_pointer<CriWare::CriMana::MovieInfo * (*)(Il2CppObject*, Cute::Cri::MoviePlayerHandle)>(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "GetMovieInfo", 1);

		auto FrameRateController_GetLayerFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "GetLayerFrameRate", 1);

		auto FrameRateController_OverrideByNormalFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByNormalFrameRate", 1);

		auto FrameRateController_OverrideByMaxFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByMaxFrameRate", 1);

		auto FrameRateController_ResetOverride_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ResetOverride", 1);

		auto FrameRateController_ReflectionFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ReflectionFrameRate", 0);

		auto BGManager_CalcBgScale_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "BGManager", "CalcBgScale", 4);

		auto GallopUtil_GotoTitleOnError_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GotoTitleOnError", 1);

		auto DialogCommon_Close_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCommon", "Close", 0);

		auto StoryViewController_ctor_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryViewController", ".ctor", 0);

		auto DialogCircleItemDonate_Initialize_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCircleItemDonate", "Initialize", 2);

		auto Object_Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");

		auto Input_get_mousePosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.Input::get_mousePosition_Injected(UnityEngine.Vector3&)");

		auto Certification_initDmmPlatformData_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "Certification", "initDmmPlatformData", IgnoreNumberOfArguments);

		auto UploadHandlerRaw_Create_addr = il2cpp_resolve_icall("UnityEngine.Networking.UploadHandlerRaw::Create()");

		auto DownloadHandler_InternalGetByteArray_addr = il2cpp_resolve_icall("UnityEngine.Networking.DownloadHandler::InternalGetByteArray()");

		auto HttpHelper_CompressRequest_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "HttpHelper", "CompressRequest", 1);

		auto HttpHelper_DecompressResponse_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "HttpHelper", "DecompressResponse", 1);

		auto LiveTheaterCharaSelect_CheckSwapChara_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "LiveTheaterCharaSelect", "CheckSwapChara", 7);

		auto GallopStandaloneInputModule_SetPointerPosition_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "UnityEngine.EventSystems", "GallopStandaloneInputModule", "SetPointerPosition", 0);

		auto GallopStandaloneInputModule_ProcessTouchEvents_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "UnityEngine.EventSystems", "GallopStandaloneInputModule", "ProcessTouchEvents", 0);

		auto CollectRaidBgCutinHelper_InstantiateTimeline_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "CollectRaidBgCutinHelper", "InstantiateTimeline", 4);

		auto Header_Initialize_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "Header", "Initialize", 0);

		auto PartsSingleModeCommonFooter_Setup_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "PartsSingleModeCommonFooter", "Setup", 0);

		auto DialogSingleModeTopMenu_Setup_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogSingleModeTopMenu", "Setup", 0);

		auto DialogSingleModeTopMenu_Setup1_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogSingleModeTopMenu", "Setup", 1);

		auto DialogSingleModeTopMenu_SetupButtonPositions_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogSingleModeTopMenu", "SetupButtonPositions", 0);

		auto SingleModeRaceEntryViewController_SetupBottomButtons_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "SingleModeRaceEntryViewController", "SetupBottomButtons", 1);

		auto StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat", "get_STAMP_ANCHORED_POS_AFTER", 0);

		auto StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat", "get_INPUT_LOCAL_POS_AFTER", 0);

		auto StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat", "get_INPUT_ANCHORED_POS_AFTER", 0);

		auto StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StaticVariableDefine/Circle/CircleChat", "get_INPUT_SIZE_DELTA_POS_AFTER", 0);

		auto CircleChat_Initialize_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "CircleChat", "Initialize", 0);

		auto ResourcePath_GetPreInTitleMoviePath_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "ResourcePath", "GetPreInTitleMoviePath", 0);

		auto ResourcePath_GetLatestAssetBundleTitleMoviePath_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "ResourcePath", "GetLatestAssetBundleTitleMoviePath", 0);

		auto DialogHomeMenuMain_ChangeView_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogHomeMenuMain", "ChangeView", 1);

		auto DialogHomeMenuMain_ChangeView2_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogHomeMenuMain", "ChangeView", 2);

		auto SceneManager_BootView_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "SceneManager", "BootView", 0);

		auto load_scene_internal_addr = il2cpp_resolve_icall("UnityEngine.SceneManagement.SceneManager::LoadSceneAsyncNameIndexInternal_Injected(System.String,System.Int32,UnityEngine.SceneManagement.LoadSceneParameters&,System.bool)");

#pragma endregion

		if (Game::CurrentGameStore != Game::Store::Steam)
		{
			ADD_HOOK(Certification_initDmmPlatformData, "Gallop.Certification::initDmmPlatformData at %p\n");
		}

		// ADD_HOOK(GallopStandaloneInputModule_SetPointerPosition, "UnityEngine.EventSystems.GallopStandaloneInputModule::SetPointerPosition at %p\n");

		// ADD_HOOK(GallopStandaloneInputModule_ProcessTouchEvents, "UnityEngine.EventSystems.GallopStandaloneInputModule::ProcessTouchEvents at %p\n");

		ADD_HOOK(Object_Internal_CloneSingleWithParent, "UnityEngine.Object::Internal_CloneSingleWithParent at %p\n");

		ADD_HOOK(DialogCircleItemDonate_Initialize, "Gallop.DialogCircleItemDonate::Initialize at %p\n");

		// ADD_HOOK(DialogCommon_Close, "Gallop.DialogCommon.Close() at %p\n");

		// ADD_HOOK(GallopUtil_GotoTitleOnError, "Gallop.GallopUtil.GotoTitleOnError() at %p\n");

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			ADD_HOOK(DownloadHandler_InternalGetByteArray, "UnityEngine.Networking.DownloadHandler::InternalGetByteArray at %p\n");
			ADD_HOOK(UploadHandlerRaw_Create, "UnityEngine.Networking.UploadHandlerRaw::Create at %p\n");
		}
		else
		{
			ADD_HOOK(HttpHelper_DecompressResponse, "Gallop.HttpHelper::DecompressResponse at %p\n");
			ADD_HOOK(HttpHelper_CompressRequest, "Gallop.HttpHelper::CompressRequest at %p\n");
		}

		if (config::anisotropic_filtering != -1)
		{
			ADD_HOOK(set_anisotropicFiltering, "UnityEngine.QualitySettings.set_anisotropicFiltering(UnityEngine.AnisotropicFiltering) at %p\n");
			set_anisotropicFiltering_hook(config::anisotropic_filtering);
		}

		if (config::vsync_count != -1)
		{
			ADD_HOOK(set_vSyncCount, "UnityEngine.QualitySettings.set_vSyncCount() at %p\n");
			set_vSyncCount_hook(config::vsync_count);
		}

		// TODO Gallop.NowLoading::ShowCustomWipeFlash
		ADD_HOOK(NowLoading_Show, "Gallop.NowLoading::Show at %p\n");

		ADD_HOOK(NowLoading_Hide, "Gallop.NowLoading::Hide at %p\n");

		ADD_HOOK(load_zekken_composite_resource, "Gallop.ModelLoader::LoadZekkenCompositeResource at %p\n");

		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			ADD_HOOK(an_text_fix_data, "AnimateToUnity.AnText::_FixData at %p\n");
			ADD_HOOK(an_text_set_material_to_textmesh, "AnimateToUnity.AnText::_SetMaterialToTextMesh at %p\n");

			// ADD_HOOK(get_preferred_width, "UnityEngine.TextGenerator::GetPreferredWidth at %p\n");

			// hook UnityEngine.TextGenerator::PopulateWithErrors to modify text
			ADD_HOOK(populate_with_errors, "UnityEngine.TextGenerator::PopulateWithErrors at %p\n");

			// ADD_HOOK(text_get_text, "UnityEngine.UI.Text::get_text at %p\n");

			ADD_HOOK(textcommon_SetTextWithLineHeadWrap, "Gallop.TextCommon::SetTextWithLineHeadWrap at %p\n");
			ADD_HOOK(textcommon_SetTextWithLineHeadWrapWithColorTag, "Gallop.TextCommon::SetTextWithLineHeadWrapWithColorTag at %p\n");
			ADD_HOOK(textcommon_SetSystemTextWithLineHeadWrap, "Gallop.TextCommon::SetSystemTextWithLineHeadWrap at %p\n");
		}

		ADD_HOOK(story_race_textasset_load, "StoryRaceTextAsset.Load at %p\n");

		ADD_HOOK(get_modified_string, "GallopUtil_GetModifiedString at %p\n");

		ADD_HOOK(update, "DG.Tweening.Core.TweenManager::Update at %p\n");

		ADD_HOOK(sqlite3_key, "Plugin::sqlite3_key at %p\n");

		if (!config::replace_text_db_path.empty())
		{
			if (MasterDB::InitReplacementMasterDB(il2cpp_u8(config::replace_text_db_path.data())))
			{
				ADD_HOOK(sqlite3_prepare_v2, "Plugin::sqlite3_prepare_v2 at %p\n");
				ADD_HOOK(sqlite3_step, "Plugin::sqlite3_step at %p\n");
				ADD_HOOK(sqlite3_reset, "Plugin::sqlite3_reset at %p\n");
				ADD_HOOK(sqlite3_bind_text, "Plugin::sqlite3_bind_text at %p\n");
				ADD_HOOK(sqlite3_bind_int, "Plugin::sqlite3_bind_int at %p\n");
				ADD_HOOK(sqlite3_bind_int64, "Plugin::sqlite3_bind_int64 at %p\n");
				ADD_HOOK(sqlite3_bind_double, "Plugin::sqlite3_bind_double at %p\n");
				ADD_HOOK(sqlite3_column_text, "Plugin::sqlite3_column_text at %p\n");
				ADD_HOOK(sqlite3_finalize, "Plugin::sqlite3_finalize at %p\n");
			}
		}

		if (config::cyspring_update_mode != -1)
		{
			ADD_HOOK(CySpringUpdater_set_SpringUpdateMode, "CySpringUpdater::set_SpringUpdateMode at %p\n");
			ADD_HOOK(CySpringUpdater_get_SpringUpdateMode, "CySpringUpdater::get_SpringUpdateMode at %p\n");
		}

		// ADD_HOOK(load_scene_internal, "SceneManager::LoadSceneAsyncNameIndexInternal at %p\n");

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			/*auto SplashViewController_KakaoStart_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"Gallop", "SplashViewController", "KakaoStart", 0);*/

				// ADD_HOOK(SplashViewController_KakaoStart, "SplashViewController::KakaoStart at %p\n");
		}

		if (config::replace_to_builtin_font || config::replace_to_custom_font)
		{
			ADD_HOOK(on_populate, "Gallop.TextCommon::OnPopulateMesh at %p\n");
			ADD_HOOK(textcommon_awake, "Gallop.TextCommon::Awake at %p\n");
			ADD_HOOK(TextMeshProUguiCommon_Awake, "Gallop.TextMeshProUguiCommon::Awake at %p\n");
			ADD_HOOK(TMP_Settings_get_instance, "TMPro.TMP_Settings::get_instance at %p\n");
		}

		if (config::max_fps > -1)
		{
			// break 30-40fps limit
			ADD_HOOK(FrameRateController_GetLayerFrameRate, "Gallop.FrameRateController::GetLayerFrameRate at %p\n");
			ADD_HOOK(FrameRateController_OverrideByNormalFrameRate, "Gallop.FrameRateController::OverrideByNormalFrameRate at %p\n");
			ADD_HOOK(FrameRateController_OverrideByMaxFrameRate, "Gallop.FrameRateController::OverrideByMaxFrameRate at %p\n");
			ADD_HOOK(FrameRateController_ResetOverride, "Gallop.FrameRateController::ResetOverride at %p\n");
			ADD_HOOK(FrameRateController_ReflectionFrameRate, "Gallop.FrameRateController::ReflectionFrameRate at %p\n");
		}

		if (config::unlock_size || config::freeform_window)
		{
			ADD_HOOK(GallopFrameBuffer_Initialize, "Gallop.GallopFrameBuffer::Initialize at %p\n");
			ADD_HOOK(GallopFrameBuffer_Initialize2, "Gallop.GallopFrameBuffer::Initialize2 at %p\n");
			ADD_HOOK(GallopFrameBuffer_Release, "Gallop.GallopFrameBuffer::Release at %p\n");
			ADD_HOOK(GallopFrameBuffer_ResizeRenderTexture, "Gallop.GallopFrameBuffer::ResizeRenderTexture at %p\n");
			ADD_HOOK(GallopInput_mousePosition, "Gallop.GallopInput::mousePosition at %p\n");
		}

		if (config::freeform_window || Game::CurrentGameStore == Game::Store::Steam)
		{
			ADD_HOOK(StandaloneWindowResize_DisableMaximizebox, "Gallop.StandaloneWindowResize::DisableMaximizebox at %p\n");
		}

		if (config::freeform_window)
		{
			ADD_HOOK(StandaloneWindowResize_ReshapeAspectRatio, "Gallop.StandaloneWindowResize::ReshapeAspectRatio at %p\n");
			ADD_HOOK(StandaloneWindowResize_KeepAspectRatio, "Gallop.StandaloneWindowResize::KeepAspectRatio at %p\n");

			ADD_HOOK(DialogSingleModeTopMenu_SetupButtonPositions, "Gallop.DialogSingleModeTopMenu::SetupButtonPositions at %p\n");
			ADD_HOOK(DialogSingleModeTopMenu_Setup, "Gallop.DialogSingleModeTopMenu::Setup at %p\n");
			ADD_HOOK(DialogSingleModeTopMenu_Setup1, "Gallop.DialogSingleModeTopMenu::Setup1 at %p\n");
			ADD_HOOK(PartsSingleModeCommonFooter_Setup, "Gallop.PartsSingleModeCommonFooter::Setup at %p\n");
			ADD_HOOK(Header_Initialize, "Gallop.Header::Initialize at %p\n");
			ADD_HOOK(SingleModeRaceEntryViewController_SetupBottomButtons, "Gallop.SingleModeRaceEntryViewController::SetupBottomButtons at %p\n");
			ADD_HOOK(CircleChat_Initialize, "Gallop.CircleChat::Initialize at %p\n");
			ADD_HOOK(StaticVariableDefine_Circle_CircleChat_STAMP_ANCHORED_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_STAMP_ANCHORED_POS_AFTER at %p\n");
			ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_LOCAL_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_LOCAL_POS_AFTER at %p\n");
			ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_ANCHORED_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_ANCHORED_POS_AFTER at %p\n");
			ADD_HOOK(StaticVariableDefine_Circle_CircleChat_INPUT_SIZE_DELTA_POS_AFTER, "Gallop.StaticVariableDefine.Circle.CircleChat::get_INPUT_SIZE_DELTA_POS_AFTER at %p\n");
			ADD_HOOK(ResourcePath_GetPreInTitleMoviePath, "Gallop.ResourcePath::GetPreInTitleMoviePath at %p\n");
			ADD_HOOK(ResourcePath_GetLatestAssetBundleTitleMoviePath, "Gallop.ResourcePath::GetLatestAssetBundleTitleMoviePath at %p\n");
			ADD_HOOK(DialogHomeMenuMain_ChangeView, "Gallop.DialogHomeMenuMain::ChangeView at %p\n");
			ADD_HOOK(DialogHomeMenuMain_ChangeView2, "Gallop.DialogHomeMenuMain::ChangeView2 at %p\n");

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			Gallop::StandaloneWindowResize::IsVirt(width <= height);

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

			auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenWidth");

			auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenHeight");

			if (width < height)
			{
				il2cpp_field_static_set_value(NUMBER1080_Field, &width);
				il2cpp_field_static_set_value(NUMBER1920_Field, &height);
				il2cpp_field_static_set_value(_originalScreenHeight_Field, &width);
				il2cpp_field_static_set_value(_originalScreenWidth_Field, &height);
			}
			else
			{
				il2cpp_field_static_set_value(NUMBER1920_Field, &width);
				il2cpp_field_static_set_value(NUMBER1080_Field, &height);
				il2cpp_field_static_set_value(_originalScreenWidth_Field, &width);
				il2cpp_field_static_set_value(_originalScreenHeight_Field, &height);
			}

			auto WaitDeviceOrientation_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"Gallop", "Screen", "WaitDeviceOrientation", 1);

			auto DeviceOrientationGuide_Show_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"Gallop", "DeviceOrientationGuide", "Show", 2);

			auto MoviePlayerForUI_AdjustScreenSize_addr = il2cpp_symbols::get_method_pointer(
				"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerForUI", "AdjustScreenSize", 2);

			// ADD_HOOK(WaitDeviceOrientation, "Gallop.Screen::WaitDeviceOrientation at %p\n");
			ADD_HOOK(DeviceOrientationGuide_Show, "DeviceOrientationGuide::Show at %p\n");
			ADD_HOOK(MoviePlayerForUI_AdjustScreenSize, "MoviePlayerForUI::AdjustScreenSize at %p\n");
		}

		if (config::graphics_quality != -1)
		{
			apply_graphics_quality_orig = apply_graphics_quality_addr;
		}

		if (config::freeform_window || config::unlock_size || config::resolution_3d_scale != 1.0f)
		{
			// ADD_HOOK(BGManager_CalcBgScale, "Gallop.BGManager::CalcBgScale at %p\n");
		}

		if (config::resolution_3d_scale != 1.0f || config::freeform_window)
		{
			ADD_HOOK(GraphicSettings_GetVirtualResolution3D, "Gallop.GraphicSettings::GetVirtualResolution3D at %p\n");
			ADD_HOOK(GraphicSettings_GetVirtualResolution3D2, "Gallop.GraphicSettings::GetVirtualResolution3D2 at %p\n");
			ADD_HOOK(GraphicSettings_GetVirtualResolution, "Gallop.GraphicSettings::GetVirtualResolution at %p\n");
		}

		if (config::anti_aliasing != -1)
		{
			ADD_HOOK(set_anti_aliasing, "UnityEngine.QualitySettings::set_antiAliasing at %p\n");
			ADD_HOOK(GraphicSettings_Get3DAntiAliasingLevel, "Gallop.GraphicSettings::Get3DAntiAliasingLevel at %p\n");
		}

		if (config::unlock_live_chara)
		{
			ADD_HOOK(LiveTheaterCharaSelect_CheckSwapChara, "Gallop.LiveTheaterCharaSelect::CheckSwapChara at %p\n");
		}

		const auto nameArray = reinterpret_cast<Il2CppArraySize_t<Il2CppString*>*(*)()>(il2cpp_resolve_icall("UnityEngine.QualitySettings::get_names()"))();
		reinterpret_cast<void(*)(int)>(il2cpp_resolve_icall("UnityEngine.QualitySettings::SetQualityLevel()"))(nameArray->max_length - 1);
	}



	Il2CppDelegate* moviePlayerResize = nullptr;

	void patch_after_criware()
	{
		il2cppstring amuid = UnityEngine::Application::companyName()->chars;
		amuid += IL2CPP_STRING(".Gallop");

		if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			amuid += IL2CPP_STRING(".Eng");
		}

		il2cppstring title;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			title = IL2CPP_STRING("우마무스메");
		}
		else if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			title = IL2CPP_STRING("Umamusume");
		}
		else
		{
			title = IL2CPP_STRING("ウマ娘");
		}

		DesktopNotificationManagerCompat::RegisterAumidAndComServer(amuid.data(), title.data());

		DesktopNotificationManagerCompat::RegisterActivator();

		unique_ptr<DesktopNotificationHistoryCompat> history;
		DesktopNotificationManagerCompat::get_History(&history);
		history->Clear();

		auto gallop_get_screenwidth_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"Screen", "get_Width", 0
		);

		auto gallop_get_screenheight_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"Screen", "get_Height", 0
		);

		auto SetResolution_Injected_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution_Injected(System.Int32,System.Int32,UnityEngine.FullScreenMode,UnityEngine.RefreshRate)");

		auto Sprite_get_texture_addr = il2cpp_resolve_icall("UnityEngine.Sprite::get_texture(UnityEngine.Sprite)");

		auto Renderer_get_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_material", 0);

		auto Renderer_get_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_materials", 0);

		auto Renderer_get_sharedMaterial_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterial", 0);

		auto Renderer_get_sharedMaterials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterials", 0);

		auto Renderer_set_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_material", 1);

		auto Renderer_set_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_materials", 1);

		auto Material_GetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::GetTextureImpl(System.String,System.Int32)");

		auto Material_SetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::SetTextureImpl(System.String,System.Int32,UnityEngine.Texture)");

		auto CharaPropRendererAccessor_SetTexture_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "CharaPropRendererAccessor", "SetTexture", 1);

		auto GameObject_GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent(System.Type)");

		auto GameObject_GetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentFastPath(System.Type,System.IntPtr)");

		if (!config::replace_assetbundle_file_paths.empty())
		{
			ADD_HOOK(GameObject_GetComponent, "UnityEngine.GameObject::GetComponent at %p\n");

			ADD_HOOK(Sprite_get_texture, "UnityEngine.Sprite::get_texture at %p\n");

			ADD_HOOK(Renderer_get_material, "UnityEngine.Renderer::get_material at %p\n");

			ADD_HOOK(Renderer_get_materials, "UnityEngine.Renderer::get_materials at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterial, "UnityEngine.Renderer::get_sharedMaterial at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterials, "UnityEngine.Renderer::get_sharedMaterials at %p\n");

			ADD_HOOK(Renderer_set_material, "UnityEngine.Renderer::set_material at %p\n");

			ADD_HOOK(Renderer_set_materials, "UnityEngine.Renderer::set_materials at %p\n");

			ADD_HOOK(Material_GetTextureImpl, "UnityEngine.Material::GetTextureImpl at %p\n");

			ADD_HOOK(Material_SetTextureImpl, "UnityEngine.Material::SetTextureImpl at %p\n");

			ADD_HOOK(CharaPropRendererAccessor_SetTexture, "Gallop.CharaPropRendererAccessor::SetTexture at %p\n");
		}

		if (config::dump_entries)
		{
			Gallop::Localize::DumpAllEntries();
		}

		if (config::auto_fullscreen || config::unlock_size || config::freeform_window)
		{
			ADD_HOOK(SetResolution_Injected, "UnityEngine.Screen.SetResolution_Injected(int, int, FullScreenMode, RefreshRate) at %p\n");
		}

		if (config::unlock_size || config::freeform_window)
		{
			ADD_HOOK(gallop_get_screenwidth, "Gallop.Screen::get_Width at %p\n");
			ADD_HOOK(gallop_get_screenheight, "Gallop.Screen::get_Height at %p\n");

			auto display = display_get_main();
			if (config::initial_width > 72 && config::initial_height > 72)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				auto ratio = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);

				if (config::initial_width < config::initial_height)
				{
					config::runtime::ratioVertical = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);
					config::runtime::ratioHorizontal = static_cast<float>(config::initial_height) / static_cast<float>(config::initial_width);

					if (config::unlock_size_use_system_resolution)
					{
						last_display_width = static_cast<float>(get_system_width(display)) * ratio;
						last_display_height = get_system_width(display);
					}
					else
					{
						last_display_width = config::initial_width;
						last_display_height = config::initial_height;
					}
				}
				else
				{
					config::runtime::ratioVertical = static_cast<float>(config::initial_height) / static_cast<float>(config::initial_width);
					config::runtime::ratioHorizontal = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);

					if (config::unlock_size_use_system_resolution)
					{
						last_display_width = get_system_width(display);
						last_display_height = static_cast<float>(get_system_width(display)) * ratio;
					}
					else
					{
						last_display_width = config::initial_width;
						last_display_height = config::initial_height;
					}
				}
			}
			else
			{
				if (config::freeform_window)
				{
					auto hWnd = GetHWND();

					RECT clientRect;
					GetClientRect(hWnd, &clientRect);

					last_display_width = clientRect.right - clientRect.left;
					last_display_height = clientRect.bottom - clientRect.top;
				}
				else
				{
					last_display_width = get_system_width(display);
					last_display_height = ceil(last_display_width / ratio_16_9);
				}
			}

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name(GallopScreen, "NUMBER1080");

			auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenWidth");

			auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name(GallopScreen, "_originalScreenHeight");

			bool isPortrait = last_display_width < last_display_height;

			if (isPortrait)
			{
				il2cpp_field_static_set_value(NUMBER1080_Field, &last_display_width);
				il2cpp_field_static_set_value(NUMBER1920_Field, &last_display_height);
				il2cpp_field_static_set_value(_originalScreenHeight_Field, &last_display_width);
				il2cpp_field_static_set_value(_originalScreenWidth_Field, &last_display_height);
			}
			else
			{
				il2cpp_field_static_set_value(NUMBER1920_Field, &last_display_width);
				il2cpp_field_static_set_value(NUMBER1080_Field, &last_display_height);
				il2cpp_field_static_set_value(_originalScreenWidth_Field, &last_display_width);
				il2cpp_field_static_set_value(_originalScreenHeight_Field, &last_display_height);
			}
		}

		if (config::auto_fullscreen || config::unlock_size || config::freeform_window)
		{
			if (config::initial_width > 72 && config::initial_height > 72)
			{
				auto display = display_get_main();
				if (config::initial_width < config::initial_height)
				{
					last_hriz_window_width = last_display_height - 400;
					last_hriz_window_height = last_hriz_window_width / config::runtime::ratioHorizontal;

					if (last_hriz_window_height >= get_system_height(display))
					{
						last_hriz_window_height = get_system_height(display) - 400;
						last_hriz_window_width = last_hriz_window_height * config::runtime::ratioHorizontal;
					}

					last_virt_window_height = last_display_width - 400;
					last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;
					}
				}
				else
				{
					last_hriz_window_width = last_display_width - 400;
					last_hriz_window_height = last_hriz_window_width / config::runtime::ratioHorizontal;

					if (last_hriz_window_height >= get_system_height(display))
					{
						last_hriz_window_height = get_system_height(display) - 400;
						last_hriz_window_width = last_hriz_window_height * config::runtime::ratioHorizontal;
					}

					last_virt_window_height = last_display_height - 400;
					last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;
					}
				}
			}
			else
			{
				if (config::freeform_window)
				{
					last_hriz_window_width = last_display_width;
					last_hriz_window_height = last_display_height;
					last_virt_window_height = last_display_height;
					last_virt_window_width = last_display_width;
				}
				else
				{
					last_hriz_window_width = last_display_width - 400;
					last_hriz_window_height = last_hriz_window_width / config::runtime::ratioHorizontal;
					last_virt_window_height = last_display_height - 400;
					last_virt_window_width = last_virt_window_height * config::runtime::ratioVertical;
				}
			}


			if (config::auto_fullscreen)
			{
				// adjust_size();
			}
		}

		if (!config::unlock_live_chara)
		{
			try
			{
				auto path = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("Cute.Core.Assembly.dll", "Cute.Core", "Device", "GetPersistentDataPath", 0)()->chars;

				if (filesystem::exists(path + il2cppstring(IL2CPP_STRING(R"(\master\master_orig.mdb)"))))
				{
					filesystem::remove_all(path + il2cppstring(IL2CPP_STRING(R"(\master)")));
				}
			}
			catch (const exception& ex)
			{
				wcerr << L"Failed to remove master_orig.mdb: " << ex.what() << endl;
			}
		}

		fullScreenFl = UnityEngine::Screen::fullScreen();

		if (config::discord_rich_presence)
		{
			discord::Core::Create(1080397170215223367, static_cast<uint64_t>(discord::CreateFlags::NoRequireDiscord), &discord);
			startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		}

		StartTickFrame();

		auto sceneManagerClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager");

		auto activeSceneChangedField = il2cpp_class_get_field_from_name(sceneManagerClass, "activeSceneChanged");

		auto action = CreateDelegateWithClassStatic(il2cpp_class_from_type(activeSceneChangedField->type), *([](void*, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::Scene scene1)
			{
				auto hWnd = GetHWND();

				if (config::freeform_window)
				{
					long style = GetWindowLongW(hWnd, GWL_STYLE);
					style |= WS_MAXIMIZEBOX;
					SetWindowLongPtrW(hWnd, GWL_STYLE, style);
				}

				auto active = il2cpp_symbols::get_method_pointer<UnityEngine::SceneManagement::Scene(*)()>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager", "GetActiveScene", IgnoreNumberOfArguments)();

				auto handleName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "Scene", "GetNameInternal", 1)(active.handle);

				if (!handleName)
				{
					return;
				}

				il2cppstring sceneName = handleName->chars;

				isPortraitBeforeFullscreen = false;

				auto uiManager = Gallop::UIManager::Instance();

				/*if (config::resolution_3d_scale != 1.0f)
				{
					auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

					if (graphicSettings)
					{
						auto _resolutionScaleField = il2cpp_class_get_field_from_name(graphicSettings->klass, "_resolutionScale");

						il2cpp_field_set_value(graphicSettings, _resolutionScaleField, &config::resolution_3d_scale);

						auto _resolutionScale2DField = il2cpp_class_get_field_from_name(graphicSettings->klass, "_resolutionScale2D");

						il2cpp_field_set_value(graphicSettings, _resolutionScale2DField, &config::resolution_3d_scale);
					}
				}*/

				/*if (config::graphics_quality > -1)
				{
					auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
					if (graphicSettings)
					{
						apply_graphics_quality_hook(graphicSettings, config::graphics_quality, true);
					}
				}*/

				// auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

				// if (graphicSettings)
				// {
					// const auto get_RenderingManager = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(graphicSettings->klass, "get_RenderingManager", 0);
					// if (get_RenderingManager)
					// {
						// auto renderingManager = get_RenderingManager->methodPointer(graphicSettings);
						// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderingManager->klass, "set_IsOverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, true);

						/*if (config::anti_aliasing == 0)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderingManager->klass, "set_OverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, 1);
						}
						else
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderingManager->klass, "set_OverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, 0);
						}*/
						// }
					// }

				/*Il2CppArraySize_t<Il2CppObject*>* CriWareInitializerList;
				CriWareInitializerList = UnityEngine::Object::FindObjectsByType(
					GetRuntimeType("CriMw.CriWare.Runtime.dll", "CriWare", "CriWareInitializer"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

				if (CriWareInitializerList && CriWareInitializerList->max_length)
				{
					auto obj = CriWareInitializerList->vector[0];
					auto useDecrypterField = il2cpp_class_get_field_from_name(obj->klass, "useDecrypter");
					bool useDecrypter;
					il2cpp_field_get_value(obj, useDecrypterField, &useDecrypter);

					cout << "useDecrypter " << boolalpha << useDecrypter << noboolalpha << endl;

					if (useDecrypter)
					{
						auto decrypterConfigField = il2cpp_class_get_field_from_name(obj->klass, "DecrypterConfig");
						Il2CppObject* decrypterConfig;
						il2cpp_field_get_value(obj, decrypterConfigField, &decrypterConfig);

						if (decrypterConfig)
						{
							auto keyField = il2cpp_class_get_field_from_name(decrypterConfig->klass, "key");
							Il2CppString* key;
							il2cpp_field_get_value(decrypterConfig, keyField, &key);

							cout << "key: " << wide_u8(key->chars) << endl;
							cout << "key length: " << (key ? il2cpp_string_length(key) : 0) << endl;
						}
					}
				}*/

				//if (sceneName == IL2CPP_STRING("Race")
				//{
				//	auto delayCallback = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
				//		{
				//			auto horseManager = GetRaceHorseManager();

				//			cout << "horseManager " << horseManager << endl;
				//		}))->delegate;

				//	// Delay 50ms
				//	il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, delayCallback, true);
				//}

				if (sceneName == IL2CPP_STRING("_Boot") && Game::CurrentGameStore == Game::Store::Steam)
				{
					Gallop::StandaloneWindowResize::IsVirt(!Gallop::Screen::IsLandscapeMode());
				}

				if (sceneName == IL2CPP_STRING("Title"))
				{
					TaskbarManager::SetProgressValue(0, 0);
					TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
				}

				if (sceneName == IL2CPP_STRING("OutGame") && Game::CurrentGameRegion == Game::Region::KOR)
				{
					auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
					auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);
					il2cppstring viewName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "SceneDefine/ViewId"), viewId)->chars;

					if (viewName == IL2CPP_STRING("Friend"))
					{
						updateFriendSearchButton = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
							{
								if (!UpdateFriendSearchButton())
								{
									il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateFriendSearchButton, true);
								}
							}))->delegate;

						// Delay 50ms
						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateFriendSearchButton, true);
					}
				}

				if (sceneName == IL2CPP_STRING("Live"))
				{
					SystemMediaTransportControlsManager::instance.IsEnabled(true);

					auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
					auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

					SystemMediaTransportControlsManager::UpdateMetadata(musicId);
					SystemMediaTransportControlsManager::instance.IsPreviousEnabled(false);
					SystemMediaTransportControlsManager::instance.IsNextEnabled(true);
				}
				else if (sceneName == IL2CPP_STRING("Home"))
				{
					SystemMediaTransportControlsManager::instance.IsEnabled(true);
					SystemMediaTransportControlsManager::instance.IsPreviousEnabled(false);
					SystemMediaTransportControlsManager::instance.IsNextEnabled(false);

					SystemMediaTransportControlsManager::UpdateMetadata();
				}
				else
				{
					SystemMediaTransportControlsManager::instance.IsEnabled(false);
					SystemMediaTransportControlsManager::instance.IsPreviousEnabled(false);
					SystemMediaTransportControlsManager::instance.IsNextEnabled(false);
				}

				if (sceneName == IL2CPP_STRING("Title"))
				{
					if (Game::CurrentGameRegion == Game::Region::KOR)
					{
						auto uncheater = GetSingletonInstanceByMethod(il2cpp_symbols::get_class("umamusume.dll", "", "UncheaterInit"));

						if (uncheater)
						{
							auto flagField = il2cpp_class_get_field_from_name(uncheater->klass, "flag");
							bool flag = true;
							il2cpp_field_set_value(uncheater, flagField, &flag);

							auto systemField = il2cpp_class_get_field_from_name(uncheater->klass, "uncheater");
							Il2CppObject* system;
							il2cpp_field_get_value(uncheater, systemField, &system);

							if (system)
							{
								auto pUncSendCommandVa_Field = il2cpp_class_get_field_from_name(system->klass, "pUncSendCommandVa_");
								auto fn = CreateDelegateStatic(*[](void*, uint32_t cmd, va_list va)
									{
										cout << cmd << endl;
										return 0;
									});
								il2cpp_field_set_value(system, pUncSendCommandVa_Field, fn);
							}
						}
					}

					if (config::character_system_text_caption)
					{
						Localify::NotificationManager::Reset();
					}

					// if (config::max_fps > -1 || config::unlock_size || config::freeform_window)
					// {
						// if (uiManager)
						// {
							// Il2CppObject* _bgCamera = uiManager._bgCamera();

							/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
								il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());*/
								// }
							// }

					if (config::freeform_window)
					{
						int width = UnityEngine::Screen::width();
						int height = UnityEngine::Screen::height();

						bool isVirt = width < height;
						Gallop::StandaloneWindowResize::IsVirt(isVirt);
					}

					if (uiManager && (config::unlock_size || config::freeform_window))
					{
						int width = UnityEngine::Screen::width();
						int height = UnityEngine::Screen::height();

						bool isVirt = width < height;

						if (config::freeform_window)
						{
							uiManager.ChangeResizeUIForPC(isVirt ? min(last_virt_window_width, last_virt_window_height) : max(last_hriz_window_width, last_hriz_window_height),
								isVirt ? max(last_virt_window_width, last_virt_window_height) : min(last_hriz_window_width, last_hriz_window_height));
						}
						else
						{
							uiManager.ChangeResizeUIForPC(isVirt ? min(last_display_width, last_display_height) : max(last_display_width, last_display_height),
								isVirt ? max(last_display_width, last_display_height) : min(last_display_width, last_display_height));
						}
					}
				}

				if (sceneName == IL2CPP_STRING("Home"))
				{
					if (config::character_system_text_caption)
					{
						Localify::NotificationManager::Init();
					}

					if (config::unlock_live_chara)
					{
						auto charaList = MsgPackModify::GetCharaList();

						auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

						auto workCharaData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_CharaData", 0)->methodPointer(workDataManager);

						auto UserCharaClass = il2cpp_symbols::get_class("umamusume.Http.dll", "Gallop", "UserChara");

						if (!UserCharaClass)
						{
							UserCharaClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UserChara");
						}

						for (auto& chara : charaList)
						{
							auto userChara = il2cpp_object_new(UserCharaClass);

							auto chara_id_field = il2cpp_class_get_field_from_name(userChara->klass, "chara_id");
							int chara_id = chara["chara_id"].int32_value();
							il2cpp_field_set_value(userChara, chara_id_field, &chara_id);

							auto training_num_field = il2cpp_class_get_field_from_name(userChara->klass, "training_num");
							int training_num = chara["training_num"].int32_value();
							il2cpp_field_set_value(userChara, training_num_field, &training_num);

							auto love_point_field = il2cpp_class_get_field_from_name(userChara->klass, "love_point");
							int love_point = chara["love_point"].int32_value();
							il2cpp_field_set_value(userChara, love_point_field, &love_point);

							auto love_point_pool_field = il2cpp_class_get_field_from_name(userChara->klass, "love_point_pool");
							if (love_point_pool_field)
							{
								int love_point_pool = chara["love_point_pool"].int32_value();
								il2cpp_field_set_value(userChara, love_point_pool_field, &love_point_pool);
							}

							auto fan_field = il2cpp_class_get_field_from_name(userChara->klass, "fan");
							uint64_t fan = chara["fan"].uint64_value();
							il2cpp_field_set_value(userChara, fan_field, &fan);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(workCharaData->klass, "UpdateCharaData", 1)->methodPointer(workCharaData, userChara);
						}
					}
				}

				if (sceneName == IL2CPP_STRING("Live") && config::champions_live_show_text)
				{
					auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
					auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

					if (musicId == 1054)
					{
						auto raceInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_raceInfo", 0)->methodPointer(loadSettings);

						auto resourceId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_ChampionsMeetingResourceId", 0)->methodPointer(raceInfo);

						if (resourceId == 0)
						{
							auto charaNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_defaults.string_class, 9);
							auto trainerNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_defaults.string_class, 9);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArray", 1)->methodPointer(raceInfo, charaNameArray);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArray", 1)->methodPointer(raceInfo, trainerNameArray);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);

							auto charaInfoList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_CharacterInfoList", 0)->methodPointer(loadSettings);

							FieldInfo* itemsField = il2cpp_class_get_field_from_name(charaInfoList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* charaInfoArr;
							il2cpp_field_get_value(charaInfoList, itemsField, &charaInfoArr);

							for (int i = 0; i < 9; i++)
							{
								auto info = charaInfoArr->vector[i];
								auto charaId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_CharaId", 0)->methodPointer(info);
								auto mobId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(info->klass, "get_MobId", 0)->methodPointer(info);

								Il2CppString* charaName;
								if (charaId == 1)
								{
									charaName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, int)>("umamusume.dll", "Gallop", "TextUtil", "GetMasterText", 2)(59, mobId);
								}
								else
								{
									charaName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, int)>("umamusume.dll", "Gallop", "TextUtil", "GetMasterText", 2)(6, charaId);
								}

								il2cpp_array_setref(charaNameArray, i, charaName);
								il2cpp_array_setref(trainerNameArray, i, il2cpp_string_new(""));
							}

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(raceInfo->klass, "set_ChampionsMeetingResourceId", 1)->methodPointer(raceInfo, config::champions_live_resource_id);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(raceInfo->klass, "set_DateYear", 1)->methodPointer(raceInfo, config::champions_live_year);
						}
					}
				}

				if (config::discord_rich_presence && discord)
				{
					auto detail = GetSceneName(il2cpp_u8(sceneName));

					discord::Activity activity{};
					activity.GetAssets().SetLargeImage("umamusume");
					activity.SetDetails(detail.data());
					activity.GetTimestamps().SetStart(startTime);
					if (sceneName == IL2CPP_STRING("Live"))
					{
						activity.SetType(discord::ActivityType::Watching);
					}
					else
					{
						activity.SetType(discord::ActivityType::Playing);
					}
					discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
				}
			})
		);
		il2cpp_field_static_set_value(activeSceneChangedField, action);

		TaskbarManager::Initialze(GetHWND());

		try
		{
			wstring newTitle = title;

			if (Game::CurrentGameRegion == Game::Region::JPN &&
				Game::CurrentGameStore == Game::Store::Steam)
			{
				newTitle.append(L" (Steam)");
			}

			SystemMediaTransportControlsManager::CreateShortcutForSMTC(newTitle.data());
			SystemMediaTransportControlsManager::Initialze(GetHWND());

			auto& smtc = SystemMediaTransportControlsManager::instance;
			if (smtc)
			{
				smtc.ButtonPressed([](winrt::Windows::Media::SystemMediaTransportControls smtc, winrt::Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs args)
					{
						auto button = args.Button();
						switch (button)
						{
						case winrt::Windows::Media::SystemMediaTransportControlsButton::Play:
							WaitForEndOfFrame(*[]()
								{
									auto controller = GetCurrentViewController();
									auto hubViewController = GetCurrentHubViewChildController();

									if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
									{
										auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);
										if (topUi)
										{
											auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(topUi->klass, "get_TempSetListPlayingData", 0)->methodPointer(topUi);

											auto SetListIdField = il2cpp_class_get_field_from_name(data->klass, "SetListId");
											int SetListId;
											il2cpp_field_get_value(data, SetListIdField, &SetListId);

											if (SetListId > 0)
											{
												auto _jukeboxBgmSelectorField = il2cpp_class_get_field_from_name(topUi->klass, "_jukeboxBgmSelector");
												Il2CppObject* _jukeboxBgmSelector;
												il2cpp_field_get_value(topUi, _jukeboxBgmSelectorField, &_jukeboxBgmSelector);

												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool, float, bool)>(_jukeboxBgmSelector->klass, "PlayCoroutinePlaySetList", 3)->methodPointer(_jukeboxBgmSelector, true, 0.0f, true);
											}
											else
											{
												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(topUi->klass, "PlayRequestSong", 0)->methodPointer(topUi);
											}
										}
									}

									if (controller && controller->klass->name == "LiveViewController"s)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "ResumeLive", 0)->methodPointer(controller);
									}
								});
							break;

						case winrt::Windows::Media::SystemMediaTransportControlsButton::Pause:
							WaitForEndOfFrame(*[]()
								{
									auto controller = GetCurrentViewController();
									auto hubViewController = GetCurrentHubViewChildController();

									if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
									{
										auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(topUi->klass, "SetPlayMusicFlag", 1)->methodPointer(topUi, false);
									}

									if (controller && controller->klass->name == "LiveViewController"s)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "PauseLive", 0)->methodPointer(controller);
									}
								});
							break;

						case winrt::Windows::Media::SystemMediaTransportControlsButton::Previous:
							WaitForEndOfFrame(*[]()
								{
									auto controller = GetCurrentViewController();
									auto hubViewController = GetCurrentHubViewChildController();

									if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
									{
										auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(topUi->klass, "OnClickSetListArrow", 1)->methodPointer(topUi, false);
									}
								});
							break;

						case winrt::Windows::Media::SystemMediaTransportControlsButton::Next:
							WaitForEndOfFrame(*[]()
								{
									auto controller = GetCurrentViewController();
									auto hubViewController = GetCurrentHubViewChildController();

									if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
									{
										auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);

										auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(topUi->klass, "get_TempSetListPlayingData", 0)->methodPointer(topUi);

										auto IsPlayingField = il2cpp_class_get_field_from_name(data->klass, "IsPlaying");
										bool IsPlaying;
										il2cpp_field_get_value(data, IsPlayingField, &IsPlaying);

										if (IsPlaying)
										{
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(topUi->klass, "OnClickSetListArrow", 1)->methodPointer(topUi, true);
										}
									}

									if (controller && controller->klass->name == "LiveViewController"s)
									{
										auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
										auto coroutine = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "SkipLive", 0)->methodPointer(controller);

										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(view->klass, "StartCoroutineManaged2", 1)->methodPointer(view, coroutine);
									}
								});
							break;
						}

						return S_OK;
					});
			}
		}
		catch (...)
		{
		}
	}
}

constexpr wchar_t* APPSIGN_PREFIX = L"AppSign";
constexpr wchar_t* CUSTOM_WARNING_CAPTION = L"Warning";
constexpr wchar_t* CUSTOM_WARNING_TEXT = L"Wellbia AppSign이 의도치 않은 동작을 감지하여 프로그램이 종료됩니다.\n\n문제가 지속되는 경우 https://wellbia.com/ 에 방문하거나, support@wellbia.com 으로 문의하시기 바랍니다.";

void* MessageBoxW_orig = nullptr;

int
WINAPI
MessageBoxW_hook(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	if (wstring(lpCaption).starts_with(APPSIGN_PREFIX))
	{
		lpCaption = CUSTOM_WARNING_CAPTION;
		lpText = CUSTOM_WARNING_TEXT;
		uType = MB_ICONWARNING;
	}
	return reinterpret_cast<decltype(MessageBoxW)*>(MessageBoxW_orig)(hWnd, lpText, lpCaption, uType);
}

void* SetWindowLongPtrW_orig = nullptr;

LONG_PTR
WINAPI
SetWindowLongPtrW_hook(
	_In_ HWND hWnd,
	_In_ int nIndex,
	_In_ LONG_PTR dwNewLong)
{
	if (config::freeform_window && hWnd == currentHWnd && nIndex == GWL_STYLE)
	{
		dwNewLong |= WS_MAXIMIZEBOX;
	}

	if (hWnd == currentHWnd && nIndex == GWLP_WNDPROC)
	{
		if ((config::unlock_size || config::freeform_window) && config::initial_width > 72 && config::initial_height > 72)
		{
			auto refreshRate = RefreshRate{ 0, 1 };
			if (config::initial_width < config::initial_height)
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_virt_window_width, last_virt_window_height, 3, &refreshRate);
			}
			else
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_hriz_window_width, last_hriz_window_height, 3, &refreshRate);
			}
		}

		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			bool isVirt = width < height;
			Gallop::StandaloneWindowResize::IsVirt(isVirt);
		}

		return reinterpret_cast<LONG_PTR>(oldWndProcPtr);
	}

	return reinterpret_cast<decltype(SetWindowLongPtrW)*>(SetWindowLongPtrW_orig)(hWnd, nIndex, dwNewLong);
}

void* SetWindowLongPtrA_orig = nullptr;

LONG_PTR
WINAPI
SetWindowLongPtrA_hook(
	_In_ HWND hWnd,
	_In_ int nIndex,
	_In_ LONG_PTR dwNewLong)
{
	if (config::freeform_window && hWnd == currentHWnd && nIndex == GWL_STYLE)
	{
		dwNewLong |= WS_MAXIMIZEBOX;
	}

	if (hWnd == currentHWnd && nIndex == GWLP_WNDPROC)
	{
		if ((config::unlock_size || config::freeform_window) && config::initial_width > 72 && config::initial_height > 72)
		{
			auto refreshRate = RefreshRate{ 0, 1 };
			if (config::initial_width < config::initial_height)
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_virt_window_width, last_virt_window_height, 3, &refreshRate);
			}
			else
			{
				reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(last_hriz_window_width, last_hriz_window_height, 3, &refreshRate);
			}
		}

		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			bool isVirt = width < height;
			Gallop::StandaloneWindowResize::IsVirt(isVirt);
		}

		return reinterpret_cast<LONG_PTR>(oldWndProcPtr);
	}

	return reinterpret_cast<decltype(SetWindowLongPtrA)*>(SetWindowLongPtrA_orig)(hWnd, nIndex, dwNewLong);
}

void* ShowWindow_orig = nullptr;
BOOL
WINAPI
ShowWindow_hook(
	_In_ HWND hWnd,
	_In_ int nCmdShow)
{
	if (!currentHWnd)
	{
		if (!config::custom_title_name.empty())
		{
			SetWindowTextW(hWnd, config::custom_title_name.data());
		}

		if (config::has_json_parse_error)
		{
			MessageBoxW(hWnd, config::json_parse_error_msg.data(), L"Umamusume Localify", MB_OK | MB_ICONWARNING);
		}

		oldWndProcPtr = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

		currentHWnd = hWnd;
	}

	MH_DisableHook(ShowWindow);
	MH_RemoveHook(ShowWindow);
	return ShowWindow(hWnd, nCmdShow);
}

void* HttpSendRequestW_orig = nullptr;
BOOL WINAPI HttpSendRequestW_hook(
	_In_ HINTERNET hRequest,
	_In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
	_In_ DWORD dwHeadersLength,
	_In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional,
	_In_ DWORD dwOptionalLength
)
{
	if (dwOptionalLength)
	{
		auto data = reinterpret_cast<char*>(lpOptional);
		rapidjson::Document doc;
		doc.Parse(data, dwOptionalLength);

		if (!doc.HasParseError())
		{
			if (doc.IsObject() && doc.HasMember("os"))
			{
				doc.GetObj()["os"].SetString(rapidjson::StringRef("android"));

				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				doc.Accept(writer);

				auto text = buffer.GetString();
				char* copy = new char[strlen(text) + 1];
				strcpy(copy, text);

				lpOptional = copy;
				dwOptionalLength = strlen(copy);
			}
		}
	}

	return reinterpret_cast<decltype(HttpSendRequestW_hook)*>(HttpSendRequestW_orig)(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

void* InternetCrackUrlW_orig = nullptr;
BOOL InternetCrackUrlW_hook(
	_In_reads_(dwUrlLength) LPCWSTR lpszUrl,
	_In_ DWORD dwUrlLength,
	_In_ DWORD dwFlags,
	_Inout_ LPURL_COMPONENTSW lpUrlComponents
)
{
	auto newUrlW = wstring(lpszUrl);
	replaceAll(newUrlW, L"windows", L"android");

	wchar_t* copy = new wchar_t[newUrlW.size() + 1];
	wcscpy(copy, newUrlW.data());

	lpszUrl = copy;

	dwUrlLength = newUrlW.size();

	return reinterpret_cast<decltype(InternetCrackUrlW_hook)*>(InternetCrackUrlW_orig)(lpszUrl, dwUrlLength, dwFlags, lpUrlComponents);
}

constexpr int MAX_DLL_COUNT = 21;
constexpr int MAX_ROOT_FILE_COUNT = 9 + /* self (.) */1 + /* parent (..) */1;

HANDLE currentFindHandle;
vector<HANDLE> findHandles;
HANDLE currentRootFindHandle;

int dllCount;
int rootFileCount;

void* CreateFileW_orig = nullptr;
HANDLE
WINAPI
CreateFileW_hook(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	auto hFile = reinterpret_cast<decltype(CreateFileW)*>(CreateFileW_orig)(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	if (filesystem::current_path() == lpFileName)
	{
		currentFindHandle = hFile;
		dllCount = 0;
	}

	return hFile;
}

void* NtCreateFile_orig = nullptr;
NTSTATUS
NTAPI
NtCreateFile_hook(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER AllocationSize,
	_In_ ULONG FileAttributes,
	_In_ ULONG ShareAccess,
	_In_ ULONG CreateDisposition,
	_In_ ULONG CreateOptions,
	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
	_In_ ULONG EaLength
)
{
	if (ObjectAttributes)
	{
		if (ObjectAttributes->ObjectName)
		{
			wstring fileName(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(WCHAR));
			if (fileName == L"dat")
			{
				return STATUS_OBJECT_NAME_NOT_FOUND;
			}
		}

		if (ObjectAttributes->RootDirectory == currentFindHandle)
		{
			findHandles.emplace_back(ObjectAttributes->RootDirectory);
		}
	}

	auto status = reinterpret_cast<decltype(NtCreateFile)*>(NtCreateFile_orig)(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock,
		AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);

	return status;
}

void* NtQueryDirectoryFile_orig = nullptr;
NTSTATUS NTAPI NtQueryDirectoryFile_hook(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_opt_ PUNICODE_STRING FileName,
	_In_ BOOLEAN RestartScan
)
{
	if (FileHandle != currentFindHandle && find(findHandles.begin(), findHandles.end(), FileHandle) != findHandles.end())
	{
		return reinterpret_cast<decltype(NtQueryDirectoryFile)*>(NtQueryDirectoryFile_orig)(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
			FileInformation, Length, FileInformationClass,
			ReturnSingleEntry, FileName, RestartScan);
	}

	NTSTATUS status = reinterpret_cast<decltype(NtQueryDirectoryFile)*>(NtQueryDirectoryFile_orig)(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
		FileInformation, Length, FileInformationClass,
		ReturnSingleEntry, FileName, RestartScan);

	if (FileInformationClass != FileFullDirectoryInformation)
	{
		return status;
	}

	if (NT_SUCCESS(status))
	{
		auto curr = reinterpret_cast<PFILE_FULL_DIR_INFORMATION>(FileInformation);
		PFILE_FULL_DIR_INFORMATION prev = nullptr;

		while (true)
		{
			std::wstring fName(curr->FileName, curr->FileNameLength / sizeof(WCHAR));

			bool removeEntry = false;

			if (fName.ends_with(L".dll"))
			{
				dllCount++;
				if (dllCount > MAX_DLL_COUNT)
				{
					removeEntry = true;
				}
			}

			if (removeEntry)
			{
				if (prev)
				{
					if (curr->NextEntryOffset != 0)
					{
						prev->NextEntryOffset += curr->NextEntryOffset;
					}
					else
					{
						prev->NextEntryOffset = 0;
					}

					if (prev->NextEntryOffset == 0)
					{
						break;
					}
					curr = reinterpret_cast<PFILE_FULL_DIR_INFORMATION>(reinterpret_cast<PUCHAR>(prev) + prev->NextEntryOffset);
					continue;
				}
				else
				{
					if (curr->NextEntryOffset != 0)
					{
						ULONG nextOffset = curr->NextEntryOffset;
						ULONG totalValidBytes = static_cast<ULONG>(IoStatusBlock->Information);
						ULONG bytesToMove = totalValidBytes - nextOffset;

						memmove(curr, reinterpret_cast<PUCHAR>(curr) + nextOffset, bytesToMove);

						IoStatusBlock->Information -= nextOffset;
						continue;
					}
					else
					{
						return STATUS_NO_MORE_FILES;
					}
				}
			}

			if (curr->NextEntryOffset == 0)
			{
				break;
			}
			prev = curr;
			curr = reinterpret_cast<PFILE_FULL_DIR_INFORMATION>(reinterpret_cast<PUCHAR>(curr) + curr->NextEntryOffset);
		}
	}

	return status;
}

void* FindNextFileW_orig = nullptr;
BOOL
WINAPI
FindNextFileW_hook(
	_In_ HANDLE hFindFile,
	_Out_ LPWIN32_FIND_DATAW lpFindFileData
)
{
	if (currentFindHandle == hFindFile && dllCount >= MAX_DLL_COUNT)
	{
		lpFindFileData = nullptr;
		SetLastError(ERROR_NO_MORE_FILES);
		return FALSE;
	}

	if (currentRootFindHandle == hFindFile && rootFileCount >= MAX_ROOT_FILE_COUNT)
	{
		lpFindFileData = nullptr;
		SetLastError(ERROR_NO_MORE_FILES);
		return FALSE;
	}

	auto result = reinterpret_cast<decltype(FindNextFileW_hook)*>(FindNextFileW_orig)(hFindFile, lpFindFileData);

	if (currentRootFindHandle == hFindFile)
	{
		if (lpFindFileData && lpFindFileData->cFileName)
		{
			rootFileCount++;

			if (!result && rootFileCount <= MAX_ROOT_FILE_COUNT && GetLastError() == ERROR_NO_MORE_FILES)
			{
				SetLastError(ERROR_SUCCESS);
				return TRUE;
			}
		}
		else if (rootFileCount < MAX_ROOT_FILE_COUNT && GetLastError() == ERROR_NO_MORE_FILES)
		{
			if (lpFindFileData)
			{
				*lpFindFileData = WIN32_FIND_DATAW{};
			}
			SetLastError(ERROR_SUCCESS);
			return TRUE;
		}
	}

	if (currentFindHandle == hFindFile)
	{
		if (lpFindFileData && lpFindFileData->cFileName)
		{
			if (wstring(lpFindFileData->cFileName).ends_with(L".dll"))
			{
				dllCount++;

				if (!result && dllCount <= MAX_DLL_COUNT && GetLastError() == ERROR_NO_MORE_FILES)
				{
					SetLastError(ERROR_SUCCESS);
					return TRUE;
				}
			}
			else if (dllCount < MAX_DLL_COUNT && GetLastError() == ERROR_NO_MORE_FILES)
			{
				dllCount++;

				// fake data
				*lpFindFileData = WIN32_FIND_DATAW{ .cFileName = L"GameAssembly.dll" };

				SetLastError(ERROR_SUCCESS);
				return TRUE;
			}
		}
	}

	return result;
}

void* FindFirstFileExW_orig = nullptr;
HANDLE
WINAPI
FindFirstFileExW_hook(
	_In_ LPCWSTR lpFileName,
	_In_ FINDEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FIND_DATAW)) LPVOID lpFindFileData,
	_In_ FINDEX_SEARCH_OPS fSearchOp,
	_Reserved_ LPVOID lpSearchFilter,
	_In_ DWORD dwAdditionalFlags
)
{
	if (wstring(lpFileName).find(L"\\dat\\") != wstring::npos)
	{
		// Skipping dat path due performance issue
		SetLastError(ERROR_FILE_NOT_FOUND);
		return INVALID_HANDLE_VALUE;
	}

	auto result = reinterpret_cast<decltype(FindFirstFileExW_hook)*>(FindFirstFileExW_orig)(lpFileName, fInfoLevelId, lpFindFileData,
		fSearchOp, lpSearchFilter, dwAdditionalFlags);

	if (filesystem::current_path().wstring() + L"\\*.dll" == lpFileName)
	{
		// reset count
		dllCount = 0;
	}

	if (filesystem::current_path().wstring() + L"\\*.*" == lpFileName)
	{
		currentRootFindHandle = result;
		rootFileCount = 1;
	}

	if (wstring(lpFileName).find(L"*.dll") != wstring::npos)
	{
		if (dllCount >= MAX_DLL_COUNT)
		{
			CloseHandle(result);
			lpFindFileData = nullptr;
			SetLastError(ERROR_FILE_NOT_FOUND);
			return INVALID_HANDLE_VALUE;
		}

		currentFindHandle = result;

		if (lpFindFileData && result != INVALID_HANDLE_VALUE)
		{
			if (wstring(reinterpret_cast<LPWIN32_FIND_DATAW>(lpFindFileData)->cFileName).ends_with(L".dll"))
			{
				dllCount++;
			}

			if (dllCount > MAX_DLL_COUNT)
			{
				CloseHandle(result);
				lpFindFileData = nullptr;
				SetLastError(ERROR_FILE_NOT_FOUND);
				return INVALID_HANDLE_VALUE;
			}
		}
	}

	return result;
}

void init_hook(filesystem::path module_path)
{
	if (MH_Initialize() != MH_OK)
	{
		return;
	}

	if (mh_inited)
	{
		return;
	}

	mh_inited = true;

	MH_CreateHook(MessageBoxW, MessageBoxW_hook, &MessageBoxW_orig);
	MH_EnableHook(MessageBoxW);

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
#ifdef EXPERIMENTS
		MH_CreateHook(HttpSendRequestW, HttpSendRequestW_hook, &HttpSendRequestW_orig);
		MH_EnableHook(HttpSendRequestW);

		MH_CreateHook(InternetCrackUrlW, InternetCrackUrlW_hook, &InternetCrackUrlW_orig);
		MH_EnableHook(InternetCrackUrlW);
#endif
	}

	MH_CreateHook(FindFirstFileExW, FindFirstFileExW_hook, &FindFirstFileExW_orig);
	MH_EnableHook(FindFirstFileExW);

	MH_CreateHook(FindNextFileW, FindNextFileW_hook, &FindNextFileW_orig);
	MH_EnableHook(FindNextFileW);

	MH_CreateHook(CreateFileW, CreateFileW_hook, &CreateFileW_orig);
	MH_EnableHook(CreateFileW);

	MH_CreateHook(NtCreateFile, NtCreateFile_hook, &NtCreateFile_orig);
	MH_EnableHook(NtCreateFile);

	MH_CreateHook(NtQueryDirectoryFile, NtQueryDirectoryFile_hook, &NtQueryDirectoryFile_orig);
	MH_EnableHook(NtQueryDirectoryFile);

	auto LoadLibraryExW_addr = GetProcAddress(GetModuleHandleW(L"KernelBase.dll"), "LoadLibraryExW");
	MH_CreateHook(LoadLibraryExW_addr, load_library_ex_w_hook, &load_library_ex_w_orig);
	MH_EnableHook(LoadLibraryExW_addr);

	auto LoadLibraryW_addr = GetProcAddress(GetModuleHandleW(L"KernelBase.dll"), "LoadLibraryW");
	MH_CreateHook(LoadLibraryW_addr, load_library_w_hook, &load_library_w_orig);
	MH_EnableHook(LoadLibraryW_addr);

	auto GetProcAddress_addr = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetProcAddress");
	MH_CreateHook(GetProcAddress_addr, GetProcAddress_hook, &GetProcAddress_orig);
	MH_EnableHook(GetProcAddress_addr);

	fullScreenFl = config::auto_fullscreen && !config::freeform_window;

	MH_CreateHook(SetWindowLongPtrW, SetWindowLongPtrW_hook, &SetWindowLongPtrW_orig);
	MH_EnableHook(SetWindowLongPtrW);
	MH_CreateHook(SetWindowLongPtrA, SetWindowLongPtrA_hook, &SetWindowLongPtrA_orig);
	MH_EnableHook(SetWindowLongPtrA);

	MH_CreateHook(ShowWindow, ShowWindow_hook, &ShowWindow_orig);
	MH_EnableHook(ShowWindow);
}

void uninit_hook()
{
	if (!mh_inited)
		return;

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
