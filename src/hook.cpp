#include <stdinclude.hpp>
#include <array>

#include <set>
#include <sstream>

#include <Tlhelp32.h>

#include <regex>

#include <psapi.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "ntdll.h"

#include "rich_presence.hpp"

#include "il2cpp_dump.h"

#include "discord/core.h"
#include "discord/activity_manager.h"

#include "libcef.h"

using namespace std;

namespace
{
	string GotoTitleError =
		"내부적으로 오류가 발생하여 홈으로 이동합니다.\n\n"
		"경우에 따라서 <color=#ff911c><i>타이틀</i></color>로 돌아가거나, \n"
		"게임 <color=#ff911c><i>다시 시작</i></color>이 필요할 수 있습니다.";

	string GotoTitleErrorJa =
		"内部的にエラーが発生し、ホームに移動します。\n\n"
		"場合によっては、<color=#ff911c><i>タイトル</i></color>に戻るか、\n"
		"ゲーム<color=#ff911c><i>再起動</i></color>が必要になる場合がありますあります。";

	void path_game_assembly();

	void patch_after_criware();

	bool mh_inited = false;

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

	void* InitializeApplication_orig = nullptr;
	void InitializeApplication_hook()
	{
		path_game_assembly();

		reinterpret_cast<decltype(InitializeApplication_hook)*>(InitializeApplication_orig)();
	}

	void* UncheaterInit_OnApplicationPause_orig;

	void UncheaterInit_OnApplicationPause_hook(Il2CppObject* _this, bool value)
	{
	}

	void* Application_Quit_orig;

	void Application_Quit_hook(int exitCode)
	{
		auto StackTrace = il2cpp_symbols::get_class("mscorlib.dll", "System.Diagnostics", "StackFrame");
		auto trace = il2cpp_object_new(StackTrace);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, bool)>(trace->klass, ".ctor", 2)->methodPointer(trace, 0, false);

		auto methodRef = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(trace->klass, "GetMethod", 0)->methodPointer(trace);

		auto method = il2cpp_method_get_from_reflection(methodRef);

		if (string(method->klass->name).find("Uncheater") != string::npos ||
			string(method->klass->name).find("Boot") != string::npos ||
			string(method->name).find("Uncheater") != string::npos)
		{
			return;
		}

		reinterpret_cast<decltype(Application_Quit_hook)*>(Application_Quit_orig)(exitCode);
	}

	extern "C" NTSTATUS NtProtectVirtualMemory(
		IN      HANDLE      ProcessHandle,
		IN OUT  PVOID * BaseAddress,
		IN OUT  PSIZE_T     NumberOfBytesToProtect,
		IN      ULONG       NewAccessProtection,
		OUT     PULONG      OldAccessProtection);

	void* VirtualProtect_orig = nullptr;

	BOOL
		WINAPI
		VirtualProtect_hook(
			_In_  LPVOID lpAddress,
			_In_  SIZE_T dwSize,
			_In_  DWORD flNewProtect,
			_Out_ PDWORD lpflOldProtect
		)
	{
		if (flNewProtect == (PAGE_EXECUTE_READ | PAGE_GUARD))
		{
			return reinterpret_cast<decltype(VirtualProtect)*>(VirtualProtect_orig)(lpAddress, dwSize, PAGE_EXECUTE_READ | PAGE_GUARD, lpflOldProtect);
		}

		HANDLE realHandle;

		DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &realHandle, 0, TRUE, DUPLICATE_SAME_ACCESS);

		auto res = NtProtectVirtualMemory(realHandle, &lpAddress, &dwSize, flNewProtect, lpflOldProtect);
		if (res != 0)
		{
			cout << "NtProtectVirtualMemory res: " << hex << res << dec << endl;
		}
		return res == 0;
	}

	void* il2cpp_init_orig = nullptr;
	bool __stdcall il2cpp_init_hook(const char* domain_name)
	{
		const auto result = reinterpret_cast<decltype(il2cpp_init_hook)*>(il2cpp_init_orig)(domain_name);
		il2cpp_symbols::init(GetModuleHandle("GameAssembly.dll"));

		il2cpp_thread_attach(il2cpp_domain_get());

		auto InitializeApplication = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GameSystem", "InitializeApplication", -1);
		MH_CreateHook(InitializeApplication, InitializeApplication_hook, &InitializeApplication_orig);
		MH_EnableHook(InitializeApplication);

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			KillProcessByName("ucldr_Umamusume_KR_loader_x64.exe");

			auto Application_Quit = il2cpp_resolve_icall("UnityEngine.Application::Quit(System.Int32)");
			MH_CreateHook(Application_Quit, Application_Quit_hook, &Application_Quit_orig);
			MH_EnableHook(Application_Quit);

			auto UncheaterInit_OnApplicationPause_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"", "UncheaterInit", "OnApplicationPause", 1);

			MH_CreateHook(UncheaterInit_OnApplicationPause_addr, UncheaterInit_OnApplicationPause_hook, &UncheaterInit_OnApplicationPause_orig);
			MH_EnableHook(UncheaterInit_OnApplicationPause_addr);

			path_game_assembly();
		}
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

	Il2CppObject* notification;

	bool isRequiredInitNotification = true;

	FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name);
	Il2CppObject* GetSingletonInstance(Il2CppClass* klass);
	void PrintStackTrace();

	bool (*uobject_IsNativeObjectAlive)(Il2CppObject* uObject);

	template<typename... T>
	Il2CppDelegate* CreateDelegateWithClass(Il2CppClass* klass, Il2CppObject* target, void (*fn)(Il2CppObject*, T...));

	void SetNotificationDisplayTime(float time)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _displayTimeField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_displayTime");
		il2cpp_field_set_value(notification, _displayTimeField, &time);
	}

	void ShowNotification(Il2CppString* text)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto canvasGroupField = il2cpp_class_get_field_from_name_wrap(notification->klass, "canvasGroup");
		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);

		auto _tweenerField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_tweener");
		Il2CppObject* _tweener;
		il2cpp_field_get_value(notification, _tweenerField, &_tweener);

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		auto _displayTimeField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_displayTime");
		float  _displayTime;
		il2cpp_field_get_value(notification, _displayTimeField, &_displayTime);

		auto _fadeOutTimeField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_fadeOutTime");
		float _fadeOutTime;
		il2cpp_field_get_value(notification, _fadeOutTimeField, &_fadeOutTime);

		if (_tweener)
		{
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
			_tweener = nullptr;
			il2cpp_field_set_value(notification, _tweenerField, _tweener);
		}
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(_Label->klass, "set_text", 1)->methodPointer(_Label, text);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasGroup->klass, "set_alpha", 1)->methodPointer(canvasGroup, 1);

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(notification->klass, "get_gameObject", 0)->methodPointer(notification);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

		_tweener = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, float, float)>("Plugins.dll", "DG.Tweening", "DOTweenModuleUI", "DOFade", 3)(canvasGroup, 0, _fadeOutTime);

		auto delayField = il2cpp_class_get_field_from_name_wrap(_tweener->klass, "delay");
		float delay = _displayTime;
		il2cpp_field_set_value(_tweener, delayField, &delay);

		auto delayCompleteField = il2cpp_class_get_field_from_name_wrap(_tweener->klass, "delayComplete");
		bool delayComplete = delay <= 0;
		il2cpp_field_set_value(_tweener, delayCompleteField, &delayComplete);

		auto onCompleteField = il2cpp_class_get_field_from_name_wrap(_tweener->klass, "onComplete");
		auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), notification, *([](Il2CppObject* _this)
			{
				auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
				if (gameObject)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, false);

					auto _tweenerField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_tweener");
					il2cpp_field_set_value(notification, _tweenerField, nullptr);
				}
			}));
		il2cpp_field_set_value(_tweener, onCompleteField, callback);

		il2cpp_field_set_value(notification, _tweenerField, _tweener);
	}

	void ShowCaptionByNotification(Il2CppObject* audioManager, Il2CppObject* elem)
	{
		if (!audioManager || !elem)
		{
			return;
		}

		auto characterIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CharacterId");
		auto voiceIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "VoiceId");
		auto textField = il2cpp_class_get_field_from_name_wrap(elem->klass, "Text");
		auto cueSheetField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueSheet");
		auto cueIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueId");

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

		auto u8Text = local::wide_u8(text->start_char);
		replaceAll(u8Text, "\n\n", " ");
		replaceAll(u8Text, "\n", " ");
		if (notification && wstring(cueSheet->start_char).find(L"_home_") == string::npos &&
			wstring(cueSheet->start_char).find(L"_tc_") == string::npos &&
			wstring(cueSheet->start_char).find(L"_title_") == string::npos &&
			wstring(cueSheet->start_char).find(L"_kakao_") == string::npos &&
			wstring(cueSheet->start_char).find(L"_gacha_") == string::npos &&
			voiceId != 95001 &&
			(characterId < 9000 || voiceId == 95006 || voiceId == 70000))
		{

			if (wstring(cueSheet->start_char).find(L"_training_") != string::npos && (cueId < 29 || cueId == 39))
			{
				if (voiceId < 93000 && !(cueId == 8 || cueId == 9 || cueId == 12 || cueId == 13))
				{
					return;
				}
			}

			auto LineHeadWrap =
				il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, int)>("umamusume.dll", "Gallop", "GallopUtil",
					"LineHeadWrap", 2);

			float length =
				il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, Il2CppString*, int)>(audioManager->klass, "GetCueLength",
					2)->methodPointer(audioManager, cueSheet, cueId);

			SetNotificationDisplayTime(length);

			ShowNotification(LineHeadWrap(il2cpp_string_new(u8Text.data()), g_character_system_text_caption_line_char_count));
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
	bool criAtomExAcb_GetCueInfoById_hook(void* acb_hn, int id, CueInfo* info)
	{
		auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoById_hook)*>(criAtomExAcb_GetCueInfoById_orig)(acb_hn, id, info);

		if (!info || !info->name)
		{
			return result;
		}
		const regex r(R"(_(?:9)*(\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
		smatch stringMatch;
		const auto cueSheet = string(info->name);
		const auto cueSheet16 = local::u8_wide(cueSheet);
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
				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(textList->klass, "_items");
				Il2CppArraySize* textArr;
				il2cpp_field_get_value(textList, itemsField, &textArr);

				if (textArr)
				{
					for (int i = 0; i < textArr->max_length; i++)
					{
						auto elem = reinterpret_cast<Il2CppObject*>(textArr->vector[i]);
						if (elem)
						{
							auto elemCueIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueId");
							auto elemCueSheetField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueSheet");

							Il2CppString* elemCueSheet;
							il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

							int elemCueId;
							il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

							if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->start_char) && info->id == elemCueId) {
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
	bool criAtomExAcb_GetCueInfoByName_hook(void* acb_hn, char* name, CueInfo* info)
	{
		auto result = reinterpret_cast<decltype(criAtomExAcb_GetCueInfoByName_hook)*>(criAtomExAcb_GetCueInfoByName_orig)(acb_hn, name, info);

		if (!info || !info->name)
		{
			return result;
		}

		const regex r(R"((\d{4})(?:\d{2})*_(\d{4})*_*(\d{2})*(?:\d{2})*$)");
		smatch stringMatch;
		const auto cueSheet = string(info->name);
		const auto cueSheet16 = local::u8_wide(cueSheet);
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
				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(textList->klass, "_items");
				Il2CppArraySize* textArr;
				il2cpp_field_get_value(textList, itemsField, &textArr);


				if (textArr)
				{
					for (int i = 0; i < textArr->max_length; i++)
					{
						auto elem = reinterpret_cast<Il2CppObject*>(textArr->vector[i]);
						if (elem)
						{
							auto elemCueIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueId");
							auto elemCueSheetField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueSheet");

							Il2CppString* elemCueSheet;
							il2cpp_field_get_value(elem, elemCueSheetField, &elemCueSheet);

							int elemCueId;
							il2cpp_field_get_value(elem, elemCueIdField, &elemCueId);

							if (elemCueSheet && cueSheet16.starts_with(elemCueSheet->start_char) && info->id == elemCueId)
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

			if (notification)
			{
				auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
				if (gameObject)
				{
					auto _tweenerField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_tweener");
					Il2CppObject* _tweener;
					il2cpp_field_get_value(notification, _tweenerField, &_tweener);
					if (_tweener)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
						_tweener = nullptr;
						il2cpp_field_set_value(notification, _tweenerField, _tweener);
					}
				}
			}
		}
	}

	void* criAtomExPlayer_StopWithoutReleaseTime_orig = nullptr;
	void criAtomExPlayer_StopWithoutReleaseTime_hook(void* player)
	{
		reinterpret_cast<decltype(criAtomExPlayer_StopWithoutReleaseTime_hook)*>(criAtomExPlayer_StopWithoutReleaseTime_orig)(player);
		if (currentPlayerHandles.contains(player) && currentPlayerHandle == player)
		{
			currentElem = nullptr;

			if (notification)
			{
				auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
				if (gameObject)
				{
					auto _tweenerField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_tweener");
					Il2CppObject* _tweener;
					il2cpp_field_get_value(notification, _tweenerField, &_tweener);
					if (_tweener)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
						_tweener = nullptr;
						il2cpp_field_set_value(notification, _tweenerField, _tweener);
					}
				}
			}
		}
	}

	void* criAtomExPlayer_Pause_orig = nullptr;
	void criAtomExPlayer_Pause_hook(void* player, bool sw)
	{
		reinterpret_cast<decltype(criAtomExPlayer_Pause_hook)*>(criAtomExPlayer_Pause_orig)(player, sw);
		if (!sw && currentPlayerHandles.contains(player) && currentPlayerHandle == player)
		{
			currentElem = nullptr;

			if (notification)
			{
				auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
				if (gameObject)
				{
					auto _tweenerField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_tweener");
					Il2CppObject* _tweener;
					il2cpp_field_get_value(notification, _tweenerField, &_tweener);
					if (_tweener)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
						_tweener = nullptr;
						il2cpp_field_set_value(notification, _tweenerField, _tweener);
					}
				}
			}
		}
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

	void* cef_cookie_manager_get_global_manager_orig = nullptr;
	cef_cookie_manager_t* cef_cookie_manager_get_global_manager_hook(
		struct _cef_completion_callback_t* callback)
	{
		auto manager = reinterpret_cast<decltype(cef_cookie_manager_get_global_manager)>(cef_cookie_manager_get_global_manager_orig)(callback);

		if (manager)
		{
			auto addr = manager->delete_cookies;

			MH_CreateHook(addr, delete_cookies_hook, &delete_cookies_orig);
			MH_EnableHook(addr);
		}

		return manager;
	}

	void* cef_initialize_orig = nullptr;
	int cef_initialize_hook(const cef_main_args_t* args,
		/*const*/ struct _cef_settings_t* settings,
		struct _cef_app_t* application,
		void* windows_sandbox_info)
	{
		return reinterpret_cast<decltype(cef_initialize)>(cef_initialize_orig)(args, settings, application, windows_sandbox_info);
	}

	void* cef_browser_host_create_browser_orig = nullptr;
	int cef_browser_host_create_browser_hook(
		const cef_window_info_t* windowInfo,
		struct _cef_client_t* client,
		const cef_string_t* url,
		const struct _cef_browser_settings_t* settings,
		struct _cef_dictionary_value_t* extra_info,
		struct _cef_request_context_t* request_context)
	{
		return reinterpret_cast<decltype(cef_browser_host_create_browser)>(cef_browser_host_create_browser_orig)(windowInfo, client, url, settings, extra_info, request_context);
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
		if (wstring(lpLibFileName).find(L"KakaoGame.dll") != wstring::npos)
		{
			auto KakaoGame = reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);

			auto cef = GetModuleHandleW(L"libcef.dll");

			cef_initialize = reinterpret_cast<decltype(cef_initialize)>(GetProcAddress(cef, "cef_initialize"));

			cef_string_wide_set = reinterpret_cast<decltype(cef_string_wide_set)>(GetProcAddress(cef, "cef_string_wide_set"));

			cef_string_utf8_set = reinterpret_cast<decltype(cef_string_utf8_set)>(GetProcAddress(cef, "cef_string_utf8_set"));

			cef_string_utf16_set = reinterpret_cast<decltype(cef_string_utf16_set)>(GetProcAddress(cef, "cef_string_utf16_set"));

			cef_browser_host_create_browser = reinterpret_cast<decltype(cef_browser_host_create_browser)>(GetProcAddress(cef, "cef_browser_host_create_browser"));

			cef_string_list_alloc = reinterpret_cast<decltype(cef_string_list_alloc)>(GetProcAddress(cef, "cef_string_list_alloc"));

			cef_string_list_size = reinterpret_cast<decltype(cef_string_list_size)>(GetProcAddress(cef, "cef_string_list_size"));

			cef_string_list_value = reinterpret_cast<decltype(cef_string_list_value)>(GetProcAddress(cef, "cef_string_list_value"));

			cef_string_list_append = reinterpret_cast<decltype(cef_string_list_append)>(GetProcAddress(cef, "cef_string_list_append"));

			cef_string_list_clear = reinterpret_cast<decltype(cef_string_list_clear)>(GetProcAddress(cef, "cef_string_list_clear"));

			cef_string_list_free = reinterpret_cast<decltype(cef_string_list_free)>(GetProcAddress(cef, "cef_string_list_free"));

			cef_string_list_copy = reinterpret_cast<decltype(cef_string_list_copy)>(GetProcAddress(cef, "cef_string_list_copy"));

			cef_cookie_manager_get_global_manager = reinterpret_cast<decltype(cef_cookie_manager_get_global_manager)>(GetProcAddress(cef, "cef_cookie_manager_get_global_manager"));

			MH_CreateHook(cef_initialize, cef_initialize_hook, &cef_initialize_orig);
			MH_EnableHook(cef_initialize);

			MH_CreateHook(cef_browser_host_create_browser, cef_browser_host_create_browser_hook, &cef_browser_host_create_browser_orig);
			MH_EnableHook(cef_browser_host_create_browser);

			MH_CreateHook(cef_cookie_manager_get_global_manager, cef_cookie_manager_get_global_manager_hook, &cef_cookie_manager_get_global_manager_orig);
			MH_EnableHook(cef_cookie_manager_get_global_manager);

			return KakaoGame;
		}

		return reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);
	}

	HWND GetHWND()
	{
		auto title = local::u8_wide(g_custom_title_name);
		if (title.empty())
		{
			title = L"umamusume";
		}

		auto hWnd = FindWindowA("UnityWndClass", local::wide_acp(title).data());

		if (!hWnd)
		{
			hWnd = FindWindowA("UnityWndClass", local::wide_acp(L"umamusume").data());
		}

		return hWnd;
	}

	void* load_library_w_orig = nullptr;
	HMODULE WINAPI load_library_w_hook(LPCWSTR lpLibFileName)
	{
		if (lpLibFileName == L"UmamusumeKr.dll"s)
		{
			const auto UmamusumeKr = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
			const auto umaFindService_addr = GetProcAddress(UmamusumeKr, "umaFindService");
			MH_CreateHook(umaFindService_addr, umaFindService_hook, &umaFindService_orig);
			MH_EnableHook(umaFindService_addr);
			return UmamusumeKr;
		}

		/*if (lpLibFileName == L"GameAssembly.dll"s)
		{
			const auto il2cpp = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
			const auto il2cpp_init_addr = GetProcAddress(il2cpp, "il2cpp_init");
			MH_CreateHook(il2cpp_init_addr, il2cpp_init_hook, &il2cpp_init_orig);
			MH_EnableHook(il2cpp_init_addr);
			return il2cpp;
		}*/

		// GameAssembly.dll code must be loaded and decrypted while loading criware library
		if (lpLibFileName == L"cri_ware_unity.dll"s)
		{
			patch_after_criware();

			MH_DisableHook(LoadLibraryW);
			MH_RemoveHook(LoadLibraryW);

			auto hWnd = FindWindowW(L"UnityWndClass", L"umamusume");
			if (hWnd)
			{
				if (!g_custom_title_name.empty())
				{
					SetWindowTextA(hWnd, local::wide_acp(local::u8_wide(g_custom_title_name)).data());
				}
				if (has_json_parse_error)
				{
					MessageBox(hWnd, json_parse_error_msg.data(), "Umamusume Localify", MB_OK | MB_ICONWARNING);
				}
			}

			// use original function beacuse we have unhooked that
			auto criware = LoadLibraryW(lpLibFileName);

			if (g_character_system_text_caption)
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

			return criware;
		}

		return reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
	}

	FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name)
	{
		if (code_map.IsNull() || code_map.HasParseError())
		{
			return il2cpp_class_get_field_from_name(klass, name);
		}
		auto className = string(klass->namespaze).append(".").append(klass->name);

		if (code_map.HasMember("!common"))
		{
			auto commonMap = code_map["!common"].GetObjectA();
			if (commonMap.HasMember(name))
			{
				auto field = il2cpp_class_get_field_from_name(klass, commonMap[name].GetString());
				if (field)
				{
					return field;
				}
			}
		}

		if (!code_map.HasMember(className.data()))
		{
			return il2cpp_class_get_field_from_name(klass, name);
		}

		auto classMap = code_map[className.data()].GetObjectA();

		if (classMap.HasMember(name))
		{
			auto field = il2cpp_class_get_field_from_name(klass, classMap[name].GetString());
			if (field)
			{
				return field;
			}
		}

		if (classMap.HasMember((name + ".index"s).data()))
		{
			void* iter = nullptr;
			int i = 0;
			int index = classMap[(name + ".index"s).data()].GetInt();
			while (FieldInfo* field = il2cpp_class_get_fields(klass, &iter))
			{
				if (index == i)
				{
					return field;
				}
				i++;
			}
		}

		if (classMap.HasMember("!extends"))
		{
			auto parentName = classMap["!extends"].GetString();
			auto parentMap = code_map[parentName].GetObjectA();
			auto parentClass = klass->parent;

			if (parentMap.HasMember((name + ".index"s).data()))
			{
				void* iter = nullptr;
				int i = 0;
				int index = parentMap[(name + ".index"s).data()].GetInt();
				while (FieldInfo* field = il2cpp_class_get_fields(parentClass, &iter))
				{
					if (index == i)
					{
						return field;
					}
					i++;
				}
			}
		}

		return il2cpp_class_get_field_from_name(klass, name);;
	}

	Il2CppObject* fontAssets = nullptr;

	vector<Il2CppObject*> replaceAssets;

	vector<string> replaceAssetNames;

	Il2CppObject* (*load_from_file)(Il2CppString* path);

	Il2CppObject* (*load_assets)(Il2CppObject* _this, Il2CppString* name, Il2CppObject* runtimeType);

	Il2CppArraySize* (*get_all_asset_names)(Il2CppObject* _this);

	Il2CppString* (*uobject_get_name)(Il2CppObject* uObject);

	Il2CppString* (*uobject_set_name)(Il2CppObject* uObject, Il2CppString* name);

	Il2CppString* (*get_unityVersion)();

	void PrintStackTrace()
	{
		Il2CppString* (*trace)() = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "StackTraceUtility", "ExtractStackTrace", 0);
		cout << local::wide_u8(wstring(trace()->start_char)) << "\n";
	}

	Il2CppObject* GetRuntimeType(const char* assemblyName, const char* namespaze, const char* klassName)
	{
		return il2cpp_type_get_object(il2cpp_class_get_type(il2cpp_symbols::get_class(assemblyName, namespaze, klassName)));
	}

	template<typename... T>
	Il2CppDelegate* CreateDelegateWithClass(Il2CppClass* klass, Il2CppObject* target, void (*fn)(Il2CppObject*, T...))
	{
		auto delegate = reinterpret_cast<MulticastDelegate*>(il2cpp_object_new(klass));
		delegate->delegates = il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Delegate"), 1);
		il2cpp_array_setref(delegate->delegates, 0, delegate);
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		auto methodInfo = reinterpret_cast<MethodInfo*>(il2cpp_object_new(
			il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo")));
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo");
		delegate->method = methodInfo;
		delegate->target = target;

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		// auto methodInfoField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_info");
		// il2cpp_field_set_value(object, methodInfoField, methodInfo);

		// auto pointerField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_ptr");
		// il2cpp_field_set_value(object, pointerField, fn);

		auto targetField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_target");
		il2cpp_field_set_value(object, targetField, target);

		return delegate;
	}

	template<typename... T>
	Il2CppDelegate* CreateDelegate(Il2CppObject* target, void (*fn)(Il2CppObject*, T...))
	{
		auto delegate = reinterpret_cast<MulticastDelegate*>(
			il2cpp_object_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "MulticastDelegate")));
		delegate->delegates = il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Delegate"), 1);
		il2cpp_array_setref(delegate->delegates, 0, delegate);
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		auto methodInfo = reinterpret_cast<MethodInfo*>(il2cpp_object_new(
			il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo")));
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo");
		delegate->method = methodInfo;
		delegate->target = target;

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		// auto methodInfoField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_info");
		// il2cpp_field_set_value(object, methodInfoField, methodInfo);

		// auto pointerField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_ptr");
		// il2cpp_field_set_value(object, pointerField, fn);

		auto targetField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_target");
		il2cpp_field_set_value(object, targetField, target);

		return delegate;
	}

	template<typename... T>
	Il2CppDelegate* CreateUnityAction(Il2CppObject* target, void (*fn)(Il2CppObject*, T...))
	{
		auto delegate = reinterpret_cast<MulticastDelegate*>(
			il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction")));
		delegate->delegates = il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Delegate"), 1);
		il2cpp_array_setref(delegate->delegates, 0, delegate);
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		auto methodInfo = reinterpret_cast<MethodInfo*>(il2cpp_object_new(
			il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo")));
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MethodInfo");
		delegate->method = methodInfo;
		delegate->target = target;

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		// auto methodInfoField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_info");
		// il2cpp_field_set_value(object, methodInfoField, methodInfo);

		// auto pointerField = il2cpp_class_get_field_from_name_wrap(object->klass, "method_ptr");
		// il2cpp_field_set_value(object, pointerField, fn);

		auto targetField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_target");
		il2cpp_field_set_value(object, targetField, target);

		return delegate;
	}

	Il2CppDelegate* GetButtonCommonOnClickDelegate(Il2CppObject* object)
	{
		if (!object)
		{
			return nullptr;
		}
		if (object->klass != il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ButtonCommon"))
		{
			return nullptr;
		}
		auto onClickField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_OnClick");
		Il2CppObject* onClick;
		il2cpp_field_get_value(object, onClickField, &onClick);
		if (onClick)
		{
			auto callsField = il2cpp_class_get_field_from_name_wrap(onClick->klass, "m_Calls");
			Il2CppObject* calls;
			il2cpp_field_get_value(onClick, callsField, &calls);
			if (calls)
			{
				auto runtimeCallsField = il2cpp_class_get_field_from_name_wrap(calls->klass,
					"m_RuntimeCalls");
				Il2CppObject* runtimeCalls;
				il2cpp_field_get_value(calls, runtimeCallsField, &runtimeCalls);

				if (runtimeCalls)
				{
					FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(runtimeCalls->klass,
						"_items");
					Il2CppArraySize* arr;
					il2cpp_field_get_value(runtimeCalls, itemsField, &arr);
					if (arr)
					{
						for (int i = 0; i < arr->max_length; i++)
						{
							auto value = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
							if (value)
							{
								auto delegateField = il2cpp_class_get_field_from_name_wrap(value->klass,
									"Delegate");
								Il2CppDelegate* delegate;
								il2cpp_field_get_value(value, delegateField, &delegate);
								if (delegate)
								{
									// Unbox delegate
									auto callbackField = il2cpp_class_get_field_from_name_wrap(
										delegate->target->klass, "callback");
									Il2CppDelegate* callback;
									il2cpp_field_get_value(delegate->target, callbackField, &callback);

									return callback;
								}
							}
						}
					}
				}
			}
		}
		return nullptr;
	}

	Il2CppObject* GetSingletonInstance(Il2CppClass* klass)
	{
		if (!klass || !klass->parent)
		{
			return nullptr;
		}
		auto instanceField = il2cpp_class_get_field_from_name_wrap(klass, "_instance");
		if (!instanceField)
		{
			return nullptr;
		}
		Il2CppObject* instance;
		il2cpp_field_static_get_value(instanceField, &instance);
		return instance;
	}

	Boolean GetBoolean(bool value)
	{
		return il2cpp_symbols::get_method_pointer<Boolean(*)(Il2CppString * value)>(
			"mscorlib.dll", "System", "Boolean", "Parse", 1)(
				il2cpp_string_new(value ? "true" : "false"));
	}

	Int32Object* GetInt32Instance(int value)
	{
		auto int32Class = il2cpp_symbols::get_class("mscorlib.dll", "System", "Int32");
		auto instance = il2cpp_object_new(int32Class);
		il2cpp_runtime_object_init(instance);
		auto m_value = il2cpp_class_get_field_from_name_wrap(int32Class, "m_value");
		il2cpp_field_set_value(instance, m_value, &value);
		return (Int32Object*)instance;
	}

	Il2CppObject* ParseEnum(Il2CppObject* runtimeType, const string& name)
	{
		return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppString*)>("mscorlib.dll", "System", "Enum", "Parse", 2)(runtimeType, il2cpp_string_new(name.data()));
	}

	Il2CppString* GetEnumName(Il2CppObject* runtimeType, int id)
	{
		return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppObject*, Int32Object*)>("mscorlib.dll", "System", "Enum", "GetName", 2)(runtimeType, GetInt32Instance(id));
	}

	unsigned long GetEnumValue(Il2CppObject* runtimeEnum)
	{
		return il2cpp_symbols::get_method_pointer<unsigned long (*)(Il2CppObject*)>("mscorlib.dll", "System", "Enum", "ToUInt64", 1)(runtimeEnum);
	}

	unsigned long GetTextIdByName(const string& name)
	{
		return GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), name));
	}

	string GetTextIdNameById(int id)
	{
		return local::wide_u8(GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), id)->start_char);
	}

	Il2CppObject* GetCustomFont()
	{
		if (!fontAssets) return nullptr;
		if (!g_font_asset_name.empty())
		{
			return load_assets(fontAssets, il2cpp_string_new(g_font_asset_name.data()), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font"));
		}
		return nullptr;
	}

	// Fallback not support outline style
	Il2CppObject* GetCustomTMPFontFallback()
	{
		if (!fontAssets) return nullptr;
		auto font = GetCustomFont();
		if (font)
		{
			return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(
				Il2CppObject * font, int samplingPointSize, int atlasPadding, int renderMode, int atlasWidth, int atlasHeight, int atlasPopulationMode, bool enableMultiAtlasSupport
				)>("Unity.TextMeshPro.dll", "TMPro", "TMP_FontAsset", "CreateFontAsset", 1)
				(font, 36, 4, 4165, 8192, 8192, 1, false);
		}
		return nullptr;
	}

	Il2CppObject* GetCustomTMPFont()
	{
		if (!fontAssets) return nullptr;
		if (!g_tmpro_font_asset_name.empty())
		{
			auto tmpFont = load_assets(fontAssets, il2cpp_string_new(g_tmpro_font_asset_name.data()), GetRuntimeType("Unity.TextMeshPro.dll", "TMPro", "TMP_FontAsset"));
			return tmpFont ? tmpFont : GetCustomTMPFontFallback();
		}
		return GetCustomTMPFontFallback();
	}

	void* assetbundle_load_asset_orig = nullptr;
	Il2CppObject* assetbundle_load_asset_hook(Il2CppObject* _this, Il2CppString* name, const Il2CppType* type);

	Il2CppObject* GetReplacementAssets(Il2CppString* name, const Il2CppType* type)
	{
		for (auto it = replaceAssets.begin(); it != replaceAssets.end(); it++)
		{
			auto assets = reinterpret_cast<decltype(assetbundle_load_asset_hook)*>(assetbundle_load_asset_orig)(*it, name, type);
			if (assets)
			{
				return assets;
			}
		}
		return nullptr;
	}

	string GetUnityVersion()
	{
		string version(local::wide_u8(get_unityVersion()->start_char));
		return version;
	}

	void* populate_with_errors_orig = nullptr;
	bool populate_with_errors_hook(Il2CppObject* _this, Il2CppString* str, TextGenerationSettings_t* settings, void* context)
	{
		return reinterpret_cast<decltype(populate_with_errors_hook)*>(populate_with_errors_orig) (
			_this, local::get_localized_string(str), settings, context
			);
	}

	void* localizeextension_text_orig = nullptr;
	Il2CppString* localizeextension_text_hook(int id)
	{
		auto orig_result = reinterpret_cast<decltype(localizeextension_text_hook)*>(localizeextension_text_orig)(id);
		auto result = g_static_entries_use_text_id_name ?
			local::get_localized_string(GetTextIdNameById(id)) :
			g_static_entries_use_hash ?
			local::get_localized_string(orig_result) : local::get_localized_string(id);

		return result ? result : orig_result;
	}

	void ReplaceTextMeshFont(Il2CppObject* textMesh, Il2CppObject* meshRenderer)
	{
		Il2CppObject* font = GetCustomFont();
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
	float get_preferred_width_hook(void* _this, Il2CppString* str, TextGenerationSettings_t* settings)
	{
		return reinterpret_cast<decltype(get_preferred_width_hook)*>(get_preferred_width_orig) (
			_this, local::get_localized_string(str), settings
			);
	}

	void* localize_get_orig = nullptr;
	Il2CppString* localize_get_hook(int id)
	{
		auto orig_result = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(id);
		auto result = g_static_entries_use_text_id_name ?
			local::get_localized_string(GetTextIdNameById(id)) :
			g_static_entries_use_hash ?
			local::get_localized_string(orig_result) : local::get_localized_string(id);

		return result ? result : orig_result;
	}

	void* an_text_set_material_to_textmesh_orig = nullptr;
	void an_text_set_material_to_textmesh_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(an_text_set_material_to_textmesh_hook)*>(an_text_set_material_to_textmesh_orig)(_this);
		if (!(fontAssets && g_replace_to_custom_font)) return;

		FieldInfo* mainField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_mainTextMesh");
		FieldInfo* mainRenderer = il2cpp_class_get_field_from_name_wrap(_this->klass, "_mainTextMeshRenderer");

		FieldInfo* outlineField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_outlineTextMeshList"); //List<TextMesh>
		FieldInfo* outlineFieldRenderers = il2cpp_class_get_field_from_name_wrap(_this->klass, "_outlineTextMeshRendererList"); //List<MeshRenderer>

		FieldInfo* shadowField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_shadowTextMesh");
		FieldInfo* shadowFieldRenderer = il2cpp_class_get_field_from_name_wrap(_this->klass, "_shadowTextMeshRenderer");

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
			FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(outlineMeshes->klass, "_items");
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
			FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(outlineMeshRenderers->klass, "_items");
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
		FieldInfo* field = il2cpp_class_get_field_from_name_wrap(_this->klass, "_text");
		Il2CppString* str;
		il2cpp_field_get_value(_this, field, &str);
		il2cpp_field_set_value(_this, field, local::get_localized_string(str));
	}

	void* update_orig = nullptr;
	void* update_hook(Il2CppObject* _this, void* updateType, float deltaTime, float independentTime)
	{
		return reinterpret_cast<decltype(update_hook)*>(update_orig)(_this, updateType, deltaTime * g_ui_animation_scale, independentTime * g_ui_animation_scale);
	}

	unordered_map<void*, SQLite::Statement*> text_queries;
	unordered_map<void*, bool> replacement_queries_can_next;

	SQLite::Database* replacementMDB;

	void* query_setup_orig = nullptr;
	void query_setup_hook(Il2CppObject* _this, void* conn, Il2CppString* sql)
	{
		reinterpret_cast<decltype(query_setup_hook)*>(query_setup_orig)(_this, conn, sql);

		auto ssql = wstring(sql->start_char);

		if (ssql.find(L"text_data") != string::npos ||
			ssql.find(L"character_system_text") != string::npos ||
			ssql.find(L"race_jikkyo_comment") != string::npos ||
			ssql.find(L"race_jikkyo_message") != string::npos)
		{
			auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
			intptr_t* stmtPtr;
			il2cpp_field_get_value(_this, stmtField, &stmtPtr);
			try
			{
				if (replacementMDB)
				{
					text_queries.emplace(stmtPtr, new SQLite::Statement(*replacementMDB, local::wide_u8(ssql)));
				}
				else
				{
					text_queries.emplace(stmtPtr, nullptr);
				}
			}
			catch (exception& e)
			{
				cout << "query_setup ERROR: " << e.what() << endl;
			}
		}
	}

	void* Plugin_sqlite3_step_orig = nullptr;
	bool Plugin_sqlite3_step_hook(intptr_t* pStmt)
	{
		if (text_queries.contains(pStmt))
		{
			try
			{
				auto stmt = text_queries.at(pStmt);
				if (stmt)
				{
					if (stmt->getQuery().find("`race_jikkyo_message`;") != string::npos ||
						stmt->getQuery().find("`race_jikkyo_comment`;") != string::npos)
					{
						if (replacement_queries_can_next.find(pStmt) == replacement_queries_can_next.end())
						{
							replacement_queries_can_next.emplace(pStmt, true);
						}
						if (replacement_queries_can_next.at(pStmt))
						{
							try
							{
								stmt->executeStep();
							}
							catch (exception& e)
							{
							}
						}
					}
					else
					{
						stmt->executeStep();
					}
				}
			}
			catch (exception& e)
			{
			}
		}

		return reinterpret_cast<decltype(Plugin_sqlite3_step_hook)*>(Plugin_sqlite3_step_orig)(pStmt);
	}

	void* Plugin_sqlite3_reset_orig = nullptr;
	bool Plugin_sqlite3_reset_hook(intptr_t* pStmt)
	{
		if (text_queries.contains(pStmt))
		{
			try
			{
				auto stmt = text_queries.at(pStmt);
				if (stmt)
				{
					stmt->reset();
					stmt->clearBindings();
					replacement_queries_can_next.insert_or_assign(pStmt, true);
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(Plugin_sqlite3_reset_hook)*>(Plugin_sqlite3_reset_orig)(pStmt);
	}

	void* query_step_orig = nullptr;
	bool query_step_hook(Il2CppObject* _this)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					stmt->executeStep();
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(query_step_hook)*>(query_step_orig)(_this);
	}

	void* prepared_query_reset_orig = nullptr;
	bool prepared_query_reset_hook(Il2CppObject* _this)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{

					stmt->reset();
					stmt->clearBindings();
					replacement_queries_can_next.insert_or_assign(stmtPtr, true);
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(prepared_query_reset_hook)*>(prepared_query_reset_orig)(_this);
	}

	void* prepared_query_bind_text_orig = nullptr;
	bool prepared_query_bind_text_hook(Il2CppObject* _this, int idx, Il2CppString* text)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					stmt->bind(idx, local::wide_u8(text->start_char));
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(prepared_query_bind_text_hook)*>(prepared_query_bind_text_orig)(_this, idx, text);
	}

	void* prepared_query_bind_int_orig = nullptr;
	bool prepared_query_bind_int_hook(Il2CppObject* _this, int idx, int iValue)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					stmt->bind(idx, iValue);
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(prepared_query_bind_int_hook)*>(prepared_query_bind_int_orig)(_this, idx, iValue);
	}

	void* prepared_query_bind_long_orig = nullptr;
	bool prepared_query_bind_long_hook(Il2CppObject* _this, int idx, int64_t lValue)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					stmt->bind(idx, lValue);
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(prepared_query_bind_long_hook)*>(prepared_query_bind_long_orig)(_this, idx, lValue);
	}

	void* prepared_query_bind_double_orig = nullptr;
	bool prepared_query_bind_double_hook(Il2CppObject* _this, int idx, double rValue)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					stmt->bind(idx, rValue);
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(prepared_query_bind_double_hook)*>(prepared_query_bind_double_orig)(_this, idx, rValue);
	}

	void* query_dispose_orig = nullptr;
	void query_dispose_hook(Il2CppObject* _this)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
			text_queries.erase(stmtPtr);

		return reinterpret_cast<decltype(query_dispose_hook)*>(query_dispose_orig)(_this);
	}

	int (*query_getint)(Il2CppObject* _this, int index) = nullptr;

	void* query_gettext_orig = nullptr;
	Il2CppString* query_gettext_hook(Il2CppObject* _this, int idx)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(_this, stmtField, &stmtPtr);
		auto result = reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(_this, idx);

		if (text_queries.contains(stmtPtr))
		{
			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					string text;
					if (stmt->hasRow())
					{
						text = stmt->getColumn(idx).getString();
						if (!text.empty())
						{
							if (stmt->getQuery().find("`race_jikkyo_message`;") != string::npos ||
								stmt->getQuery().find("`race_jikkyo_comment`;") != string::npos)
							{
								int id = query_getint(_this, 0);
								int id1 = stmt->getColumn(0).getInt();
								int groupId = query_getint(_this, 1);
								int groupId1 = stmt->getColumn(1).getInt();
								if (stmt->hasRow())
								{
									if (id == id1 && groupId == groupId1)
									{
										replacement_queries_can_next.insert_or_assign(stmtPtr, true);
										return il2cpp_string_new(text.data());
									}
									else
									{
										replacement_queries_can_next.insert_or_assign(stmtPtr, false);
									}
								}
							}
							else if (stmt->getQuery().find("character_system_text") != string::npos)
							{
								int cueId, cueId1;
								string cueSheet, cueSheet1;
								if (stmt->getQuery().find("`voice_id`=?") != string::npos)
								{
									cueId = query_getint(_this, 2);
									cueId1 = stmt->getColumn(2).getInt();
									cueSheet = local::wide_u8(
										reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(_this, 1)->start_char
									);
									cueSheet1 = stmt->getColumn(1).getString();
								}
								else
								{
									cueId = query_getint(_this, 3);
									cueId1 = stmt->getColumn(3).getInt();
									cueSheet = local::wide_u8(
										reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(_this, 2)->start_char
									);
									cueSheet1 = stmt->getColumn(2).getString();
								}
								if (cueId == cueId1 && cueSheet == cueSheet1)
								{
									return il2cpp_string_new(text.data());
								}
							}
							else
							{
								return il2cpp_string_new(text.data());
							}
						}
					}
				}
			}
			catch (exception& e)
			{
			}
			return local::get_localized_string(result);
		}

		return result;
	}

	void* MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId_orig = nullptr;
	Il2CppObject* MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId_hook(Il2CppObject* _this, Il2CppObject* query, int characterId)
	{
		auto stmtField = il2cpp_class_get_field_from_name_wrap(query->klass, "_stmt");
		intptr_t* stmtPtr;
		il2cpp_field_get_value(query, stmtField, &stmtPtr);
		if (text_queries.contains(stmtPtr))
		{

			try
			{
				auto stmt = text_queries.at(stmtPtr);
				if (stmt)
				{
					if (replacement_queries_can_next.find(stmtPtr) == replacement_queries_can_next.end())
					{
						replacement_queries_can_next.emplace(stmtPtr, true);
					}
					if (replacement_queries_can_next.at(stmtPtr))
					{
						try
						{
							stmt->executeStep();
						}
						catch (exception& e)
						{
						}
					}
					if (stmt->hasRow())
					{
						int voiceId = query_getint(query, 0);
						int voiceId1 = stmt->getColumn(0).getInt();
						int cueId = query_getint(query, 3);
						int cueId1 = stmt->getColumn(3).getInt();
						string cueSheet = local::wide_u8(
							reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(query, 2)->start_char
						);
						string cueSheet1 = stmt->getColumn(2).getString();

						if (voiceId == voiceId1 && cueId == cueId1 && cueSheet == cueSheet1)
						{
							replacement_queries_can_next.insert_or_assign(stmtPtr, true);
						}
						else
						{
							replacement_queries_can_next.insert_or_assign(stmtPtr, false);
						}
					}
				}
			}
			catch (exception& e)
			{
			}
		}
		return reinterpret_cast<decltype(MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId_hook)*>(
			MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId_orig
			)(_this, query, characterId);
	}

	void* CySpringUpdater_set_SpringUpdateMode_orig = nullptr;
	void CySpringUpdater_set_SpringUpdateMode_hook(Il2CppObject* _this, int value)
	{
		reinterpret_cast<decltype(CySpringUpdater_set_SpringUpdateMode_hook)*>(CySpringUpdater_set_SpringUpdateMode_orig)(_this, g_cyspring_update_mode);
	}

	void* CySpringUpdater_get_SpringUpdateMode_orig = nullptr;
	int CySpringUpdater_get_SpringUpdateMode_hook(Il2CppObject* _this)
	{
		CySpringUpdater_set_SpringUpdateMode_hook(_this, g_cyspring_update_mode);
		return reinterpret_cast<decltype(CySpringUpdater_get_SpringUpdateMode_hook)*>(CySpringUpdater_get_SpringUpdateMode_orig)(_this);
	}

	void* story_timeline_controller_play_orig;
	void* story_timeline_controller_play_hook(Il2CppObject* _this)
	{
		FieldInfo* timelineDataField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_timelineData");

		Il2CppObject* timelineData;
		il2cpp_field_get_value(_this, timelineDataField, &timelineData);
		FieldInfo* StoryTimelineDataClass_TitleField = il2cpp_class_get_field_from_name_wrap(timelineData->klass, "Title");
		FieldInfo* StoryTimelineDataClass_BlockListField = il2cpp_class_get_field_from_name_wrap(timelineData->klass, "BlockList");

		Il2CppClass* story_timeline_text_clip_data_class = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineTextClipData");
		FieldInfo* nameField = il2cpp_class_get_field_from_name_wrap(story_timeline_text_clip_data_class, "Name");
		FieldInfo* textField = il2cpp_class_get_field_from_name_wrap(story_timeline_text_clip_data_class, "Text");
		FieldInfo* choiceDataListField = il2cpp_class_get_field_from_name_wrap(story_timeline_text_clip_data_class, "ChoiceDataList");
		FieldInfo* colorTextInfoListField = il2cpp_class_get_field_from_name_wrap(story_timeline_text_clip_data_class, "ColorTextInfoList");

		Il2CppString* title;
		il2cpp_field_get_value(timelineData, StoryTimelineDataClass_TitleField, &title);
		il2cpp_field_set_value(timelineData, StoryTimelineDataClass_TitleField, local::get_localized_string(title));

		Il2CppObject* blockList;
		il2cpp_field_get_value(timelineData, StoryTimelineDataClass_BlockListField, &blockList);

		Il2CppArraySize* blockArray;
		il2cpp_field_get_value(blockList, il2cpp_class_get_field_from_name_wrap(blockList->klass, "_items"), &blockArray);

		for (size_t i = 0; i < blockArray->max_length; i++)
		{
			Il2CppObject* blockData = reinterpret_cast<Il2CppObject*>(blockArray->vector[i]);
			if (!blockData) break;
			FieldInfo* StoryTimelineBlockDataClass_trackListField = il2cpp_class_get_field_from_name_wrap(blockData->klass, "_trackList");
			Il2CppObject* trackList;
			il2cpp_field_get_value(blockData, StoryTimelineBlockDataClass_trackListField, &trackList);

			Il2CppArraySize* trackArray;
			il2cpp_field_get_value(trackList, il2cpp_class_get_field_from_name_wrap(trackList->klass, "_items"), &trackArray);

			for (size_t i = 0; i < trackArray->max_length; i++)
			{
				Il2CppObject* trackData = reinterpret_cast<Il2CppObject*>(trackArray->vector[i]);
				if (!trackData) break;
				FieldInfo* StoryTimelineTrackDataClass_ClipListField = il2cpp_class_get_field_from_name_wrap(trackData->klass, "ClipList");
				Il2CppObject* clipList;
				il2cpp_field_get_value(trackData, StoryTimelineTrackDataClass_ClipListField, &clipList);


				Il2CppArraySize* clipArray;
				il2cpp_field_get_value(clipList, il2cpp_class_get_field_from_name_wrap(clipList->klass, "_items"), &clipArray);

				for (size_t i = 0; i < clipArray->max_length; i++)
				{
					Il2CppObject* clipData = reinterpret_cast<Il2CppObject*>(clipArray->vector[i]);
					if (!clipData) break;
					if (story_timeline_text_clip_data_class == clipData->klass)
					{
						Il2CppString* name;
						il2cpp_field_get_value(clipData, nameField, &name);
						il2cpp_field_set_value(clipData, nameField, local::get_localized_string(name));
						Il2CppString* text;
						il2cpp_field_get_value(clipData, textField, &text);
						il2cpp_field_set_value(clipData, textField, local::get_localized_string(text));
						Il2CppObject* choiceDataList;
						il2cpp_field_get_value(clipData, choiceDataListField, &choiceDataList);
						if (choiceDataList)
						{
							Il2CppArraySize* choiceDataArray;
							il2cpp_field_get_value(choiceDataList, il2cpp_class_get_field_from_name_wrap(choiceDataList->klass, "_items"), &choiceDataArray);

							for (size_t i = 0; i < choiceDataArray->max_length; i++)
							{
								Il2CppObject* choiceData = reinterpret_cast<Il2CppObject*>(choiceDataArray->vector[i]);
								if (!choiceData) break;
								FieldInfo* choiceDataTextField = il2cpp_class_get_field_from_name_wrap(choiceData->klass, "Text");

								Il2CppString* text;
								il2cpp_field_get_value(choiceData, choiceDataTextField, &text);
								il2cpp_field_set_value(choiceData, choiceDataTextField, local::get_localized_string(text));
							}
						}
						Il2CppObject* colorTextInfoList;
						il2cpp_field_get_value(clipData, colorTextInfoListField, &colorTextInfoList);
						if (colorTextInfoList)
						{
							Il2CppArraySize* colorTextInfoArray;
							il2cpp_field_get_value(colorTextInfoList, il2cpp_class_get_field_from_name_wrap(colorTextInfoList->klass, "_items"), &colorTextInfoArray);

							for (size_t i = 0; i < colorTextInfoArray->max_length; i++)
							{
								Il2CppObject* colorTextInfo = reinterpret_cast<Il2CppObject*>(colorTextInfoArray->vector[i]);
								if (!colorTextInfo) break;
								FieldInfo* colorTextInfoTextField = il2cpp_class_get_field_from_name_wrap(colorTextInfo->klass, "Text");

								Il2CppString* text;
								il2cpp_field_get_value(colorTextInfo, colorTextInfoTextField, &text);
								il2cpp_field_set_value(colorTextInfo, colorTextInfoTextField, local::get_localized_string(text));
							}
						}
					}

				}
			}
		}

		return reinterpret_cast<decltype(story_timeline_controller_play_hook)*>(story_timeline_controller_play_orig)(_this);
	}

	void* story_race_textasset_load_orig;
	void story_race_textasset_load_hook(Il2CppObject* _this)
	{
		FieldInfo* textDataField = { il2cpp_class_get_field_from_name_wrap(_this->klass, "textData") };
		Il2CppObject* textData;
		il2cpp_field_get_value(_this, textDataField, &textData);

		auto enumerator = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(textData->klass, "GetEnumerator", 0)->methodPointer(textData);
		auto get_current = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this)>(enumerator->klass, "get_Current", 0)->methodPointer;
		auto move_next = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject * _this)>(enumerator->klass, "MoveNext", 0)->methodPointer;

		while (move_next(enumerator))
		{
			auto key = get_current(enumerator);
			FieldInfo* textField = { il2cpp_class_get_field_from_name_wrap(key->klass, "text") };
			Il2CppString* text;
			il2cpp_field_get_value(key, textField, &text);
			il2cpp_field_set_value(key, textField, local::get_localized_string(text));
		}

		reinterpret_cast<decltype(story_race_textasset_load_hook)*>(story_race_textasset_load_orig)(_this);
	}

	bool useDefaultFPS = false;

	void* set_fps_orig = nullptr;
	void set_fps_hook(int value)
	{
		reinterpret_cast<decltype(set_fps_hook)*>(set_fps_orig)(useDefaultFPS ? value : g_max_fps);
	}

	bool (*is_virt)() = nullptr;

	Il2CppObject* (*display_get_main)();

	int (*get_system_width)(Il2CppObject* _this);

	int (*get_system_height)(Il2CppObject* _this);

	int (*get_rendering_width)(Il2CppObject* _this);

	int (*get_rendering_height)(Il2CppObject* _this);

	const float ratio_16_9 = 1.778f;
	const float ratio_9_16 = 0.563f;

	float ratio_vertical = 0.5625f;
	float ratio_horizontal = 1.7777778f;

	int last_display_width = 0, last_display_height = 0;
	int last_virt_window_width = 0, last_virt_window_height = 0;
	int last_hriz_window_width = 0, last_hriz_window_height = 0;

	bool fullScreenFl = false;
	bool fullScreenFlOverride = false;

	void (*get_resolution)(Resolution_t* buffer);

	void (*set_resolution)(int width, int height, bool fullscreen);

	void get_resolution_stub(Resolution_t* r)
	{
		get_resolution(r);

		int width = min(r->height, r->width) * g_aspect_ratio;
		if (r->width > r->height)
			r->width = width;
		else
			r->height = width;
	}

	void* get_virt_size_orig = nullptr;
	Vector3_t* get_virt_size_hook(Vector3_t* pVec3, int width, int height)
	{
		auto size = reinterpret_cast<decltype(get_virt_size_hook)*>(get_virt_size_orig)(pVec3, width, height);

		height = width * g_aspect_ratio;

		size->x = width;
		size->y = height;
		size->z = g_aspect_ratio;

		return size;
	}

	void* get_hori_size_orig = nullptr;
	Vector3_t* get_hori_size_hook(Vector3_t* pVec3, int width, int height)
	{
		auto size = reinterpret_cast<decltype(get_hori_size_hook)*>(get_hori_size_orig)(pVec3, width, height);

		width = height * g_aspect_ratio;

		size->x = width;
		size->y = height;
		size->z = g_aspect_ratio;

		return size;
	}

	void* gallop_get_screenheight_orig;
	int gallop_get_screenheight_hook()
	{
		if (!g_freeform_window)
		{
			int w = max(last_display_width, last_display_height), h = min(last_display_width, last_display_height);

			return is_virt() ? w : h;
		}

		return il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();
	}

	void* gallop_get_screenwidth_orig;
	int gallop_get_screenwidth_hook()
	{
		if (!g_freeform_window)
		{
			int w = max(last_display_width, last_display_height), h = min(last_display_width, last_display_height);

			return is_virt() ? h : w;
		}

		return il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
	}

	void (*set_scale_factor)(void*, float);

	void* canvas_scaler_setres_orig;
	void canvas_scaler_setres_hook(Il2CppObject* _this, Vector2_t res)
	{
		int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
		int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

		res.x = width;
		res.y = height;

		if (g_freeform_window)
		{
			if (width < height)
			{
				float scale = min(g_freeform_ui_scale_portrait, max(1, height * ratio_vertical) * g_freeform_ui_scale_portrait);
				set_scale_factor(_this, scale);
			}
			else
			{
				float scale = min(g_freeform_ui_scale_landscape, max(1, width / ratio_horizontal) * g_freeform_ui_scale_landscape);
				set_scale_factor(_this, scale);
			}
		}
		else
		{
			// set scale factor to make ui bigger on hi-res screen
			if (width < height)
			{
				float scale = min(g_ui_scale, max(1, height * ratio_vertical) * g_ui_scale);
				set_scale_factor(_this, scale);
			}
			else
			{
				float scale = min(g_ui_scale, max(1, width / ratio_horizontal) * g_ui_scale);
				set_scale_factor(_this, scale);
			}
		}

		reinterpret_cast<decltype(canvas_scaler_setres_hook)*>(canvas_scaler_setres_orig)(_this, res);
	}

	void* UIManager_UpdateCanvasScaler_orig = nullptr;

	void UIManager_UpdateCanvasScaler_hook(Il2CppObject* canvasScaler)
	{
		auto display = display_get_main();
		canvas_scaler_setres_hook(
			canvasScaler, Vector2_t{ static_cast<float>(get_system_width(display)), static_cast<float>(get_system_height(display)) });
		reinterpret_cast<decltype(UIManager_UpdateCanvasScaler_hook)*>(
			UIManager_UpdateCanvasScaler_orig)(canvasScaler);
	}

	void* BGManager_CalcBgScale_orig = nullptr;
	float BGManager_CalcBgScale_hook(Il2CppObject* _this, int width, int height, int renderTextureWidth, int renderTextureHeight)
	{
		int width1 = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
		int height1 = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();
		float ratio1 = (float)width1 / (float)height1;

		float bgCanvasScalerBaseScale = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*, int, int)>(_this->klass, "GetBgCanvasScalerBaseScale", 2)->methodPointer(_this, renderTextureWidth, renderTextureHeight);
		if (is_virt())
		{
			return max((float)renderTextureHeight / (float)height, (float)height / (float)renderTextureHeight) * (g_freeform_window ? 2 : 1) / bgCanvasScalerBaseScale;
		}
		return max((float)renderTextureWidth / (float)width, (float)width / (float)renderTextureWidth) * (g_freeform_window ? 2 : 1) / bgCanvasScalerBaseScale * ratio1;
	}

	void SetBGCanvasScalerSize()
	{
		auto bgManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "BGManager", "get_Instance", 0)();
		if (bgManager)
		{
			auto _mainBgField = il2cpp_class_get_field_from_name_wrap(bgManager->klass, "_mainBg");
			Il2CppObject* _mainBg;
			il2cpp_field_get_value(bgManager, _mainBgField, &_mainBg);

			if (_mainBg)
			{
				auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainBg->klass, "get_transform", 0)->methodPointer(_mainBg);

				int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
				int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

				if (width > height)
				{
					auto pos = il2cpp_class_get_method_from_name_type<Vector3_t(*)(Il2CppObject*)>(transform->klass, "get_localPosition", 0)->methodPointer(transform);

					if (pos.y == 0)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector3_t)>(transform->klass, "set_localPosition", 1)->methodPointer(transform, Vector3_t{ 0, 0, 0 });
					}
				}
			}

			auto _bgCanvasScalerField = il2cpp_class_get_field_from_name_wrap(bgManager->klass, "_bgCanvasScaler");
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
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			miniDirectors = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), 1, 0);
		}
		else
		{
			miniDirectors = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), true);
		}

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

							auto TextureResolution = il2cpp_class_get_method_from_name_type<Vector2Int_t(*)(Il2CppObject*)>(DirectorUI->klass, "get_TextureResolution", 0)->methodPointer(DirectorUI);

							auto _cameraField = il2cpp_class_get_field_from_name_wrap(cameraController->klass, "_camera");
							Il2CppObject* _camera;
							il2cpp_field_get_value(cameraController, _cameraField, &_camera);

							if (_camera)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2Int_t)>(cameraController->klass, "ResizeRenderTexture", 1)->methodPointer(cameraController, TextureResolution);

								auto _renderTextureField = il2cpp_class_get_field_from_name_wrap(cameraController->klass, "_renderTexture");
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

	Il2CppArraySize_t<Il2CppObject*>* (*UIManager_GetCanvasScalerList)(Il2CppObject* _this);

	void* UIManager_ChangeResizeUIForPC_orig = nullptr;
	void UIManager_ChangeResizeUIForPC_hook(Il2CppObject* _this, int width, int height)
	{
		// reinterpret_cast<decltype(UIManager_ChangeResizeUIForPC_hook)*>(UIManager_ChangeResizeUIForPC_orig)(_this, width, height);
		Il2CppArraySize_t<Il2CppObject*>* scalers;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			scalers = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), 1, 0);
		}
		else
		{
			scalers = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), true);
		}

		// auto scalers = UIManager_GetCanvasScalerList(_this);
		for (int i = 0; i < scalers->max_length; i++)
		{
			auto scaler = scalers->vector[i];
			if (scaler)
			{
				auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(scaler->klass, "get_gameObject", 0)->methodPointer(scaler);

				bool keepActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(gameObject->klass, "get_activeSelf", 0)->methodPointer(gameObject);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

				auto scaleMode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(scaler->klass, "get_uiScaleMode", 0)->methodPointer(scaler);

				if (g_freeform_window)
				{
					if (scaleMode == 1)
					{
						if (width < height)
						{
							float scale = min(g_freeform_ui_scale_portrait, max(1, height * ratio_vertical) * g_freeform_ui_scale_portrait);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, Vector2_t{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
						}
						else
						{
							float scale = min(g_freeform_ui_scale_landscape, max(1, width / ratio_horizontal) * g_freeform_ui_scale_landscape);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, Vector2_t{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
						}
					}

					if (scaleMode == 0)
					{
						if (width < height)
						{
							float scale = min(g_freeform_ui_scale_portrait, max(1, height * ratio_vertical) * g_freeform_ui_scale_portrait);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
						}
						else
						{
							float scale = min(g_freeform_ui_scale_landscape, max(1, width / ratio_horizontal) * g_freeform_ui_scale_landscape);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
						}
					}
				}
				else
				{
					if (scaleMode == 1)
					{
						if (width < height)
						{
							float scale = min(g_ui_scale, max(1, height * ratio_vertical) * g_ui_scale);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, Vector2_t{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
						}
						else
						{
							float scale = min(g_ui_scale, max(1, width / ratio_horizontal) * g_ui_scale);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, Vector2_t{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
						}
					}
					if (scaleMode == 0)
					{
						// set scale factor to make ui bigger on hi-res screen
						if (width < height)
						{
							float scale = min(g_ui_scale, max(1, height * ratio_vertical) * g_ui_scale);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
						}
						else
						{
							float scale = min(g_ui_scale, max(1, width / ratio_horizontal) * g_ui_scale);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
						}
					}
				}

				// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_uiScaleMode", 1)->methodPointer(scaler, 0);

				// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_screenMatchMode", 1)->methodPointer(scaler, 0);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, keepActive);
			}
		}

		if (g_unlock_size || g_freeform_window)
		{
			SetBGCanvasScalerSize();
		}
	}

	void* GetLimitSize_orig = nullptr;
	Vector2_t GetLimitSize_hook()
	{
		auto orig = reinterpret_cast<decltype(GetLimitSize_hook)*>(GetLimitSize_orig)();
		Resolution_t r;
		get_resolution(&r);
		orig.x = r.width;
		orig.y = r.height;
		return orig;
	}

	bool altEnterPressed = false;

	Il2CppObject* delayTweener;

	void RemakeTextures()
	{
		auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

		auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(graphicSettings->klass, "Update3DRenderTexture", 0)->methodPointer(graphicSettings);

		Il2CppArraySize_t<Il2CppObject*>* renders;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			renders = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "CutInImageEffectPostRender"), 1, 0);
		}
		else
		{
			renders = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "CutInImageEffectPostRender"), true);
		}

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
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* cuts;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			cuts = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LimitBreakCut"), 1, 0);
		}
		else
		{
			cuts = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LimitBreakCut"), true);
		}

		if (cuts)
		{
			for (int i = 0; i < cuts->max_length; i++)
			{
				auto obj = cuts->vector[i];

				if (obj)
				{
					auto _frameBufferField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_frameBuffer");
					Il2CppObject* _frameBuffer;
					il2cpp_field_get_value(obj, _frameBufferField, &_frameBuffer);

					if (_frameBuffer)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* raceEffect;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			raceEffect = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "RaceImageEffect"), 1, 0);
		}
		else
		{
			raceEffect = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "RaceImageEffect"), true);
		}

		if (raceEffect)
		{
			for (int i = 0; i < raceEffect->max_length; i++)
			{
				auto obj = raceEffect->vector[i];

				if (obj)
				{
					auto buffer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_FrameBuffer", 0)->methodPointer(obj);
					if (buffer)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* storyEffect;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			storyEffect = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "StoryImageEffect"), 1, 0);
		}
		else
		{
			storyEffect = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "StoryImageEffect"), true);
		}

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
		}

		Il2CppArraySize_t<Il2CppObject*>* lowResCameras;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			lowResCameras = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LowResolutionCameraBase"), 1, 0);
		}
		else
		{
			lowResCameras = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LowResolutionCameraBase"), true);
		}

		if (lowResCameras)
		{
			for (int i = 0; i < lowResCameras->max_length; i++)
			{
				auto obj = lowResCameras->vector[i];

				if (obj)
				{
					auto method = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(obj->klass, "UpdateDirection", 0);
					if (method)
					{
						method->methodPointer(obj);
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* liveTheaterCharaSelects;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			liveTheaterCharaSelects = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LiveTheaterCharaSelect"), 1, 0);
		}
		else
		{
			liveTheaterCharaSelects = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "LiveTheaterCharaSelect"), true);
		}

		if (liveTheaterCharaSelects)
		{
			for (int i = 0; i < liveTheaterCharaSelects->max_length; i++)
			{
				auto obj = liveTheaterCharaSelects->vector[i];

				if (obj)
				{
					auto _sceneField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_scene");
					Il2CppObject* _scene;
					il2cpp_field_get_value(obj, _sceneField, &_scene);

					if (_scene)
					{
						auto camera = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_scene->klass, "GetCamera", 0)->methodPointer(_scene);
						auto texture = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(camera->klass, "get_RenderTexture", 0)->methodPointer(camera);

						auto _formationAllField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_formationAll");
						Il2CppObject* _formationAll;
						il2cpp_field_get_value(obj, _formationAllField, &_formationAll);

						if (_formationAll)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_formationAll->klass, "SetRenderTex", 1)->methodPointer(_formationAll, texture);
						}

						auto _formationMainField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_formationMain");
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
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			miniDirectors = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), 1, 0);
		}
		else
		{
			miniDirectors = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), true);
		}

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
			delayTweener = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, callback, true);
		}

		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
		auto GetCurrentViewController = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "SceneManager", [](const MethodInfo* info)
			{
				return info->name == "GetCurrentViewController"s && info->methodPointer;
			});
		auto controller = GetCurrentViewController(sceneManager);

		if (controller)
		{
			if (controller->klass->name == "FanRaidViewController"s)
			{
				auto _fanRaidTopSequenceField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_fanRaidTopSequence");
				Il2CppObject* _fanRaidTopSequence;
				il2cpp_field_get_value(controller, _fanRaidTopSequenceField, &_fanRaidTopSequence);

				if (_fanRaidTopSequence)
				{
					auto _frameBufferField = il2cpp_class_get_field_from_name_wrap(_fanRaidTopSequence->klass, "_frameBuffer");
					Il2CppObject* _frameBuffer;
					il2cpp_field_get_value(_fanRaidTopSequence, _frameBufferField, &_frameBuffer);

					if (_frameBuffer)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
					}
				}
			}

			if (controller->klass->name == "GachaMainViewController"s)
			{
				auto _contextField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_context");
				Il2CppObject* _context;
				il2cpp_field_get_value(controller, _contextField, &_context);

				if (_context)
				{
					auto FrameBufferField = il2cpp_class_get_field_from_name_wrap(_context->klass, "FrameBuffer");
					Il2CppObject* FrameBuffer;
					il2cpp_field_get_value(_context, FrameBufferField, &FrameBuffer);

					if (FrameBuffer)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(FrameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(FrameBuffer);
					}
				}
			}

			if (controller->klass->name == "SingleModeSuccessionCutViewController"s ||
				controller->klass->name == "EpisodeMainUnlockRaceCutinViewController"s ||
				controller->klass->name == "SingleModeSuccessionEventViewController"s)
			{
				auto _resultField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_result");
				Il2CppObject* _result;
				il2cpp_field_get_value(controller, _resultField, &_result);

				if (_result)
				{
					auto _resultCameraField = il2cpp_class_get_field_from_name_wrap(_result->klass, "_resultCamera");
					Il2CppObject* _resultCamera;
					il2cpp_field_get_value(_result, _resultCameraField, &_resultCamera);

					if (_resultCamera)
					{
						auto texture = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(uiManager->klass, "get_UITexture", 0)->methodPointer(uiManager);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_resultCamera->klass, "set_targetTexture", 1)->methodPointer(_resultCamera, texture);
					}
				}
			}

			if (string(controller->klass->name).ends_with("PaddockViewController"))
			{
				auto _frameBufferField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_frameBuffer");
				Il2CppObject* _frameBuffer;
				il2cpp_field_get_value(controller, _frameBufferField, &_frameBuffer);

				if (_frameBuffer)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_frameBuffer->klass, "RemakeRenderTexture", 0)->methodPointer(_frameBuffer);
				}

			}
		}

		if (GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryManager")))
		{
			auto storySceneController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "StoryManager", "get_StorySceneController", -1)();
			if (storySceneController)
			{
				auto CurrentDisplayModeField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineController"), "CurrentDisplayMode");
				int mode;
				il2cpp_field_static_get_value(CurrentDisplayModeField, &mode);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(storySceneController->klass, "ChangeCameraDirection", 1)->methodPointer(storySceneController, mode);
			}
		}
	}

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* _this, Vector2_t dispRectWH, bool isPanScan);

	void ResizeMoviePlayer()
	{
		auto movieManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "get_Instance", -1)();

		if (movieManager)
		{
			auto playerDicField = il2cpp_class_get_field_from_name_wrap(movieManager->klass, "playerDic");
			Il2CppObject* playerDic;
			il2cpp_field_get_value(movieManager, playerDicField, &playerDic);

			if (playerDic)
			{
				auto entriesField = il2cpp_class_get_field_from_name_wrap(playerDic->klass, "entries");
				Il2CppArraySize_t<Entry<MoviePlayerHandle, Il2CppObject*>>* entries;
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

										if (uobject_get_name(parent)->start_char == L"MainCanvas"s)
										{
											auto array1 = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
												"umamusume.dll", "Gallop", "StoryMovieView")), true, true, false, false, nullptr);

											if (array1)
											{
												if (array1->max_length > 0)
												{
													auto fullPlayer = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryFullMoviePlayer"));
													auto _handleField = il2cpp_class_get_field_from_name_wrap(fullPlayer->klass, "_handle");
													il2cpp_field_set_value(fullPlayer, _handleField, &entry.key);

													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(fullPlayer->klass, "AdjustMovieSize", 1)->methodPointer(fullPlayer, is_virt() ? 0 : 1);

													return;
												}
											}

											auto array2 = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
												"umamusume.dll", "Gallop", "StoryView")), true, true, false, false, nullptr);

											if (array2)
											{
												if (array2->max_length > 0)
												{
													auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
													auto GetCurrentViewController = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "SceneManager", [](const MethodInfo* info)
														{
															return info->name == "GetCurrentViewController"s && info->methodPointer;
														});
													auto controller = GetCurrentViewController(sceneManager);

													auto _wipeControllerField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_wipeController");
													Il2CppObject* _wipeController;
													il2cpp_field_get_value(controller, _wipeControllerField, &_wipeController);

													if (_wipeController)
													{
														auto _moviePlayerField = il2cpp_class_get_field_from_name_wrap(_wipeController->klass, "_moviePlayer");
														Il2CppObject* _moviePlayer;
														il2cpp_field_get_value(_wipeController, _moviePlayerField, &_moviePlayer);

														if (_moviePlayer)
														{
															auto StoryTimelineController = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineController");
															auto CurrentDisplayModeField = il2cpp_class_get_field_from_name_wrap(StoryTimelineController->klass, "CurrentDisplayMode");
															int CurrentDisplayMode;
															il2cpp_field_static_get_value(CurrentDisplayModeField, &CurrentDisplayMode);

															if (CurrentDisplayMode == 3 && !is_virt())
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

											auto newSize = il2cpp_symbols::get_method_pointer<Vector2_t(*)()>("umamusume.dll", "Gallop", "MovieScreenSizeHelper", "GetMovieTargetCanvasSize", -1)();

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
														il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(&array->obj, j);
													if (!obj) continue;

													auto newSize = il2cpp_class_get_method_from_name_type<Vector2_t(*)(Il2CppObject*)>(obj->klass, "CalcMovieRectSize", 0)->methodPointer(obj);

													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(parent->klass, "set_sizeDelta", 1)->methodPointer(parent, newSize);

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

	void resizeWindow(HWND hWnd, int updateWidth, int updateHeight)
	{
		if (updateWidth < 72 || updateHeight < 72)
		{
			return;
		}

		auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

		auto lastWidthField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastWidth");
		auto lastHeightField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastHeight");

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

		auto _aspectRatioField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "_aspectRatio");
		float _aspectRatio = contentWidth / contentHeight;
		il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);

		il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
		il2cpp_class_get_method_from_name_type<void (*)()>(StandaloneWindowResize, "DisableWindowHitTest", -1)->methodPointer();

		bool isPortrait = contentWidth < contentHeight;

		il2cpp_symbols::get_method_pointer<void (*)(bool)>("umamusume.dll", "Gallop", "StandaloneWindowResize", "set_IsVirt", 1)(isPortrait);

		auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

		auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

		auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

		auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenWidth");

		auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenHeight");

		auto SCREEN_ORIENTATION_CATEGORIES_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "SCREEN_ORIENTATION_CATEGORIES");
		Il2CppObject* SCREEN_ORIENTATION_CATEGORIES;
		il2cpp_field_static_get_value(SCREEN_ORIENTATION_CATEGORIES_Field, &SCREEN_ORIENTATION_CATEGORIES);

		if (lastWidth < lastHeight)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 1, 1);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 2, 1);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 3, 1);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 4, 1);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 5, 1);
		}
		else
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 1, 3);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 2, 3);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 3, 3);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 4, 3);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 5, 3);
		}

		int unityWidth = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
		int unityHeight = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

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

		auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

		if (uiManager)
		{
			//auto loadingCanvas = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(uiManager->klass, "get_LoadingCanvas", -1)->methodPointer();
			//if (loadingCanvas)
			//{
			//	auto canvas = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadingCanvas->klass, "get_Canvas", 0)->methodPointer(loadingCanvas);

			//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(canvas->klass, "set_sortingLayerName", 1)->methodPointer(canvas, il2cpp_string_new("Default"));
			//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvas->klass, "set_sortingOrder", 1)->methodPointer(canvas, 0);
			//	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(canvas->klass, "set_overrideSorting", 1)->methodPointer(canvas, true);
			//}

			//auto nowLoading = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "NowLoading", "get_Instance", -1)();
			//if (nowLoading)
			//{
			//	auto _bgImageField = il2cpp_class_get_field_from_name_wrap(nowLoading->klass, "_bgImage");
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

			//	auto _backCanvasGroupField = il2cpp_class_get_field_from_name_wrap(nowLoading->klass, "_backCanvasGroup");
			//	Il2CppObject* _backCanvasGroup;
			//	il2cpp_field_get_value(nowLoading, _backCanvasGroupField, &_backCanvasGroup);

			//	if (_backCanvasGroup)
			//	{
			//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_backCanvasGroup->klass, "set_alpha", 1)->methodPointer(_backCanvasGroup, 1);
			//	}

			//	auto ids = il2cpp_symbols::get_method_pointer<Il2CppArraySize * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "GetSortingLayerIDsInternal", -1)();
			//	for (int i = 0; i < ids->max_length; i++)
			//	{
			//		auto id = reinterpret_cast<int>(ids->vector[i]);
			//		cout << "Layer Id: " << id << endl;
			//		cout << "Layer name: " << local::wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "IDToName", 1)(id)->start_char) << endl;
			//	}

			//	auto _activeHorseShoeParticleField = il2cpp_class_get_field_from_name_wrap(nowLoading->klass, isPortrait ? "_horseShoeParticleVertical" : "_horseShoeParticleHorizontal");
			//	Il2CppObject* _activeHorseShoeParticle;
			//	il2cpp_field_get_value(nowLoading, _activeHorseShoeParticleField, &_activeHorseShoeParticle);

			//	if (_activeHorseShoeParticle)
			//	{
			//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_activeHorseShoeParticle->klass, "SetActive", 1)->methodPointer(_activeHorseShoeParticle, true);
			//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_activeHorseShoeParticle->klass, "SetParticleAlpha", 1)->methodPointer(_activeHorseShoeParticle, 1);
			//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, int)>(_activeHorseShoeParticle->klass, "SetLayer", 2)->methodPointer(_activeHorseShoeParticle, il2cpp_string_new("Default"), 0);

			//		auto _particleRendererField = il2cpp_class_get_field_from_name_wrap(_activeHorseShoeParticle->klass, "_particleRenderer");
			//		Il2CppObject* _particleRenderer;
			//		il2cpp_field_get_value(_activeHorseShoeParticle, _particleRendererField, &_particleRenderer);

			//		if (_particleRenderer)
			//		{
			//			reinterpret_cast<void (*)(Il2CppObject*, int)>(il2cpp_resolve_icall("UnityEngine.Renderer::set_rendererPriority(System.Int32)"))(_particleRenderer, 0);
			//		}
			//	}

			//	auto _activeHorseShoeParticle1Field = il2cpp_class_get_field_from_name_wrap(nowLoading->klass, isPortrait ? "_horseShoeParticleHorizontal" : "_horseShoeParticleVertical");
			//	Il2CppObject* _activeHorseShoeParticle1;
			//	il2cpp_field_get_value(nowLoading, _activeHorseShoeParticle1Field, &_activeHorseShoeParticle1);

			//	if (_activeHorseShoeParticle1)
			//	{
			//		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_activeHorseShoeParticle1->klass, "SetActive", 1)->methodPointer(_activeHorseShoeParticle1, false);
			//	}
			//}

			/*auto _bgCameraSettingsField = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_bgCameraSettings");

			Il2CppObject* _bgCameraSettings;
			il2cpp_symbols::get_method_pointer<void (*)(bool, Il2CppObject**)>("umamusume.dll", "Gallop", "Screen", "InitializeChangeScaleForPC", 2)(isPortrait, &_bgCameraSettings);

			il2cpp_field_static_set_value(_bgCameraSettingsField, &_bgCameraSettings);*/
			/*auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgCamera");
			Il2CppObject* _bgCamera;

			il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
				il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", -1)());*/

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Disable", 0)->methodPointer(tapEffectController);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation)>(uiManager->klass, "SetCameraSizeByOrientation", 1)->methodPointer(uiManager, ScreenOrientation::Portrait);
		}

		auto anRootManager = GetSingletonInstance(il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", "AnRootManager"));

		if (anRootManager)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(anRootManager->klass, "set_ScreenRate", 1)->methodPointer(anRootManager, _aspectRatio);
		}

		// il2cpp_class_get_method_from_name_type<void (*)()>(GallopScreen, "UpdateForPC", -1)->methodPointer();

		if (uiManager)
		{
			// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(uiManager->klass, "ChangeResizeUIForPC", 2)->methodPointer(uiManager, lastWidth, lastHeight);

			// AutoRotation
			// il2cpp_symbols::get_method_pointer<void (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "set_orientation", 1)(5);

			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(uiManager->klass, "get_gameObject", 0)->methodPointer(uiManager);

			auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gameObject->klass, "get_transform", 0)->methodPointer(gameObject);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector3_t)>(transform->klass, "set_localScale", 1)->methodPointer(transform, Vector3_t{ 1, 1, 1 });

			// auto _bgCameraSettingsField = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_bgCameraSettings");

			// Il2CppObject* _bgCameraSettings;
			// il2cpp_field_static_get_value(_bgCameraSettingsField, &_bgCameraSettings);

			// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject**)>(uiManager->klass, "EndOrientation", 1)->methodPointer(uiManager, &_bgCameraSettings);

			// il2cpp_field_static_set_value(_bgCameraSettingsField, &_bgCameraSettings);

			if (tapEffectController)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);
			}

			Il2CppArraySize_t<Il2CppObject*>* canvasScalerList;
			if (Game::CurrentGameRegion == Game::Region::KOR)
			{
				canvasScalerList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
					GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), 1, 0);
			}
			else
			{
				canvasScalerList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
					GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), true);
			}
			// auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(uiManager->klass, "GetCanvasScalerList", 0)->methodPointer(uiManager);

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
						float scale = min(g_freeform_ui_scale_portrait, max(1, contentHeight * ratio_vertical) * g_freeform_ui_scale_portrait);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, Vector2_t{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
					}
					else
					{
						float scale = min(g_freeform_ui_scale_landscape, max(1, contentWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, Vector2_t{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
					}*/

					// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvasScaler->klass, "set_uiScaleMode", 1)->methodPointer(canvasScaler, 0);

					// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(canvasScaler->klass, "set_screenMatchMode", 1)->methodPointer(canvasScaler, 0);

					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, keepActive);

					auto scaleMode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(canvasScaler->klass, "get_uiScaleMode", 0)->methodPointer(canvasScaler);

					if (scaleMode == 1)
					{
						if (isPortrait)
						{
							float scale = min(g_freeform_ui_scale_portrait, max(1, contentHeight * ratio_vertical) * g_freeform_ui_scale_portrait);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, Vector2_t{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
						}
						else
						{
							float scale = min(g_freeform_ui_scale_landscape, max(1, contentWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, Vector2_t{ static_cast<float>(contentWidth / scale), static_cast<float>(contentHeight / scale) });
						}
					}

					if (scaleMode == 0)
					{
						if (isPortrait)
						{
							float scale = min(g_freeform_ui_scale_portrait, max(1, contentHeight * ratio_vertical) * g_freeform_ui_scale_portrait);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
						}
						else
						{
							float scale = min(g_freeform_ui_scale_landscape, max(1, contentWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
						}
					}
				}
			}

			SetBGCanvasScalerSize();
		}

		if (uiManager)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "AdjustSafeArea", 0)->methodPointer(uiManager);
			auto _bgManagerField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgManager");
			Il2CppObject* _bgManager;
			il2cpp_field_get_value(uiManager, _bgManagerField, &_bgManager);
			if (_bgManager)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_bgManager->klass, "OnChangeResolutionByGraphicsSettings", 0)->methodPointer(_bgManager);
			}

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "CheckUIToFrameBufferBlitInstance", 0)->methodPointer(uiManager);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "ReleaseRenderTexture", 0)->methodPointer(uiManager);


			auto renderTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture"));
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int, int)>(renderTexture->klass, ".ctor", 3)->methodPointer(renderTexture, contentWidth, contentHeight, 24);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderTexture->klass, "set_autoGenerateMips", 1)->methodPointer(renderTexture, false);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderTexture->klass, "set_useMipMap", 1)->methodPointer(renderTexture, false);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderTexture->klass, "set_antiAliasing", 1)->methodPointer(renderTexture, 1);

			auto _uiTextureField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_uiTexture");
			il2cpp_field_set_value(uiManager, _uiTextureField, renderTexture);

			if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(renderTexture->klass, "Create", 0)->methodPointer(renderTexture))
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "ReleaseRenderTexture", 0)->methodPointer(uiManager);
			}

			auto _uiCommandBufferField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_uiCommandBuffer");
			Il2CppObject* _uiCommandBuffer;
			il2cpp_field_get_value(uiManager, _uiCommandBufferField, &_uiCommandBuffer);

			auto _blitToFrameMaterialField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_blitToFrameMaterial");
			Il2CppObject* _blitToFrameMaterial;
			il2cpp_field_get_value(uiManager, _blitToFrameMaterialField, &_blitToFrameMaterial);

			auto _uiCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_uiCamera");
			Il2CppObject* _uiCamera;
			il2cpp_field_get_value(uiManager, _uiCameraField, &_uiCamera);

			auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgCamera");
			Il2CppObject* _bgCamera;
			il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

			auto _noImageEffectUICameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_noImageEffectUICamera");
			Il2CppObject* _noImageEffectUICamera;
			il2cpp_field_get_value(uiManager, _noImageEffectUICameraField, &_noImageEffectUICamera);

			auto _uiToFrameBufferBlitCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_uiToFrameBufferBlitCamera");
			Il2CppObject* _uiToFrameBufferBlitCamera;
			il2cpp_field_get_value(uiManager, _uiToFrameBufferBlitCameraField, &_uiToFrameBufferBlitCamera);

			if (_uiCommandBuffer)
			{
				auto dest = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppClass*, int)>("UnityEngine.CoreModule.dll", "UnityEngine.Rendering", "RenderTargetIdentifier", "op_Implicit", 1)(
					il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Rendering", "RenderTargetIdentifier"), 1);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, Il2CppObject*)>(_uiCommandBuffer->klass, "Blit", 3)->methodPointer(_uiCommandBuffer, renderTexture, dest, _blitToFrameMaterial);
			}
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

		auto raceCameraManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "RaceCameraManager"));
		if (raceCameraManager)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(raceCameraManager->klass, "SetupOrientation", 1)->methodPointer(raceCameraManager, isPortrait ? 7 : 6);
		}

		auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
		if (director)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(director->klass, "SetupOrientation", 1)->methodPointer(director, isPortrait ? 2 : 1);

			auto ChampionsTextControllerField = il2cpp_class_get_field_from_name_wrap(director->klass, "ChampionsTextController");
			Il2CppObject* ChampionsTextController;
			il2cpp_field_get_value(director, ChampionsTextControllerField, &ChampionsTextController);

			if (ChampionsTextController)
			{
				auto _flashPlayerField = il2cpp_class_get_field_from_name_wrap(ChampionsTextController->klass, "_flashPlayer");
				Il2CppObject* _flashPlayer;
				il2cpp_field_get_value(ChampionsTextController, _flashPlayerField, &_flashPlayer);

				auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

				if (Game::CurrentGameRegion == Game::Region::KOR)
				{
					if (contentWidth < contentHeight)
					{
						float scale = min(g_freeform_ui_scale_portrait, max(1, contentHeight * ratio_vertical) * g_freeform_ui_scale_portrait);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(contentHeight / scale), static_cast<float>(contentHeight / scale) });
					}
					else
					{
						float scale = min(g_freeform_ui_scale_landscape, max(1, contentWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(contentHeight / scale), static_cast<float>(contentHeight / scale) });
					}
				}
				else
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(contentHeight), static_cast<float>(contentHeight) });
				}
			}
		}

		if (tapEffectController)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "RefreshAll", 0)->methodPointer(tapEffectController);
		}

		if (uiManager)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "AdjustMissionClearContentsRootRect", 0)->methodPointer(uiManager);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(uiManager->klass, "AdjustSafeAreaToAnnounceRect", 0)->methodPointer(uiManager);

			auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgCamera");
			Il2CppObject* _bgCamera;
			il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

			if (_bgCamera)
			{
				/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
					il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", -1)());*/
			}
		}

		if (isPortrait)
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

		il2cpp_class_get_method_from_name_type<void (*)()>(StandaloneWindowResize, "EnableWindowHitTest", -1)->methodPointer();
		// il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(false);

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

		auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
		if (uiManager)
		{
			auto _noImageEffectCanvasField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_noImageEffectCanvas");
			if (_noImageEffectCanvasField)
			{
				Il2CppObject* _noImageEffectCanvas;
				il2cpp_field_get_value(uiManager, _noImageEffectCanvasField, &_noImageEffectCanvas);

				if (_noImageEffectCanvas)
				{
					auto camera = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_noImageEffectCanvas->klass, "get_worldCamera", 0)->methodPointer(_noImageEffectCanvas);
					if (camera)
					{
						return IsRunChangeView || IsRunChangeScene || il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(camera->klass, "get_enabled", 0)->methodPointer(camera);
					}
				}
			}
		}

		return false;
	}

	void* Camera_set_orthographicSize_orig = nullptr;
	void Camera_set_orthographicSize_hook(Il2CppObject* _this, float value)
	{
		if (wstring(uobject_get_name(_this)->start_char).find(L"UICamera") != wstring::npos)
		{
			auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), _this, *([](Il2CppObject* _this)
				{
					RemakeTextures();
				}));

			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.1, callback, true);

			auto current = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(_this->klass, "get_orthographicSize", 0)->methodPointer(_this);
			if (current == 5.0f)
			{
				return;
			}
			value = 5.0f;
		}
		reinterpret_cast<decltype(Camera_set_orthographicSize_hook)*>(Camera_set_orthographicSize_orig)(_this, value);
	}

	void* RectTransform_get_rect_Injected_orig = nullptr;
	void RectTransform_get_rect_Injected_hook(Il2CppObject* _this, Rect_t* rect)
	{
		reinterpret_cast<decltype(RectTransform_get_rect_Injected_hook)*>(RectTransform_get_rect_Injected_orig)(_this, rect);

		if (isRequestChangeResolution && il2cpp_symbols::get_method_pointer<bool (*)()>("umamusume.dll", "Gallop", "GameSystem", "IsExecutingSoftwareReset", -1)())
		{
			float tmp = rect->height;
			rect->height = rect->width;
			rect->width = tmp;
			return;
		}


		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
		if (sceneManager)
		{
			auto _changeViewEnumeratorField = il2cpp_class_get_field_from_name_wrap(sceneManager->klass, "_changeViewEnumerator");
			Il2CppObject* _changeViewEnumerator;
			il2cpp_field_get_value(sceneManager, _changeViewEnumeratorField, &_changeViewEnumerator);

			if (_changeViewEnumerator)
			{
				auto current = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_changeViewEnumerator->klass, "System.Collections.IEnumerator.get_Current", 0)->methodPointer(_changeViewEnumerator);

				if (current && string(current->klass->name).find("ChangeScreenOrientation") != string::npos)
				{
					auto scaler = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(_this->klass, "GetComponent", 1)->methodPointer(_this, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"));

					if (scaler)
					{
						if (isRequestChangeResolution && IsChangingResolution())
						{
							float tmp = rect->height;
							rect->height = rect->width;
							rect->width = tmp;
						}
						else
						{
							isRequestChangeResolution = false;
						}
					}
				}
			}
		}
	}

	void* set_resolution_orig;
	void set_resolution_hook(int width, int height, int fullscreenMode, int perferredRefreshRate)
	{
		if (width < 72)
		{
			if (is_virt())
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
			if (is_virt())
			{
				height = 128;
			}
			else
			{
				height = 72;
			}
		}

		if (g_freeform_window)
		{
			auto hWnd = GetHWND();

			long style = GetWindowLongW(hWnd, GWL_STYLE);
			style |= WS_MAXIMIZEBOX;
			SetWindowLongPtrW(hWnd, GWL_STYLE, style);

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

			isRequestChangeResolution = true;

			/*if (width < height)
			{
				auto display = display_get_main();
				if (g_initial_width < g_initial_height)
				{
					ratio_vertical = static_cast<float>(g_initial_width) / static_cast<float>(g_initial_height);
					last_virt_window_height = get_system_width(display) - 400;
					last_virt_window_width = last_virt_window_height * ratio_vertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;
					}
				}
				else
				{
					ratio_vertical = static_cast<float>(g_initial_height) / static_cast<float>(g_initial_width);
					last_virt_window_height = get_system_height(display) - 400;
					last_virt_window_width = last_virt_window_height * ratio_vertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;
					}
				}

				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
				return;
			}

			reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(width, height, 3, perferredRefreshRate);*/
			return;
		}

		Resolution_t r;
		get_resolution(&r);

		bool reqVirt = width < height;

		bool unlockSize = g_unlock_size || g_freeform_window;

		if (is_virt() && fullScreenFl)
		{
			fullScreenFl = false;
			fullScreenFlOverride = false;
			if (unlockSize)
			{
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
			}
			else
			{
				r.height *= 0.825f;
				r.width = r.height * ratio_vertical;
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(r.width, r.height, 3, perferredRefreshRate);
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
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
			}
			else
			{
				r.height *= 0.825f;
				r.width = r.height * ratio_vertical;
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(r.width, r.height, 3, perferredRefreshRate);
			}
			return;
		}

		bool need_fullscreen = false;

		if (g_auto_fullscreen)
		{
			auto ratio = static_cast<float>(r.width) / static_cast<float>(r.height);
			ratio *= 1000;
			ratio = roundf(ratio) / 1000;

			if (is_virt() && ratio == ratio_9_16)
			{
				need_fullscreen = true;
			}
			else if (!is_virt() && ratio == ratio_16_9)
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
					last_hriz_window_height = last_hriz_window_width * ratio_horizontal;
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

			auto round_ratio_vertical = ratio_vertical * 1000;
			round_ratio_vertical = roundf(round_ratio_vertical) / 1000;

			auto round_ratio_horizontal = ratio_horizontal * 1000;
			round_ratio_horizontal = roundf(round_ratio_horizontal) / 1000;

			if (reqVirt && ratio != round_ratio_vertical)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				if (last_virt_window_width > last_virt_window_height)
				{
					auto display = display_get_main();
					if (g_initial_width < g_initial_height)
					{
						last_virt_window_height = get_system_width(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;

						if (last_virt_window_height >= get_system_height(display))
						{
							last_virt_window_height = get_system_height(display) - 400;
							last_virt_window_width = last_virt_window_height * ratio_vertical;
						}
					}
					else
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;

						if (last_virt_window_height >= get_system_height(display))
						{
							last_virt_window_height = get_system_height(display) - 400;
							last_virt_window_width = last_virt_window_height * ratio_vertical;
						}
					}
				}

				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
				return;

			}

			if (!reqVirt && ratio != round_ratio_horizontal)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				if (last_hriz_window_width < last_hriz_window_height)
				{
					auto display = display_get_main();
					if (g_initial_width < g_initial_height)
					{
						last_hriz_window_width = get_system_height(display) - 400;
						last_hriz_window_height = last_hriz_window_width / ratio_horizontal;

						if (last_hriz_window_height >= get_system_height(display))
						{
							last_hriz_window_height = get_system_height(display) - 400;
							last_hriz_window_width = last_hriz_window_height * ratio_horizontal;
						}
					}
					else
					{
						last_hriz_window_width = get_system_width(display) - 400;
						last_hriz_window_height = last_hriz_window_width / ratio_horizontal;

						if (last_hriz_window_height >= get_system_height(display))
						{
							last_hriz_window_height = get_system_height(display) - 400;
							last_hriz_window_width = last_hriz_window_height * ratio_horizontal;
						}
					}
				}

				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_hriz_window_width, last_hriz_window_height, 3, perferredRefreshRate);
				return;
			}
		}

		reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(
			fullScreenFl ? r.width : width, fullScreenFl ? r.height : height, fullScreenFl ? 1 : 3, perferredRefreshRate);
	}

	static bool isExitOpened = false;

	void ShowExitDialog()
	{
		if (isExitOpened)
		{
			return;
		}

		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop",
				"DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
			Il2CppString * headerTextArg,
			Il2CppString * message,
			Il2CppDelegate * onRight,
			unsigned long leftTextId,
			unsigned long rightTextId,
			Il2CppDelegate * onLeft,
			int dialogFormType)>(
				il2cpp_class_get_method_from_name(dialogData->klass,
					"SetSimpleTwoButtonMessage",
					7)->methodPointer
				)(dialogData,
					localizeextension_text_hook(GetTextIdByName("Title0040")),
					localizeextension_text_hook(GetTextIdByName("Title0041")),
					CreateDelegate(dialogData, *[](Il2CppObject* data)
						{
							isExitOpened = false;
							ExitProcess(0);
						}),
					GetTextIdByName("Common0004"),
					GetTextIdByName("Common0003"),
					CreateDelegate(dialogData, *[](Il2CppObject* data)
						{
							isExitOpened = false;
						}),
					2);

		isExitOpened = true;

		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*, bool)>(
			"umamusume.dll", "Gallop", "DialogManager", "PushSystemDialog", 2)(
				dialogData, true);
	}

	bool isNumKeyDown = false;

	void StepTrainingItem(WPARAM wParam)
	{
		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));

		if (!sceneManager)
		{
			return;
		}

		auto GetCurrentViewController = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "SceneManager", [](const MethodInfo* info)
			{
				return info->name == "GetCurrentViewController"s && info->methodPointer;
			});
		auto controller = GetCurrentViewController(sceneManager);

		if (controller)
		{
			if (controller->klass->name == "SingleModeMainViewController"s)
			{
				auto trainingController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_TrainingController", 0)->methodPointer(controller);
				if (il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(trainingController->klass, "get_IsInTraining", 0)->methodPointer(trainingController))
				{
					auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "GetViewBase", 0)->methodPointer(controller);
					if (view)
					{
						auto footer = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_TrainingFooter", 0)->methodPointer(view);
						if (footer)
						{
							auto _itemsField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_items");
							Il2CppObject* _items;
							il2cpp_field_get_value(footer, _itemsField, &_items);

							int count = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(_items->klass, "get_Count", 0)->methodPointer(_items);

							FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(_items->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* arr;
							il2cpp_field_get_value(_items, itemsField, &arr);

							auto selectedItem = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footer->klass, "get_SelectItem", 0)->methodPointer(footer);
							auto selectedMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footer->klass, "get_SelectedMenu", 0)->methodPointer(footer);

							if (wParam == VK_RETURN)
							{
								auto _onClickEnableField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_onClickEnable");
								bool _onClickEnable = true;
								il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>(footer->klass, "OnClickItem", 2)->methodPointer(footer, selectedItem, selectedMenu);
							}
							else if ((0 < (wParam - 48) && (wParam - 48) <= 5))
							{
								if (isNumKeyDown)
								{
									return;
								}

								int number = wParam - 48;

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_preSelectedMenu");
								Il2CppObject* footerItem = arr->vector[number - 1];

								auto trainingMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footerItem->klass, "get_TrainingMenu", 0)->methodPointer(footerItem);
								il2cpp_field_set_value(footer, _preSelectedMenuField, trainingMenu);

								isNumKeyDown = true;

								if (selectedMenu == trainingMenu)
								{
									auto _onClickEnableField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_onClickEnable");
									bool _onClickEnable = true;
									il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>(footer->klass, "OnClickItem", 2)->methodPointer(footer, selectedItem, selectedMenu);
									return;
								}

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, bool, bool)>(footer->klass, "Select", 4)->methodPointer(footer, footerItem, trainingMenu, false, false);
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

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_preSelectedMenu");
								Il2CppObject* footerItem = arr->vector[selectedIndex];

								auto trainingMenu = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(footerItem->klass, "get_TrainingMenu", 0)->methodPointer(footerItem);
								il2cpp_field_set_value(footer, _preSelectedMenuField, trainingMenu);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, bool, bool)>(footer->klass, "Select", 4)->methodPointer(footer, footerItem, trainingMenu, false, false);
							}
						}
					}
				}
			}
		}
	}

	bool isPortraitBeforeFullscreen = false;

	bool isWndProcInitRequired = true;

	void* wndproc_orig = nullptr;
	LRESULT wndproc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_XBUTTONDOWN && GET_KEYSTATE_WPARAM(wParam) == MK_XBUTTON1)
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
			return TRUE;
		}

		if (uMsg == WM_KEYDOWN)
		{
			if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_RETURN || (0 < (wParam - 48) && (wParam - 48) <= 5))
			{
				StepTrainingItem(wParam);
				return TRUE;
			}
		}

		if (uMsg == WM_KEYUP)
		{
			if ((0 < (wParam - 48) && (wParam - 48) <= 5))
			{
				isNumKeyDown = false;
				return TRUE;
			}
		}

		if (uMsg == WM_SYSKEYDOWN)
		{

			bool altDown = (lParam & (static_cast<long long>(1) << 29)) != 0;
			if ((g_auto_fullscreen || g_unlock_size || g_freeform_window) &&
				wParam == VK_RETURN &&
				altDown &&
				!altEnterPressed)
			{
				altEnterPressed = true;

				Resolution_t r;
				get_resolution(&r);

				auto display = display_get_main();

				auto rendering_ratio = static_cast<float>(get_rendering_width(display)) / static_cast<float>(get_rendering_height(display));
				rendering_ratio *= 1000;
				rendering_ratio = roundf(rendering_ratio) / 1000;

				auto system_ratio = static_cast<float>(get_system_width(display)) / static_cast<float>(get_system_height(display));
				system_ratio *= 1000;
				system_ratio = roundf(system_ratio) / 1000;

				if ((!is_virt() && rendering_ratio == system_ratio) ||
					g_freeform_window)
				{
					if (!fullScreenFlOverride)
					{
						fullScreenFlOverride = true;
					}
					fullScreenFl = !fullScreenFl;
					if (g_unlock_size || g_freeform_window)
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
						r.height = r.width / ratio_horizontal;
					}

					reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(r.width, r.height, fullScreenFl ? 1 : 3, 0);
				}
				return TRUE;

			}
			if (g_max_fps > -1 && wParam == 'F' && altDown)
			{
				useDefaultFPS = !useDefaultFPS;
				set_fps_hook(30);
				return TRUE;
			}
		}

		if (g_cyspring_update_mode != -1)
		{
			bool altDown = (lParam & (static_cast<long long>(1) << 29)) != 0;
			if (altDown)
			{
				switch (wParam)
				{
				case '1':
					g_cyspring_update_mode = 0;
					break;
				case '2':
					g_cyspring_update_mode = 1;
					break;
				case '3':
					g_cyspring_update_mode = 2;
					break;
				case '4':
					g_cyspring_update_mode = 3;
					break;
				}
			}
		}

		if (uMsg == WM_SYSCOMMAND && !g_freeform_window)
		{
			if (wParam == SC_MAXIMIZE)
			{
				il2cpp_symbols::get_method_pointer<void (*)()>("umamusume.dll", "Gallop", "StandaloneWindowResize", "DisableMaximizebox", -1)();
				return TRUE;
			}
		}


		if (g_auto_fullscreen || g_unlock_size || g_freeform_window)
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
			if (!il2cpp_class_get_method_from_name_type<bool (*)()>(StandaloneWindowResize, "get_IsEnableWindowHitTest", -1)->methodPointer())
			{
				return FALSE;
			}
		}

		if (uMsg == WM_ENTERSIZEMOVE)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowSizeChangingField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize->klass, "_isWindowSizeChanging");
			bool _isWindowSizeChanging = true;
			il2cpp_field_static_set_value(_isWindowSizeChangingField, &_isWindowSizeChanging);
		}

		if (uMsg == WM_MOVING)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowDraggingField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize->klass, "_isWindowDragging");
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

				auto StartCoroutine = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "MonoBehaviour", [](const MethodInfo* method)
					{
						return method->name == "StartCoroutine"s && method->parameters[0].parameter_type->type == IL2CPP_TYPE_CLASS;
					});
				auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
				if (uiManager)
				{
					StartCoroutine(uiManager, il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(StandaloneWindowResize->klass, "ClearStopFlagAfterWhile", 0)->methodPointer());
				}
				break;
			}
			case SIZE_MINIMIZED:
			{

				auto _isWindowDraggingField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize->klass, "_isWindowDragging");
				bool _isWindowDragging = true;
				il2cpp_field_static_set_value(_isWindowDraggingField, &_isWindowDragging);
				break;
			}
			}
		}

		if (uMsg == WM_EXITSIZEMOVE)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto _isWindowDraggingField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize->klass, "_isWindowDragging");
			auto _isWindowSizeChangingField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize->klass, "_isWindowSizeChanging");
			bool _isWindowSizeChanging = false;
			il2cpp_field_static_set_value(_isWindowDraggingField, &_isWindowSizeChanging);
			il2cpp_field_static_set_value(_isWindowSizeChangingField, &_isWindowSizeChanging);

			if (g_freeform_window)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;
				resizeWindow(hWnd, windowWidth, windowHeight);
			}
		}

		if (uMsg == WM_SIZE && g_freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			auto lastWidthField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastHeight");

			int lastWidth, lastHeight;

			il2cpp_field_static_get_value(lastWidthField, &lastWidth);
			il2cpp_field_static_get_value(lastHeightField, &lastHeight);

			if (lastWidth != LOWORD(lParam) || lastHeight != HIWORD(lParam))
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;
				resizeWindow(hWnd, windowWidth, windowHeight);


				WNDPROC oldWndProcPtr = nullptr;

				auto oldWndProcPtrField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "oldWndProcPtr");
				il2cpp_field_static_get_value(oldWndProcPtrField, &oldWndProcPtr);


				if (oldWndProcPtr)
				{
					return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
				}

				return TRUE;
			}
		}

		if (uMsg == WM_SIZING && g_freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			auto lastWidthField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastHeight");

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

				int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
				int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

				il2cpp_class_get_method_from_name_type<void (*)(float, float)>(StandaloneWindowResize, "SaveChangedWidth", 2)->methodPointer(width, height);

				auto _aspectRatioField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "_aspectRatio");
				float _aspectRatio = contentWidth / contentHeight;
				il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);
			}

			WNDPROC oldWndProcPtr = nullptr;

			auto oldWndProcPtrField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "oldWndProcPtr");
			il2cpp_field_static_get_value(oldWndProcPtrField, &oldWndProcPtr);


			if (oldWndProcPtr)
			{
				return CallWindowProcW(oldWndProcPtr, hWnd, uMsg, wParam, lParam);
			}

			return TRUE;
		}

		if (uMsg == WM_SIZING && !g_freeform_window)
		{
			RECT* rect = reinterpret_cast<RECT*>(lParam);

			float ratio = is_virt() ? ratio_vertical : ratio_horizontal;

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			if (is_virt() && ratio != (9.0 / 16.0))
			{
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
			}

			if (!is_virt() && ratio != (16.0 / 9.0))
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
			auto lastWidthField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastWidth");
			auto lastHeightField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "windowLastHeight");

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

			auto _aspectRatioField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "_aspectRatio");
			float _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			il2cpp_field_static_set_value(_aspectRatioField, &_aspectRatio);


			auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
			if (uiManager)
			{
				bool isVirt = width < height;
				if (g_unlock_size)
				{
					UIManager_ChangeResizeUIForPC_hook(uiManager, isVirt ? min(last_display_width, last_display_height) : max(last_display_width, last_display_height),
						isVirt ? max(last_display_width, last_display_height) : min(last_display_width, last_display_height));

				}
				else
				{
					UIManager_ChangeResizeUIForPC_hook(uiManager, isVirt ? 1080 : 1920, isVirt ? 1920 : 1080);
				}
			}

			auto anRootManager = GetSingletonInstance(il2cpp_symbols::get_class("Plugins.dll", "AnimateToUnity", "AnRootManager"));

			if (anRootManager)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(anRootManager->klass, "set_ScreenRate", 1)->methodPointer(anRootManager, _aspectRatio);
			}

			il2cpp_symbols::get_method_pointer<void (*)()>(
				"umamusume.dll", "Gallop",
				"StandaloneWindowResize", "DisableMaximizebox", -1
			)();
			return TRUE;
		}

		if (uMsg == WM_CLOSE)
		{
			if (isExitOpened)
			{
				ExitProcess(0);
				return TRUE;
			}

			ShowExitDialog();
			return TRUE;
		}

		WNDPROC oldWndProcPtr = nullptr;

		auto oldWndProcPtrField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize"), "oldWndProcPtr");
		il2cpp_field_static_get_value(oldWndProcPtrField, &oldWndProcPtr);

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
		style |= WS_MAXIMIZEBOX;
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

	void* Screen_SetResolution_orig = nullptr;

	void Screen_SetResolution_hook(int w, int h, bool fullscreen, bool forceUpdate)
	{

	}

	void* Screen_IsCurrentOrientation_orig = nullptr;

	bool Screen_IsCurrentOrientation_hook(ScreenOrientation target)
	{
		return true;
	}

	void* GallopInput_mousePosition_orig = nullptr;

	Vector3_t GallopInput_mousePosition_hook()
	{
		return il2cpp_symbols::get_method_pointer<Vector3_t(*)()>("UnityEngine.InputLegacyModule.dll", "UnityEngine", "Input", "get_mousePosition", -1)();
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
			auto _drawPathField = il2cpp_class_get_field_from_name_wrap(buffer->klass, "_drawPass");
			Il2CppObject* _drawPath;
			il2cpp_field_get_value(buffer, _drawPathField, &_drawPath);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 1)->methodPointer(buffer, _drawPath);
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
		if (g_replace_to_builtin_font && text_get_linespacing(_this) != 1.05f)
		{
			text_set_style(_this, 1);
			text_set_size(_this, text_get_size(_this) - 4);
			text_set_linespacing(_this, 1.05f);
		}
		if (g_replace_to_custom_font)
		{
			auto font = text_get_font(_this);
			Il2CppString* name = uobject_get_name(font);
			if (g_font_asset_name.find(local::wide_u8(name->start_char)) == string::npos)
			{
				text_set_font(_this, GetCustomFont());
			}
		}
		auto textId = textcommon_get_TextId(_this);
		if (textId)
		{
			if (GetTextIdByName("Common0121") == textId ||
				GetTextIdByName("Common0186") == textId ||
				GetTextIdByName("Outgame0028") == textId ||
				GetTextIdByName("Outgame0231") == textId ||
				GetTextIdByName("Outgame0393") == textId ||
				GetTextIdByName("SingleMode0265") == textId ||
				GetTextIdByName("Character0039") == textId ||
				GetTextIdByName("Character0325") == textId)
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
		if (g_replace_to_builtin_font)
		{
			text_assign_font(_this);
		}
		if (g_replace_to_custom_font)
		{
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
		auto fontAssetField = il2cpp_class_get_field_from_name_wrap(tmpSettings->klass,
			"m_defaultFontAsset");
		il2cpp_field_set_value(tmpSettings, fontAssetField, GetCustomTMPFont());
		return tmpSettings;
	}

	void* TextMeshProUguiCommon_Awake_orig = nullptr;
	void TextMeshProUguiCommon_Awake_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(TextMeshProUguiCommon_Awake_hook)*>(TextMeshProUguiCommon_Awake_orig)(_this);
		auto customFont = GetCustomTMPFont();
		auto customFontMaterialField = il2cpp_class_get_field_from_name_wrap(customFont->klass, "material");
		Il2CppObject* customFontMaterial;
		il2cpp_field_get_value(customFont, customFontMaterialField, &customFontMaterial);

		auto SetFloat = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, float)>(customFontMaterial->klass, "SetFloat", 2)->methodPointer;
		auto SetColor = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, Color_t)>(customFontMaterial->klass, "SetColor", 2)->methodPointer;

		auto origOutlineWidth = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(_this->klass, "get_outlineWidth", 0)->methodPointer(_this);

		auto outlineColorDictField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ColorPreset"), "OutlineColorDictionary");
		Il2CppObject* outlineColorDict;
		il2cpp_field_static_get_value(outlineColorDictField, &outlineColorDict);
		auto colorEnum = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(_this->klass, "get_OutlineColor", 0)->methodPointer(_this);

		auto entriesField = il2cpp_class_get_field_from_name_wrap(outlineColorDict->klass, "entries");
		Il2CppArraySize* entries;
		il2cpp_field_get_value(outlineColorDict, entriesField, &entries);

		auto colorType = GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType");

		auto color32 = 0xFFFFFFFF;
		for (int i = 0; i < entries->max_length; i++)
		{
			auto entry = reinterpret_cast<unsigned long long>(entries->vector[i]);
			auto color = (entry & 0xFFFFFFFF00000000) >> 32;
			auto key = entry & 0xFFFFFFFF;
			if (key == colorEnum && (color != 0xFFFFFFFF && color != 0))
			{
				color32 = color;
				break;
			}
			auto enumName = local::wide_u8(GetEnumName(colorType, colorEnum)->start_char);
			if (enumName == "White"s || enumName == "Black"s)
			{
				color32 = color;
				break;
			}
		}

		float a = ((color32 & 0xFF000000) >> 24) / static_cast<float>(0xff);
		float b = ((color32 & 0xFF0000) >> 16) / static_cast<float>(0xff);
		float g = ((color32 & 0xFF00) >> 8) / static_cast<float>(0xff);
		float r = (color32 & 0xFF) / static_cast<float>(0xff);
		auto origOutlineColor = Color_t{ r, g, b, a };

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
		if (fontAssets && g_replace_to_custom_font)
		{
			auto font = GetCustomFont();
			if (font)
			{
				FieldInfo* zekkenFontField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_fontZekken");
				il2cpp_field_set_value(_this, zekkenFontField, font);
			}
		}
		reinterpret_cast<decltype(load_zekken_composite_resource_hook)*>(load_zekken_composite_resource_orig)(_this);
	}

	void* wait_resize_ui_orig = nullptr;
	Il2CppObject* wait_resize_ui_hook(Il2CppObject* _this, bool isPortrait, bool isShowOrientationGuide)
	{
		return reinterpret_cast<decltype(wait_resize_ui_hook)*>(wait_resize_ui_orig)(_this, isPortrait, g_ui_loading_show_orientation_guide ? false : isShowOrientationGuide);
	}

	void* get_modified_string_orig = nullptr;
	Il2CppString* get_modified_string_hook(Il2CppString* text, Il2CppObject* input, bool allowNewLine)
	{
		if (!allowNewLine)
		{
			auto u8str = local::wide_u8(text->start_char);
			replaceAll(u8str, "\n", "");
			return il2cpp_string_new(u8str.data());
		}
		return text;
	}

	void* set_anti_aliasing_orig = nullptr;
	void set_anti_aliasing_hook(int level)
	{
		reinterpret_cast<decltype(set_anti_aliasing_hook)*>(set_anti_aliasing_orig)(g_anti_aliasing);
	}

	void* set_shadowResolution_orig = nullptr;
	void set_shadowResolution_hook(int level)
	{
		reinterpret_cast<decltype(set_shadowResolution_hook)*>(set_shadowResolution_orig)(3);
	}

	void* set_anisotropicFiltering_orig = nullptr;
	void set_anisotropicFiltering_hook(int mode)
	{
		reinterpret_cast<decltype(set_anisotropicFiltering_hook)*>(set_anisotropicFiltering_orig)(2);
	}

	void* set_shadows_orig = nullptr;
	void set_shadows_hook(int quality)
	{
		reinterpret_cast<decltype(set_shadows_hook)*>(set_shadows_orig)(2);
	}

	void* set_softVegetation_orig = nullptr;
	void set_softVegetation_hook(bool enable)
	{
		reinterpret_cast<decltype(set_softVegetation_hook)*>(set_softVegetation_orig)(true);
	}

	void* set_realtimeReflectionProbes_orig = nullptr;
	void set_realtimeReflectionProbes_hook(bool enable)
	{
		reinterpret_cast<decltype(set_realtimeReflectionProbes_hook)*>(set_realtimeReflectionProbes_orig)(true);
	}

	void* Light_set_shadowResolution_orig = nullptr;
	void Light_set_shadowResolution_hook(Il2CppObject* _this, int level)
	{
		reinterpret_cast<decltype(Light_set_shadowResolution_hook)*>(Light_set_shadowResolution_orig)(_this, 3);
	}

	void* apply_graphics_quality_orig = nullptr;
	void apply_graphics_quality_hook(Il2CppObject* _this, int quality, bool force)
	{
		reinterpret_cast<decltype(apply_graphics_quality_hook)*>(apply_graphics_quality_orig)(_this, g_graphics_quality, true);
		if (g_graphics_quality >= 3)
		{
			set_shadowResolution_hook(3);
			set_anisotropicFiltering_hook(2);
			set_shadows_hook(2);
			set_softVegetation_hook(true);
			set_realtimeReflectionProbes_hook(true);
		}
	}
	void* GraphicSettings_GetVirtualResolution_orig = nullptr;
	Vector2Int_t GraphicSettings_GetVirtualResolution_hook(Il2CppObject* _this)
	{
		if (g_freeform_window)
		{
			int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			if (width < height)
			{
				return Vector2Int_t{ number1080, number1920 };
			}

			return Vector2Int_t{ number1920, number1080 };
		}

		return reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution_hook)*>(GraphicSettings_GetVirtualResolution_orig)(_this);
	}

	void* GraphicSettings_GetVirtualResolution3D_orig = nullptr;
	Vector2Int_t GraphicSettings_GetVirtualResolution3D_hook(Il2CppObject* _this, bool isForcedWideAspect)
	{
		if (g_freeform_window)
		{
			int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			number1920 *= g_resolution_3d_scale;
			number1080 *= g_resolution_3d_scale;

			if (width < height)
			{
				return Vector2Int_t{ number1080, number1920 };
			}

			return Vector2Int_t{ number1920, number1080 };
		}

		auto resolution = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution3D_hook)*>(GraphicSettings_GetVirtualResolution3D_orig)(_this, isForcedWideAspect);
		if (g_unlock_size)
		{
			Resolution_t res;
			get_resolution(&res);
			if (resolution.x > resolution.y)
			{
				resolution.x = res.width;
				resolution.y = res.height;
			}
			else
			{
				resolution.x = res.height;
				resolution.y = res.width;
			}
		}
		resolution.x *= g_resolution_3d_scale;
		resolution.y *= g_resolution_3d_scale;
		return resolution;
	}

	void* PathResolver_GetLocalPath_orig = nullptr;
	Il2CppString* PathResolver_GetLocalPath_hook(Il2CppObject* _this, int kind, Il2CppString* hname)
	{
		auto hnameU8 = local::wide_u8(hname->start_char);
		if (g_replace_assets.find(hnameU8) != g_replace_assets.end())
		{
			auto& replaceAsset = g_replace_assets.at(hnameU8);
			return il2cpp_string_new(replaceAsset.path.data());
		}
		return reinterpret_cast<decltype(PathResolver_GetLocalPath_hook)*>(PathResolver_GetLocalPath_orig)(_this, kind, hname);
	}

	Il2CppObject* Renderer_get_material_hook(Il2CppObject* _this);
	Il2CppArraySize* Renderer_get_materials_hook(Il2CppObject* _this);
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* _this);
	Il2CppArraySize* Renderer_get_sharedMaterials_hook(Il2CppObject* _this);

	int (*Shader_PropertyToID)(Il2CppString* name);

	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* _this, int nameID);
	void Material_SetTextureImpl_hook(Il2CppObject* _this, int nameID, Il2CppObject* texture);

	void ReplaceRendererTexture(Il2CppObject* renderer)
	{
		if (!uobject_IsNativeObjectAlive(renderer) || true)
		{
			return;
		}
		Renderer_get_materials_hook(renderer);
		Renderer_get_material_hook(renderer);
		Renderer_get_sharedMaterials_hook(renderer);
		Renderer_get_sharedMaterial_hook(renderer);
	}

	void ReplaceMaterialTexture(Il2CppObject* material)
	{
		if (!uobject_IsNativeObjectAlive(material))
		{
			return;
		}
		auto mainTexture = Material_GetTextureImpl_hook(material, Shader_PropertyToID(il2cpp_string_new("_MainTex")));
		if (mainTexture)
		{
			auto uobject_name = uobject_get_name(mainTexture);
			if (!local::wide_u8(uobject_name->start_char).empty())
			{
				auto newTexture = GetReplacementAssets(
					uobject_name,
					(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
				if (newTexture)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
						(newTexture, 32);
					Material_SetTextureImpl_hook(material, Shader_PropertyToID(il2cpp_string_new("_MainTex")), newTexture);
				}
			}
		}
	}

	void ReplaceAssetHolderTextures(Il2CppObject* holder)
	{
		if (!uobject_IsNativeObjectAlive(holder))
		{
			return;
		}
		auto objectList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_ObjectList", 0)->methodPointer(holder);
		FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(objectList->klass, "_items");
		Il2CppArraySize* arr;
		il2cpp_field_get_value(objectList, itemsField, &arr);
		for (int i = 0; i < arr->max_length; i++)
		{
			auto pair = (Il2CppObject*)arr->vector[i];
			if (!pair) continue;
			auto field = il2cpp_class_get_field_from_name_wrap(pair->klass, "Value");
			Il2CppObject* obj;
			il2cpp_field_get_value(pair, field, &obj);
			if (obj)
			{
				//cout << "AssetHolder: " << i << " " << obj->klass->name << endl;
				if (obj->klass->name == "GameObject"s && uobject_IsNativeObjectAlive(obj))
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
								il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(&array->obj, j);
							if (!obj) continue;
							/*if (obj && obj->klass && obj->klass->name != "Transform"s)
							{
								cout << obj->klass->name << endl;
							}*/
							if (string(obj->klass->name).find("MeshRenderer") != string::npos)
							{
								ReplaceRendererTexture(obj);
							}
						}
					}
				}
				if (obj->klass->name == "Texture2D"s)
				{
					auto uobject_name = uobject_get_name(obj);
					//cout << "Texture2D: " << local::wide_u8(uobject_name->start_char) << endl;
					if (!local::wide_u8(uobject_name->start_char).empty())
					{
						auto newTexture = GetReplacementAssets(
							uobject_name,
							(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
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
		if (!uobject_IsNativeObjectAlive(rawImage))
		{
			return;
		}
		auto textureField = il2cpp_class_get_field_from_name_wrap(rawImage->klass, "m_Texture");
		Il2CppObject* texture;
		il2cpp_field_get_value(rawImage, textureField, &texture);
		if (texture)
		{
			auto uobject_name = uobject_get_name(texture);
			if (uobject_name)
			{
				auto nameU8 = local::wide_u8(uobject_name->start_char);
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
								reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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

	void ReplaceGameObjectTextures(Il2CppObject* gameObject)
	{
		auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppType*)>(gameObject->klass, "GetComponent", 1)->methodPointer;
		auto getComponents =
			il2cpp_class_get_method_from_name_type<Il2CppArraySize * (*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;

		auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "Object")), true, true, true, false, nullptr);

		if (array)
		{
			for (int j = 0; j < array->max_length; j++)
			{
				auto obj =
					il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(&array->obj, j);
				if (!obj) continue;
				/*if (obj && obj->klass && obj->klass->name != "Transform"s)
				{
					cout << "GameObject -> " << obj->klass->name << endl;
				}*/

				if ("AssetHolder"s == obj->klass->name)
				{
					ReplaceAssetHolderTextures(obj);
				}
			}
		}

		auto rawImages = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "RawImageCommon")), true, true, true, false, nullptr);

		if (rawImages && rawImages->max_length)
		{
			for (int i = 0; i < rawImages->max_length; i++)
			{
				auto rawImage = reinterpret_cast<Il2CppObject*>(rawImages->vector[i]);
				if (rawImage)
				{
					ReplaceRawImageTexture(rawImage);
				}
			}
		}

		auto assetholder = getComponent(gameObject, (Il2CppType*)GetRuntimeType("umamusume.dll", "Gallop", "AssetHolder"));
		if (assetholder)
		{
			ReplaceAssetHolderTextures(assetholder);
		}
	}

	void* assetbundle_LoadFromFile_orig = nullptr;
	Il2CppObject* assetbundle_LoadFromFile_hook(Il2CppString* path, uint32_t crc, uint64_t offset)
	{
		stringstream pathStream(local::wide_u8(path->start_char));
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '\\'))
		{
			splited.emplace_back(segment);
		}
		if (g_replace_assets.find(splited.back()) != g_replace_assets.end())
		{
			auto& replaceAsset = g_replace_assets.at(splited.back());
			auto assets = reinterpret_cast<decltype(assetbundle_LoadFromFile_hook)*>(assetbundle_LoadFromFile_orig)(il2cpp_string_new(replaceAsset.path.data()), crc, offset);
			replaceAsset.asset = assets;
			return assets;
		}
		auto assetBundle = reinterpret_cast<decltype(assetbundle_LoadFromFile_hook)*>(assetbundle_LoadFromFile_orig)(path, crc, offset);
		/*auto names = get_all_asset_names(assetBundle);
		for (int i = 0; i < names->max_length; i++)
		{
			auto name = reinterpret_cast<Il2CppString*>(names->vector[i]);
			if (!name) continue;
			stringstream pathStream(local::wide_u8(name->start_char));
			string segment;
			vector<string> splited;
			while (getline(pathStream, segment, '/'))
			{
				splited.emplace_back(segment);
			}
			auto& fileName = splited.back();
			cout << "AssetBundle Name: " << fileName << endl;
		}*/
		//auto array =
		//	il2cpp_class_get_method_from_name_type<Il2CppArraySize * (*)(Il2CppObject*, const Il2CppType*)>(assetBundle->klass, "LoadAllAssets", 1)->methodPointer
		//	(assetBundle, reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Object")));
		//if (array)
		//{
		//	for (int j = 0; j < array->max_length; j++)
		//	{
		//		auto obj = 
		//			il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(&array->obj, j);
		//		if (!obj) continue;
		//		if (obj && obj->klass && obj->klass->name != "Transform"s)
		//		{
		//			stringstream pathStream(local::wide_u8(uobject_get_name(obj)->start_char));
		//			string segment;
		//			vector<string> splited;
		//			while (getline(pathStream, segment, '/'))
		//			{
		//				splited.emplace_back(segment);
		//			}
		//			auto fileName = splited.empty() ? "Unnamed"s : splited.back();
		//			cout << "AssetBundle: " << fileName << " " << obj->klass->name << endl;
		//		}
		//		if (string(obj->klass->name).find("MeshRenderer") != string::npos)
		//		{
		//			// ReplaceRendererTexture(obj);
		//		}
		//		if (obj->klass->name == "Material"s)
		//		{
		//			// ReplaceMaterialTexture(obj);
		//		}
		//	}
		//}
		return assetBundle;
	}

	Il2CppObject* assetbundle_load_asset_hook(Il2CppObject* _this, Il2CppString* name, const Il2CppType* type)
	{
		stringstream pathStream(local::wide_u8(name->start_char));
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '/'))
		{
			splited.emplace_back(segment);
		}
		auto& fileName = splited.back();
		if (find_if(replaceAssetNames.begin(), replaceAssetNames.end(), [fileName](const string& item)
			{
				return item.find(fileName) != string::npos;
			}) != replaceAssetNames.end())
		{
			return GetReplacementAssets(il2cpp_string_new(fileName.data()), type);
		}
		auto obj = reinterpret_cast<decltype(assetbundle_load_asset_hook)*>(assetbundle_load_asset_orig)(_this, name, type);
		if (obj->klass->name == "GameObject"s)
		{
			ReplaceGameObjectTextures(obj);
		}
		return obj;
	}

	void* assetbundle_unload_orig = nullptr;
	void assetbundle_unload_hook(Il2CppObject* _this, bool unloadAllLoadedObjects)
	{
		reinterpret_cast<decltype(assetbundle_unload_hook)*>(assetbundle_unload_orig)(_this, unloadAllLoadedObjects);
		for (auto& pair : g_replace_assets)
		{
			if (pair.second.asset == _this)
			{
				pair.second.asset = nullptr;
			}
		}
	}

	void* AssetBundleRequest_GetResult_orig = nullptr;
	Il2CppObject* AssetBundleRequest_GetResult_hook(Il2CppObject* _this)
	{
		auto obj = reinterpret_cast<decltype(AssetBundleRequest_GetResult_hook)*>(AssetBundleRequest_GetResult_orig)(_this);
		if (obj)
		{
			auto name = uobject_get_name(obj);
			auto u8Name = local::wide_u8(name->start_char);
			if (find(replaceAssetNames.begin(), replaceAssetNames.end(), u8Name) != replaceAssetNames.end())
			{
				return GetReplacementAssets(name, il2cpp_class_get_type(obj->klass));
			}
		}
		return obj;
	}

	Il2CppObject* resources_load_hook(Il2CppString* path, Il2CppObject* type);

	Il2CppObject* GetOptionItemTitle(const char* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemtitle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemTitle = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemTitle->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemTitle, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_text(textCommon, il2cpp_string_new(title));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		return optionItemTitle;
	}

	Il2CppObject* GetOptionItemOnOff(const char* name, const char* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemonoff"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemOnOff = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		uobject_set_name(optionItemOnOff, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_text(textCommon, il2cpp_string_new(title));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		return optionItemOnOff;
	}

	void SetOptionItemOnOffAction(const char* name, bool isOn, void (*onChange)(Il2CppObject*, bool))
	{
		auto optionItemOnOff = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemOnOff)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array2 = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "PartsOnOffToggleSwitch")), true, true, false, false, nullptr);

			auto toggleSwitch = array2->vector[0];

			auto action = CreateUnityAction(toggleSwitch, onChange);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool, Il2CppDelegate*)>(toggleSwitch->klass, "Setup", 2)->methodPointer(toggleSwitch, isOn, action);
		}
	}

	Il2CppObject* GetOptionItemButton(const char* name, const char* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitembutton"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemButton = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		uobject_set_name(optionItemButton, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemButton->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array1 = getComponents(optionItemButton, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array1->vector[0];

		text_set_text(textCommon, il2cpp_string_new(title));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		auto array2 = getComponents(optionItemButton, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ButtonCommon")), true, true, false, false, nullptr);

		auto buttonCommon = array2->vector[0];

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(buttonCommon->klass, "SetInteractable", 1)->methodPointer(buttonCommon, true);

		return optionItemButton;
	}

	void SetOptionItemButtonAction(const char* name, void (*onClick)(Il2CppObject*))
	{
		auto optionItemButton = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemButton)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemButton->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array2 = getComponents(optionItemButton, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "ButtonCommon")), true, true, false, false, nullptr);

			auto buttonCommon = array2->vector[0];

			auto action = CreateUnityAction(buttonCommon, onClick);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(buttonCommon->klass, "SetOnClick", 1)->methodPointer(buttonCommon, action);
		}
	}

	Il2CppObject* GetOptionItemAttention(const char* text)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemattention"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemAttention = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemAttention->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemAttention, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_text(textCommon, il2cpp_string_new(text));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		return optionItemAttention;
	}

	Il2CppObject* GetOptionItemInfo(const char* text)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptioniteminfo"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemInfo->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemInfo, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_text(textCommon, il2cpp_string_new(text));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		return optionItemInfo;
	}

	Il2CppObject* GetOptionItemSimple(const char* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemsimple"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemSimple = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemSimple->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemSimple, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_text(textCommon, il2cpp_string_new(title));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);

		return optionItemSimple;
	}

	Il2CppObject* GetRectTransform(Il2CppObject* object)
	{
		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(object->klass, "GetComponentsInternal", 6)->methodPointer;
		auto rectTransformArray = getComponents(object, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

		if (rectTransformArray->max_length)
		{
			return rectTransformArray->vector[0];
		}

		return  nullptr;
	}

	void AddToLayout(Il2CppObject* parentRectTransform, vector<Il2CppObject*> objects)
	{
		for (int i = objects.size() - 1; i >= 0; i--)
		//for (int i = 0; i < objects.size(); i++)
		{
			auto rectTransform = GetRectTransform(objects[i]);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(rectTransform->klass, "SetParent", 1)->methodPointer(rectTransform, parentRectTransform);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(rectTransform->klass, "SetAsFirstSibling", 0)->methodPointer(rectTransform);
		}
	}

	void OpenSettings()
	{
		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegate(dialogData, *([](Il2CppObject*)
			{
				cout << "onLeft" << endl;
			}));

		auto onRight = CreateDelegate(dialogData, *([](Il2CppObject*)
			{
				cout << "onRight" << endl;
			}));

		dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
			Il2CppString * headerTextArg,
			Il2CppString * message,
			Il2CppDelegate * onRight,
			ULONG leftTextId,
			ULONG rightTextId,
			Il2CppDelegate * onLeft,
			int dialogFormType)>(
				il2cpp_class_get_method_from_name(dialogData->klass, "SetSimpleTwoButtonMessage",
					7)->methodPointer
				)(dialogData, il2cpp_string_new("Localify Settings"), nullptr, onRight, GetTextIdByName("Common0004"), GetTextIdByName("Common0261"), onLeft, 10);

		cout << "dialogData " << dialogData << endl;

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 1;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto basicSetting = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionpagebasicsetting"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
		auto _mainCanvasField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_mainCanvas");
		Il2CppObject* _mainCanvas;
		il2cpp_field_get_value(uiManager, _mainCanvasField, &_mainCanvas);

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		basicSetting = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingleWithParent", 3)
			(basicSetting, transform, false);

		cout << "basicSetting " << basicSetting << endl;

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(basicSetting->klass, "GetComponentsInternal", 6)->methodPointer;

		auto rectTransformArray = getComponents(basicSetting, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

		for (int j = 0; j < rectTransformArray->max_length; j++)
		{
			auto rectTransform = rectTransformArray->vector[j];

			if (rectTransform && local::wide_u8(uobject_get_name(rectTransform)->start_char) == "Content")
			{
				// il2cpp_resolve_icall_type<void (*)(Il2CppObject*)>("UnityEngine.Transform::DetachChildren()")(rectTransform);
				break;
			}
		}

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, basicSetting);

		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void InitOptionLayout(Il2CppObject* parentRectTransform)
	{
		AddToLayout(parentRectTransform,
			{
				GetOptionItemTitle("Localify Settings"),
				GetOptionItemButton("open_settings", "Open Settings"),
				GetOptionItemButton("show_caption", "Show caption"),
				GetOptionItemAttention("<color=#ff911c>Attention with Color</color>\nAttention"),
				GetOptionItemSimple("Simple"),
				GetOptionItemOnOff("on_off", "On Off"),
				GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			}
		);
	}

	void SetupOptionLayout()
	{
		SetOptionItemButtonAction("open_settings", *([](Il2CppObject*)
			{
				// OpenSettings();
				auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(uiManager->klass, "ShowNotification", 1)->methodPointer(uiManager, localize_get_hook(GetTextIdByName("Home0072")));
			}));

		SetOptionItemOnOffAction("on_off", false, *([](Il2CppObject*, bool isOn)
			{
				auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

				stringstream text;

				text << "Changed to " << (isOn ? "On" : "Off");

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(uiManager->klass, "ShowNotification", 1)->methodPointer(uiManager, il2cpp_string_new(text.str().data()));
			}));

		SetOptionItemButtonAction("show_caption", *([](Il2CppObject*)
			{
				SetNotificationDisplayTime(1);
				ShowNotification(il2cpp_string_new("Sample caption"));
			}));
	}

	void* Object_Internal_CloneSingleWithParent_orig = nullptr;
	Il2CppObject* Object_Internal_CloneSingleWithParent_hook(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		auto cloned = reinterpret_cast<decltype(Object_Internal_CloneSingleWithParent_hook)*>(Object_Internal_CloneSingleWithParent_orig)(data, parent, worldPositionStays);

		if (wstring(uobject_get_name(cloned)->start_char).find(L"DialogOptionHome") != wstring::npos)
		{
			SetupOptionLayout();
		}

		if (wstring(uobject_get_name(cloned)->start_char).find(L"LiveChampionsTextController") != wstring::npos)
		{
			auto updateScreenReferenceSize = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), cloned, *([](Il2CppObject* _this)
				{
					auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
					if (director)
					{
						auto ChampionsTextControllerField = il2cpp_class_get_field_from_name_wrap(director->klass, "ChampionsTextController");
						Il2CppObject* ChampionsTextController;
						il2cpp_field_get_value(director, ChampionsTextControllerField, &ChampionsTextController);

						if (ChampionsTextController)
						{
							auto _flashPlayerField = il2cpp_class_get_field_from_name_wrap(ChampionsTextController->klass, "_flashPlayer");
							Il2CppObject* _flashPlayer;
							il2cpp_field_get_value(ChampionsTextController, _flashPlayerField, &_flashPlayer);

							auto root = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_flashPlayer->klass, "get_Root", 0)->methodPointer(_flashPlayer);

							int unityWidth = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();

							int unityHeight = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

							if (Game::CurrentGameRegion == Game::Region::KOR)
							{
								auto _flashCanvasScalerField = il2cpp_class_get_field_from_name_wrap(ChampionsTextController->klass, "_flashCanvasScaler");
								Il2CppObject* _flashCanvasScaler;
								il2cpp_field_get_value(ChampionsTextController, _flashCanvasScalerField, &_flashCanvasScaler);

								if (unityWidth < unityHeight)
								{
									float scale = min(g_freeform_ui_scale_portrait, max(1, unityHeight * ratio_vertical) * g_freeform_ui_scale_portrait);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(_flashCanvasScaler->klass, "set_referenceResolution", 1)->methodPointer(_flashCanvasScaler, Vector2_t{ static_cast<float>(unityWidth / scale), static_cast<float>(unityHeight / scale) });
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(unityHeight / scale), static_cast<float>(unityHeight / scale) });
								}
								else
								{
									float scale = min(g_freeform_ui_scale_landscape, max(1, unityWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(_flashCanvasScaler->klass, "set_referenceResolution", 1)->methodPointer(_flashCanvasScaler, Vector2_t{ static_cast<float>(unityWidth / scale), static_cast<float>(unityHeight / scale) });
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(unityHeight / scale), static_cast<float>(unityHeight / scale) });
								}
							}
							else
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, Vector2_t{ ratio_16_9 * static_cast<float>(unityHeight), static_cast<float>(unityHeight) });
							}
						}
					}
				}));

			// Delay 50ms
			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateScreenReferenceSize, true);
		}

		return cloned;
	}

	Il2CppObject* currentOptionObj = nullptr;

	void* resources_load_orig = nullptr;
	Il2CppObject* resources_load_hook(Il2CppString* path, Il2CppObject* type)
	{
		string u8Name = local::wide_u8(path->start_char);

		if (u8Name == "ui/views/titleview"s)
		{
			if (find_if(replaceAssetNames.begin(), replaceAssetNames.end(), [](const string& item)
				{
					return item.find("utx_obj_title_logo_umamusume") != string::npos;
				}) != replaceAssetNames.end())
			{
				auto gameObj = reinterpret_cast<decltype(resources_load_hook)*>(resources_load_orig)(path, type);
				auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppType*)>(gameObj->klass, "GetComponent", 1)->methodPointer;
				auto component = getComponent(gameObj, (Il2CppType*)GetRuntimeType("umamusume.dll", "Gallop", "TitleView"));

				auto imgField = il2cpp_class_get_field_from_name_wrap(component->klass, "TitleLogoImage");
				Il2CppObject* imgCommon;
				il2cpp_field_get_value(component, imgField, &imgCommon);
				auto texture = GetReplacementAssets(
					il2cpp_string_new("utx_obj_title_logo_umamusume.png"),
					(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
				auto m_TextureField = il2cpp_class_get_field_from_name_wrap(imgCommon->klass->parent, "m_Texture");
				il2cpp_field_set_value(imgCommon, m_TextureField, texture);
				return gameObj;
			}
		}

		if (u8Name == "TMP Settings"s && g_replace_to_custom_font && fontAssets)
		{
			auto object = reinterpret_cast<decltype(resources_load_hook)*>(resources_load_orig)(path, type);
			auto fontAssetField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_defaultFontAsset");
			il2cpp_field_set_value(object, fontAssetField, GetCustomTMPFont());
			return object;
		}

		if (u8Name.ends_with("dialogoptionhome"))
		{
			auto object = reinterpret_cast<decltype(resources_load_hook)*>(resources_load_orig)(path, type);

			if (object != currentOptionObj)
			{
				currentOptionObj = object;
			}
			else
			{
				return object;
			}

			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(object->klass, "GetComponentsInternal", 6)->methodPointer;

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(object->klass, "SetActive", 1)->methodPointer(object, true);


			auto array = getComponents(object, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "PartsOptionPageBasicSetting")), true, true, false, false, nullptr);

			if (array)
			{
				for (int i = 0; i < array->max_length; i++)
				{
					auto obj = array->vector[i];

					if (obj && obj->klass->name == "PartsOptionPageBasicSetting"s)
					{
						auto _notityObjListField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_notityObjList");
						Il2CppObject* _notityObjList;
						il2cpp_field_get_value(obj, _notityObjListField, &_notityObjList);

						if (_notityObjList)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_notityObjList->klass, "Clear", 0)->methodPointer(_notityObjList);
						}

						/*auto _fpsObjListField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_fpsObjList");
						Il2CppObject* _fpsObjList;
						il2cpp_field_get_value(obj, _fpsObjListField, &_fpsObjList);

						if (_fpsObjList)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_fpsObjList->klass, "Clear", 0)->methodPointer(_fpsObjList);
						}*/

						auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_gameObject", 0)->methodPointer(obj);

						auto rectTransformArray = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
							"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

						for (int j = 0; j < rectTransformArray->max_length; j++)
						{
							auto rectTransform = rectTransformArray->vector[j];

							if (rectTransform && local::wide_u8(uobject_get_name(rectTransform)->start_char) == "Content")
							{
								InitOptionLayout(rectTransform);
								break;
							}
						}
					}
				}
			}

			return object;
		}

		return reinterpret_cast<decltype(resources_load_hook)*>(resources_load_orig)(path, type);

	}

	void* Sprite_get_texture_orig = nullptr;
	Il2CppObject* Sprite_get_texture_hook(Il2CppObject* _this)
	{
		auto texture2D = reinterpret_cast<decltype(Sprite_get_texture_hook)*>(Sprite_get_texture_orig)(_this);
		auto uobject_name = uobject_get_name(texture2D);
		if (!local::wide_u8(uobject_name->start_char).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_name,
				(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			if (newTexture)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
					(newTexture, 32);
				return newTexture;
			}
		}
		return texture2D;
	}

	void* Renderer_get_material_orig = nullptr;
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
	Il2CppArraySize* Renderer_get_materials_hook(Il2CppObject* _this)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_materials_hook)*>(Renderer_get_materials_orig)(_this);
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = (Il2CppObject*)materials->vector[i];
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
	Il2CppArraySize* Renderer_get_sharedMaterials_hook(Il2CppObject* _this)
	{
		auto materials = reinterpret_cast<decltype(Renderer_get_sharedMaterials_hook)*>(Renderer_get_sharedMaterials_orig)(_this);
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = (Il2CppObject*)materials->vector[i];
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

	void* Renderer_set_sharedMaterial_orig = nullptr;
	void Renderer_set_sharedMaterial_hook(Il2CppObject* _this, Il2CppObject* material)
	{
		if (material)
		{
			ReplaceMaterialTexture(material);
		}
		reinterpret_cast<decltype(Renderer_set_sharedMaterial_hook)*>(Renderer_set_sharedMaterial_orig)(_this, material);
	}

	void* Renderer_set_sharedMaterials_orig = nullptr;
	void Renderer_set_sharedMaterials_hook(Il2CppObject* _this, Il2CppArraySize* materials)
	{
		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = (Il2CppObject*)materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
		reinterpret_cast<decltype(Renderer_set_sharedMaterials_hook)*>(Renderer_set_sharedMaterials_orig)(_this, materials);
	}

	void* Material_set_mainTexture_orig = nullptr;
	void Material_set_mainTexture_hook(Il2CppObject* _this, Il2CppObject* texture)
	{
		if (texture)
		{
			if (!local::wide_u8(uobject_get_name(texture)->start_char).empty())
			{
				auto newTexture = GetReplacementAssets(
					uobject_get_name(texture),
					(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
				if (newTexture)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
						(newTexture, 32);
					reinterpret_cast<decltype(Material_set_mainTexture_hook)*>(Material_set_mainTexture_orig)(_this, newTexture);
					return;
				}
			}
		}
		reinterpret_cast<decltype(Material_set_mainTexture_hook)*>(Material_set_mainTexture_orig)(_this, texture);
	}

	void* Material_get_mainTexture_orig = nullptr;
	Il2CppObject* Material_get_mainTexture_hook(Il2CppObject* _this)
	{
		auto texture = reinterpret_cast<decltype(Material_get_mainTexture_hook)*>(Material_get_mainTexture_orig)(_this);
		if (texture)
		{
			auto uobject_name = uobject_get_name(texture);
			if (!local::wide_u8(uobject_name->start_char).empty())
			{
				auto newTexture = GetReplacementAssets(
					uobject_name,
					(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
				if (newTexture)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
						(newTexture, 32);
					return newTexture;
				}
			}
		}
		return texture;
	}

	void* Material_SetTextureI4_orig = nullptr;
	void Material_SetTextureI4_hook(Il2CppObject* _this, int nameID, Il2CppObject* texture)
	{
		if (texture && !local::wide_u8(uobject_get_name(texture)->start_char).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_get_name(texture),
				(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
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
		if (texture && !local::wide_u8(uobject_get_name(texture)->start_char).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_get_name(texture),
				(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
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
		if (texture && !local::wide_u8(uobject_get_name(texture)->start_char).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_get_name(texture),
				(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
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
		if (!local::wide_u8(uobject_get_name(texture)->start_char).empty())
		{
			auto newTexture = GetReplacementAssets(
				uobject_get_name(texture),
				(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
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

	struct CastHelper
	{
		Il2CppObject* obj;
		uintptr_t oneFurtherThanResultValue;
	};

	void* GameObject_GetComponentFastPath_orig = nullptr;
	void GameObject_GetComponentFastPath_hook(Il2CppObject* _this, Il2CppObject* type, uintptr_t oneFurtherThanResultValue)
	{
		/*auto name = il2cpp_class_get_method_from_name_type<Il2CppString* (*)(Il2CppObject*)>(type->klass, "get_FullName", 0)->methodPointer(type);
		cout << "GameObject_GetComponentFastPath " << local::wide_u8(name->start_char) << endl;*/
		reinterpret_cast<decltype(GameObject_GetComponentFastPath_hook)*>(GameObject_GetComponentFastPath_orig)(_this, type, oneFurtherThanResultValue);
		auto helper = CastHelper{};
		int objSize = sizeof(helper.obj);
		memmove(&helper, reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), sizeof(CastHelper));
		if (helper.obj)
		{
			// cout << "Helper " << helper.obj->klass->name << endl;
			if (string(helper.obj->klass->name).find("MeshRenderer") != string::npos)
			{
				ReplaceRendererTexture(helper.obj);
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
			memmove(reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), &helper, sizeof(CastHelper));
		}
	}

	void* ChangeScreenOrientation_orig = nullptr;

	Il2CppObject* ChangeScreenOrientation_hook(ScreenOrientation targetOrientation, bool isForce)
	{
		return reinterpret_cast<decltype(ChangeScreenOrientation_hook)*>(ChangeScreenOrientation_orig)(targetOrientation, isForce);
	}

	void* ChangeScreenOrientationPortraitAsync_orig = nullptr;

	Il2CppObject* ChangeScreenOrientationPortraitAsync_hook()
	{
		return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>(
			"umamusume.dll",
			"Gallop",
			"Screen", "ChangeScreenOrientationLandscapeAsync", -1)();
	}

	void* get_IsVertical_orig = nullptr;

	Boolean get_IsVertical_hook()
	{
		return GetBoolean(true);
	}

	void* Screen_set_orientation_orig = nullptr;

	void Screen_set_orientation_hook(ScreenOrientation orientation)
	{
		if (!g_freeform_window)
		{
			reinterpret_cast<decltype(Screen_set_orientation_hook)*>(Screen_set_orientation_orig)(
				orientation);
		}
	}

	void* Screen_RequestOrientation_orig = nullptr;

	void Screen_RequestOrientation_hook(ScreenOrientation orientation)
	{
		if (g_freeform_window)
		{
			reinterpret_cast<decltype(Screen_RequestOrientation_hook)*>(Screen_RequestOrientation_orig)(ScreenOrientation::AutoRotation);

			auto hWnd = GetHWND();

			long style = GetWindowLongW(hWnd, GWL_STYLE);
			style |= WS_MAXIMIZEBOX;
			SetWindowLongPtrW(hWnd, GWL_STYLE, style);
		}
		else
		{
			reinterpret_cast<decltype(Screen_RequestOrientation_hook)*>(Screen_RequestOrientation_orig)(
				orientation);
		}
	}

	void* DeviceOrientationGuide_Show_orig = nullptr;

	void DeviceOrientationGuide_Show_hook(Il2CppObject* _this, bool isTargetOrientationPortrait,
		int target)
	{
		if (!g_freeform_window)
		{
			reinterpret_cast<decltype(DeviceOrientationGuide_Show_hook)*>(DeviceOrientationGuide_Show_orig)(
				_this, isTargetOrientationPortrait, target);
		}
	}

	void* NowLoading_Show_orig = nullptr;

	void NowLoading_Show_hook(Il2CppObject* _this, int type, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType)
	{
		// NowLoadingOrientation
		if (type == 2 && (g_freeform_window || !g_ui_loading_show_orientation_guide))
		{
			// NowLoadingTips
			type = 0;
		}
		if (!g_hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Show_hook)*>(NowLoading_Show_orig)(
				_this,
				type,
				onComplete, overrideDuration, easeType);
		}
		if (onComplete && g_hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* NowLoading_Hide_orig = nullptr;

	void NowLoading_Hide_hook(Il2CppObject* _this, Il2CppDelegate* onComplete, Il2CppObject* overrideDuration, int easeType)
	{
		if (!g_hide_now_loading)
		{
			reinterpret_cast<decltype(NowLoading_Hide_hook)*>(NowLoading_Hide_orig)(_this, onComplete, overrideDuration, easeType);
		}
		if (onComplete && g_hide_now_loading)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(onComplete->method_ptr)(onComplete->target);
		}
	}

	void* WaitDeviceOrientation_orig = nullptr;

	void WaitDeviceOrientation_hook(ScreenOrientation targetOrientation)
	{
		if (!g_freeform_window)
		{
			reinterpret_cast<decltype(WaitDeviceOrientation_hook)*>(WaitDeviceOrientation_orig)(
				targetOrientation);
		}
	}

	Il2CppObject* UIManager_WaitBootSetup_hook(Il2CppObject* _this);

	void* BootSystem_Awake_orig = nullptr;

	void BootSystem_Awake_hook(Il2CppObject* _this)
	{
		// Resolution_t r;
		// get_resolution_stub(&r);
		// last_display_width = r.width;
		// last_display_height = r.height;

		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)()>("Cute.Core.Assembly.dll", "Cute.Core", "Perf/Time", "get_Instance", -1)();

		auto StartCoroutine = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "MonoBehaviour", [](const MethodInfo* method)
			{
				return method->name == "StartCoroutine"s && method->parameters[0].parameter_type->type == IL2CPP_TYPE_CLASS;
			});

		StartCoroutine(_this, il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_this->klass, "BootCoroutine", 0)->methodPointer(_this));

		/*auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

		auto defaultResolution = il2cpp_class_get_method_from_name_type<Vector2_t(*)()>(uiManager->klass, "get_DefaultResolution", 0)->methodPointer();

		auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(uiManager->klass, "GetCanvasScalerList", 0)->methodPointer(uiManager);
		for (int i = 0; i < canvasScalerList->max_length; i++)
		{
			auto canvasScaler = canvasScalerList->vector[i];
			if (canvasScaler)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, defaultResolution);

				if (defaultResolution.x < defaultResolution.y)
				{
					float scale = min(1, max(1, defaultResolution.y / 1080) * g_freeform_ui_scale_portrait / (defaultResolution.y / 1080));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
				else
				{
					float scale = min(1, max(1, defaultResolution.x / 1920) * g_freeform_ui_scale_landscape / (defaultResolution.x / 1920));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
			}
		}

		auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgCamera");
		Il2CppObject* _bgCamera;
		il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
			il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", -1)());

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "AdjustSafeArea", 0)->methodPointer(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "CreateRenderTextureFromScreen", 0)->methodPointer(_this);*/

		if (!g_freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

			bool isLandscape = width > height;

			auto changedSize = il2cpp_class_get_method_from_name_type<Vector2_t(*)(float, float, bool)>(StandaloneWindowResize, "GetChangedSize", 3)->methodPointer(isLandscape ? height : width, isLandscape ? width : height, true);

			if (il2cpp_class_get_method_from_name_type<bool (*)(float, float)>(StandaloneWindowResize, "CheckOverScreenSize", 2)->methodPointer(changedSize.x, changedSize.y))
			{
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(false);
				il2cpp_class_get_method_from_name_type<void (*)(float, float)>(StandaloneWindowResize, "KeepAspectRatio", 2)->methodPointer(changedSize.x, changedSize.y);
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);
				return;
			}
			il2cpp_symbols::get_method_pointer<void (*)(int, int, bool, bool)>("umamusume.dll", "Gallop", "Screen", "SetResolution", 4)(changedSize.x, changedSize.y, false, false);
		}
	}

	void* SplashViewController_KakaoStart_orig;
	void SplashViewController_KakaoStart_hook(Il2CppObject* _this)
	{
		// no-op
	}

	Il2CppObject* (*MoviePlayerBase_get_MovieInfo)(Il2CppObject* _this);
	Il2CppObject* (*MovieManager_GetMovieInfo)(Il2CppObject* _this, MoviePlayerHandle playerHandle);

	void* MovieManager_SetImageUvRect_orig = nullptr;

	void MovieManager_SetImageUvRect_hook(Il2CppObject* _this, MoviePlayerHandle playerHandle, Rect_t uv)
	{
		auto movieInfo = MovieManager_GetMovieInfo(_this, playerHandle);
		if (!movieInfo)
		{
			return;
		}

		auto widthField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "width");
		auto heightField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "height");
		unsigned int width, height;
		il2cpp_field_get_value(movieInfo, widthField, &width);
		il2cpp_field_get_value(movieInfo, heightField, &height);
		if (width < height && !is_virt())
		{
			int rWidth = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int rHeight = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();
			if (roundf(ratio_horizontal * (max(1.0f, rHeight * ratio_vertical) * (g_freeform_window ? g_freeform_ui_scale_landscape : g_ui_scale))) == uv.height)
			{
				uv.height = rWidth;
			}
			uv.width = rHeight;
		}

		reinterpret_cast<decltype(MovieManager_SetImageUvRect_hook)*>(MovieManager_SetImageUvRect_orig)(_this, playerHandle, uv);
	}

	void* MovieManager_SetScreenSize_orig = nullptr;

	void MovieManager_SetScreenSize_hook(Il2CppObject* _this, MoviePlayerHandle playerHandle, Vector2_t screenSize)
	{
		auto movieInfo = MovieManager_GetMovieInfo(_this, playerHandle);
		if (!movieInfo)
		{
			return;
		}

		auto widthField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "width");
		auto heightField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "height");
		unsigned int width, height;
		il2cpp_field_get_value(movieInfo, widthField, &width);
		il2cpp_field_get_value(movieInfo, heightField, &height);
		if (width < height && !is_virt())
		{
			int rWidth = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int rHeight = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();
			if (roundf(ratio_horizontal * (max(1.0f, rHeight * ratio_vertical) * (g_freeform_window ? g_freeform_ui_scale_landscape : g_ui_scale))) == screenSize.y)
			{
				screenSize.y = rWidth;
			}
			screenSize.x = rHeight;
		}

		reinterpret_cast<decltype(MovieManager_SetScreenSize_hook)*>(MovieManager_SetScreenSize_orig)(_this, playerHandle, screenSize);
	}

	void* MoviePlayerForUI_AdjustScreenSize_orig = nullptr;

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* _this, Vector2_t dispRectWH, bool isPanScan)
	{
		auto movieInfo = MoviePlayerBase_get_MovieInfo(_this);
		if (!movieInfo)
		{
			return;
		}

		auto widthField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "width");
		auto heightField = il2cpp_class_get_field_from_name_wrap(movieInfo->klass, "height");

		unsigned int width, height;
		il2cpp_field_get_value(movieInfo, widthField, &width);
		il2cpp_field_get_value(movieInfo, heightField, &height);

		auto ratio = floorf(static_cast<float>(width) / height * 100) / 100;
		auto ratio1 = static_cast<float>(width) / static_cast<float>(height);

		if ((width < height || ratio == 1.33f) && !is_virt())
		{
			int rWidth;
			int rHeight;

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			rWidth = number1920;
			rHeight = number1080;

			if (roundf(ratio_horizontal * (max(1.0f, rHeight * ratio_vertical) * (g_freeform_window ? g_freeform_ui_scale_landscape : g_ui_scale))) == dispRectWH.y)
			{
				dispRectWH.y = rWidth;
				dispRectWH.x = rHeight;
			}
			else
			{
				float scale = min(g_freeform_ui_scale_landscape, max(1, rWidth / ratio_horizontal) * g_freeform_ui_scale_landscape);

				if (roundf(dispRectWH.y * scale) != rHeight)
				{
					dispRectWH.x = dispRectWH.y * ratio_16_9;
				}
				else
				{
					dispRectWH.x = dispRectWH.y * ratio1;
				}
			}
		}

		reinterpret_cast<decltype(MoviePlayerForUI_AdjustScreenSize_hook)*>(MoviePlayerForUI_AdjustScreenSize_orig)(_this, dispRectWH, isPanScan);
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
		set_fps_hook(30);
	}

	Il2CppObject* errorDialog = nullptr;

	void* DialogCommon_Close_orig = nullptr;
	void DialogCommon_Close_hook(Il2CppObject* _this)
	{
		if (_this == errorDialog)
		{
			auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
			if (sceneManager)
			{
				// Home 100
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, Il2CppObject*, Il2CppObject*, Il2CppObject*, bool)>(sceneManager->klass, "ChangeView", 5)->methodPointer
				(sceneManager, 100, nullptr, nullptr, nullptr, true);
			}
		}
		reinterpret_cast<decltype(DialogCommon_Close_hook)*>(DialogCommon_Close_orig)(_this);
	}

	void* GallopUtil_GotoTitleOnError_orig = nullptr;
	void GallopUtil_GotoTitleOnError_hook(Il2CppString* text)
	{
		// Bypass SoftwareReset
		auto okText = GetTextIdByName("Common0009");
		auto errorText = GetTextIdByName("Common0071");

		auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);
		dialogData =
			il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this, unsigned long headerTextId, Il2CppString * message, Il2CppObject * onClickCenterButton, unsigned long closeTextId)>(dialogData->klass, "SetSimpleOneButtonMessage", 4)->methodPointer
			(dialogData, errorText, local::get_localized_string(il2cpp_string_new(GotoTitleErrorJa.data())), nullptr, okText);
		errorDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data, bool isEnableOutsideClick)>("umamusume.dll", "Gallop", "DialogManager", "PushSystemDialog", 2)(dialogData, true);
	}

	void* CriMana_Player_SetFile_orig = nullptr;
	bool CriMana_Player_SetFile_hook(Il2CppObject* _this, Il2CppObject* binder, Il2CppString* moviePath, int setMode)
	{
		stringstream pathStream(local::wide_u8(moviePath->start_char));
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '\\'))
		{
			splited.emplace_back(segment);
		}
		if (g_replace_assets.find(splited[splited.size() - 1]) != g_replace_assets.end())
		{
			auto& replaceAsset = g_replace_assets.at(splited[splited.size() - 1]);
			moviePath = il2cpp_string_new(replaceAsset.path.data());
		}
		return reinterpret_cast<decltype(CriMana_Player_SetFile_hook)*>(CriMana_Player_SetFile_orig)(_this, binder, moviePath, setMode);
	}

	Il2CppObject* voiceButtonTarget = nullptr;

	Il2CppDelegate* updateVoiceButton = nullptr;

	void UpdateVoiceButton()
	{
		Il2CppArraySize_t<Il2CppObject*>* voiceButtonList;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			voiceButtonList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "PartsEpisodeExtraVoiceButton"), 0, 0);
		}
		else
		{
			voiceButtonList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
				GetRuntimeType("umamusume.dll", "Gallop", "PartsEpisodeExtraVoiceButton"), false);
		}

		if (voiceButtonList && voiceButtonList->max_length)
		{
			for (int i = 0; i < voiceButtonList->max_length; i++)
			{
				auto voiceButton = voiceButtonList->vector[i];

				if (voiceButton)
				{
					auto buttonField = il2cpp_class_get_field_from_name_wrap(voiceButton->klass, "_playVoiceButton");
					Il2CppObject* button;
					il2cpp_field_get_value(voiceButton, buttonField, &button);

					auto callback = GetButtonCommonOnClickDelegate(button);
					if (callback)
					{
						if (voiceButtonTarget != callback->target)
						{
							voiceButtonTarget = callback->target;

							auto newFn = *(
								[](Il2CppObject* _this)
								{
									auto storyIdField = il2cpp_class_get_field_from_name_wrap(voiceButtonTarget->klass, "storyId");
									int storyId;
									il2cpp_field_get_value(voiceButtonTarget, storyIdField, &storyId);

									auto masterDataManager = GetSingletonInstance(
										il2cpp_symbols::get_class(
											"umamusume.dll", "Gallop",
											"MasterDataManager"));
									auto masterBannerData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(
										masterDataManager->klass,
										"get_masterBannerData",
										0)->methodPointer(masterDataManager);

									auto bannerList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(
										Il2CppObject*,
										int)>(
											masterBannerData->klass,
											"GetListWithGroupId",
											1)->methodPointer(masterBannerData,
												7);

									FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(
										bannerList->klass, "_items");
									Il2CppArraySize* arr;
									il2cpp_field_get_value(bannerList, itemsField,
										&arr);

									int announceId = -1;

									for (int i = 0; i < arr->max_length; i++)
									{
										auto item = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
										if (item)
										{
											auto typeField = il2cpp_class_get_field_from_name_wrap(
												item->klass, "Type");
											int type;
											il2cpp_field_get_value(item, typeField,
												&type);
											auto conditionValueField = il2cpp_class_get_field_from_name_wrap(
												item->klass, "ConditionValue");
											int conditionValue;
											il2cpp_field_get_value(item,
												conditionValueField,
												&conditionValue);
											if (type == 7 &&
												conditionValue == storyId)
											{
												auto transitionField = il2cpp_class_get_field_from_name_wrap(
													item->klass, "Transition");
												il2cpp_field_get_value(item,
													transitionField,
													&announceId);
												break;
											}
										}
									}

									if (announceId == -1 && storyId < 1005)
									{
										announceId = storyId - 1002;
									}

									auto action = CreateDelegate(voiceButtonTarget, *([](Il2CppObject*) {}));

									il2cpp_symbols::get_method_pointer<void (*)(int,
										Il2CppDelegate*,
										Il2CppDelegate*)>(
											"umamusume.dll", "Gallop",
											"DialogAnnounceEvent", "Open", 3)(announceId, action, action);
								});
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button,
								CreateUnityAction(voiceButtonTarget, newFn));
						}
					}
				}
			}
		}
	}

	void* PartsEpisodeList_SetupStoryExtraEpisodeList_orig = nullptr;

	void PartsEpisodeList_SetupStoryExtraEpisodeList_hook(Il2CppObject* _this, Il2CppObject* extraSubCategory, Il2CppObject* partDataList, Il2CppObject* partData, Il2CppDelegate* onClick)
	{
		reinterpret_cast<decltype(PartsEpisodeList_SetupStoryExtraEpisodeList_hook)*>(PartsEpisodeList_SetupStoryExtraEpisodeList_orig)(_this, extraSubCategory, partDataList, partData, onClick);

		int partDataId;

		auto partDataIdField = il2cpp_class_get_field_from_name_wrap(partData->klass, "<Id>k__BackingField");
		il2cpp_field_get_value(partData, partDataIdField, &partDataId);

		auto voiceButtonField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_voiceButton");
		Il2CppObject* voiceButton;
		il2cpp_field_get_value(_this, voiceButtonField, &voiceButton);

		auto buttonField = il2cpp_class_get_field_from_name_wrap(voiceButton->klass, "_playVoiceButton");
		Il2CppObject* button;
		il2cpp_field_get_value(voiceButton, buttonField, &button);

		if (button)
		{
			auto callback = GetButtonCommonOnClickDelegate(button);
			if (callback)
			{
				auto newFn = *(
					[](Il2CppObject* storyIdBox)
					{

						int storyId = *reinterpret_cast<int*>(il2cpp_object_unbox(storyIdBox));

						auto masterDataManager = GetSingletonInstance(
							il2cpp_symbols::get_class(
								"umamusume.dll", "Gallop",
								"MasterDataManager"));
						auto masterBannerData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(
							masterDataManager->klass,
							"get_masterBannerData",
							0)->methodPointer(masterDataManager);

						auto bannerList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(
							Il2CppObject*,
							int)>(
								masterBannerData->klass,
								"GetListWithGroupId",
								1)->methodPointer(masterBannerData,
									7);

						FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(
							bannerList->klass, "_items");
						Il2CppArraySize* arr;
						il2cpp_field_get_value(bannerList, itemsField,
							&arr);

						int announceId = -1;

						for (int i = 0; i < arr->max_length; i++)
						{
							auto item = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
							if (item)
							{
								auto typeField = il2cpp_class_get_field_from_name_wrap(
									item->klass, "Type");
								int type;
								il2cpp_field_get_value(item, typeField,
									&type);
								auto conditionValueField = il2cpp_class_get_field_from_name_wrap(
									item->klass, "ConditionValue");
								int conditionValue;
								il2cpp_field_get_value(item,
									conditionValueField,
									&conditionValue);
								if (type == 7 &&
									conditionValue == storyId)
								{
									auto transitionField = il2cpp_class_get_field_from_name_wrap(
										item->klass, "Transition");
									il2cpp_field_get_value(item,
										transitionField,
										&announceId);
									break;
								}
							}
						}

						if (announceId == -1 && storyId < 1005)
						{
							announceId = storyId - 1002;
						}

						auto action = CreateDelegate(storyIdBox, *([](Il2CppObject*) {}));

						il2cpp_symbols::get_method_pointer<void (*)(int,
							Il2CppDelegate*,
							Il2CppDelegate*)>(
								"umamusume.dll", "Gallop",
								"DialogAnnounceEvent", "Open", 3)(announceId, action, action);
					});
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button,
					CreateUnityAction(il2cpp_value_box(il2cpp_symbols::get_class("mscorlib.dll", "System", "Int32"), &partDataId), newFn));
			}
		}
	}

	void* DialogCircleItemDonate_Initialize_orig = nullptr;

	void DialogCircleItemDonate_Initialize_hook(Il2CppObject* _this, Il2CppObject* dialog, Il2CppObject* itemRequestInfo)
	{
		reinterpret_cast<decltype(DialogCircleItemDonate_Initialize_hook)*>(DialogCircleItemDonate_Initialize_orig)(
			_this, dialog, itemRequestInfo);
		auto donateCountField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_donateCount");
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

	void* UIManager_GetCameraSizeByOrientation_orig = nullptr;

	float UIManager_GetCameraSizeByOrientation_hook(int orientation)
	{
		return 5;
	}

	void* UIManager_get_DefaultResolution_orig = nullptr;

	Vector2_t UIManager_get_DefaultResolution_hook()
	{
		int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
		int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();
		return Vector2_t{ static_cast<float>(width), static_cast<float>(height) };
	}

	void* UIManager_WaitBootSetup_orig = nullptr;

	Il2CppObject* UIManager_WaitBootSetup_hook(Il2CppObject* _this)
	{
		auto enumerator = reinterpret_cast<decltype(UIManager_WaitBootSetup_hook)*>(UIManager_WaitBootSetup_orig)(_this);

		auto move_next = il2cpp_class_get_method_from_name_type<bool (*)(
			Il2CppObject * _this)>(enumerator->klass,
				"MoveNext",
				0)->methodPointer;
		while (move_next(enumerator)) {}

		auto defaultResolution = UIManager_get_DefaultResolution_hook();

		auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(_this->klass, "GetCanvasScalerList", 0)->methodPointer(_this);
		for (int i = 0; i < canvasScalerList->max_length; i++)
		{
			auto canvasScaler = canvasScalerList->vector[i];
			if (canvasScaler)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector2_t)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, defaultResolution);

				if (defaultResolution.x < defaultResolution.y)
				{
					float scale = min(1, max(1, defaultResolution.y / 1080) * g_freeform_ui_scale_portrait / (defaultResolution.y / 1080));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
				else
				{
					float scale = min(1, max(1, defaultResolution.x / 1920) * g_freeform_ui_scale_landscape / (defaultResolution.x / 1920));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
			}
		}

		auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_bgCamera");
		Il2CppObject* _bgCamera;
		il2cpp_field_get_value(_this, _bgCameraField, &_bgCamera);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
			il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", -1)());

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "AdjustSafeArea", 0)->methodPointer(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "CreateRenderTextureFromScreen", 0)->methodPointer(_this);

		return enumerator;
	}

	void* Input_get_mousePosition_Injected_orig = nullptr;

	void Input_get_mousePosition_Injected_hook(Vector3_t* out)
	{
		reinterpret_cast<decltype(Input_get_mousePosition_Injected_hook)*>(Input_get_mousePosition_Injected_orig)(out);
	}

	void adjust_size()
	{
		thread([]()
			{
				auto tr = il2cpp_thread_attach(il2cpp_domain_get());

				Resolution_t r;
				get_resolution_stub(&r);

				auto target_height = r.height - 100;

				set_resolution(target_height * ratio_vertical, target_height, false);

				il2cpp_thread_detach(tr);
			}).detach();
	}

	void* load_scene_internal_orig = nullptr;
	void* load_scene_internal_hook(Il2CppString* sceneName, int sceneBuildIndex, void* parameters, bool mustCompleteNextFrame)
	{
		wprintf(L"%s\n", sceneName->start_char);
		return reinterpret_cast<decltype(load_scene_internal_hook)*>(load_scene_internal_orig)(sceneName, sceneBuildIndex, parameters, mustCompleteNextFrame);
	}

	void dump_all_entries()
	{
		vector<wstring> static_entries;
		vector<pair<const string, const wstring>> text_id_static_entries;
		vector<pair<const string, const wstring>> text_id_not_matched_entries;
		// 0 is None
		for (int i = 1;; i++)
		{
			auto* str = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(i);

			if (str && *str->start_char)
			{
				if (g_static_entries_use_text_id_name)
				{
					string textIdName = GetTextIdNameById(i);
					text_id_static_entries.emplace_back(textIdName, str->start_char);
					if (local::get_localized_string(textIdName) == nullptr ||
						local::wide_u8(local::get_localized_string(textIdName)->start_char) == local::wide_u8(str->start_char))
					{
						text_id_not_matched_entries.emplace_back(textIdName, str->start_char);
					}
				}
				else if (g_static_entries_use_hash)
				{
					static_entries.emplace_back(str->start_char);
				}
				else
				{
					logger::write_entry(i, str->start_char);
				}
			}
			else
			{
				// check next string, if it's still empty, then we are done!
				auto* nextStr = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(i + 1);
				if (!(nextStr && *nextStr->start_char))
					break;
			}
		}
		if (g_static_entries_use_text_id_name)
		{
			logger::write_text_id_static_dict(text_id_static_entries, text_id_not_matched_entries);
		}
		else if (g_static_entries_use_hash)
		{
			logger::write_static_dict(static_entries);
		}
	}

	discord::Core* discord;

	uint64_t startTime;

	void path_game_assembly()
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			auto binData = il2cpp_symbols::get_class("uncheatercsd.dll", "Uncheater", "SystemBins64");
			auto binArrayField = il2cpp_class_get_field_from_name_wrap(binData->klass, "UNCHEATER_DATA");
			auto array = il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Object"), 128);

			auto byteArray = il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Byte"), 0);

			il2cpp_array_setref(array, 8, byteArray);
			il2cpp_array_setref(array, 9, byteArray);

			il2cpp_field_static_set_value(binArrayField, array);
		}

		if (!mh_inited)
			return;

		printf("Trying to patch GameAssembly.dll...\n");

		auto il2cpp_module = GetModuleHandle("GameAssembly.dll");

		// load il2cpp exported functions
		il2cpp_symbols::init(il2cpp_module);


		if (g_dump_il2cpp)
		{
			il2cpp_dump();
		}

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
		load_assets = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * _this, Il2CppString * name, Il2CppObject * runtimeType)>(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine",
			"AssetBundle", "LoadAsset", 2);

		get_all_asset_names = il2cpp_symbols::get_method_pointer<Il2CppArraySize * (*)(Il2CppObject * _this)>(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine",
			"AssetBundle", "GetAllAssetNames", 0);

		uobject_get_name = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppObject * uObject)>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Object", "GetName", -1);

		uobject_set_name = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppObject * uObject, Il2CppString * name)>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Object", "SetName", 2);

		uobject_IsNativeObjectAlive = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject * uObject)>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Object", "IsNativeObjectAlive", 1);

		get_unityVersion = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Application", "get_unityVersion", -1);

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

		// have to do this way because there's Get(TextId id) and Get(string id)
		// the string one looks like will not be called by elsewhere
		auto localize_get_addr = il2cpp_symbols::find_method("umamusume.dll", "Gallop", "Localize", [](const MethodInfo* method)
			{
				return method->name == "Get"s &&
					method->parameters->parameter_type->type == IL2CPP_TYPE_VALUETYPE;
			});

		auto update_addr = il2cpp_symbols::get_method_pointer(
			"DOTween.dll", "DG.Tweening.Core", "TweenManager", "Update", 3
		);

		auto update_timeline_data_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "StoryTimelineBlockData", "UpdateBlockData", 4
		);

		auto query_setup_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Query", "_Setup", 2
		);

		auto Plugin_sqlite3_step_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Plugin", "sqlite3_step", 1
		);

		auto Plugin_sqlite3_reset_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Plugin", "sqlite3_reset", 1
		);

		auto query_step_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Query", "Step", 0
		);

		auto prepared_query_reset_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"PreparedQuery", "Reset", 0
		);

		auto prepared_query_bind_text_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"PreparedQuery", "BindText", 2
		);

		auto prepared_query_bind_int_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"PreparedQuery", "BindInt", 2
		);

		auto prepared_query_bind_long_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"PreparedQuery", "BindLong", 2
		);

		auto prepared_query_bind_double_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"PreparedQuery", "BindDouble", 2
		);

		auto query_gettext_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Query", "GetText", 1
		);

		query_getint = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*, int)>(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Query", "GetInt", 1
		);

		auto query_dispose_addr = il2cpp_symbols::get_method_pointer(
			"LibNative.Runtime.dll", "LibNative.Sqlite3",
			"Query", "Dispose", 0
		);

		auto MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"MasterCharacterSystemText", "_CreateOrmByQueryResultWithCharacterId", 2
		);

		auto CySpringUpdater_set_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop.Model.Component",
			"CySpringUpdater", "set_SpringUpdateMode", 1
		);

		auto CySpringUpdater_get_SpringUpdateMode_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop.Model.Component",
			"CySpringUpdater", "get_SpringUpdateMode", 0
		);

		auto set_fps_addr = il2cpp_resolve_icall("UnityEngine.Application::set_targetFrameRate(System.Int32)");

		auto wndproc_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "WndProc", 4
		);

		auto StandaloneWindowResize_DisableMaximizebox_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "DisableMaximizebox", -1
		);

		auto StandaloneWindowResize_ReshapeAspectRatio_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "ReshapeAspectRatio", -1
		);

		auto StandaloneWindowResize_KeepAspectRatio_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "KeepAspectRatio", 2
		);

		auto Screen_SetResolution_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "SetResolution", 4
		);

		auto Screen_IsCurrentOrientation_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "IsCurrentOrientation", 1
		);

		auto GallopInput_mousePosition_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopInput", "mousePosition", -1
		);

		auto GallopFrameBuffer_Initialize_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopFrameBuffer", "Initialize", 1
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
			"Display", "get_main", -1);

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

		is_virt = il2cpp_symbols::get_method_pointer<bool(*)()>(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "get_IsVirt", 0
		);

		get_resolution = reinterpret_cast<decltype(get_resolution)>(il2cpp_resolve_icall("UnityEngine.Screen::get_currentResolution_Injected(UnityEngine.Resolution&)"));

		auto story_timeline_controller_play_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryTimelineController", "Play", 0);

		auto story_race_textasset_load_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryRaceTextAsset", "Load", 0);

		auto get_modified_string_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GetModifiedString", -1);

		auto canvas_scaler_setres_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.UI.dll", "UnityEngine.UI",
			"CanvasScaler", "set_referenceResolution", 1
		);

		auto UIManager_UpdateCanvasScaler_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "UIManager", "UpdateCanvasScaler", 1);

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
			"Unity.TextMeshPro.dll", "TMPro", "TMP_Settings", "get_instance", -1);

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

		auto wait_resize_ui_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "UIManager", "WaitResizeUI", 2);

		auto set_anti_aliasing_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_antiAliasing(System.Int32)");

		auto set_shadowResolution_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_shadowResolution(UnityEngine.ShadowResolution)");

		auto set_anisotropicFiltering_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_anisotropicFiltering(UnityEngine.AnisotropicFiltering)");

		auto set_shadows_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_shadows(UnityEngine.ShadowQuality)");

		auto set_softVegetation_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_softVegetation(System.Boolean)");

		auto set_realtimeReflectionProbes_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_realtimeReflectionProbes(System.Boolean)");

		auto Light_set_shadowResolution_addr = il2cpp_resolve_icall("UnityEngine.Light::set_shadowResolution(UnityEngine.Light,UnityEngine.Rendering.LightShadowResolution)");

		auto apply_graphics_quality_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "ApplyGraphicsQuality", 2);

		auto GraphicSettings_GetVirtualResolution3D_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "GetVirtualResolution3D", 1);

		auto GraphicSettings_GetVirtualResolution_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"GraphicSettings", "GetVirtualResolution", 0);

		auto NowLoading_Show_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "NowLoading", "Show", 4);

		auto NowLoading_Hide_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "NowLoading", "Hide", 3);

		auto BootSystem_Awake_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "BootSystem", "Awake", 0);

		UIManager_GetCanvasScalerList = il2cpp_symbols::get_method_pointer<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(
			"umamusume.dll", "Gallop", "UIManager", "GetCanvasScalerList", 0);

		MoviePlayerBase_get_MovieInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerBase", "get_MovieInfo", 0);

		MovieManager_GetMovieInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, MoviePlayerHandle)>(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "GetMovieInfo", 1);

		auto PartsEpisodeList_SetupStoryExtraEpisodeList_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "PartsEpisodeList", "SetupStoryExtraEpisodeList", 4);

		load_from_file = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppString * path)>(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine", "AssetBundle",
			"LoadFromFile", 1);

		auto PathResolver_GetLocalPath_addr = il2cpp_symbols::get_method_pointer(
			"_Cyan.dll", "Cyan.LocalFile", "PathResolver",
			"GetLocalPath", 2);

		Shader_PropertyToID = il2cpp_symbols::get_method_pointer<int (*)(Il2CppString*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Shader", "PropertyToID", 1);

		auto FrameRateController_OverrideByNormalFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByNormalFrameRate", 1);

		auto FrameRateController_OverrideByMaxFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByMaxFrameRate", 1);

		auto FrameRateController_ResetOverride_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ResetOverride", 1);

		auto FrameRateController_ReflectionFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ReflectionFrameRate", 0);

		auto BGManager_CalcBgScale_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "BGManager", "CalcBgScale", 4);

		auto GallopUtil_GotoTitleOnError_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GotoTitleOnError", 1);

		auto DialogCommon_Close_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCommon", "Close", 0);

		auto StoryViewController_ctor_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryViewController", ".ctor", 0);

		auto CriMana_Player_SetFile_addr = il2cpp_symbols::get_method_pointer("CriMw.CriWare.Runtime.dll", "CriWare.CriMana", "Player", "SetFile", 3);

		auto DialogCircleItemDonate_Initialize_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCircleItemDonate", "Initialize", 2);

		auto UIManager_GetCameraSizeByOrientation_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "UIManager", "GetCameraSizeByOrientation", 1);

		auto UIManager_get_DefaultResolution_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "UIManager", "get_DefaultResolution", -1);

		auto UIManager_WaitBootSetup_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "UIManager", "WaitBootSetup", 0);

		auto Object_Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");

		auto Input_get_mousePosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.Input::get_mousePosition_Injected(UnityEngine.Vector3&)");

		auto load_scene_internal_addr = il2cpp_resolve_icall("UnityEngine.SceneManagement.SceneManager::LoadSceneAsyncNameIndexInternal_Injected(System.String,System.Int32,UnityEngine.SceneManagement.LoadSceneParameters&,System.bool)");

#pragma endregion

		ADD_HOOK(Object_Internal_CloneSingleWithParent, "UnityEngine.Object::Internal_CloneSingleWithParent at %p\n");

		// ADD_HOOK(UIManager_WaitBootSetup, "Gallop.UIManager::WaitBootSetup at %p\n");

		ADD_HOOK(PartsEpisodeList_SetupStoryExtraEpisodeList, "Gallop.PartsEpisodeList::SetupStoryExtraEpisodeList at %p\n");

		ADD_HOOK(DialogCircleItemDonate_Initialize, "Gallop.DialogCircleItemDonate::Initialize at %p\n");

		ADD_HOOK(CriMana_Player_SetFile, "CriWare.CriMana.Player::SetFile at %p\n");

		// ADD_HOOK(DialogCommon_Close, "Gallop.DialogCommon.Close() at %p\n");

		// ADD_HOOK(GallopUtil_GotoTitleOnError, "Gallop.GallopUtil.GotoTitleOnError() at %p\n");

		ADD_HOOK(set_shadowResolution, "UnityEngine.QualitySettings.set_shadowResolution(ShadowResolution) at %p\n");

		ADD_HOOK(set_anisotropicFiltering, "UnityEngine.QualitySettings.set_anisotropicFiltering(UnityEngine.AnisotropicFiltering) at %p\n");

		ADD_HOOK(set_shadows, "UnityEngine.QualitySettings.set_shadows(UnityEngine.ShadowQuality) at %p\n");

		ADD_HOOK(set_softVegetation, "UnityEngine.QualitySettings.set_softVegetation(System.Boolean) at %p\n");

		ADD_HOOK(set_realtimeReflectionProbes, "UnityEngine.QualitySettings.set_realtimeReflectionProbes(System.Boolean) at %p\n");

		ADD_HOOK(Light_set_shadowResolution, "UnityEngine.Light.set_shadowResolution(UnityEngine.Rendering.LightShadowResolution) at %p\n");

		ADD_HOOK(NowLoading_Show, "Gallop.NowLoading::Show at %p\n");

		ADD_HOOK(NowLoading_Hide, "Gallop.NowLoading::Hide at %p\n");

		ADD_HOOK(PathResolver_GetLocalPath, "Cyan.Loader.PathResolver::GetLocalPath at %p\n");

		ADD_HOOK(get_preferred_width, "UnityEngine.TextGenerator::GetPreferredWidth at %p\n");

		ADD_HOOK(an_text_fix_data, "AnimateToUnity.AnText::_FixData at %p\n");

		ADD_HOOK(an_text_set_material_to_textmesh, "AnimateToUnity.AnText::_SetMaterialToTextMesh at %p\n");

		ADD_HOOK(load_zekken_composite_resource, "Gallop.ModelLoader::LoadZekkenCompositeResource at %p\n");

		// hook UnityEngine.TextGenerator::PopulateWithErrors to modify text
		ADD_HOOK(populate_with_errors, "UnityEngine.TextGenerator::PopulateWithErrors at %p\n");

		// ADD_HOOK(text_get_text, "UnityEngine.UI.Text::get_text at %p\n");

		ADD_HOOK(textcommon_SetTextWithLineHeadWrap, "Gallop.TextCommon::SetTextWithLineHeadWrap at %p\n");
		ADD_HOOK(textcommon_SetTextWithLineHeadWrapWithColorTag, "Gallop.TextCommon::SetTextWithLineHeadWrapWithColorTag at %p\n");
		ADD_HOOK(textcommon_SetSystemTextWithLineHeadWrap, "Gallop.TextCommon::SetSystemTextWithLineHeadWrap at %p\n");

		ADD_HOOK(localizeextension_text, "Gallop.LocalizeExtention.Text(TextId) at %p\n");
		// Looks like they store all localized texts that used by code in a dict
		ADD_HOOK(localize_get, "Gallop.Localize.Get(TextId) at %p\n");

		// ADD_HOOK(text_clip_data_ctor, "Gallop.StoryTimelineTextClipData::ctor at %p\n")

		ADD_HOOK(story_timeline_controller_play, "StoryTimelineController::Play at %p\n");

		ADD_HOOK(story_race_textasset_load, "StoryRaceTextAsset.Load at %p\n");

		ADD_HOOK(get_modified_string, "GallopUtil_GetModifiedString at %p\n");

		ADD_HOOK(update, "DG.Tweening.Core.TweenManager::Update at %p\n");

		ADD_HOOK(query_setup, "Query::_Setup at %p\n");
		ADD_HOOK(query_gettext, "Query::GetString at %p\n");
		ADD_HOOK(query_dispose, "Query::Dispose at %p\n");

		if (!g_replace_text_db_path.empty())
		{
			try
			{
				replacementMDB = new SQLite::Database(g_replace_text_db_path.data());
				ADD_HOOK(Plugin_sqlite3_step, "Plugin::sqlite3_step at %p\n");
				ADD_HOOK(Plugin_sqlite3_reset, "Plugin::sqlite3_reset at %p\n");
				ADD_HOOK(query_step, "Query::Step at %p\n");
				ADD_HOOK(prepared_query_reset, "PreparedQuery::Reset at %p\n");
				ADD_HOOK(prepared_query_bind_text, "PreparedQuery::BindText at %p\n");
				ADD_HOOK(prepared_query_bind_int, "PreparedQuery::BindInt at %p\n");
				ADD_HOOK(prepared_query_bind_long, "PreparedQuery::BindLong at %p\n");
				ADD_HOOK(prepared_query_bind_double, "PreparedQuery::BindDouble at %p\n");
				ADD_HOOK(MasterCharacterSystemText_CreateOrmByQueryResultWithCharacterId,
					"MasterCharacterSystemText::_CreateOrmByQueryResultWithCharacterId at %p\n");
			}
			catch (exception& e)
			{
			}
		}

		if (g_cyspring_update_mode != -1)
		{
			ADD_HOOK(CySpringUpdater_set_SpringUpdateMode, "CySpringUpdater::set_SpringUpdateMode at %p\n");
			ADD_HOOK(CySpringUpdater_get_SpringUpdateMode, "CySpringUpdater::get_SpringUpdateMode at %p\n");

			if (Game::CurrentGameRegion == Game::Region::KOR)
			{
				thread([]()
					{
						il2cpp_thread_attach(il2cpp_domain_get());

						while (true)
						{
							Sleep(500);
							auto threadClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "CySpringController/CySpringThread");

							auto instanceField = il2cpp_class_get_field_from_name_wrap(threadClass, "_instance");
							Il2CppObject* instance;

							il2cpp_field_static_get_value(instanceField, &instance);

							if (instance)
							{
								auto _taskQueueArrayField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_taskQueueArray");
								Il2CppArraySize* _taskQueueArray;

								il2cpp_field_get_value(instance, _taskQueueArrayField, &_taskQueueArray);

								for (int i = 0; i < _taskQueueArray->max_length; i++)
								{
									auto item = reinterpret_cast<Il2CppObject*>(_taskQueueArray->vector[i]);
									if (item)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(item->klass, "set_UpdateMode", 1)->methodPointer(item, g_cyspring_update_mode);
									}
								}
							}
						}
					}).detach();
			}
		}

		// ADD_HOOK(load_scene_internal, "SceneManager::LoadSceneAsyncNameIndexInternal at %p\n");

		ADD_HOOK(BootSystem_Awake, "Gallop.BootSystem::Awake at %p\n");

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			/*auto UncheaterInit_OnApplicationPause_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"", "UncheaterInit", "OnApplicationPause", 1);*/

				/*auto SplashViewController_KakaoStart_addr = il2cpp_symbols::get_method_pointer(
					"umamusume.dll",
					"Gallop", "SplashViewController", "KakaoStart", 0);*/

					// ADD_HOOK(UncheaterInit_OnApplicationPause, "UncheaterInit::OnApplicationPause at %p\n");
					// ADD_HOOK(SplashViewController_KakaoStart, "SplashViewController::KakaoStart at %p\n");
		}

		if (g_unlock_size || g_freeform_window)
		{
			ADD_HOOK(canvas_scaler_setres, "UnityEngine.UI.CanvasScaler::set_referenceResolution at %p\n");
			// ADD_HOOK(UIManager_UpdateCanvasScaler, "Gallop.UIManager::UpdateCanvasScaler at %p\n");
		}

		if (g_replace_to_builtin_font || g_replace_to_custom_font)
		{
			ADD_HOOK(on_populate, "Gallop.TextCommon::OnPopulateMesh at %p\n");
			ADD_HOOK(textcommon_awake, "Gallop.TextCommon::Awake at %p\n");
			ADD_HOOK(TextMeshProUguiCommon_Awake, "Gallop.TextMeshProUguiCommon::Awake at %p\n");
			ADD_HOOK(TMP_Settings_get_instance, "TMPro.TMP_Settings::get_instance at %p\n");
		}

		if (g_max_fps > -1)
		{
			// break 30-40fps limit
			ADD_HOOK(FrameRateController_OverrideByNormalFrameRate, "Gallop.FrameRateController::OverrideByNormalFrameRate at %p\n");
			ADD_HOOK(FrameRateController_OverrideByMaxFrameRate, "Gallop.FrameRateController::OverrideByMaxFrameRate at %p\n");
			ADD_HOOK(FrameRateController_ResetOverride, "Gallop.FrameRateController::ResetOverride at %p\n");
			ADD_HOOK(FrameRateController_ReflectionFrameRate, "Gallop.FrameRateController::ReflectionFrameRate at %p\n");
			ADD_HOOK(set_fps, "UnityEngine.Application.set_targetFrameRate at %p\n");
		}

		if (g_unlock_size || g_freeform_window)
		{
			// ADD_HOOK(wndproc, "Gallop.StandaloneWindowResize::WndProc at %p\n");

			if (g_freeform_window)
			{
				ADD_HOOK(UIManager_GetCameraSizeByOrientation, "Gallop.UIManager::GetCameraSizeByOrientation at %p\n");
				ADD_HOOK(UIManager_get_DefaultResolution, "Gallop.UIManager::get_DefaultResolution at %p\n");
				ADD_HOOK(StandaloneWindowResize_DisableMaximizebox, "Gallop.StandaloneWindowResize::DisableMaximizebox at %p\n");
				ADD_HOOK(StandaloneWindowResize_ReshapeAspectRatio, "Gallop.StandaloneWindowResize::ReshapeAspectRatio at %p\n");
				ADD_HOOK(StandaloneWindowResize_KeepAspectRatio, "Gallop.StandaloneWindowResize::KeepAspectRatio at %p\n");
				ADD_HOOK(Screen_SetResolution, "Gallop.Screen::SetResolution at %p\n");
				ADD_HOOK(Screen_IsCurrentOrientation, "Gallop.Screen::IsCurrentOrientation at %p\n");
				ADD_HOOK(GallopInput_mousePosition, "Gallop.GallopInput::mousePosition at %p\n");
			}
			ADD_HOOK(GallopFrameBuffer_Initialize, "Gallop.GallopFrameBuffer::Initialize at %p\n");
			ADD_HOOK(GallopFrameBuffer_Release, "Gallop.GallopFrameBuffer::Release at %p\n");
			ADD_HOOK(GallopFrameBuffer_ResizeRenderTexture, "Gallop.GallopFrameBuffer::ResizeRenderTexture at %p\n");
		}

		if (g_graphics_quality != -1)
		{
			ADD_HOOK(apply_graphics_quality, "Gallop.GraphicSettings.ApplyGraphicsQuality at %p\n");
		}

		if (g_freeform_window || g_unlock_size || g_resolution_3d_scale != 1.0f)
		{
			// ADD_HOOK(BGManager_CalcBgScale, "Gallop.BGManager::CalcBgScale at %p\n");
		}

		if (g_resolution_3d_scale != 1.0f || g_freeform_window)
		{
			ADD_HOOK(GraphicSettings_GetVirtualResolution3D, "Gallop.GraphicSettings.GetVirtualResolution3D at %p\n");
			ADD_HOOK(GraphicSettings_GetVirtualResolution, "Gallop.GraphicSettings.GetVirtualResolution at %p\n");
		}

		if (g_anti_aliasing != -1)
		{
			ADD_HOOK(set_anti_aliasing, "UnityEngine.QualitySettings.set_antiAliasing(int) at %p\n");
		}
	}

	void InitNotification()
	{
		if (!isRequiredInitNotification)
		{
			return;
		}

		if (notification)
		{
			if (uobject_IsNativeObjectAlive(notification))
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(notification);
			}
			notification = nullptr;
		}

		auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
		auto _noticeCanvasField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_noticeCanvas");
		Il2CppObject* _noticeCanvas;
		il2cpp_field_get_value(uiManager, _noticeCanvasField, &_noticeCanvas);

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_noticeCanvas->klass, "get_transform", 0)->methodPointer(_noticeCanvas);

		auto object = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppString*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Resources", "Load", 2)(
			il2cpp_string_new("UI/Parts/Notification"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		auto instantiated = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingleWithParent", 3)
			(object, transform, false);
		auto helper = new CastHelper{};
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*, uintptr_t*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentFastPath", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "Notification"), &helper->oneFurtherThanResultValue);
		notification = helper->obj;

		auto canvasGroupField = il2cpp_class_get_field_from_name_wrap(notification->klass, "canvasGroup");
		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_fontSize", 1)->methodPointer(_Label, g_character_system_text_caption_font_size);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_FontColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), g_character_system_text_caption_font_color.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineSize", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), g_character_system_text_caption_outline_size.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), g_character_system_text_caption_outline_color.data())));

		auto background = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentInChildren", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(background->klass, "SetAlpha", 1)->methodPointer(background, g_character_system_text_caption_background_alpha);

		auto canvasGroupTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasGroup->klass, "get_transform", 0)->methodPointer(canvasGroup);

		auto position = il2cpp_class_get_method_from_name_type<Vector3_t(*)(Il2CppObject*)>(canvasGroupTransform->klass, "get_position", 0)->methodPointer(canvasGroupTransform);

		position.x = g_character_system_text_caption_position_x;
		position.y = g_character_system_text_caption_position_y;

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Vector3_t)>(canvasGroupTransform->klass, "set_position", 1)->methodPointer(canvasGroupTransform, position);


		auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
		if (gameObject)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, false);
			isRequiredInitNotification = false;
		}
		else
		{
			notification = nullptr;
			isRequiredInitNotification = true;
		}
	}

	Il2CppDelegate* moviePlayerResize = nullptr;

	void patch_after_criware()
	{
		auto get_virt_size_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "getOptimizedWindowSizeVirt", 2
		);

		auto get_hori_size_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"StandaloneWindowResize", "getOptimizedWindowSizeHori", 2
		);

		auto gallop_get_screenwidth_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"Screen", "get_Width", 0
		);

		auto gallop_get_screenheight_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop",
			"Screen", "get_Height", 0
		);

		auto Camera_set_orthographicSize_addr = il2cpp_resolve_icall("UnityEngine.Camera::set_orthographicSize(System.Single)");

		auto RectTransform_get_rect_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_rect_Injected(UnityEngine.Rect&)");

		auto ChangeScreenOrientation_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop",
			"Screen", "ChangeScreenOrientation", 2);

		auto Screen_set_orientation_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Screen", "set_orientation", 1);

		auto Screen_RequestOrientation_addr = il2cpp_resolve_icall("UnityEngine.Screen::RequestOrientation(UnityEngine.ScreenOrientation)");

		auto set_resolution_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution(System.Int32,System.Int32,UnityEngine.FullScreenMode,System.Int32)");

		auto WaitDeviceOrientation_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "Screen", "WaitDeviceOrientation", 1);

		auto GetLimitSize_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "StandaloneWindowResize", "GetLimitSize", -1);

		auto ChangeScreenOrientationPortraitAsync_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "ChangeScreenOrientationPortraitAsync", -1);

		auto get_IsVertical_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "get_IsVertical", -1);

		auto DeviceOrientationGuide_Show_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "DeviceOrientationGuide", "Show", 2);

		auto UIManager_ChangeResizeUIForPC_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "UIManager", "ChangeResizeUIForPC", 2);

		auto MovieManager_SetImageUvRect_addr = il2cpp_symbols::get_method_pointer(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "SetImageUvRect", 2);

		auto MovieManager_SetScreenSize_addr = il2cpp_symbols::get_method_pointer(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "SetScreenSize", 2);

		auto MoviePlayerForUI_AdjustScreenSize_addr = il2cpp_symbols::get_method_pointer(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerForUI", "AdjustScreenSize", 2);

		auto assetbundle_LoadFromFile_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadFromFile_Internal(System.String,System.UInt32,System.UInt64)");

		auto AssetBundleRequest_GetResult_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine", "AssetBundleRequest",
			"GetResult", 0);

		auto assetbundle_load_asset_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)");

		auto assetbundle_unload_addr = il2cpp_symbols::get_method_pointer("UnityEngine.AssetBundleModule.dll", "UnityEngine", "AssetBundle", "Unload", 1);

		auto resources_load_addr = il2cpp_resolve_icall("UnityEngine.ResourcesAPIInternal::Load()");

		auto Sprite_get_texture_addr = il2cpp_resolve_icall("UnityEngine.Sprite::get_texture(UnityEngine.Sprite)");

		auto Renderer_get_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_material", 0);

		auto Renderer_get_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_materials", 0);

		auto Renderer_get_sharedMaterial_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterial", 0);

		auto Renderer_get_sharedMaterials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterials", 0);

		auto Renderer_set_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_material", 1);

		auto Renderer_set_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_materials", 1);

		auto Renderer_set_sharedMaterial_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_sharedMaterial", 1);

		auto Renderer_set_sharedMaterials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_sharedMaterials", 1);

		auto Material_get_mainTexture_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Material", "get_mainTexture", 0);

		auto Material_set_mainTexture_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Material", "set_mainTexture", 1);

		auto Material_SetTextureI4_addr = il2cpp_symbols::find_method("UnityEngine.CoreModule.dll", "UnityEngine", "Material", [](const MethodInfo* method)
			{
				return method->name == "SetTexture"s &&
					method->parameters->parameter_type->type == IL2CPP_TYPE_I4;
			});

		auto Material_GetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::GetTextureImpl(System.String,System.Int32)");

		auto Material_SetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::SetTextureImpl(System.String,System.Int32,UnityEngine.Texture)");

		auto CharaPropRendererAccessor_SetTexture_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "CharaPropRendererAccessor", "SetTexture", 1);

		auto GameObject_GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent(System.Type)");

		auto GameObject_GetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentFastPath(System.Type,System.IntPtr)");

#pragma region LOAD_ASSETBUNDLE
		if (!fontAssets && !g_font_assetbundle_path.empty() && g_replace_to_custom_font)
		{
			auto assetbundlePath = local::u8_wide(g_font_assetbundle_path);
			if (PathIsRelativeW(assetbundlePath.data()))
			{
				assetbundlePath.insert(0, ((wstring)filesystem::current_path().native()).append(L"/"));
			}
			fontAssets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

			if (!fontAssets && filesystem::exists(assetbundlePath))
			{
				cout << "Asset founded but not loaded. Maybe Asset BuildTarget is not for Windows\n";
			}
		}

		if (fontAssets)
		{
			cout << "Asset loaded: " << fontAssets << "\n";
		}

		if (!g_replace_assetbundle_file_path.empty())
		{
			auto assetbundlePath = local::u8_wide(g_replace_assetbundle_file_path);
			if (PathIsRelativeW(assetbundlePath.data()))
			{
				assetbundlePath.insert(0, ((wstring)filesystem::current_path().native()).append(L"/"));
			}

			auto assets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

			if (!assets && filesystem::exists(assetbundlePath))
			{
				cout << "Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows\n";
			}
			else
			{
				cout << "Replacement AssetBundle loaded: " << assets << "\n";
				replaceAssets.emplace_back(assets);
			}
		}

		if (!g_replace_assetbundle_file_paths.empty())
		{
			for (auto it = g_replace_assetbundle_file_paths.begin(); it != g_replace_assetbundle_file_paths.end(); it++)
			{

				auto assetbundlePath = local::u8_wide(*it);
				if (PathIsRelativeW(assetbundlePath.data()))
				{
					assetbundlePath.insert(0, ((wstring)filesystem::current_path().native()).append(L"/"));
				}

				auto assets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

				if (!assets && filesystem::exists(assetbundlePath))
				{
					cout << "Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows\n";
				}
				else if (assets)
				{
					cout << "Replacement AssetBundle loaded: " << assets << "\n";
					replaceAssets.emplace_back(assets);
				}
			}
		}

		if (!replaceAssets.empty())
		{
			for (auto it = replaceAssets.begin(); it != replaceAssets.end(); it++)
			{
				auto names = get_all_asset_names(*it);
				for (int i = 0; i < names->max_length; i++)
				{
					auto u8Name = local::wide_u8(static_cast<Il2CppString*>(names->vector[i])->start_char);
					replaceAssetNames.emplace_back(u8Name);
				}
			}
		}
#pragma endregion

		ADD_HOOK(AssetBundleRequest_GetResult, "UnityEngine.AssetBundleRequest::GetResult at %p\n");

		ADD_HOOK(assetbundle_LoadFromFile, "UnityEngine.AssetBundle::LoadFromFile at %p\n");

		ADD_HOOK(assetbundle_unload, "UnityEngine.AssetBundle::Unload at %p\n");

		ADD_HOOK(resources_load, "UnityEngine.Resources::Load at %p\n");

		if (!replaceAssets.empty())
		{
			ADD_HOOK(assetbundle_load_asset, "UnityEngine.AssetBundle::LoadAsset at %p\n");

			ADD_HOOK(GameObject_GetComponent, "UnityEngine.GameObject::GetComponent at %p\n");

			// ADD_HOOK(GameObject_GetComponentFastPath, "UnityEngine.GameObject::GetComponentFastPath at %p\n");

			ADD_HOOK(Sprite_get_texture, "UnityEngine.Sprite::get_texture at %p\n");

			ADD_HOOK(Renderer_get_material, "UnityEngine.Renderer::get_material at %p\n");

			ADD_HOOK(Renderer_get_materials, "UnityEngine.Renderer::get_materials at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterial, "UnityEngine.Renderer::get_sharedMaterial at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterials, "UnityEngine.Renderer::get_sharedMaterials at %p\n");

			ADD_HOOK(Renderer_set_material, "UnityEngine.Renderer::set_material at %p\n");

			ADD_HOOK(Renderer_set_materials, "UnityEngine.Renderer::set_materials at %p\n");

			ADD_HOOK(Renderer_set_sharedMaterial, "UnityEngine.Renderer::set_sharedMaterial at %p\n");

			ADD_HOOK(Renderer_set_sharedMaterials, "UnityEngine.Renderer::set_sharedMaterials at %p\n");

			ADD_HOOK(Material_get_mainTexture, "UnityEngine.Material::get_mainTexture at %p\n");

			ADD_HOOK(Material_set_mainTexture, "UnityEngine.Material::set_mainTexture at %p\n");

			ADD_HOOK(Material_SetTextureI4, "UnityEngine.Material::SetTexture at %p\n");

			ADD_HOOK(Material_GetTextureImpl, "UnityEngine.Material::GetTextureImpl at %p\n");

			ADD_HOOK(Material_SetTextureImpl, "UnityEngine.Material::SetTextureImpl at %p\n");

			ADD_HOOK(CharaPropRendererAccessor_SetTexture, "Gallop.CharaPropRendererAccessor::SetTexture at %p\n");
		}

		if (g_freeform_window)
		{
			int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
			int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

			il2cpp_symbols::get_method_pointer<void (*)(bool)>("umamusume.dll", "Gallop", "StandaloneWindowResize", "set_IsVirt", 1)(width <= height);

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenWidth");

			auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenHeight");

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

			auto SCREEN_ORIENTATION_CATEGORIES_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "SCREEN_ORIENTATION_CATEGORIES");
			Il2CppObject* SCREEN_ORIENTATION_CATEGORIES;
			il2cpp_field_static_get_value(SCREEN_ORIENTATION_CATEGORIES_Field, &SCREEN_ORIENTATION_CATEGORIES);

			if (width < height)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::Portrait, ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::PortraitUpsideDown, ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::LandscapeLeft, ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::LandscapeRight, ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::AutoRotation, ScreenOrientation::Portrait);
			}
			else
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::Portrait, ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::PortraitUpsideDown, ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::LandscapeLeft, ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::LandscapeRight, ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, ScreenOrientation, ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, ScreenOrientation::AutoRotation, ScreenOrientation::LandscapeLeft);
			}

			ADD_HOOK(Camera_set_orthographicSize, "UnityEngine.Camera::set_orthographicSize at %p\n");
			ADD_HOOK(RectTransform_get_rect_Injected, "UnityEngine.RectTransform::get_rect_Injected at %p\n");

			ADD_HOOK(WaitDeviceOrientation, "Gallop.Screen::WaitDeviceOrientation at %p\n");
			// ADD_HOOK(Screen_set_orientation, "Gallop.Screen::set_orientation at %p\n");
			ADD_HOOK(Screen_RequestOrientation, "UnityEngine.Screen::RequestOrientation at %p\n");
			ADD_HOOK(DeviceOrientationGuide_Show, "DeviceOrientationGuide::Show at %p\n");
			// ADD_HOOK(ChangeScreenOrientation, "ChangeScreenOrientation at %p\n");
			// ADD_HOOK(ChangeScreenOrientationPortraitAsync, "ChangeScreenOrientationPortraitAsync at %p\n");
			ADD_HOOK(get_IsVertical, "get_IsVertical at %p\n");
			// ADD_HOOK(MovieManager_SetScreenSize, "MovieManager::SetScreenSize at %p\n");
			// ADD_HOOK(MovieManager_SetImageUvRect, "MovieManager::SetImageUvRect at %p\n");
			ADD_HOOK(MoviePlayerForUI_AdjustScreenSize, "MoviePlayerForUI::AdjustScreenSize at %p\n");
		}

		if (g_dump_entries)
		{
			dump_all_entries();
		}


		if (g_auto_fullscreen || g_unlock_size || g_freeform_window)
		{
			ADD_HOOK(set_resolution, "UnityEngine.Screen.SetResolution(int, int, bool) at %p\n");
			ADD_HOOK(UIManager_ChangeResizeUIForPC, "Gallop.UIManager::ChangeResizeUIForPC at %p\n");
		}

		if (g_unlock_size || g_freeform_window)
		{
			// break 1080p size limit
			ADD_HOOK(get_virt_size, "Gallop.StandaloneWindowResize.getOptimizedWindowSizeVirt at %p \n");
			ADD_HOOK(get_hori_size, "Gallop.StandaloneWindowResize.getOptimizedWindowSizeHori at %p \n");

			// remove fixed 1080p render resolution
			ADD_HOOK(gallop_get_screenheight, "Gallop.Screen::get_Height at %p\n");
			ADD_HOOK(gallop_get_screenwidth, "Gallop.Screen::get_Width at %p\n");

			ADD_HOOK(GetLimitSize, "Gallop.StandaloneWindowResize::GetChangedSize at %p\n");

			auto display = display_get_main();
			if (g_initial_width > 72 && g_initial_height > 72)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				auto ratio = static_cast<float>(g_initial_width) / static_cast<float>(g_initial_height);

				if (g_initial_width < g_initial_height)
				{
					ratio_vertical = static_cast<float>(g_initial_width) / static_cast<float>(g_initial_height);
					ratio_horizontal = static_cast<float>(g_initial_height) / static_cast<float> (g_initial_width);

					if (g_unlock_size_use_system_resolution)
					{
						last_display_width = static_cast<float>(get_system_width(display)) * ratio;
						last_display_height = get_system_width(display);
					}
					else
					{
						last_display_width = g_initial_width;
						last_display_height = g_initial_height;
					}
				}
				else
				{
					ratio_vertical = static_cast<float>(g_initial_height) / static_cast<float>(g_initial_width);
					ratio_horizontal = static_cast<float>(g_initial_width) / static_cast<float>(g_initial_height);

					if (g_unlock_size_use_system_resolution)
					{
						last_display_width = get_system_width(display);
						last_display_height = static_cast<float>(get_system_width(display)) * ratio;
					}
					else
					{
						last_display_width = g_initial_width;
						last_display_height = g_initial_height;
					}
				}
			}
			else
			{
				if (g_freeform_window)
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
					last_display_height = get_system_height(display);
				}
			}

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenWidth");

			auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenHeight");

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

			thread([]() {
				auto t = il2cpp_thread_attach(il2cpp_domain_get());
				while (true)
				{
					Sleep(100);
					SetBGCanvasScalerSize();
				}
				}).detach();
		}

		if (g_auto_fullscreen || g_unlock_size || g_freeform_window)
		{
			if (g_initial_width > 72 && g_initial_height > 72)
			{
				auto display = display_get_main();
				if (g_initial_width < g_initial_height)
				{
					last_hriz_window_width = last_display_height - 400;
					last_hriz_window_height = last_hriz_window_width / ratio_horizontal;

					if (last_hriz_window_height >= get_system_height(display))
					{
						last_hriz_window_height = get_system_height(display) - 400;
						last_hriz_window_width = last_hriz_window_height * ratio_horizontal;
					}

					last_virt_window_height = last_display_width - 400;
					last_virt_window_width = last_virt_window_height * ratio_vertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;
					}
				}
				else
				{
					last_hriz_window_width = last_display_width - 400;
					last_hriz_window_height = last_hriz_window_width / ratio_horizontal;

					if (last_hriz_window_height >= get_system_height(display))
					{
						last_hriz_window_height = get_system_height(display) - 400;
						last_hriz_window_width = last_hriz_window_height * ratio_horizontal;
					}

					last_virt_window_height = last_display_height - 400;
					last_virt_window_width = last_virt_window_height * ratio_vertical;

					if (last_virt_window_height >= get_system_height(display))
					{
						last_virt_window_height = get_system_height(display) - 400;
						last_virt_window_width = last_virt_window_height * ratio_vertical;
					}
				}
			}
			else
			{
				if (g_freeform_window)
				{
					last_hriz_window_width = last_display_width;
					last_hriz_window_height = last_display_height;
					last_virt_window_height = last_display_height;
					last_virt_window_width = last_display_width;
				}
				else
				{
					last_hriz_window_width = last_display_width - 400;
					last_hriz_window_height = last_hriz_window_width / ratio_horizontal;
					last_virt_window_height = last_display_height - 400;
					last_virt_window_width = last_virt_window_height * ratio_vertical;
				}
			}


			if (g_auto_fullscreen)
			{
				// adjust_size();
			}
		}

		if (g_discord_rich_presence)
		{
			discord::Core::Create(1080397170215223367, static_cast<uint64_t>(discord::CreateFlags::NoRequireDiscord), &discord);
			if (discord)
			{
				startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				thread t([]()
					{
						il2cpp_thread_attach(il2cpp_domain_get());
						while (true)
						{
							auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
							if (director)
							{
								float currentTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveCurrentTime", 0)->methodPointer(director);
								float totalTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveTotalTime", 0)->methodPointer(director);
								string name;

								auto titleField = il2cpp_class_get_field_from_name_wrap(director->klass, "TitleController");
								Il2CppObject* title;
								il2cpp_field_get_value(director, titleField, &title);
								if (title)
								{
									auto textField = il2cpp_class_get_field_from_name_wrap(title->klass, "_titleText");
									Il2CppObject* text;
									il2cpp_field_get_value(title, textField, &text);
									if (text)
									{
										name = local::wide_u8(text_get_text(text)->start_char);
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
									activity.GetTimestamps().SetEnd(
										(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() + (static_cast<int64_t>(totalTime) * 1000)) - (static_cast<int64_t>(currentTime) * 1000));
									discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
								}
							}
							else
							{
								auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
								auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
								if (uiManager && sceneManager)
								{
									string detail;

									auto CommonHeaderTitle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(uiManager->klass, "get_CommonHeaderTitle", 0)->methodPointer(uiManager);

									if (CommonHeaderTitle)
									{
										auto _cacheTextField = il2cpp_class_get_field_from_name_wrap(CommonHeaderTitle->klass, "_cacheText");
										Il2CppString* _cacheText;
										il2cpp_field_get_value(CommonHeaderTitle, _cacheTextField, &_cacheText);

										if (_cacheText)
										{
											detail = local::wide_u8(_cacheText->start_char);
										}
									}

									if (detail.empty())
									{
										auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);
										auto viewName = local::wide_u8(GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "SceneDefine/ViewId"), viewId)->start_char);
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
										activity.SetType(discord::ActivityType::Playing);
										discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
									}
								}
							}
							Sleep(100);

							if (discord)
							{
								discord->RunCallbacks();
							}
						}
					});
				t.detach();
			}
		}

		if (g_max_fps > -1 || g_unlock_size || g_freeform_window || g_discord_rich_presence || g_character_system_text_caption)
		{
			auto sceneManagerClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager");

			auto activeSceneChangedField = il2cpp_class_get_field_from_name_wrap(sceneManagerClass, "activeSceneChanged");

			auto action = CreateDelegateWithClass(il2cpp_class_from_type(activeSceneChangedField->type),
				reinterpret_cast<Il2CppObject*>(il2cpp_object_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Object"))), *([](Il2CppObject* _this, Scene* scene, Scene* scene1)
					{
						auto hWnd = GetHWND();

						if (g_freeform_window)
						{
							long style = GetWindowLongW(hWnd, GWL_STYLE);
							style |= WS_MAXIMIZEBOX;
							SetWindowLongPtrW(hWnd, GWL_STYLE, style);
						}

						isPortraitBeforeFullscreen = false;

						auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));

						auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
						if (graphicSettings)
						{
							apply_graphics_quality_hook(graphicSettings, g_graphics_quality, true);
						}

						if (scene1)
						{
							string sceneName = local::wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Scene*)>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "Scene", "GetNameInternal", 1)(scene1)->start_char);

							if (sceneName == "Title")
							{
								if (g_character_system_text_caption)
								{
									isRequiredInitNotification = true;
								}

								if (g_max_fps > -1 || g_unlock_size || g_freeform_window)
								{
									if (isWndProcInitRequired)
									{
										isWndProcInitRequired = false;
										auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
										WNDPROC oldWndProcPtr = nullptr;
										WNDPROC newWndProcPtr = nullptr;

										auto oldWndProcPtrField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "oldWndProcPtr");
										auto newWndProcPtrField = il2cpp_class_get_field_from_name_wrap(StandaloneWindowResize, "newWndProcPtr");
										il2cpp_field_static_get_value(oldWndProcPtrField, &oldWndProcPtr);
										il2cpp_field_static_get_value(newWndProcPtrField, &newWndProcPtr);

										reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oldWndProcPtr)));
										auto oldWndProcPtr2 = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc_hook)));
										il2cpp_field_static_set_value(oldWndProcPtrField, &oldWndProcPtr2);

										if ((g_unlock_size || g_freeform_window) && g_initial_width > 72 && g_initial_height > 72)
										{
											if (g_initial_width < g_initial_height)
											{
												reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, 0);
											}
											else
											{
												reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_hriz_window_width, last_hriz_window_height, 3, 0);
											}
										}
									}

									if (uiManager)
									{
										auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager->klass, "_bgCamera");
										Il2CppObject* _bgCamera;
										il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

										/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
											il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", -1)());*/
									}
								}

								if (Game::CurrentGameRegion == Game::Region::KOR)
								{
									auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
									updateVoiceButton = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), uiManager, *([](Il2CppObject* _this)
										{
											UpdateVoiceButton();
											il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateVoiceButton, true);
										}));

									// Delay 50ms
									il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateVoiceButton, true);
								}

								if (g_freeform_window)
								{
									int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
									int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

									bool isVirt = width < height;
									il2cpp_symbols::get_method_pointer<void (*)(bool)>("umamusume.dll", "Gallop", "StandaloneWindowResize", "set_IsVirt", 1)(isVirt);

									auto uiManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "UIManager"));
									moviePlayerResize = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), uiManager, *([](Il2CppObject* _this)
										{
											ResizeMoviePlayer();
											il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, moviePlayerResize, true);
										}));

									// Delay 50ms
									il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, moviePlayerResize, true);
								}

								if (uiManager && (g_unlock_size || g_freeform_window))
								{
									int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", -1)();
									int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", -1)();

									bool isVirt = width < height;

									if (g_freeform_window)
									{
										UIManager_ChangeResizeUIForPC_hook(uiManager, isVirt ? min(last_virt_window_width, last_virt_window_height) : max(last_hriz_window_width, last_hriz_window_height),
											isVirt ? max(last_virt_window_width, last_virt_window_height) : min(last_hriz_window_width, last_hriz_window_height));
									}
									else
									{
										UIManager_ChangeResizeUIForPC_hook(uiManager, isVirt ? min(last_display_width, last_display_height) : max(last_display_width, last_display_height),
											isVirt ? max(last_display_width, last_display_height) : min(last_display_width, last_display_height));
									}
								}
							}

							if (sceneName == "Home")
							{
								if (g_character_system_text_caption)
								{
									InitNotification();
								}

								thread([]()
									{
										auto t = il2cpp_thread_attach(il2cpp_domain_get());

										while (true)
										{
											Sleep(100);
											__try
											{
												Il2CppArraySize_t<Il2CppObject*>* CharacterHomeTopUIList;
												if (Game::CurrentGameRegion == Game::Region::KOR)
												{
													CharacterHomeTopUIList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, int, int)>("UnityEngine.Object::FindObjectsByType()")(
														GetRuntimeType("umamusume.dll", "Gallop", "CharacterHomeTopUI"), 1, 0);
												}
												else
												{
													CharacterHomeTopUIList = il2cpp_resolve_icall_type<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, bool)>("UnityEngine.Object::FindObjectsOfType()")(
														GetRuntimeType("umamusume.dll", "Gallop", "CharacterHomeTopUI"), true);
												}

												for (int i = 0; i < CharacterHomeTopUIList->max_length; i++)
												{
													auto CharacterHomeTopUI = CharacterHomeTopUIList->vector[i];
													if (CharacterHomeTopUI)
													{
														auto _cardRootButtonField = il2cpp_class_get_field_from_name_wrap(CharacterHomeTopUI->klass, "_cardRootButton");
														Il2CppObject* _cardRootButton;
														il2cpp_field_get_value(CharacterHomeTopUI, _cardRootButtonField, &_cardRootButton);

														if (_cardRootButton)
														{
															auto targetText = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_cardRootButton->klass, "get_TargetText", 0)->methodPointer(_cardRootButton);

															if (targetText)
															{
																text_set_horizontalOverflow(targetText, 1);
																text_set_verticalOverflow(targetText, 1);

																il2cpp_thread_detach(t);
																return;
															}
														}
													}
												}
											}
											__except (EXCEPTION_EXECUTE_HANDLER)
											{
											}
										}

									}).detach();
							}

							if (g_discord_rich_presence && discord)
							{
								auto detail = GetSceneName(sceneName);

								discord::Activity activity{};
								activity.GetAssets().SetLargeImage("umamusume");
								activity.SetDetails(detail.data());
								activity.GetTimestamps().SetStart(startTime);
								if (sceneName == "Live"s)
								{
									activity.SetType(discord::ActivityType::Watching);
								}
								else
								{
									activity.SetType(discord::ActivityType::Playing);
								}
								discord->ActivityManager().UpdateActivity(activity, [](discord::Result res) {});
							}
						}
					})
			);
			il2cpp_field_static_set_value(activeSceneChangedField, action);
		}
	}
}

void* OriginalNtCreateSection = nullptr;

NTSTATUS
HookedNtCreateSection(
	_Out_ PHANDLE SectionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG SectionPageProtection,
	_In_ ULONG AllocationAttributes,
	_In_opt_ HANDLE FileHandle
)
{
	DesiredAccess = SECTION_ALL_ACCESS;

	AllocationAttributes &= ~SEC_NO_CHANGE;

	// SEC_IMAGE and pagefile-backed sections may be PAGE_EXECUTE_READWRITE with no need for write access to the file.
	// Additionally, if the originally requested protection was already PAGE_EXECUTE_READWRITE, obviously do not change it.
	/*SectionPageProtection = (AllocationAttributes & SEC_IMAGE) != 0 || FileHandle == nullptr || SectionPageProtection == PAGE_EXECUTE_READWRITE
		? PAGE_EXECUTE_READWRITE
		: PAGE_EXECUTE_WRITECOPY;*/
	if (SectionPageProtection == PAGE_EXECUTE_READ)
	{
		SectionPageProtection = PAGE_EXECUTE_READWRITE;
	}

	auto res = reinterpret_cast<decltype(HookedNtCreateSection)*>(OriginalNtCreateSection)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);

	if (res != 0)
	{
		cout << "NtCreateSection: " << hex << res << dec << endl;
	}

	return res;
}

void* OriginalNtMapViewOfSection = nullptr;

NTSTATUS
HookedNtMapViewOfSection(
	_In_ HANDLE SectionHandle,
	_In_ HANDLE ProcessHandle,
	_Inout_ _At_(*BaseAddress, _Readable_bytes_(*ViewSize) _Writable_bytes_(*ViewSize) _Post_readable_byte_size_(*ViewSize)) PVOID* BaseAddress,
	_In_ ULONG_PTR ZeroBits,
	_In_ SIZE_T CommitSize,
	_Inout_opt_ PLARGE_INTEGER SectionOffset,
	_Inout_ PSIZE_T ViewSize,
	_In_ SECTION_INHERIT InheritDisposition,
	_In_ ULONG AllocationType,
	_In_ ULONG Win32Protect
)
{
	AllocationType &= ~SEC_NO_CHANGE;

	// Assume section is SEC_COMMIT and backed by a real file.
	// If PAGE_EXECUTE_READWRITE protection is desired, call NtQuerySection first to verify the section is SEC_IMAGE.
	if (Win32Protect == PAGE_EXECUTE_READ)
	{
		Win32Protect = PAGE_EXECUTE_READWRITE;
	}

	return reinterpret_cast<decltype(HookedNtMapViewOfSection)*>(OriginalNtMapViewOfSection)(SectionHandle, ProcessHandle, BaseAddress, ZeroBits,
		CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Win32Protect);
}

void* MessageBoxW_orig = nullptr;

int
WINAPI
MessageBoxW_hook(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	if (wstring(lpCaption).starts_with(L"AppSign"))
	{
		lpText = L"Wellbia AppSign이 의도치 않은 동작을 감지하여 프로그램이 종료됩니다.\n\n문제가 지속되는 경우 https://wellbia.com/ 에 방문하거나, support@wellbia.com 으로 문의하시기 바랍니다.";
		lpCaption = L"Warning";
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
	if (nIndex == GWL_STYLE)
	{
		dwNewLong |= WS_MAXIMIZEBOX;
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
	if (nIndex == GWL_STYLE)
	{
		dwNewLong |= WS_MAXIMIZEBOX;
	}
	return reinterpret_cast<decltype(SetWindowLongPtrA)*>(SetWindowLongPtrA_orig)(hWnd, nIndex, dwNewLong);
}

void* ExitProcess_orig = nullptr;

DECLSPEC_NORETURN
VOID
WINAPI
ExitProcess_hook(
	_In_ UINT uExitCode
)
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		auto module_path = filesystem::current_path();
		auto uncheater_path_new = module_path.string().append("\\umamusume_Data\\StreamingAssets\\_Uncheater"s).data();
		auto uncheater_path = module_path.string().append("\\umamusume_Data\\StreamingAssets\\Uncheater"s).data();
		if (filesystem::exists(uncheater_path_new))
		{
			try
			{
				filesystem::rename(uncheater_path_new, uncheater_path);
			}
			catch (exception& e)
			{
				cout << "Uncheater rename error: " << e.what() << endl;
			}
		}
	}

	reinterpret_cast<decltype(ExitProcess)*>(ExitProcess_orig)(uExitCode);
}

bool init_hook_base()
{
	if (mh_inited)
		return false;

	if (MH_Initialize() != MH_OK)
		return false;

	mh_inited = true;

	MH_CreateHook(MessageBoxW, MessageBoxW_hook, &MessageBoxW_orig);
	MH_EnableHook(MessageBoxW);

	MH_CreateHook(LoadLibraryExW, load_library_ex_w_hook, &load_library_ex_w_orig);
	MH_EnableHook(LoadLibraryExW);

	MH_CreateHook(LoadLibraryW, load_library_w_hook, &load_library_w_orig);
	MH_EnableHook(LoadLibraryW);

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		MH_CreateHook(ExitProcess, ExitProcess_hook, &ExitProcess_orig);
		MH_EnableHook(ExitProcess);
	}

	return true;
}

bool init_hook()
{
	// auto ntdll = GetModuleHandleA("ntdll.dll");

	/*auto NtCreateSection = GetProcAddress(ntdll, "NtCreateSection");
	MH_CreateHook(NtCreateSection, HookedNtCreateSection, &OriginalNtCreateSection);
	MH_EnableHook(NtCreateSection);*/

	/*auto NtMapViewOfSection = GetProcAddress(ntdll, "NtMapViewOfSection");
	MH_CreateHook(NtMapViewOfSection, HookedNtMapViewOfSection, &OriginalNtMapViewOfSection);
	MH_EnableHook(NtMapViewOfSection);*/

	/*MH_CreateHook(VirtualProtect, VirtualProtect_hook, &VirtualProtect_orig);
	MH_EnableHook(VirtualProtect);*/

	fullScreenFl = g_auto_fullscreen && !g_freeform_window;

	if (g_freeform_window)
	{
		MH_CreateHook(SetWindowLongPtrW, SetWindowLongPtrW_hook, &SetWindowLongPtrW_orig);
		MH_EnableHook(SetWindowLongPtrW);
		MH_CreateHook(SetWindowLongPtrA, SetWindowLongPtrA_hook, &SetWindowLongPtrA_orig);
		MH_EnableHook(SetWindowLongPtrA);
	}

	while (true)
	{
		auto il2cpp = GetModuleHandleW(L"GameAssembly.dll");

		if (!il2cpp)
		{
			Sleep(1000);
		}
		else
		{
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

			const auto il2cpp_init_addr = GetProcAddress(il2cpp, "il2cpp_init");
			MH_CreateHook(il2cpp_init_addr, il2cpp_init_hook, &il2cpp_init_orig);
			MH_EnableHook(il2cpp_init_addr);

			il2cpp_init_hook("IL2CPP Root Domain");
			break;
		}
	}
	// reinterpret_cast<decltype(il2cpp_init_hook)*>(il2cpp_init_orig)("IL2CPP Root Domain");

	return true;
}

void uninit_hook()
{
	if (!mh_inited)
		return;

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
