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
#include "scripts/UnityEngine.CoreModule/UnityEngine/Color32.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Display.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Shader.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Material.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector4.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2Int.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"
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
#include "scripts/umamusume/Gallop/TextCommon.hpp"
#include "scripts/umamusume/Gallop/TextFontManager.hpp"
#include "scripts/umamusume/Gallop/Screen.hpp"
#include "scripts/umamusume/Gallop/StoryViewController.hpp"
#include "scripts/umamusume/Gallop/FrameRateController.hpp"
#include "scripts/umamusume/Gallop/GameSystem.hpp"
#include "scripts/umamusume/Gallop/GallopUtil.hpp"
#include "scripts/umamusume/Gallop/GraphicSettings.hpp"
#ifdef _MSC_VER
#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"
#include "scripts/umamusume/Gallop/WindowsGamepadControl.hpp"
#include "scripts/umamusume/Gallop/LandscapeUIManager.hpp"
#endif

#include "scripts/Plugins/AnimateToUnity/AnRootManager.hpp"

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

	void init_il2cpp()
	{
		if (config::dump_il2cpp)
		{
			il2cpp_dump();
		}

		il2cpp_symbols::init_defaults();
		il2cpp_symbols::call_init_callbacks();

		il2cpp_symbols::late_init_callbacks.emplace_back(patch_game_assembly);

		il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.SubsystemsModule.dll", "UnityEngine", "SubsystemManager"));
		il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.SubsystemsModule.dll", "UnityEngine.SubsystemsImplementation", "SubsystemDescriptorStore"));
		//il2cpp_runtime_class_init(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "BeforeRenderHelper"));

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			KillProcessByName(L"ucldr_Umamusume_KR_loader_x64.exe");
		}
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

			float length =
				il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, Il2CppString*, int)>(audioManager->klass, "GetCueLength", 2)->methodPointer(audioManager, cueSheet, cueId);

			Localify::NotificationManager::SetDisplayTime(length);

			Localify::NotificationManager::Show(Gallop::GallopUtil::LineHeadWrap(il2cpp_string_new16(u16Text.data()), config::character_system_text_caption_line_char_count));
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

				auto display = UnityEngine::Display::main();
				auto systemHeight = display.systemHeight();

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

	bool useDefaultFPS = false;

	constexpr float ratio_16_9 = 1.7777f;
	constexpr float ratio_9_16 = 0.5625f;
	constexpr float ratio_4_3 = 1.3333f;
	constexpr float ratio_3_4 = 0.75f;

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
				auto _currentBgWidthField = il2cpp_class_get_field_from_name(bgManager->klass, "_currentBgWidth");
				int _currentBgWidth;
				il2cpp_field_get_value(bgManager, _currentBgWidthField, &_currentBgWidth);

				auto _currentBgHeightField = il2cpp_class_get_field_from_name(bgManager->klass, "_currentBgHeight");
				int _currentBgHeight;
				il2cpp_field_get_value(bgManager, _currentBgHeightField, &_currentBgHeight);

				if (!_currentBgWidth || !_currentBgHeight)
				{
					return;
				}

				float ratio = static_cast<float>(_currentBgWidth) / static_cast<float>(_currentBgHeight);

				int width = Gallop::Screen::Width();
				int height = Gallop::Screen::Height();

				if (_currentBgWidth < _currentBgHeight)
				{
					_currentBgHeight = height;
					_currentBgWidth = static_cast<int>(_currentBgHeight * ratio);
				}
				else
				{
					_currentBgWidth = width;
					_currentBgHeight = static_cast<int>(_currentBgWidth / ratio);
				}

				il2cpp_field_set_value(bgManager, _currentBgWidthField, &_currentBgWidth);
				il2cpp_field_set_value(bgManager, _currentBgHeightField, &_currentBgHeight);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(bgManager->klass, "RecalcBgSize", 0)->methodPointer(bgManager);
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

	bool altEnterPressed = false;

	Il2CppObject* delayTweener;

	void RemakeTextures()
	{
		auto uiManager = Gallop::UIManager::Instance();

		auto graphicSettings = Gallop::GraphicSettings::Instance();
		if (!graphicSettings)
		{
			return;
		}

		graphicSettings.Update3DRenderTexture();

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

			auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), uiManager, *([](Il2CppObject* self)
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
			if (controller->klass->name == "SingleModeMainViewController"s)
			{
				auto ScenarioController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_ScenarioController", 0)->methodPointer(controller);

				if (ScenarioController && ScenarioController->klass->name == "SingleModeMainViewScenarioBreedersController"s)
				{
					auto IsStoryActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(controller->klass, "get_IsStoryActive", 0)->methodPointer(controller);

					if (!IsStoryActive)
					{
						auto trainingController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_TrainingController", 0)->methodPointer(controller);
						if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(trainingController->klass, "get_IsInTraining", 0)->methodPointer(trainingController))
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(ScenarioController->klass, "PlayCutIn", 0)->methodPointer(ScenarioController);
						}
					}
				}
			}

			if (controller->klass->name == "PhotoStudioViewController"s)
			{
				auto _photoStudioTopCharaViewerField = il2cpp_class_get_field_from_name(controller->klass, "_photoStudioTopCharaViewer");
				Il2CppObject* _photoStudioTopCharaViewer;
				il2cpp_field_get_value(controller, _photoStudioTopCharaViewerField, &_photoStudioTopCharaViewer);

				if (_photoStudioTopCharaViewer)
				{
					auto _lowResolutionCameraField = il2cpp_class_get_field_from_name(_photoStudioTopCharaViewer->klass, "_lowResolutionCamera");
					Il2CppObject* _lowResolutionCamera;
					il2cpp_field_get_value(_photoStudioTopCharaViewer, _lowResolutionCameraField, &_lowResolutionCamera);

					if (_lowResolutionCamera)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_photoStudioTopCharaViewer->klass, "OnCreateTexture", 1)->methodPointer(_photoStudioTopCharaViewer, _lowResolutionCamera);
					}
				}
			}

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

				Gallop::StoryViewController storyViewController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "StoryManager", "get_ViewController", 0)(storyManager);

				auto IsSingleModeOrGallery = storyViewController.IsSingleModeOrGallery();

				auto scene = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(storySceneController->klass, "GetSceneBase", 0)->methodPointer(storySceneController);

				if (!IsSingleModeOrGallery)
				{
					storyViewController.SetDisplayMode(DisplayMode);
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

				storyViewController.SetupUIOnChangeOrientation();
			}
		}
	}

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* self, UnityEngine::Vector2 dispRectWH, bool isPanScan);

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
			auto gameSystem = Gallop::GameSystem::Instance();
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(gameSystem, CreateDelegateStatic(fn));
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "WaitForEndOfFrame error: " << e.ex->message->chars << endl;
			PrintStackTrace();
		}
	}

	void WaitForEndOfFrame(Il2CppObject* target, void (*fn)(Il2CppObject* self))
	{
		try
		{
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

						auto lastWidth = updateWidth;
						auto lastHeight = updateHeight;

						Gallop::StandaloneWindowResize::windowLastWidth(lastWidth);
						Gallop::StandaloneWindowResize::windowLastHeight(lastHeight);

						Gallop::StandaloneWindowResize::SaveChangedWidth(lastWidth, lastHeight);

						float _aspectRatio = contentWidth / contentHeight;
						Gallop::StandaloneWindowResize::AspectRatio(_aspectRatio);

						Gallop::StandaloneWindowResize::IsPreventReShape(true);
						Gallop::StandaloneWindowResize::DisableWindowHitTest();

						bool isPortrait = contentWidth < contentHeight;

						Gallop::StandaloneWindowResize::IsVirt(isPortrait);

						int unityWidth = UnityEngine::Screen::width();
						int unityHeight = UnityEngine::Screen::height();

						bool isUnityPortrait = unityWidth < unityHeight;

						Gallop::Screen::OriginalScreenWidth(isUnityPortrait ? contentHeight : contentWidth);
						Gallop::Screen::OriginalScreenHeight(isUnityPortrait ? contentWidth : contentHeight);

						auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));

						auto uiManager = Gallop::UIManager::Instance();

						if (uiManager)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Disable", 0)->methodPointer(tapEffectController);

							uiManager.SetCameraSizeByOrientation(UnityEngine::ScreenOrientation::Portrait);
						}

						auto anRootManager = AnimateToUnity::AnRootManager::Instance();

						if (anRootManager)
						{
							anRootManager.ScreenRate(_aspectRatio);
						}

						if (uiManager)
						{
							auto gameObject = uiManager.gameObject();

							auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gameObject, "get_transform", 0)->methodPointer(gameObject);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(transform->klass, "set_localScale", 1)->methodPointer(transform, UnityEngine::Vector3{ 1, 1, 1 });

							if (tapEffectController)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);
							}

							Il2CppArraySize_t<Il2CppObject*>* canvasScalerList;
							canvasScalerList = UnityEngine::Object::FindObjectsByType(
								GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);

							for (int i = 0; i < canvasScalerList->max_length; i++)
							{
								auto canvasScaler = canvasScalerList->vector[i];
								if (canvasScaler)
								{
									auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasScaler->klass, "get_gameObject", 0)->methodPointer(canvasScaler);

									bool keepActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(gameObject->klass, "get_activeSelf", 0)->methodPointer(gameObject);

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

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
								auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));

								bool isPortrait = _contentWidth < _contentHeight;

								auto uiManager = Gallop::UIManager::Instance();

								if (uiManager)
								{
									uiManager.AdjustSafeArea();
									Il2CppObject* _bgManager = uiManager._bgManager();
									if (_bgManager)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_bgManager->klass, "OnChangeResolutionByGraphicsSettings", 0)->methodPointer(_bgManager);
									}

									uiManager.CreateRenderTextureFromScreen();
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

								Gallop::Screen::OriginalScreenWidth(isPortrait ? _contentHeight : _contentWidth);
								Gallop::Screen::OriginalScreenHeight(isPortrait ? _contentWidth : _contentHeight);

								Gallop::StandaloneWindowResize::EnableWindowHitTest();
								Gallop::StandaloneWindowResize::IsPreventReShape(false);
							});
					});
			});


		long style = GetWindowLongW(hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
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
		auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SaveDataManager"));
		auto SaveLoader = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)->methodPointer(SaveDataManager);
		auto get_IsEnableKeyboard = il2cpp_class_get_method_from_name_type<bool(*)(Il2CppObject*)>(SaveLoader->klass, "get_IsEnableKeyboard", 0);
		if (get_IsEnableKeyboard)
		{
			auto IsEnableKeyboard = get_IsEnableKeyboard->methodPointer(SaveLoader);

			if (IsEnableKeyboard)
			{
				return false;
			}
		}

		if (il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SteamGamepadControl"))
		{
			return false;
		}

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

			/*if (PressDialogButton(wParam))
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
			}*/

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
				// return TRUE;
			}
		}

		if (uMsg == WM_SYSKEYDOWN)
		{
			bool altDown = (lParam & (static_cast<long long>(1) << 29)) != 0;
			if (config::freeform_window)
			{
				if (wParam == VK_RETURN && altDown && !altEnterPressed)
				{
					altEnterPressed = true;

					UnityEngine::Resolution r = UnityEngine::Screen::currentResolution();

					static bool fullScreenFl = false;

					fullScreenFl = !fullScreenFl;

					auto refreshRate = RefreshRate{ 0, 1 };
					UnityEngine::Screen::SetResolution_Injected(r.width, r.height, fullScreenFl ? UnityEngine::FullScreenMode::FullScreenWindow : UnityEngine::FullScreenMode::Windowed, &refreshRate);

					return TRUE;
				}
			}

			if (config::max_fps > -1 && wParam == 'F' && altDown)
			{
				config::runtime::useDefaultFPS = !config::runtime::useDefaultFPS;
				Gallop::FrameRateController::Instance().ReflectionFrameRate();
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
				Gallop::StandaloneWindowResize::DisableMaximizebox();
				return TRUE;
			}
		}


		if (config::unlock_size || config::freeform_window)
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
			if (!Gallop::StandaloneWindowResize::IsEnableWindowHitTest())
			{
				return FALSE;
			}
		}

		if (uMsg == WM_ENTERSIZEMOVE)
		{
			Gallop::StandaloneWindowResize::_isWindowSizeChanging(true);
		}

		if (uMsg == WM_MOVING)
		{
			Gallop::StandaloneWindowResize::_isWindowDragging(true);
		}

		if (uMsg == WM_SIZE)
		{
			switch (wParam)
			{
			case SIZE_RESTORED:
			{
				auto uiManager = Gallop::UIManager::Instance();
				if (uiManager)
				{
					uiManager.StartCoroutineManaged2(Gallop::StandaloneWindowResize::ClearStopFlagAfterWhile());
				}
				break;
			}
			case SIZE_MINIMIZED:
			{
				Gallop::StandaloneWindowResize::_isWindowDragging(true);
				break;
			}
			}
		}

		if (uMsg == WM_EXITSIZEMOVE)
		{
			Gallop::StandaloneWindowResize::_isWindowDragging(false);
			Gallop::StandaloneWindowResize::_isWindowSizeChanging(false);

			if (config::freeform_window)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left;
				int windowHeight = windowRect.bottom - windowRect.top;
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
				}
			);
		}

		if (uMsg == WM_SIZE && config::freeform_window)
		{
			int lastWidth = Gallop::StandaloneWindowResize::windowLastWidth();
			int lastHeight = Gallop::StandaloneWindowResize::windowLastHeight();

			if (lastWidth != LOWORD(lParam) || lastHeight != HIWORD(lParam))
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left;
				int windowHeight = windowRect.bottom - windowRect.top;
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
			int lastWidth = Gallop::StandaloneWindowResize::windowLastWidth();
			int lastHeight = Gallop::StandaloneWindowResize::windowLastHeight();

			RECT* rect = reinterpret_cast<RECT*>(lParam);

			if (lastWidth != rect->right - rect->left || lastHeight != rect->bottom - rect->top)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left;
				int windowHeight = windowRect.bottom - windowRect.top;

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

				Gallop::StandaloneWindowResize::windowLastWidth(lastWidth);
				Gallop::StandaloneWindowResize::windowLastHeight(lastHeight);

				int width = UnityEngine::Screen::width();
				int height = UnityEngine::Screen::height();

				Gallop::StandaloneWindowResize::SaveChangedWidth(width, height);

				float _aspectRatio = contentWidth / contentHeight;
				Gallop::StandaloneWindowResize::AspectRatio(_aspectRatio);

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

			/*if (Gallop::StandaloneWindowResize::IsVirt() && ratio != (9.0 / 16.0))
			{
				Gallop::StandaloneWindowResize::IsPreventReShape(true);
			}

			if (!Gallop::StandaloneWindowResize::IsVirt() && ratio != (16.0 / 9.0))
			{
				Gallop::StandaloneWindowResize::IsPreventReShape(true);
			}*/

			RECT windowRect;
			GetWindowRect(hWnd, &windowRect);
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			int clientWidth = (clientRect.right - clientRect.left);
			int clientHeight = (clientRect.bottom - clientRect.top);

			auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));
			if (tapEffectController)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Disable", 0)->methodPointer(tapEffectController);
			}

			int width = rect->right - rect->left;
			int height = rect->bottom - rect->top;

			Gallop::StandaloneWindowResize::windowLastWidth(width);
			Gallop::StandaloneWindowResize::windowLastHeight(height);

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

			if (tapEffectController)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);
			}

			rect->right += borderWidth;
			rect->bottom += borderHeight;

			width = rect->right - rect->left;
			height = rect->bottom - rect->top;

			Gallop::StandaloneWindowResize::SaveChangedWidth(width, height);

			float _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			Gallop::StandaloneWindowResize::AspectRatio(_aspectRatio);

			auto uiManager = Gallop::UIManager::Instance();
			if (uiManager)
			{
				bool isVirt = width < height;
				if (config::unlock_size || isLandscape)
				{
					int minSize;
					int maxSize;

					if (config::initial_width >= 72 && config::initial_height >= 72)
					{
						minSize = min(config::initial_width, config::initial_height);
						maxSize = max(config::initial_width, config::initial_height);
					}
					else
					{
						auto display = UnityEngine::Display::main();
						minSize = min(display.systemWidth(), display.systemHeight());
						maxSize = max(display.systemWidth(), display.systemHeight());
					}

					uiManager.ChangeResizeUIForPC(isVirt ? minSize : maxSize, isVirt ? maxSize : minSize);
				}
				else
				{
					uiManager.ChangeResizeUIForPC(isVirt ? 1080 : 1920, isVirt ? 1920 : 1080);
				}
			}

			auto anRootManager = AnimateToUnity::AnRootManager::Instance();

			if (anRootManager)
			{
				anRootManager.ScreenRate(_aspectRatio);
			}

			Gallop::StandaloneWindowResize::DisableMaximizebox();

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

	void* TextMeshProUguiCommon_Awake_orig = nullptr;
	void TextMeshProUguiCommon_Awake_hook(Il2CppObject* self)
	{
		reinterpret_cast<decltype(TextMeshProUguiCommon_Awake_hook)*>(TextMeshProUguiCommon_Awake_orig)(self);
		auto customFont = GetCustomTMPFont();

		if (!customFont)
		{
			return;
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "set_font", 1)->methodPointer(self, customFont);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(self->klass, "set_enableWordWrapping", 1)->methodPointer(self, false);

		auto customFontMaterialField = il2cpp_class_get_field_from_name(customFont->klass, "material");
		Il2CppObject* customFontMaterial;
		il2cpp_field_get_value(customFont, customFontMaterialField, &customFontMaterial);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(self->klass, "set_fontSharedMaterial", 1)->methodPointer(self, customFontMaterial);

		auto origOutlineWidth = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(self->klass, "get_outlineWidth", 0)->methodPointer(self);

		auto outlineColorDictField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ColorPreset"), "OutlineColorDictionary");
		Il2CppObject* outlineColorDict;
		il2cpp_field_static_get_value(outlineColorDictField, &outlineColorDict);
		auto colorEnum = il2cpp_class_get_method_from_name_type<uint32_t(*)(Il2CppObject*)>(self->klass, "get_OutlineColor", 0)->methodPointer(self);

		auto entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "_entries");
		if (!entriesField)
		{
			entriesField = il2cpp_class_get_field_from_name(outlineColorDict->klass, "entries");
		}
		Il2CppArraySize_t<System::Collections::Generic::Dictionary<uint32_t, UnityEngine::Color32>::Entry>* entries;
		il2cpp_field_get_value(outlineColorDict, entriesField, &entries);

		UnityEngine::Color32 color32{};
		for (int i = 0; i < entries->max_length; i++)
		{
			auto entry = entries->vector[i];
			if (entry.key == colorEnum)
			{
				color32 = entry.value;
				break;
			}
		}

		float a = color32.a / static_cast<float>(0xff);
		float b = color32.b / static_cast<float>(0xff);
		float g = color32.g / static_cast<float>(0xff);
		float r = color32.r / static_cast<float>(0xff);

		auto origOutlineColor = UnityEngine::Color{ r, g, b, a };

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, float)>(customFontMaterial->klass, "SetFloat", 2)->methodPointer(customFontMaterial, il2cpp_string_new("_OutlineWidth"), origOutlineWidth);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, UnityEngine::Color)>(customFontMaterial->klass, "SetColor", 2)->methodPointer(customFontMaterial, il2cpp_string_new("_OutlineColor"), origOutlineColor);
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

	void* Object_Internal_CloneSingleWithParent_orig = nullptr;
	Il2CppObject* Object_Internal_CloneSingleWithParent_hook(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		auto cloned = reinterpret_cast<decltype(Object_Internal_CloneSingleWithParent_hook)*>(Object_Internal_CloneSingleWithParent_orig)(data, parent, worldPositionStays);

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
						auto textCommon = Gallop::TextCommon(targetText);
						textCommon.horizontalOverflow(UnityEngine::HorizontalWrapMode::Overflow);
						textCommon.verticalOverflow(UnityEngine::VerticalWrapMode::Overflow);
					}
				}
			}
		}

		if (il2cppstring(UnityEngine::Object::Name(cloned)->chars).find(IL2CPP_STRING("LiveView")) != wstring::npos)
		{
			auto gameObject = UnityEngine::GameObject(cloned);
			auto contentsRoot = gameObject.transform().Find(il2cpp_string_new(config::live_slider_always_show ? "ContentsRoot" : "ContentsRoot/MenuRoot"));
			auto slider = Localify::UIParts::GetLiveSlider("live_slider", 0, 0, 180, false,
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
			auto updateScreenReferenceSize = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), cloned, *([](Il2CppObject* self)
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
	Il2CppObject* Sprite_get_texture_hook(Il2CppObject* self)
	{
		auto texture2D = reinterpret_cast<decltype(Sprite_get_texture_hook)*>(Sprite_get_texture_orig)(self);
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

	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* self, int nameID);
	Il2CppObject* Renderer_get_material_hook(Il2CppObject* self);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_materials_hook(Il2CppObject* self);
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* self);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_sharedMaterials_hook(Il2CppObject* self);

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

	void* Renderer_get_material_orig = nullptr;
	Il2CppObject* Renderer_get_material_hook(Il2CppObject* self)
	{
		auto material = reinterpret_cast<decltype(Renderer_get_material_hook)*>(Renderer_get_material_orig)(self);
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		return material;
	}

	void* Renderer_get_materials_orig = nullptr;
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_materials_hook(Il2CppObject* self)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_materials_hook)*>(Renderer_get_materials_orig)(self);
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
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* self)
	{
		auto material = reinterpret_cast<decltype(Renderer_get_sharedMaterial_hook)*>(Renderer_get_sharedMaterial_orig)(self);
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		return material;
	}

	void* Renderer_get_sharedMaterials_orig = nullptr;
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_sharedMaterials_hook(Il2CppObject* self)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_sharedMaterials_hook)*>(Renderer_get_sharedMaterials_orig)(self);
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
	void Renderer_set_material_hook(Il2CppObject* self, Il2CppObject* material)
	{
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		reinterpret_cast<decltype(Renderer_set_material_hook)*>(Renderer_set_material_orig)(self, material);
	}

	void* Renderer_set_materials_orig = nullptr;
	void Renderer_set_materials_hook(Il2CppObject* self, Il2CppArraySize* materials)
	{
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = (Il2CppObject*)materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
		reinterpret_cast<decltype(Renderer_set_materials_hook)*>(Renderer_set_materials_orig)(self, materials);
	}

	void* Material_SetTextureI4_orig = nullptr;
	void Material_SetTextureI4_hook(Il2CppObject* self, int nameID, Il2CppObject* texture)
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
				reinterpret_cast<decltype(Material_SetTextureI4_hook)*>(Material_SetTextureI4_orig)(self, nameID, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(Material_SetTextureI4_hook)*>(Material_SetTextureI4_orig)(self, nameID, texture);
	}

	void* Material_GetTextureImpl_orig = nullptr;
	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* self, int nameID)
	{
		auto texture = reinterpret_cast<decltype(Material_GetTextureImpl_hook)*>(Material_GetTextureImpl_orig)(self, nameID);
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
	void Material_SetTextureImpl_hook(Il2CppObject* self, int nameID, Il2CppObject* texture)
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
				reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_orig)(self, nameID, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(Material_SetTextureImpl_hook)*>(Material_SetTextureImpl_orig)(self, nameID, texture);
	}

	void* BGManager_SetMainBg_orig = nullptr;
	void BGManager_SetMainBg_hook(Il2CppObject* self, Il2CppString* path, int width, int height, float posX, float posY, bool isSetScale)
	{
		float ratio = static_cast<float>(width) / static_cast<float>(height);
		int width1 = Gallop::Screen::Width();
		int height1 = Gallop::Screen::Height();

		if (width < height)
		{
			width = width1;
			height = static_cast<int>(width / ratio);
		}
		else
		{
			height = height1;
			width = static_cast<int>(height * ratio);
		}

		reinterpret_cast<decltype(BGManager_SetMainBg_hook)*>(BGManager_SetMainBg_orig)(self, path, width, height, 0, 0, true);
	}

	void* BGManager_SetMainBgLocalPos_orig = nullptr;
	void BGManager_SetMainBgLocalPos_hook(Il2CppObject* self, UnityEngine::Vector2 pos)
	{
		// no-op
	}

	void* CharaPropRendererAccessor_SetTexture_orig = nullptr;
	void CharaPropRendererAccessor_SetTexture_hook(Il2CppObject* self, Il2CppObject* texture)
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
				reinterpret_cast<decltype(CharaPropRendererAccessor_SetTexture_hook)*>(CharaPropRendererAccessor_SetTexture_orig)(self, newTexture);
				return;
			}
		}
		reinterpret_cast<decltype(CharaPropRendererAccessor_SetTexture_hook)*>(CharaPropRendererAccessor_SetTexture_orig)(self, texture);
	}

	void* DeviceOrientationGuide_Show_orig = nullptr;
	void DeviceOrientationGuide_Show_hook(Il2CppObject* self, bool isTargetOrientationPortrait, int target)
	{
		if (!config::freeform_window)
		{
			reinterpret_cast<decltype(DeviceOrientationGuide_Show_hook)*>(DeviceOrientationGuide_Show_orig)(self, isTargetOrientationPortrait, target);
		}
	}

	CriWare::CriMana::MovieInfo* (*MoviePlayerBase_get_MovieInfo)(Il2CppObject* self);
	CriWare::CriMana::MovieInfo* (*MovieManager_GetMovieInfo)(Il2CppObject* self, Cute::Cri::MoviePlayerHandle playerHandle);

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

	void* LiveTheaterCharaSelect_CheckSwapChara_orig = nullptr;
	void LiveTheaterCharaSelect_CheckSwapChara_hook(Il2CppObject* self, int index, int oldCharaId, int oldDressId, int oldDressColorId, int oldDressId2, int oldDressColorId2, int newCharaId)
	{

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
						name = il2cpp_u8(Gallop::TextCommon(text).text()->chars);
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

								textCommon.text(il2cpp_string_new16((timeMinIl2Cpp + IL2CPP_STRING(":") + str.str()).data()));
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

								textCommonTotal.text(il2cpp_string_new16((timeMinIl2Cpp + IL2CPP_STRING(":") + str.str()).data()));
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

	void patch_game_assembly()
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			if (auto binData = il2cpp_symbols::get_class("uncheatercsd.dll", "Uncheater", "SystemBins64"))
			{
				auto binArrayField = il2cpp_class_get_field_from_name(binData->klass, "UNCHEATER_DATA");
				auto array = il2cpp_array_new(il2cpp_defaults.object_class, 128);

				auto byteArray = il2cpp_array_new(il2cpp_defaults.byte_class, 0);

				il2cpp_array_setref(array, 8, byteArray);
				il2cpp_array_setref(array, 9, byteArray);

				il2cpp_field_static_set_value(binArrayField, array);
			}
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

		MoviePlayerBase_get_MovieInfo = il2cpp_symbols::get_method_pointer<decltype(MoviePlayerBase_get_MovieInfo)>("Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerBase", "get_MovieInfo", 0);

		MovieManager_GetMovieInfo = il2cpp_symbols::get_method_pointer<decltype(MovieManager_GetMovieInfo)>("Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "GetMovieInfo", 1);

#pragma endregion

		auto Object_Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");
		ADD_HOOK(Object_Internal_CloneSingleWithParent, "UnityEngine.Object::Internal_CloneSingleWithParent at %p\n");

		if (config::replace_to_builtin_font || config::replace_to_custom_font)
		{
			auto TextMeshProUguiCommon_Awake_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TextMeshProUguiCommon", "Awake", 0);
			ADD_HOOK(TextMeshProUguiCommon_Awake, "Gallop.TextMeshProUguiCommon::Awake at %p\n");
		}

		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			Gallop::StandaloneWindowResize::IsVirt(width <= height);

			bool isPortrait = width <= height;
			Gallop::Screen::OriginalScreenWidth(isPortrait ? height : width);
			Gallop::Screen::OriginalScreenHeight(isPortrait ? width : height);

			auto DeviceOrientationGuide_Show_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DeviceOrientationGuide", "Show", 2);
			ADD_HOOK(DeviceOrientationGuide_Show, "DeviceOrientationGuide::Show at %p\n");

			auto MoviePlayerForUI_AdjustScreenSize_addr = il2cpp_symbols::get_method_pointer("Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerForUI", "AdjustScreenSize", 2);
			ADD_HOOK(MoviePlayerForUI_AdjustScreenSize, "MoviePlayerForUI::AdjustScreenSize at %p\n");

			auto BGManager_SetMainBgLocalPos_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "BGManager", "SetMainBgLocalPos", 1);
			ADD_HOOK(BGManager_SetMainBgLocalPos, "Gallop.BGManager::SetMainBgLocalPos at %p\n");
		}

		if (config::freeform_window || config::unlock_size || config::resolution_3d_scale != 1.0f)
		{
			auto BGManager_SetMainBg_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "BGManager", "SetMainBg", 6);
			ADD_HOOK(BGManager_SetMainBg, "Gallop.BGManager::SetMainBg at %p\n");
		}

		if (config::unlock_live_chara)
		{
			auto LiveTheaterCharaSelect_CheckSwapChara_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "LiveTheaterCharaSelect", "CheckSwapChara", 7);
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

		if (!config::replace_assetbundle_file_paths.empty())
		{
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

		if (config::unlock_size || config::freeform_window)
		{
			auto display = UnityEngine::Display::main();

			if (config::initial_width > 72 && config::initial_height > 72)
			{
				if (config::initial_width < config::initial_height)
				{
					config::runtime::ratioVertical = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);
					config::runtime::ratioHorizontal = static_cast<float>(config::initial_height) / static_cast<float>(config::initial_width);
				}
				else
				{
					config::runtime::ratioVertical = static_cast<float>(config::initial_height) / static_cast<float>(config::initial_width);
					config::runtime::ratioHorizontal = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);
				}

				config::runtime::initialWidth = config::initial_width;
				config::runtime::initialHeight = config::initial_height;
			}
			else
			{
				if (config::freeform_window)
				{
					auto hWnd = GetHWND();

					RECT clientRect;
					GetClientRect(hWnd, &clientRect);

					config::runtime::initialWidth = clientRect.right - clientRect.left;
					config::runtime::initialHeight = clientRect.bottom - clientRect.top;
				}
				else
				{
					config::runtime::initialWidth = display.systemWidth();
					config::runtime::initialHeight = ceil(config::runtime::initialWidth / ratio_16_9);
				}
			}

			bool isPortrait = config::runtime::initialWidth < config::runtime::initialHeight;

			Gallop::Screen::OriginalScreenWidth(isPortrait ? config::runtime::initialHeight : config::runtime::initialWidth);
			Gallop::Screen::OriginalScreenHeight(isPortrait ? config::runtime::initialWidth : config::runtime::initialHeight);
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

				auto uiManager = Gallop::UIManager::Instance();

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

				if (sceneName == IL2CPP_STRING("_Boot"))
				{
					Gallop::StandaloneWindowResize::IsVirt(!Gallop::Screen::IsLandscapeMode());
				}

				if (sceneName == IL2CPP_STRING("Title"))
				{
					TaskbarManager::SetProgressValue(0, 0);
					TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
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
					if (config::character_system_text_caption)
					{
						Localify::NotificationManager::Reset();
					}

					if (config::freeform_window)
					{
						int width = UnityEngine::Screen::width();
						int height = UnityEngine::Screen::height();

						bool isVirt = width < height;
						Gallop::Screen::OriginalScreenWidth(width);
						Gallop::Screen::OriginalScreenHeight(height);
						Gallop::StandaloneWindowResize::IsVirt(isVirt);
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

										UnityEngine::MonoBehaviour(view).StartCoroutineManaged2(coroutine);
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
			UnityEngine::Screen::SetResolution_Injected(config::runtime::initialWidth, config::runtime::initialHeight, UnityEngine::FullScreenMode::Windowed, &refreshRate);
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
			UnityEngine::Screen::SetResolution_Injected(config::runtime::initialWidth, config::runtime::initialHeight, UnityEngine::FullScreenMode::Windowed, &refreshRate);
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
