#include "hook.h"

#include <stdinclude.hpp>

#include <Shlobj.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>

#include <array>

#include <algorithm>

#include <chrono>

#include <set>
#include <sstream>

#include <format>

#include <Tlhelp32.h>

#include <regex>

#include <psapi.h>
#include <wininet.h>

#include <wrl.h>
#include <wil/com.h>

#include <propkey.h>
#include <propvarutil.h>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <windows.ui.notifications.h>
#include <winrt/Windows.Foundation.h>

#include <WinTrust.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "ntdll.h"

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

#include "scripts/mscorlib/System/Boolean.hpp"
#include "scripts/mscorlib/System/Enum.hpp"
#include "scripts/mscorlib/System/Int32.hpp"
#include "scripts/mscorlib/System/Nullable.hpp"
#include "scripts/mscorlib/System/Collections/Generic/Dictionary.hpp"

#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomEx.hpp"
#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomExPlayback.hpp"

#include "scripts/Cute.Cri.Assembly/Cute/Cri/AudioPlayback.hpp"
#include "scripts/Cute.Cri.Assembly/Cute/Cri/MoviePlayerHandle.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2Int.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Resolution.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ScreenOrientation.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/SceneManagement/Scene.hpp"
#include "scripts/UnityEngine.TextRenderingModule/UnityEngine/TextGenerationSettings.hpp"

#include "scripts/umamusume/Gallop/DialogCommonBase.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"
#include "scripts/umamusume/Gallop/LiveViewController.hpp"
#include "scripts/umamusume/Gallop/RaceCameraManager.hpp"
#include "scripts/umamusume/Gallop/LowResolutionCameraUtil.hpp"
#ifdef _MSC_VER
#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"
#endif

#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredBool.hpp"
#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredInt.hpp"
#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredLong.hpp"

#include "string_utils.hpp"

#include "dllproxy/proxy.hpp"

using namespace std;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace ABI::Windows::Data::Xml::Dom;

namespace
{
	wstring GotoTitleError =
		L"내부적으로 오류가 발생하여 홈으로 이동합니다.\n\n"
		"경우에 따라서 <color=#ff911c><i>타이틀</i></color>로 돌아가거나, \n"
		"게임 <color=#ff911c><i>다시 시작</i></color>이 필요할 수 있습니다.";

	wstring GotoTitleErrorJa =
		L"内部的にエラーが発生し、ホームに移動します。\n\n"
		"場合によっては、<color=#ff911c><i>タイトル</i></color>に戻るか、\n"
		"ゲーム<color=#ff911c><i>再起動</i></color>が必要になる場合がありますあります。";

	void patch_game_assembly();

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
		patch_game_assembly();

		reinterpret_cast<decltype(InitializeApplication_hook)*>(InitializeApplication_orig)();
	}

	void* UncheaterInit_OnApplicationPause_orig;

	void UncheaterInit_OnApplicationPause_hook(Il2CppObject* _this, bool value)
	{
	}

	void PrintStackTrace();

	void* UpdateDispatcher_Initialize_orig = nullptr;
	void UpdateDispatcher_Initialize_hook()
	{
		auto klass = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "UpdateDispatcher");
		auto isQuitField = il2cpp_class_get_field_from_name(klass, "isQuit");
		bool isQuit = false;

		il2cpp_field_static_set_value(isQuitField, &isQuit);
	}

	void* set_resolution_orig;
	void set_resolution_hook(int width, int height, int fullscreenMode, int perferredRefreshRate);

	void init_il2cpp(bool attachIl2CppThread = false)
	{
		il2cpp_symbols::init_defaults();
		il2cpp_symbols::call_init_callbacks();

		Il2CppThread* t = nullptr;

		if (attachIl2CppThread)
		{
			t = il2cpp_thread_attach(il2cpp_domain_get());
		}

		auto InitializeApplication = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GameSystem", "InitializeApplication", IgnoreNumberOfArguments);
		MH_CreateHook(InitializeApplication, InitializeApplication_hook, &InitializeApplication_orig);
		MH_EnableHook(InitializeApplication);

		auto UpdateDispatcher_Initialize_addr = il2cpp_symbols::get_method_pointer("Cute.Core.Assembly.dll", "Cute.Core", "UpdateDispatcher", "Initialize", IgnoreNumberOfArguments);
		MH_CreateHook(UpdateDispatcher_Initialize_addr, UpdateDispatcher_Initialize_hook, &UpdateDispatcher_Initialize_orig);
		MH_EnableHook(UpdateDispatcher_Initialize_addr);

		il2cpp_symbols::get_method_pointer<void (*)()>("UnityEngine.SubsystemsModule.dll", "UnityEngine", "SubsystemManager", ".cctor", IgnoreNumberOfArguments)();
		il2cpp_symbols::get_method_pointer<void (*)()>("UnityEngine.SubsystemsModule.dll", "UnityEngine.SubsystemsImplementation", "SubsystemDescriptorStore", ".cctor", IgnoreNumberOfArguments)();

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			auto set_resolution = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution(System.Int32,System.Int32,UnityEngine.FullScreenMode,System.Int32)");
			MH_CreateHook(set_resolution, set_resolution_hook, &set_resolution_orig);
			MH_EnableHook(set_resolution);

			KillProcessByName(L"ucldr_Umamusume_KR_loader_x64.exe");

			auto UncheaterInit_OnApplicationPause_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"", "UncheaterInit", "OnApplicationPause", 1);

			MH_CreateHook(UncheaterInit_OnApplicationPause_addr, UncheaterInit_OnApplicationPause_hook, &UncheaterInit_OnApplicationPause_orig);
			MH_EnableHook(UncheaterInit_OnApplicationPause_addr);

			patch_game_assembly();
		}

		if (attachIl2CppThread)
		{
			il2cpp_thread_detach(t);
		}
	}

	void* il2cpp_init_orig = nullptr;
	bool __stdcall il2cpp_init_hook(const char* domain_name)
	{
		const auto result = reinterpret_cast<decltype(il2cpp_init_hook)*>(il2cpp_init_orig)(domain_name);

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

	Il2CppObject* notification;

	bool isRequiredInitNotification = true;

	Il2CppObject* GetSingletonInstance(Il2CppClass* klass);

	bool (*uobject_IsNativeObjectAlive)(Il2CppObject* uObject);

	template<typename... T, typename R>
	Il2CppMulticastDelegate* CreateDelegateWithClass(Il2CppClass* klass, Il2CppObject* target, R(*fn)(Il2CppObject*, T...));

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

	void ShowUINotification(Il2CppString* text)
	{
		Gallop::UIManager::Instance().ShowNotification(text);
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

	unsigned long GetEnumValue(Il2CppObject* runtimeEnum);
	Il2CppObject* ParseEnum(Il2CppObject* runtimeType, const wstring& name);

	void SetNotificationFontSize(int size)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_fontSize", 1)->methodPointer(_Label, size);
	}

	void SetNotificationFontColor(wstring color)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_FontColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), color)));
	}

	void SetNotificationOutlineSize(wstring size)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineSize", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), size)));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_Label->klass, "UpdateOutline", 0)->methodPointer(_Label);
	}

	void SetNotificationOutlineColor(wstring color)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), color)));
	}

	void SetNotificationBackgroundAlpha(float alpha)
	{
		if (!notification)
		{
			return;
		}

		if (!uobject_IsNativeObjectAlive(notification))
		{
			return;
		}

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(notification->klass, "get_gameObject", 0)->methodPointer(notification);
		auto background = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentInChildren", 2)(gameObject, GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(background->klass, "SetAlpha", 1)->methodPointer(background, alpha);
	}

	void SetNotificationPosition(float x, float y)
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

		auto canvasGroupTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasGroup->klass, "get_transform", 0)->methodPointer(canvasGroup);

		auto position = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(canvasGroupTransform->klass, "get_position", 0)->methodPointer(canvasGroupTransform);

		position.x = x;
		position.y = y;

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(canvasGroupTransform->klass, "set_position", 1)->methodPointer(canvasGroupTransform, position);
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

		auto wText = wstring(text->chars);
		replaceAll(wText, L"\n\n", L" ");
		replaceAll(wText, L"\n", L" ");
		if (notification && wstring(cueSheet->chars).find(L"_home_") == string::npos &&
			wstring(cueSheet->chars).find(L"_tc_") == string::npos &&
			wstring(cueSheet->chars).find(L"_title_") == string::npos &&
			wstring(cueSheet->chars).find(L"_kakao_") == string::npos &&
			wstring(cueSheet->chars).find(L"_gacha_") == string::npos &&
			voiceId != 95001 &&
			(characterId < 9000 || voiceId == 95005 || voiceId == 95006 || voiceId == 70000))
		{

			if (wstring(cueSheet->chars).find(L"_training_") != string::npos && (cueId < 29 || cueId == 39))
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

			SetNotificationDisplayTime(length);

			ShowNotification(LineHeadWrap(il2cpp_string_new16(wText.data()), config::character_system_text_caption_line_char_count));
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
		const auto cueSheet16 = u8_wide(cueSheet);
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
				Il2CppArraySize_t<Il2CppObject*>* textArr;
				il2cpp_field_get_value(textList, itemsField, &textArr);

				if (textArr)
				{
					for (int i = 0; i < textArr->max_length; i++)
					{
						auto elem = textArr->vector[i];
						if (elem)
						{
							auto elemCueIdField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueId");
							auto elemCueSheetField = il2cpp_class_get_field_from_name_wrap(elem->klass, "CueSheet");

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
		const auto cueSheet16 = u8_wide(cueSheet);
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

			if (notification && uobject_IsNativeObjectAlive(notification))
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

	void* CriMana_SetFileNew_orig = nullptr;
	void CriMana_SetFileNew_hook(int player_id, void* binder, const char* path)
	{
		string pathString = path;
		replaceAll(pathString, "\\", "/");
		stringstream pathStream(pathString);
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '/'))
		{
			splited.emplace_back(segment);
		}
		auto fileName = u8_wide(splited.back());

		if (config::replace_assets.find(fileName) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(fileName);
			reinterpret_cast<decltype(CriMana_SetFileNew_hook)*>(CriMana_SetFileNew_orig)(player_id, binder, wide_u8(replaceAsset.path).data());
			return;
		}

		reinterpret_cast<decltype(CriMana_SetFileNew_hook)*>(CriMana_SetFileNew_orig)(player_id, binder, path);
	}

	void* CriMana_SetFileAppend_orig = nullptr;
	bool CriMana_SetFileAppend_hook(int player_id, void* binder, const char* path, bool repeat)
	{
		string pathString = path;
		replaceAll(pathString, "\\", "/");
		stringstream pathStream(pathString);
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '/'))
		{
			splited.emplace_back(segment);
		}
		auto fileName = u8_wide(splited.back());

		if (config::replace_assets.find(fileName) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(fileName);
			return reinterpret_cast<decltype(CriMana_SetFileAppend_hook)*>(CriMana_SetFileAppend_orig)(player_id, binder, wide_u8(replaceAsset.path).data(), repeat);
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
		if (wstring(lpLibFileName).find(L"KakaoGame.dll") != wstring::npos)
		{
			auto KakaoGame = reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);

			auto cef = GetModuleHandleW(L"libcef.dll");

			const auto cef_cookie_manager_get_global_manager_addr = reinterpret_cast<decltype(cef_cookie_manager_get_global_manager)>(GetProcAddress(cef, "cef_cookie_manager_get_global_manager"));

			MH_CreateHook(cef_cookie_manager_get_global_manager_addr, cef_cookie_manager_get_global_manager_hook, &cef_cookie_manager_get_global_manager_orig);
			MH_EnableHook(cef_cookie_manager_get_global_manager_addr);

			return KakaoGame;
		}

		return reinterpret_cast<decltype(LoadLibraryExW)*>(load_library_ex_w_orig)(lpLibFileName, hFile, dwFlags);
	}

	HWND GetHWND()
	{
		auto title = config::custom_title_name;
		if (title.empty())
		{
			title = L"umamusume";
		}

		static HWND hWndFound;

		EnumWindows(reinterpret_cast<WNDENUMPROC>(*([](HWND hWnd, LPARAM lParam)
			{
				TCHAR buf[MAX_CLASS_NAME];
				GetClassNameW(hWnd, (LPTSTR)&buf, MAX_CLASS_NAME);

				if (wstring(buf).find(L"UnityWndClass") != wstring::npos)
				{
					DWORD dwWndProcID = 0;
					GetWindowThreadProcessId(hWnd, &dwWndProcID);

					if (dwWndProcID == GetCurrentProcessId())
					{
						hWndFound = hWnd;
						return FALSE;
					}
				}

				return TRUE;
			}
			)), NULL);

		return hWndFound;
	}

	HINSTANCE hInstance;

	void* UnityMain_orig = nullptr;
	int __stdcall UnityMain_hook(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
	{
		::hInstance = hInstance;
		Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
		return reinterpret_cast<decltype(UnityMain_hook)*>(UnityMain_orig)(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	}

	extern "C" __declspec(dllexport) int __stdcall UnityMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
	{
		auto unity = LoadLibraryW(L"UnityPlayer.orig.dll");
		UnityMain_orig = GetProcAddress(unity, "UnityMain");

		return UnityMain_hook(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	}

	Il2CppObject* GetRuntimeType(const char* assemblyName, const char* namespaze, const char* klassName);

	template<typename R>
	Il2CppDelegate* CreateUnityActionStatic(R(*fn)());

	IUnityInterfaces* unityInterfaces;

	void* UnityPluginLoad_orig = nullptr;
	void UnityPluginLoad_hook(IUnityInterfaces* unityInterfaces)
	{
		reinterpret_cast<decltype(UnityPluginLoad_hook)*>(UnityPluginLoad_orig)(unityInterfaces);

		::unityInterfaces = unityInterfaces;

		Unity::OpenXR::InitLibrary(unityInterfaces);

		Unity::OpenXR::DiagnosticReport::StartReport();
		Unity::OpenXR::Init();
		// Unity::OpenXR::Start();
	}

	Il2CppObject* (*display_get_main)();

	int (*get_system_width)(Il2CppObject* _this);

	int (*get_system_height)(Il2CppObject* _this);

	void* KGInterfaceBrokerRequest_orig = nullptr;
	LPCWSTR KGInterfaceBrokerRequest_hook(const wchar_t* request)
	{
		auto text = wide_u8(request);

#ifdef _DEBUG
		replaceAll(text, "gameWeb", "googlePlay");
#endif

		if (text.find("Zinny://Support.setViewSize") != string::npos)
		{
			rapidjson::Document doc;
			doc.Parse(text.data());

			if (!doc.HasParseError())
			{
				auto params = doc.GetArray()[2].GetObjectW();

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

			const auto il2cpp_init_addr = GetProcAddress(il2cpp, "il2cpp_init");

			il2cpp_symbols::init(il2cpp);

			MH_CreateHook(il2cpp_init_addr, il2cpp_init_hook, &il2cpp_init_orig);
			auto result = MH_EnableHook(il2cpp_init_addr);

			if (result != MH_OK)
			{
				cout << "WARN: il2cpp_init Failed: " << MH_StatusToString(result) << " LastError: " << GetLastError() << endl << endl;
				thread([]()
					{
						Sleep(200);

						init_il2cpp(true);
					}
				).detach();
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
			auto hWnd = FindWindowW(L"UnityWndClass", L"umamusume");
			if (hWnd)
			{
				if (!config::custom_title_name.empty())
				{
					SetWindowTextW(hWnd, config::custom_title_name.data());
				}
				if (config::has_json_parse_error)
				{
					MessageBoxW(hWnd, config::json_parse_error_msg.data(), L"Umamusume Localify", MB_OK | MB_ICONWARNING);
				}
			}

			criwareInit = true;
			patch_after_criware();

			// use original function beacuse we have unhooked that
			auto criware = reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);

			if (Game::CurrentGameRegion != Game::Region::KOR)
			{
				MH_DisableHook(LoadLibraryW);
				MH_RemoveHook(LoadLibraryW);
			}

			auto UnityPluginLoad_addr = GetProcAddress(criware, "UnityPluginLoad");

			MH_CreateHook(UnityPluginLoad_addr, UnityPluginLoad_hook, &UnityPluginLoad_orig);
			MH_EnableHook(UnityPluginLoad_addr);

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
				auto CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE8778888A");

				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE310ABCEC");
				}

				if (!CriMana_SetFileNew_addr)
				{
					CriMana_SetFileNew_addr = GetProcAddress(criware, "CRIWARE6A5D4549");
				}

				MH_CreateHook(CriMana_SetFileNew_addr, CriMana_SetFileNew_hook, &CriMana_SetFileNew_orig);
				MH_EnableHook(CriMana_SetFileNew_addr);

				auto CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWAREE7861E0D");

				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWARE925FC233");
				}

				if (!CriMana_SetFileAppend_addr)
				{
					CriMana_SetFileAppend_addr = GetProcAddress(criware, "CRIWARE46A04F06");
				}

				MH_CreateHook(CriMana_SetFileAppend_addr, CriMana_SetFileAppend_hook, &CriMana_SetFileAppend_orig);
				MH_EnableHook(CriMana_SetFileAppend_addr);
			}

			return criware;
		}

		return reinterpret_cast<decltype(LoadLibraryW)*>(load_library_w_orig)(lpLibFileName);
	}

	Il2CppObject* fontAssets = nullptr;

	vector<Il2CppObject*> replaceAssets;

	vector<wstring> replaceAssetNames;

	Il2CppObject* (*load_from_file)(Il2CppString* path);

	Il2CppObject* (*load_asset)(Il2CppObject* _this, Il2CppString* name, Il2CppObject* runtimeType);

	Il2CppArraySize_t<Il2CppString*>* (*get_all_asset_names)(Il2CppObject* _this);

	void PrintStackTrace()
	{
		Il2CppString* (*trace)() = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "StackTraceUtility", "ExtractStackTrace", 0);
		wcout << trace()->chars << endl;
	}

	Il2CppObject* GetRuntimeType(const char* assemblyName, const char* namespaze, const char* klassName)
	{
		return il2cpp_type_get_object(il2cpp_class_get_type(il2cpp_symbols::get_class(assemblyName, namespaze, klassName)));
	}

	Il2CppObject* GetRuntimeType(Il2CppClass* klass)
	{
		return il2cpp_type_get_object(il2cpp_class_get_type(klass));
	}

	template<typename... Ts, typename = Il2CppObject*>
	Il2CppClass* GetGenericClass(Il2CppObject* baseRuntimeType, Ts... runtimeTypes)
	{
		auto typeArray = reinterpret_cast<Il2CppArraySize*>(il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Type"), sizeof...(runtimeTypes)));

		int i = 0;
		for (const auto type : { runtimeTypes... })
		{
			il2cpp_array_setref(typeArray, i, type);
			i++;
		}

		auto runtimeType = il2cpp_class_get_method_from_name_type<Il2CppReflectionRuntimeType * (*)(Il2CppObject*, Il2CppArraySize*)>(baseRuntimeType->klass, "MakeGenericType", 1)->methodPointer(baseRuntimeType, typeArray);
		auto newType = runtimeType->type.type;

		return il2cpp_class_from_type(newType);
	}

	template<typename... Ts, typename = Il2CppObject*>
	const MethodInfo* GetGenericMethod(const MethodInfo* baseMethodInfo, Ts... runtimeTypes)
	{
		auto runtimeMethodInfo = il2cpp_method_get_object(baseMethodInfo, nullptr);
		auto typeArray = reinterpret_cast<Il2CppArraySize*>(il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Type"), sizeof...(runtimeTypes)));

		int i = 0;
		for (const auto type : { runtimeTypes... })
		{
			il2cpp_array_setref(typeArray, i, type);
			i++;
		}

		auto runtimeType = il2cpp_class_get_method_from_name_type<Il2CppReflectionMethod * (*)(Il2CppObject*, Il2CppArraySize*)>(runtimeMethodInfo->object.klass, "MakeGenericMethod", 1)->methodPointer(&runtimeMethodInfo->object, typeArray);
		return il2cpp_method_get_from_reflection(runtimeType);
	}

	template<typename... T, typename R, size_t... S>
	InvokerMethod GetInvokerMethod(R(*fn)(Il2CppObject*, T...), index_sequence<S...>)
	{
		return *([](Il2CppMethodPointer fn, const MethodInfo* method, void* obj, void** params)
			{
				return reinterpret_cast<void* (*)(void*, ...)>(fn)(obj, params[S]...);
			});
	}

	template<typename... T, typename R, size_t... S>
	InvokerMethod GetInvokerMethod(R(*fn)(void*, T...), index_sequence<S...>)
	{
		return *([](Il2CppMethodPointer fn, const MethodInfo* method, void*, void** params)
			{
				return reinterpret_cast<void* (*)(void*, ...)>(fn)(nullptr, params[S]...);
			});
	}

	template<typename... T, typename R>
	Il2CppMulticastDelegate* CreateDelegateWithClass(Il2CppClass* klass, Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(klass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return delegate;
	}

	template<typename... T, typename R>
	Il2CppDelegate* CreateDelegate(Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
	{
		auto delegate = reinterpret_cast<Il2CppDelegate*>(il2cpp_object_new(il2cpp_defaults.delegate_class));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->invoke_impl = methodInfo->invoker_method;

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		il2cpp_gc_wbarrier_set_field(object, reinterpret_cast<void**>(&(delegate)->target), target);

		return delegate;
	}

	template<typename... T, typename R>
	Il2CppMulticastDelegate* CreateUnityAction(Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
	{
		const auto delegateClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction");
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(delegateClass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return delegate;
	}

	template<typename... T, typename R>
	Il2CppReflectionMethod* GetRuntimeMethodInfo(R(*fn)(void*, T...))
	{
		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		return il2cpp_method_get_object(methodInfo, methodInfo->klass);
	}

	template<typename... T, typename R>
	Il2CppMulticastDelegate* CreateDelegateWithClassStatic(Il2CppClass* klass, R(*fn)(void*, T...))
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(klass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return delegate;
	}

	template<typename R>
	Il2CppMulticastDelegate* CreateDelegateWithClassStatic(Il2CppClass* klass, R(*fn)())
	{
		return CreateDelegateWithClassStatic(klass, reinterpret_cast<R(*)(void*)>(fn));
	}

	template<typename... T, typename R>
	Il2CppDelegate* CreateDelegateStatic(R(*fn)(void*, T...))
	{
		auto delegate = reinterpret_cast<Il2CppDelegate*>(il2cpp_object_new(il2cpp_defaults.delegate_class));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->invoke_impl = methodInfo->invoker_method;

		return delegate;
	}

	template<typename R>
	Il2CppDelegate* CreateDelegateStatic(R(*fn)())
	{
		return CreateDelegateStatic(reinterpret_cast<R(*)(void*)>(fn));
	}

	template<typename... T, typename R>
	Il2CppMulticastDelegate* CreateUnityActionStatic(R(*fn)(void*, T...))
	{
		const auto delegateClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction");
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.mono_method_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(delegateClass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return delegate;
	}

	template<typename R>
	Il2CppMulticastDelegate* CreateUnityActionStatic(R(*fn)())
	{
		return CreateUnityActionStatic(reinterpret_cast<R(*)(void*)>(fn));
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
			instanceField = il2cpp_class_get_field_from_name_wrap(klass, "instance");
			if (!instanceField)
			{
				return nullptr;
			}
		}
		Il2CppObject* instance;
		il2cpp_field_static_get_value(instanceField, &instance);
		return instance;
	}

	Il2CppObject* GetSingletonInstanceByMethod(Il2CppClass* klass)
	{
		if (!klass || !klass->parent)
		{
			return nullptr;
		}
		auto get_Instance = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(klass, "get_Instance", IgnoreNumberOfArguments);
		if (get_Instance)
		{
			return get_Instance->methodPointer();
		}
		return nullptr;
	}

	Il2CppObject* GetInt32Instance(int value)
	{
		return il2cpp_value_box(il2cpp_defaults.int32_class, &value);
	}

	Il2CppObject* ParseEnum(Il2CppObject* runtimeType, const wstring& name)
	{
		return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppString*)>("mscorlib.dll", "System", "Enum", "Parse", 2)(runtimeType, il2cpp_string_new16(name.data()));
	}

	Il2CppString* GetEnumName(Il2CppObject* runtimeType, int id)
	{
		return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppObject*, Il2CppObject*)>("mscorlib.dll", "System", "Enum", "GetName", 2)(runtimeType, GetInt32Instance(id));
	}

	unsigned long GetEnumValue(Il2CppObject* runtimeEnum)
	{
		return il2cpp_symbols::get_method_pointer<unsigned long (*)(Il2CppObject*)>("mscorlib.dll", "System", "Enum", "ToUInt64", 1)(runtimeEnum);
	}

	unsigned long GetTextIdByName(const wstring& name)
	{
		return GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), name));
	}

	wstring GetTextIdNameById(int id)
	{
		return GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), id)->chars;
	}

	Il2CppObject* GetCustomFont()
	{
		if (!fontAssets) return nullptr;
		if (!config::font_asset_name.empty())
		{
			return load_asset(fontAssets, il2cpp_string_new16(config::font_asset_name.data()), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font"));
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
		if (!config::tmpro_font_asset_name.empty())
		{
			auto tmpFont = load_asset(fontAssets, il2cpp_string_new16(config::tmpro_font_asset_name.data()), GetRuntimeType("Unity.TextMeshPro.dll", "TMPro", "TMP_FontAsset"));
			return tmpFont ? tmpFont : GetCustomTMPFontFallback();
		}
		return GetCustomTMPFontFallback();
	}

	void* assetbundle_load_asset_orig = nullptr;
	Il2CppObject* assetbundle_load_asset_hook(Il2CppObject* _this, Il2CppString* name, const Il2CppType* type);

	void* assetbundle_load_asset_async_orig = nullptr;
	Il2CppObject* assetbundle_load_asset_async_hook(Il2CppObject* _this, Il2CppString* name, const Il2CppType* type);

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

	Il2CppObject* GetReplacementAssetsAsync(Il2CppString* name, const Il2CppType* type)
	{
		for (auto it = replaceAssets.begin(); it != replaceAssets.end(); it++)
		{
			auto assets = reinterpret_cast<decltype(assetbundle_load_asset_async_hook)*>(assetbundle_load_asset_async_orig)(*it, name, type);
			if (assets)
			{
				return assets;
			}
		}
		return nullptr;
	}

	string GetUnityVersion()
	{
		string version(wide_u8(UnityEngine::Application::unityVersion()->chars));
		return version;
	}

	void* populate_with_errors_orig = nullptr;
	bool populate_with_errors_hook(Il2CppObject* _this, Il2CppString* str, UnityEngine::TextGenerationSettings* settings, void* context)
	{
		return reinterpret_cast<decltype(populate_with_errors_hook)*>(populate_with_errors_orig) (
			_this, local::get_localized_string(str), settings, context
			);
	}

	void* localizeextension_text_orig = nullptr;
	Il2CppString* localizeextension_text_hook(int id)
	{
		auto orig_result = reinterpret_cast<decltype(localizeextension_text_hook)*>(localizeextension_text_orig)(id);
		auto result = config::static_entries_use_text_id_name ?
			local::get_localized_string(GetTextIdNameById(id)) :
			config::static_entries_use_hash ?
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
	float get_preferred_width_hook(void* _this, Il2CppString* str, UnityEngine::TextGenerationSettings* settings)
	{
		return reinterpret_cast<decltype(get_preferred_width_hook)*>(get_preferred_width_orig) (
			_this, local::get_localized_string(str), settings
			);
	}

	void* localize_get_orig = nullptr;
	Il2CppString* localize_get_hook(int id)
	{
		auto orig_result = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(id);
		auto result = config::static_entries_use_text_id_name ?
			local::get_localized_string(GetTextIdNameById(id)) :
			config::static_entries_use_hash ?
			local::get_localized_string(orig_result) : local::get_localized_string(id);

		return result ? result : orig_result;
	}

	void* an_text_set_material_to_textmesh_orig = nullptr;
	void an_text_set_material_to_textmesh_hook(Il2CppObject* _this)
	{
		reinterpret_cast<decltype(an_text_set_material_to_textmesh_hook)*>(an_text_set_material_to_textmesh_orig)(_this);
		if (!(fontAssets && config::replace_to_custom_font)) return;

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
		return reinterpret_cast<decltype(update_hook)*>(update_orig)(_this, updateType, deltaTime * config::ui_animation_scale, independentTime * config::ui_animation_scale);
	}

	unordered_map<void*, SQLite::Statement*> text_queries;
	unordered_map<void*, bool> replacement_queries_can_next;

	void* query_setup_orig = nullptr;
	void query_setup_hook(Il2CppObject* _this, void* conn, Il2CppString* sql)
	{
		reinterpret_cast<decltype(query_setup_hook)*>(query_setup_orig)(_this, conn, sql);

		auto ssql = wstring(sql->chars);

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
				if (MasterDB::replacementMasterDB)
				{
					text_queries.emplace(stmtPtr, new SQLite::Statement(*MasterDB::replacementMasterDB, wide_u8(ssql)));
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
					stmt->bind(idx, wide_u8(text->chars));
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
									cueSheet = wide_u8(
										reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(_this, 1)->chars
									);
									cueSheet1 = stmt->getColumn(1).getString();
								}
								else
								{
									cueId = query_getint(_this, 3);
									cueId1 = stmt->getColumn(3).getInt();
									cueSheet = wide_u8(
										reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(_this, 2)->chars
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
						string cueSheet = wide_u8(
							reinterpret_cast<decltype(query_gettext_hook)*>(query_gettext_orig)(query, 2)->chars
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
		reinterpret_cast<decltype(CySpringUpdater_set_SpringUpdateMode_hook)*>(CySpringUpdater_set_SpringUpdateMode_orig)(_this, config::cyspring_update_mode);
	}

	void* CySpringUpdater_get_SpringUpdateMode_orig = nullptr;
	int CySpringUpdater_get_SpringUpdateMode_hook(Il2CppObject* _this)
	{
		CySpringUpdater_set_SpringUpdateMode_hook(_this, config::cyspring_update_mode);
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

	int (*get_rendering_width)(Il2CppObject* _this);

	int (*get_rendering_height)(Il2CppObject* _this);

	constexpr float ratio_16_9 = 1.778f;
	constexpr float ratio_9_16 = 0.563f;

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

			return Gallop::StandaloneWindowResize::IsVirt() ? h : w;
		}

		return UnityEngine::Screen::width();
	}

	void (*set_scale_factor)(void*, float);

	void* canvas_scaler_setres_orig;
	void canvas_scaler_setres_hook(Il2CppObject* _this, UnityEngine::Vector2 res)
	{
		int width = UnityEngine::Screen::width();
		int height = UnityEngine::Screen::height();

		res.x = width;
		res.y = height;

		if (config::freeform_window)
		{
			if (width < height)
			{
				float scale = min(config::freeform_ui_scale_portrait, max(1.0f, height * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
				set_scale_factor(_this, scale);
			}
			else
			{
				float scale = min(config::freeform_ui_scale_landscape, max(1.0f, width / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
				set_scale_factor(_this, scale);
			}
		}
		else
		{
			// set scale factor to make ui bigger on hi-res screen
			if (width < height)
			{
				float scale = min(config::ui_scale, max(1.0f, height * config::runtime::ratioVertical) * config::ui_scale);
				set_scale_factor(_this, scale);
			}
			else
			{
				float scale = min(config::ui_scale, max(1.0f, width / config::runtime::ratioHorizontal) * config::ui_scale);
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
			canvasScaler, UnityEngine::Vector2{ static_cast<float>(get_system_width(display)), static_cast<float>(get_system_height(display)) });
		reinterpret_cast<decltype(UIManager_UpdateCanvasScaler_hook)*>(
			UIManager_UpdateCanvasScaler_orig)(canvasScaler);
	}

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
			auto _mainBgField = il2cpp_class_get_field_from_name_wrap(bgManager->klass, "_mainBg");
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
			miniDirectors = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			miniDirectors = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), true);
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

							auto TextureResolution = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2Int(*)(Il2CppObject*)>(DirectorUI->klass, "get_TextureResolution", 0)->methodPointer(DirectorUI);

							auto _cameraField = il2cpp_class_get_field_from_name_wrap(cameraController->klass, "_camera");
							Il2CppObject* _camera;
							il2cpp_field_get_value(cameraController, _cameraField, &_camera);

							if (_camera)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2Int)>(cameraController->klass, "ResizeRenderTexture", 1)->methodPointer(cameraController, TextureResolution);

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

	Il2CppObject* GetCurrentViewController()
	{
		auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));

		if (!sceneManager)
		{
			return nullptr;
		}

		auto GetCurrentViewController = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "SceneManager", [](const MethodInfo* info)
			{
				return info->name == "GetCurrentViewController"s && info->methodPointer;
			});

		return GetCurrentViewController(sceneManager);
	}

	Il2CppObject* GetCurrentHubViewChildController()
	{
		auto viewController = GetCurrentViewController();

		if (!viewController)
		{
			return nullptr;
		}

		if (viewController->klass->parent->klass->name == "HubViewControllerBase"s)
		{
			return il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(viewController->klass, "get_ChildCurrentController", 0)->methodPointer(viewController);
		}

		return nullptr;
	}

	bool altEnterPressed = false;

	Il2CppObject* delayTweener;

	void RemakeTextures()
	{
		auto uiManager = Gallop::UIManager::Instance();

		auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(graphicSettings->klass, "Update3DRenderTexture", 0)->methodPointer(graphicSettings);

		Il2CppArraySize_t<Il2CppObject*>* renders;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			renders = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "CutInImageEffectPostRender"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			renders = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "CutInImageEffectPostRender"), true);
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
			cuts = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "LimitBreakCut"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			cuts = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "LimitBreakCut"), true);
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
			raceEffect = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "RaceImageEffect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			raceEffect = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "RaceImageEffect"), true);
		}

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
							auto _drawPassField = il2cpp_class_get_field_from_name_wrap(buffer->klass, "_drawPass");
							uint64_t* _drawPass;
							il2cpp_field_get_value(buffer, _drawPassField, &_drawPass);

							if (!_drawPass)
							{
								uint64_t defPass = 0;
								il2cpp_field_set_value(buffer, _drawPassField, &defPass);
							}


							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(buffer->klass, "RemakeRenderTexture", 0)->methodPointer(buffer);
						}
					}
					else
					{
						break;
					}
				}
			}
		}

		Il2CppArraySize_t<Il2CppObject*>* storyEffect;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			storyEffect = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "StoryImageEffect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			storyEffect = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "StoryImageEffect"), true);
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
			lowResCameras = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "LowResolutionCameraBase"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			lowResCameras = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "LowResolutionCameraBase"), true);
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
			liveTheaterCharaSelects = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "LiveTheaterCharaSelect"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			liveTheaterCharaSelects = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "LiveTheaterCharaSelect"), true);
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
			miniDirectors = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			miniDirectors = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("umamusume.dll", "Gallop", "MiniDirector"), true);
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
			delayTweener = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, &callback->delegate, true);
		}

		auto controller = GetCurrentViewController();

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
						auto texture = uiManager.UITexture();
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
			auto storySceneController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "StoryManager", "get_StorySceneController", IgnoreNumberOfArguments)();
			if (storySceneController)
			{
				auto CurrentDisplayModeField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineController"), "CurrentDisplayMode");
				int mode;
				il2cpp_field_static_get_value(CurrentDisplayModeField, &mode);

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(storySceneController->klass, "ChangeCameraDirection", 1)->methodPointer(storySceneController, mode);
			}
		}
	}

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* _this, UnityEngine::Vector2 dispRectWH, bool isPanScan);

	void ResizeMoviePlayer()
	{
		auto movieManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "get_Instance", IgnoreNumberOfArguments)();

		if (movieManager)
		{
			auto playerDicField = il2cpp_class_get_field_from_name_wrap(movieManager->klass, "playerDic");
			Il2CppObject* playerDic;
			il2cpp_field_get_value(movieManager, playerDicField, &playerDic);

			if (playerDic)
			{
				auto entriesField = il2cpp_class_get_field_from_name_wrap(playerDic->klass, "entries");
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

										if (UnityEngine::Object::Name(parent)->chars == L"MainCanvas"s)
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

													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(fullPlayer->klass, "AdjustMovieSize", 1)->methodPointer(fullPlayer, Gallop::StandaloneWindowResize::IsVirt() ? 0 : 1);

													return;
												}
											}

											auto array2 = getComponents(parentGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
												"umamusume.dll", "Gallop", "StoryView")), true, true, false, false, nullptr);

											if (array2)
											{
												if (array2->max_length > 0)
												{
													auto controller = GetCurrentViewController();

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


	Il2CppObject* GetOptionSlider(const char* name);
	float GetOptionSliderValue(Il2CppObject* slider);

	void WaitForEndOfFrame(void (*fn)())
	{
		try
		{
			auto uiManager = Gallop::UIManager::Instance();
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(uiManager, CreateDelegateStatic(fn));
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "WaitForEndOfFrame error: " << e.ex->message->chars << endl;
		}
	}

	void WaitForEndOfFrame(Il2CppObject* target, void (*fn)(Il2CppObject* _this))
	{
		try
		{
			auto uiManager = Gallop::UIManager::Instance();
			auto delegate = &CreateUnityAction(target, fn)->delegate;
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(uiManager, delegate);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			wcout << "WaitForEndOfFrame error: " << e.ex->message->chars << endl;
		}
	}

	void resizeWindow(HWND _hWnd, int _updateWidth, int _updateHeight)
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
						il2cpp_class_get_method_from_name_type<void (*)()>(StandaloneWindowResize, "DisableWindowHitTest", IgnoreNumberOfArguments)->methodPointer();

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

						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);

						if (lastWidth < lastHeight)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 1, 1);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 2, 1);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 3, 1);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 4, 1);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 5, 1);
						}
						else
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 1, 3);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 2, 3);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 3, 3);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 4, 3);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, 5, 3);
						}

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

							//	auto ids = il2cpp_symbols::get_method_pointer<Il2CppArraySize * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "GetSortingLayerIDsInternal", IgnoreNumberOfArguments)();
							//	for (int i = 0; i < ids->max_length; i++)
							//	{
							//		auto id = reinterpret_cast<int>(ids->vector[i]);
							//		cout << "Layer Id: " << id << endl;
							//		cout << "Layer name: " << wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine", "SortingLayer", "IDToName", 1)(id)->chars) << endl;
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
							/*auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager, "_bgCamera");
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

							// auto _bgCameraSettingsField = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_bgCameraSettings");

							// Il2CppObject* _bgCameraSettings;
							// il2cpp_field_static_get_value(_bgCameraSettingsField, &_bgCameraSettings);

							// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject**)>(uiManager, "EndOrientation", 1)->methodPointer(uiManager, &_bgCameraSettings);

							// il2cpp_field_static_set_value(_bgCameraSettingsField, &_bgCameraSettings);

							if (tapEffectController)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(tapEffectController->klass, "Enable", 0)->methodPointer(tapEffectController);
							}

							Il2CppArraySize_t<Il2CppObject*>* canvasScalerList;
							if (Game::CurrentGameRegion == Game::Region::KOR)
							{
								canvasScalerList = UnityEngine::Object::FindObjectsByType(
									GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
							}
							else
							{
								canvasScalerList = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), true);
							}
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

						auto sizeTupleClass = GetGenericClass(GetRuntimeType(il2cpp_symbols::get_class("mscorlib.dll", "System", "Tuple`2")), GetRuntimeType(il2cpp_defaults.int32_class), GetRuntimeType(il2cpp_defaults.int32_class));
						static Il2CppObject* sizeTuple;
						sizeTuple = il2cpp_object_new(sizeTupleClass);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(sizeTupleClass, ".ctor", 2)->methodPointer(sizeTuple, contentWidth, contentHeight);

						WaitForEndOfFrame(*[]()
							{
								int contentWidth = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sizeTuple->klass, "get_Item1", 0)->methodPointer(sizeTuple);
								int contentHeight = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sizeTuple->klass, "get_Item2", 0)->methodPointer(sizeTuple);

								auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

								auto tapEffectController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "TapEffectController"));

								bool isPortrait = contentWidth < contentHeight;

								auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

								auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

								auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

								auto _originalScreenWidth_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenWidth");

								auto _originalScreenHeight_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "_originalScreenHeight");

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
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int, int)>(renderTexture->klass, ".ctor", 3)->methodPointer(renderTexture, contentWidth, contentHeight, 24);
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
									else
									{
										// Deprecated behavior
										Il2CppObject* _uiCommandBuffer = uiManager._uiCommandBuffer();
										Il2CppObject* _blitToFrameMaterial = uiManager._blitToFrameMaterial();

										if (_uiCommandBuffer)
										{
											auto dest = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppClass*, int)>("UnityEngine.CoreModule.dll", "UnityEngine.Rendering", "RenderTargetIdentifier", "op_Implicit", 1)(
												il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Rendering", "RenderTargetIdentifier"), 1);
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, Il2CppObject*)>(_uiCommandBuffer->klass, "Blit", 3)->methodPointer(_uiCommandBuffer, renderTexture, dest, _blitToFrameMaterial);
										}
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

								auto raceCameraManager = Gallop::RaceCameraManager::Instance();
								if (raceCameraManager)
								{
									raceCameraManager.SetupOrientation(isPortrait ? Gallop::LowResolutionCameraUtil::DrawDirection::Portrait : Gallop::LowResolutionCameraUtil::DrawDirection::Landscape);
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
												float scale = min(config::freeform_ui_scale_portrait, max(1.0f, contentHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(contentHeight / scale), static_cast<float>(contentHeight / scale) });
											}
											else
											{
												float scale = min(config::freeform_ui_scale_landscape, max(1.0f, contentWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(contentHeight / scale), static_cast<float>(contentHeight / scale) });
											}
										}
										else
										{
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(contentHeight), static_cast<float>(contentHeight) });
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

	void* Camera_set_orthographicSize_orig = nullptr;
	void Camera_set_orthographicSize_hook(Il2CppObject* _this, float value)
	{
		if (wstring(UnityEngine::Object::Name(_this)->chars).find(L"UICamera") != wstring::npos)
		{
			auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), _this, *([](Il2CppObject* _this)
				{
					RemakeTextures();
				}));

			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.1, &callback->delegate, true);

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
	void RectTransform_get_rect_Injected_hook(Il2CppObject* _this, UnityEngine::Rect* rect)
	{
		reinterpret_cast<decltype(RectTransform_get_rect_Injected_hook)*>(RectTransform_get_rect_Injected_orig)(_this, rect);

		if (il2cpp_symbols::get_method_pointer<bool (*)()>("umamusume.dll", "Gallop", "GameSystem", "IsExecutingSoftwareReset", IgnoreNumberOfArguments)())
		{
			if (isRequestChangeResolution || config::freeform_window && Gallop::StandaloneWindowResize::IsVirt())
			{
				float tmp = rect->height;
				rect->height = rect->width;
				rect->width = tmp;
			}
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

	// void* set_resolution_orig;
	void set_resolution_hook(int width, int height, int fullscreenMode, int perferredRefreshRate)
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
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(last_virt_window_width, last_virt_window_height, 3, perferredRefreshRate);
			}
			else
			{
				r.height *= 0.825f;
				r.width = r.height * config::runtime::ratioVertical;
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
				r.width = r.height * config::runtime::ratioVertical;
				reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(r.width, r.height, 3, perferredRefreshRate);
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
					localizeextension_text_hook(GetTextIdByName(L"Title0040")),
					localizeextension_text_hook(GetTextIdByName(L"Title0041")),
					CreateDelegateStatic(*[]()
						{
							isExitOpened = false;
							UnityEngine::Application::Exit(0);
						}),
					GetTextIdByName(L"Common0004"),
					GetTextIdByName(L"Common0003"),
					CreateDelegateStatic(*[]()
						{
							isExitOpened = false;
						}),
					2);

		isExitOpened = true;

		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*, bool)>(
			"umamusume.dll", "Gallop", "DialogManager", "PushSystemDialog", 2)(
				dialogData, true);
	}

	Il2CppObject* GetFrontDialog()
	{
		return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "DialogManager", "GetForeFrontDialog", IgnoreNumberOfArguments)();
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

		auto _dataField = il2cpp_class_get_field_from_name_wrap(dialog->klass, "_data");
		Il2CppObject* _data;
		il2cpp_field_get_value(dialog, _dataField, &_data);

		if (!_data)
		{
			return false;
		}

		auto FormTypeField = il2cpp_class_get_field_from_name_wrap(_data->klass, "FormType");

		Gallop::DialogCommonBase::FormType type = Gallop::DialogCommonBase::FormType::WITHOUT_FRAME;
		il2cpp_field_get_value(_data, FormTypeField, &type);

		bool isOneButton = type == Gallop::DialogCommonBase::FormType::SMALL_ONE_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_ONE_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON;
		bool isTwoButton = type == Gallop::DialogCommonBase::FormType::SMALL_TWO_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_TWO_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON;
		bool isThreeButton = type == Gallop::DialogCommonBase::FormType::SMALL_THREE_BUTTON || type == Gallop::DialogCommonBase::FormType::MIDDLE_THREE_BUTTON || type == Gallop::DialogCommonBase::FormType::BIG_THREE_BUTTON;

		auto _currentDialogObjField = il2cpp_class_get_field_from_name_wrap(dialog->klass, "_currentDialogObj");

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

						auto _nextMonthButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_nextMonthButton");
						Il2CppObject* _nextMonthButton;
						il2cpp_field_get_value(stablesPanel, _nextMonthButtonField, &_nextMonthButton);

						auto _skillGetButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_skillGetButton");
						Il2CppObject* _skillGetButton;
						il2cpp_field_get_value(stablesPanel, _skillGetButtonField, &_skillGetButton);

						auto _outingSummerButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_outingSummerButton");
						Il2CppObject* _outingSummerButton;
						il2cpp_field_get_value(stablesPanel, _outingSummerButtonField, &_outingSummerButton);

						auto _trainingButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_trainingButton");
						Il2CppObject* _trainingButton;
						il2cpp_field_get_value(stablesPanel, _trainingButtonField, &_trainingButton);

						auto _outingButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_outingButton");
						Il2CppObject* _outingButton;
						il2cpp_field_get_value(stablesPanel, _outingButtonField, &_outingButton);

						Il2CppObject* outingButton = nullptr;
						if (_outingButton)
						{
							outingButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_outingButton->klass, "get_Button", 0)->methodPointer(_outingButton);
						}

						auto _hospitalButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_hospitalButton");
						Il2CppObject* _hospitalButton;
						il2cpp_field_get_value(stablesPanel, _hospitalButtonField, &_hospitalButton);

						Il2CppObject* hospitalButton = nullptr;
						if (_hospitalButton)
						{
							hospitalButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_hospitalButton->klass, "get_Button", 0)->methodPointer(_hospitalButton);
						}

						auto _raceRegistButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_raceRegistButton");
						Il2CppObject* _raceRegistButton;
						il2cpp_field_get_value(stablesPanel, _raceRegistButtonField, &_raceRegistButton);

						Il2CppObject* raceRegistButton = nullptr;
						if (_raceRegistButton)
						{
							raceRegistButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_raceRegistButton->klass, "get_Button", 0)->methodPointer(_raceRegistButton);
						}

						auto _targetRaceSkillGetButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_targetRaceSkillGetButton");
						Il2CppObject* _targetRaceSkillGetButton;
						il2cpp_field_get_value(stablesPanel, _targetRaceSkillGetButtonField, &_targetRaceSkillGetButton);

						Il2CppObject* targetRaceSkillGetButton = nullptr;
						if (_targetRaceSkillGetButton)
						{
							targetRaceSkillGetButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_targetRaceSkillGetButton->klass, "get_Button", 0)->methodPointer(_targetRaceSkillGetButton);
						}

						auto _targetRaceRegistButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_targetRaceRegistButton");
						Il2CppObject* _targetRaceRegistButton;
						il2cpp_field_get_value(stablesPanel, _targetRaceRegistButtonField, &_targetRaceRegistButton);

						auto _addonScenarioButtonField = il2cpp_class_get_field_from_name_wrap(stablesPanel->klass, "_addonScenarioButton");
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

						auto leftSkipArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_LeftSkipArrowButton", 0)->methodPointer(view);

						auto rightArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_RightArrowButton", 0)->methodPointer(view);

						auto rightSkipArrowButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_RightSkipArrowButton", 0)->methodPointer(view);

						if (wParam == VK_LEFT)
						{
							if (shiftKeyDown)
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
							if (shiftKeyDown)
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
					auto _itemListField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_itemList");
					Il2CppObject* _itemList;
					il2cpp_field_get_value(controller, _itemListField, &_itemList);

					if (_itemList)
					{
						FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(_itemList->klass, "_items");
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
						auto _treeArrayField = il2cpp_class_get_field_from_name_wrap(treeList->klass, "_treeArray");
						Il2CppArraySize_t<Il2CppObject*>* _treeArray;
						il2cpp_field_get_value(treeList, _treeArrayField, &_treeArray);

						if (_treeArray)
						{
							auto treeItem = _treeArray->vector[choiceIndex];
							if (treeItem)
							{
								auto _buttonField = il2cpp_class_get_field_from_name_wrap(treeItem->klass, "_button");
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
								auto enabledObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredBool(*)(Il2CppObject*)>(selectedMenu->klass, "get_IsEnable", 0)->methodPointer(selectedMenu);
								auto enabled = enabledObscured.GetDecrypted();

								if (enabled)
								{
									auto _onClickEnableField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_onClickEnable");
									bool _onClickEnable = true;
									il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);

									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>(footer->klass, "OnClickItem", 2)->methodPointer(footer, selectedItem, selectedMenu);
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

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_preSelectedMenu");
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
										auto _onClickEnableField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_onClickEnable");
										bool _onClickEnable = true;
										il2cpp_field_set_value(footer, _onClickEnableField, &_onClickEnable);

										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>(footer->klass, "OnClickItem", 2)->methodPointer(footer, selectedItem, selectedMenu);
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

								auto _preSelectedMenuField = il2cpp_class_get_field_from_name_wrap(footer->klass, "_preSelectedMenu");
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

		auto _choiceButtonListField = il2cpp_class_get_field_from_name_wrap(choiceController->klass, "_choiceButtonList");
		Il2CppObject* _choiceButtonList;

		il2cpp_field_get_value(choiceController, _choiceButtonListField, &_choiceButtonList);

		if (!_choiceButtonList)
		{
			return false;
		}

		auto choiceButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(_choiceButtonList->klass, "get_Item", 1)->methodPointer(_choiceButtonList, choiceIndex);
		auto _buttonField = il2cpp_class_get_field_from_name_wrap(choiceButton->klass, "_button");
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

		auto _choiceControllerField = il2cpp_class_get_field_from_name_wrap(storyReplay->klass, "_choiceController");
		Il2CppObject* _choiceController;
		il2cpp_field_get_value(storyReplay, _choiceControllerField, &_choiceController);

		if (!_choiceController)
		{
			return false;
		}

		auto _dialogTypeField = il2cpp_class_get_field_from_name_wrap(_choiceController->klass, "_dialogType");
		int _dialogType;
		il2cpp_field_get_value(_choiceController, _dialogTypeField, &_dialogType);

		if (_dialogType == 0)
		{
			return false;
		}

		auto _choicesArrayField = il2cpp_class_get_field_from_name_wrap(_choiceController->klass, "_choicesArray");
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
		auto ButtonField = il2cpp_class_get_field_from_name_wrap(choiceParam->klass, "Button");
		Il2CppObject* Button;

		il2cpp_field_get_value(choiceParam, ButtonField, &Button);

		if (!Button)
		{
			return false;
		}

		auto _buttonField = il2cpp_class_get_field_from_name_wrap(Button->klass, "_button");
		Il2CppObject* _button;

		il2cpp_field_get_value(Button, _buttonField, &_button);

		if (_button)
		{
			PressButton(_button);
			return true;
		}

		return false;
	}

	void MoveLivePlayback(float value)
	{
		auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
		if (director)
		{
			bool isPauseLive = il2cpp_class_get_method_from_name_type<bool (*)()>(director->klass, "IsPauseLive", 0)->methodPointer();

			auto _liveCurrentTimeField = il2cpp_class_get_field_from_name_wrap(director->klass, "_liveCurrentTime");
			il2cpp_field_set_value(director, _liveCurrentTimeField, &value);

			auto LiveTimeController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(director->klass, "get_LiveTimeController", 0)->methodPointer(director);

			if (!isPauseLive)
			{
				// prevent voice desync
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(LiveTimeController->klass, "PauseLive", 0)->methodPointer(LiveTimeController);
			}

			auto _elapsedTimeField = il2cpp_class_get_field_from_name_wrap(LiveTimeController->klass, "_elapsedTime");
			il2cpp_field_set_value(LiveTimeController, _elapsedTimeField, &value);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(LiveTimeController->klass, "set_CurrentTime", 1)->methodPointer(LiveTimeController, value);

			auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));
			auto CriAudioManager = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AudioManager->klass, "get_CriAudioManager", 0)->methodPointer(AudioManager);

			auto audioCtrlDictField = il2cpp_class_get_field_from_name_wrap(CriAudioManager->klass, "audioCtrlDict");
			Il2CppObject* audioCtrlDict;
			il2cpp_field_get_value(CriAudioManager, audioCtrlDictField, &audioCtrlDict);

			auto _songPlaybackField = il2cpp_class_get_field_from_name_wrap(AudioManager->klass, "_songPlayback");
			Cute::Cri::AudioPlayback _songPlayback;
			il2cpp_field_get_value(AudioManager, _songPlaybackField, &_songPlayback);

			auto _songCharaPlaybacksField = il2cpp_class_get_field_from_name_wrap(AudioManager->klass, "_songCharaPlaybacks");
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

			auto poolField = il2cpp_class_get_field_from_name_wrap(audioCtrl->klass, "pool");
			Il2CppObject* pool;
			il2cpp_field_get_value(audioCtrl, poolField, &pool);

			if (pool)
			{
				FieldInfo* sourceListField = il2cpp_class_get_field_from_name_wrap(pool->klass, "sourceList");
				Il2CppObject* sourceList;
				il2cpp_field_get_value(pool, sourceListField, &sourceList);

				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(sourceList->klass, "_items");
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
					auto sourceListField = il2cpp_class_get_field_from_name_wrap(cuteAudioSource->klass, "sourceList");
					Il2CppObject* sourceList;
					il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

					auto usingIndexField = il2cpp_class_get_field_from_name_wrap(cuteAudioSource->klass, "usingIndex");
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

					if (isPauseLive)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);
					}

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

					auto poolField = il2cpp_class_get_field_from_name_wrap(audioCtrl->klass, "pool");
					Il2CppObject* pool;
					il2cpp_field_get_value(audioCtrl, poolField, &pool);

					if (pool)
					{
						FieldInfo* sourceListField = il2cpp_class_get_field_from_name_wrap(pool->klass, "sourceList");
						Il2CppObject* sourceList;
						il2cpp_field_get_value(pool, sourceListField, &sourceList);

						FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(sourceList->klass, "_items");
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
							auto sourceListField = il2cpp_class_get_field_from_name_wrap(cuteAudioSource->klass, "sourceList");
							Il2CppObject* sourceList;
							il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

							auto usingIndexField = il2cpp_class_get_field_from_name_wrap(cuteAudioSource->klass, "usingIndex");
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

							if (isPauseLive)
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);
							}

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

	bool isPortraitBeforeFullscreen = false;

	bool isWndProcInitRequired = true;

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

					reinterpret_cast<decltype(set_resolution_hook)*>(set_resolution_orig)(r.width, r.height, fullScreenFl ? 1 : 3, 0);
				}
				return TRUE;

			}
			if (config::max_fps > -1 && wParam == 'F' && altDown)
			{
				config::runtime::useDefaultFPS = !config::runtime::useDefaultFPS;
				UnityEngine::Application::targetFrameRate(30);
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
				auto uiManager = Gallop::UIManager::Instance();
				if (uiManager)
				{
					uiManager.StartCoroutineManaged2(il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(StandaloneWindowResize->klass, "ClearStopFlagAfterWhile", 0)->methodPointer());
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

			if (config::freeform_window)
			{
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				int windowWidth = windowRect.right - windowRect.left,
					windowHeight = windowRect.bottom - windowRect.top;
				resizeWindow(hWnd, windowWidth, windowHeight);
			}
		}

		if ((uMsg == WM_EXITSIZEMOVE || uMsg == WM_SIZE) && config::character_system_text_caption)
		{
			auto callback = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
				{
					auto sliderX = GetOptionSlider("character_system_text_caption_position_x");
					auto sliderY = GetOptionSlider("character_system_text_caption_position_y");

					if (sliderX && sliderY)
					{
						SetNotificationPosition(GetOptionSliderValue(sliderX) / 10, GetOptionSliderValue(sliderY) / 10);
					}
					else
					{
						SetNotificationPosition(config::character_system_text_caption_position_x, config::character_system_text_caption_position_y);
					}
				}))->delegate;

			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.01, callback, true);
		}

		if (uMsg == WM_SIZE && config::freeform_window)
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

		if (uMsg == WM_SIZING && config::freeform_window)
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

				int width = UnityEngine::Screen::width();
				int height = UnityEngine::Screen::height();

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

		if (uMsg == WM_SIZING && !config::freeform_window)
		{
			RECT* rect = reinterpret_cast<RECT*>(lParam);

			float ratio = Gallop::StandaloneWindowResize::IsVirt() ? config::runtime::ratioVertical : config::runtime::ratioHorizontal;

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


			auto uiManager = Gallop::UIManager::Instance();
			if (uiManager)
			{
				bool isVirt = width < height;
				if (config::unlock_size)
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

	bool Screen_IsCurrentOrientation_hook(UnityEngine::ScreenOrientation target)
	{
		return true;
	}

	void* GallopInput_mousePosition_orig = nullptr;

	UnityEngine::Vector3 GallopInput_mousePosition_hook()
	{
		return il2cpp_symbols::get_method_pointer<UnityEngine::Vector3(*)()>("UnityEngine.InputLegacyModule.dll", "UnityEngine", "Input", "get_mousePosition", IgnoreNumberOfArguments)();
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
			if (config::font_asset_name.find(name->chars) == string::npos)
			{
				text_set_font(_this, GetCustomFont());
			}
		}
		auto textId = textcommon_get_TextId(_this);
		if (textId)
		{
			if (GetTextIdByName(L"Common0121") == textId ||
				GetTextIdByName(L"Common0186") == textId ||
				GetTextIdByName(L"Outgame0028") == textId ||
				GetTextIdByName(L"Outgame0231") == textId ||
				GetTextIdByName(L"Outgame0393") == textId ||
				GetTextIdByName(L"SingleMode0265") == textId ||
				GetTextIdByName(L"Character0039") == textId ||
				GetTextIdByName(L"Character0325") == textId)
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
		auto SetColor = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, UnityEngine::Color)>(customFontMaterial->klass, "SetColor", 2)->methodPointer;

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
			auto enumName = GetEnumName(colorType, colorEnum)->chars;
			if (enumName == L"White"s || enumName == L"Black"s)
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
		if (fontAssets && config::replace_to_custom_font)
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

	void* get_modified_string_orig = nullptr;
	Il2CppString* get_modified_string_hook(Il2CppString* text, Il2CppObject* input, bool allowNewLine)
	{
		if (!allowNewLine)
		{
			auto str = wstring(text->chars);
			replaceAll(str, L"\n", L"");
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

	void* rendertexture_set_anti_aliasing_orig = nullptr;
	void rendertexture_set_anti_aliasing_hook(Il2CppObject* _this, int level)
	{
		if (config::anti_aliasing < 1)
		{
			if (config::anti_aliasing < 0)
			{
				reinterpret_cast<decltype(rendertexture_set_anti_aliasing_hook)*>(rendertexture_set_anti_aliasing_orig)(_this, level);
			}
			else
			{
				reinterpret_cast<decltype(rendertexture_set_anti_aliasing_hook)*>(rendertexture_set_anti_aliasing_orig)(_this, 1);
			}
			return;
		}

		reinterpret_cast<decltype(rendertexture_set_anti_aliasing_hook)*>(rendertexture_set_anti_aliasing_orig)(_this, config::anti_aliasing);
	}

	void* set_shadowResolution_orig = nullptr;
	void set_shadowResolution_hook(int level)
	{
		reinterpret_cast<decltype(set_shadowResolution_hook)*>(set_shadowResolution_orig)(3);
	}

	void* set_anisotropicFiltering_orig = nullptr;
	void set_anisotropicFiltering_hook(int mode)
	{
		reinterpret_cast<decltype(set_anisotropicFiltering_hook)*>(set_anisotropicFiltering_orig)(config::anisotropic_filtering);
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
		reinterpret_cast<decltype(apply_graphics_quality_hook)*>(apply_graphics_quality_orig)(_this, config::graphics_quality, true);
		if (config::graphics_quality >= 3)
		{
			set_anti_aliasing_hook(8);
			set_shadowResolution_hook(3);
			set_shadows_hook(2);
			set_softVegetation_hook(true);
			set_realtimeReflectionProbes_hook(true);
		}
	}
	void* GraphicSettings_GetVirtualResolution_orig = nullptr;
	UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution_hook(Il2CppObject* _this)
	{
		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

			int number1920;
			int number1080;

			il2cpp_field_static_get_value(NUMBER1920_Field, &number1920);
			il2cpp_field_static_get_value(NUMBER1080_Field, &number1080);

			if (width < height)
			{
				return UnityEngine::Vector2Int{ number1080, number1920 };
			}

			return UnityEngine::Vector2Int{ number1920, number1080 };
		}

		return reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution_hook)*>(GraphicSettings_GetVirtualResolution_orig)(_this);
	}

	void* GraphicSettings_GetVirtualResolution3D_orig = nullptr;
	UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution3D_hook(Il2CppObject* _this, bool isForcedWideAspect)
	{
		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto NUMBER1920_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1920");

			auto NUMBER1080_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "NUMBER1080");

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

			return UnityEngine::Vector2Int{ number1920, number1080 };
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

	void* PathResolver_GetLocalPath_orig = nullptr;
	Il2CppString* PathResolver_GetLocalPath_hook(Il2CppObject* _this, int kind, Il2CppString* hname)
	{
		if (config::replace_assets.find(hname->chars) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(hname->chars);
			return il2cpp_string_new16(replaceAsset.path.data());
		}
		return reinterpret_cast<decltype(PathResolver_GetLocalPath_hook)*>(PathResolver_GetLocalPath_orig)(_this, kind, hname);
	}

	void* Renderer_get_material_orig = nullptr;
	Il2CppObject* Renderer_get_material_hook(Il2CppObject* _this);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_materials_hook(Il2CppObject* _this);
	Il2CppObject* Renderer_get_sharedMaterial_hook(Il2CppObject* _this);
	Il2CppArraySize_t<Il2CppObject*>* Renderer_get_sharedMaterials_hook(Il2CppObject* _this);

	void ReplaceMaterialTexture(Il2CppObject* material);

	int (*Shader_PropertyToID)(Il2CppString* name);

	Il2CppObject* Material_GetTextureImpl_hook(Il2CppObject* _this, int nameID);
	void Material_SetTextureImpl_hook(Il2CppObject* _this, int nameID, Il2CppObject* texture);
	bool (*Material_HasProperty)(Il2CppObject* _this, int nameID);

	void ReplaceRendererTexture(Il2CppObject* renderer)
	{
		if (!uobject_IsNativeObjectAlive(renderer))
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

	void ReplaceMaterialTextureProperty(Il2CppObject* material, Il2CppString* property)
	{
		if (Material_HasProperty(material, Shader_PropertyToID(property)))
		{
			auto texture = Material_GetTextureImpl_hook(material, Shader_PropertyToID(property));
			if (texture)
			{
				auto uobject_name = UnityEngine::Object::Name(texture);
				// cout << "Material " << wide_u8(property->chars) << " " << wide_u8(uobject_name->chars) << endl;
				if (!wstring(uobject_name->chars).empty())
				{
					auto newTexture = GetReplacementAssets(
						uobject_name,
						(Il2CppType*)GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
					if (newTexture)
					{
						Material_SetTextureImpl_hook(material, Shader_PropertyToID(property), newTexture);
					}
				}
			}
		}
	}

	void ReplaceMaterialTexture(Il2CppObject* material)
	{
		if (!uobject_IsNativeObjectAlive(material))
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
		if (!uobject_IsNativeObjectAlive(holder))
		{
			return;
		}

		auto objectList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_ObjectList", 0)->methodPointer(holder);

		FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(objectList->klass, "_items");
		Il2CppArraySize_t<Il2CppObject*>* arr;
		il2cpp_field_get_value(objectList, itemsField, &arr);

		for (int i = 0; i < arr->max_length; i++)
		{
			auto pair = arr->vector[i];

			if (!pair)
			{
				continue;
			}

			auto field = il2cpp_class_get_field_from_name_wrap(pair->klass, "Value");
			Il2CppObject* obj;
			il2cpp_field_get_value(pair, field, &obj);

			if (obj)
			{
				// cout << "AssetHolder: " << i << " " << obj->klass->name << " " << wide_u8(UnityEngine::Object::Name(obj)->chars) << endl;
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
					if (!wstring(uobject_name->chars).empty())
					{
						auto newTexture = GetReplacementAssets(
							uobject_name,
							reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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
			auto uobject_name = UnityEngine::Object::Name(texture);
			if (uobject_name)
			{
				auto nameU8 = wide_u8(uobject_name->chars);
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

	void ReplaceAnimateToUnityTextures(Il2CppObject* anRoot)
	{
		if (!uobject_IsNativeObjectAlive(anRoot))
		{
			return;
		}

		auto meshParameterGroup = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(anRoot->klass, "get_MeshParameterGroup", 0)->methodPointer(anRoot);

		if (meshParameterGroup)
		{
			auto meshParameterList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(meshParameterGroup->klass, "get_MeshParameterList", 0)->methodPointer(meshParameterGroup);

			if (meshParameterList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(meshParameterList->klass, "_items");
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
								FieldInfo* items1Field = il2cpp_class_get_field_from_name_wrap(meshParameterGroupList->klass, "_items");
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
												if (!wstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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
												if (!wstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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
								FieldInfo* items1Field = il2cpp_class_get_field_from_name_wrap(customMeshInfoParameterList->klass, "_items");
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
												if (!wstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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
												if (!wstring(uobject_name->chars).empty())
												{
													auto newTexture = GetReplacementAssets(
														uobject_name,
														reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
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
		if (!uobject_IsNativeObjectAlive(transform))
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
		if (!uobject_IsNativeObjectAlive(controller))
		{
			return;
		}

		auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_Data", 0)->methodPointer(controller);

		if (data)
		{
			auto worksheetList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(data->klass, "GetWorkSheetList", 0)->methodPointer(data);

			if (worksheetList)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(worksheetList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* items;
				il2cpp_field_get_value(worksheetList, itemsField, &items);

				if (items)
				{
					for (int i = 0; i < items->max_length; i++)
					{
						auto worksheet = items->vector[i];

						if (worksheet)
						{
							auto effectListField = il2cpp_class_get_field_from_name_wrap(worksheet->klass, "_effectList");
							Il2CppObject* effectList;
							il2cpp_field_get_value(worksheet, effectListField, &effectList);

							if (effectList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name_wrap(effectList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(effectList, items1Field, &items1);


								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto effectData = items1->vector[j];

										if (effectData)
										{
											auto keysField = il2cpp_class_get_field_from_name_wrap(effectData->klass, "_keys");
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

							auto bgListField = il2cpp_class_get_field_from_name_wrap(worksheet->klass, "_bgList");
							Il2CppObject* bgList;
							il2cpp_field_get_value(worksheet, bgListField, &bgList);

							if (bgList)
							{
								FieldInfo* items1Field = il2cpp_class_get_field_from_name_wrap(bgList->klass, "_items");
								Il2CppArraySize_t<Il2CppObject*>* items1;
								il2cpp_field_get_value(bgList, items1Field, &items1);


								if (items1)
								{
									for (int j = 0; j < items1->max_length; j++)
									{
										auto effectData = items1->vector[j];

										if (effectData)
										{
											auto keysField = il2cpp_class_get_field_from_name_wrap(effectData->klass, "_keys");
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
		auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppType*)>(gameObject->klass, "GetComponent", 1)->methodPointer;
		auto getComponents =
			il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;

		auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "Object")), true, true, true, false, nullptr);

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

		auto rawImages = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop", "RawImageCommon")),
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

		auto anRoots = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("Plugins.dll", "AnimateToUnity", "AnRoot")),
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

		auto cutInTimelineControllers = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop.CutIn.Cutt", "CutInTimelineController")),
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
			auto transforms = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Transform")),
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

		auto assetholder = getComponent(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop", "AssetHolder")));

		if (assetholder)
		{
			ReplaceAssetHolderTextures(assetholder);
		}
	}

	void ReplaceFontTexture(Il2CppObject* font)
	{
		if (!uobject_IsNativeObjectAlive(font))
		{
			return;
		}

		auto material = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(font->klass, "get_material", 0)->methodPointer(font);

		if (material)
		{
			ReplaceMaterialTexture(material);
		}
	}

	void* assetbundle_LoadFromFile_orig = nullptr;
	Il2CppObject* assetbundle_LoadFromFile_hook(Il2CppString* path, uint32_t crc, uint64_t offset)
	{
		wstringstream pathStream(path->chars);
		wstring segment;
		vector<wstring> splited;
		while (getline(pathStream, segment, L'\\'))
		{
			splited.emplace_back(segment);
		}

		auto name = splited.back();

		if (config::replace_assets.find(name) != config::replace_assets.end())
		{
			auto& replaceAsset = config::replace_assets.at(name);
			auto assets = reinterpret_cast<decltype(assetbundle_LoadFromFile_hook)*>(assetbundle_LoadFromFile_orig)(il2cpp_string_new16(replaceAsset.path.data()), crc, offset);
			replaceAsset.asset = assets;
			return assets;
		}
		auto assetBundle = reinterpret_cast<decltype(assetbundle_LoadFromFile_hook)*>(assetbundle_LoadFromFile_orig)(path, crc, offset);
		/*auto names = get_all_asset_names(assetBundle);
		for (int i = 0; i < names->max_length; i++)
		{
			auto name = reinterpret_cast<Il2CppString*>(names->vector[i]);
			if (!name) continue;
			stringstream pathStream(wide_u8(name->chars));
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
		//			stringstream pathStream(wide_u8(UnityEngine::Object::Name(obj)->chars));
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
		wstringstream pathStream(name->chars);
		wstring segment;
		vector<wstring> splited;
		while (getline(pathStream, segment, L'/'))
		{
			splited.emplace_back(segment);
		}
		auto& fileName = splited.back();
		if (find_if(replaceAssetNames.begin(), replaceAssetNames.end(), [fileName](const wstring& item)
			{
				return item.find(fileName) != wstring::npos;
			}) != replaceAssetNames.end())
		{
			return GetReplacementAssets(il2cpp_string_new16(fileName.data()), type);
		}
		auto obj = reinterpret_cast<decltype(assetbundle_load_asset_hook)*>(assetbundle_load_asset_orig)(_this, name, type);

		// cout << fileName << " " << obj->klass->name << endl;

		if (!obj)
		{
			return nullptr;
		}

		if (obj->klass->name == "GameObject"s)
		{
			ReplaceGameObjectTextures(obj);
		}

		if (obj->klass->name == "Material"s)
		{
			ReplaceMaterialTexture(obj);
		}

		if (obj->klass->name == "Font"s)
		{
			ReplaceFontTexture(obj);
		}

		return obj;
	}

	Il2CppObject* assetbundle_load_asset_async_hook(Il2CppObject* _this, Il2CppString* name, const Il2CppType* type)
	{
		wstringstream pathStream(name->chars);
		wstring segment;
		vector<wstring> splited;
		while (getline(pathStream, segment, L'/'))
		{
			splited.emplace_back(segment);
		}
		auto& fileName = splited.back();
		if (find_if(replaceAssetNames.begin(), replaceAssetNames.end(), [fileName](const wstring& item)
			{
				return item.find(fileName) != string::npos;
			}) != replaceAssetNames.end())
		{
			return GetReplacementAssetsAsync(il2cpp_string_new16(fileName.data()), type);
		}
		return reinterpret_cast<decltype(assetbundle_load_asset_async_hook)*>(assetbundle_load_asset_async_orig)(_this, name, type);
	}

	void* assetbundle_unload_orig = nullptr;
	void assetbundle_unload_hook(Il2CppObject* _this, bool unloadAllLoadedObjects)
	{
		reinterpret_cast<decltype(assetbundle_unload_hook)*>(assetbundle_unload_orig)(_this, unloadAllLoadedObjects);
		for (auto& pair : config::replace_assets)
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
			auto name = UnityEngine::Object::Name(obj);
			wstring wName = name->chars;
			if (find(replaceAssetNames.begin(), replaceAssetNames.end(), wName) != replaceAssetNames.end())
			{
				return GetReplacementAssets(name, il2cpp_class_get_type(obj->klass));
			}

			if (obj->klass->name == "GameObject"s)
			{
				ReplaceGameObjectTextures(obj);
			}
		}
		return obj;
	}

	Il2CppObject* resources_load_hook(Il2CppString* path, Il2CppObject* type);

	Il2CppArraySize_t<Il2CppObject*>* GetRectTransformArray(Il2CppObject* object)
	{
		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(object->klass, "GetComponentsInternal", 6)->methodPointer;
		auto rectTransformArray = getComponents(object, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

		return rectTransformArray;
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

		return nullptr;
	}

	Il2CppObject* CreateGameObject()
	{
		return UnityEngine::GameObject();
	}

	Il2CppObject* AddComponent(Il2CppObject* gameObject, Il2CppObject* componentType)
	{
		return il2cpp_resolve_icall_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.GameObject::Internal_AddComponentWithType()")(gameObject, componentType);
	}

	void AddToLayout(Il2CppObject* parentRectTransform, vector<Il2CppObject*> objects)
	{
		for (int i = objects.size() - 1; i >= 0; i--)
			// for (int i = 0; i < objects.size(); i++)
		{
			auto rectTransform = GetRectTransform(objects[i]);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(rectTransform->klass, "SetParent", 2)->methodPointer(rectTransform, parentRectTransform, false);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(rectTransform->klass, "SetAsFirstSibling", 0)->methodPointer(rectTransform);
		}
	}

	Il2CppObject* GetTextCommon(const char* name)
	{
		auto gameObject = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (gameObject)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

			return array->vector[0];
		}
		return nullptr;
	}

	void SetTextCommonText(Il2CppObject* textCommon, const wchar_t* text)
	{
		text_set_text(textCommon, il2cpp_string_new16(text));

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);
	}

	void SetTextCommonTextWithCustomTag(Il2CppObject* textCommon, const wchar_t* text)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, float, int)>(textCommon->klass, "SetTextWithCustomTag", 3)->methodPointer(textCommon, il2cpp_string_new16(text), 1, 0);

		auto textIdStrField = il2cpp_class_get_field_from_name_wrap(textCommon->klass, "m_textid_str");
		il2cpp_field_set_value(textCommon, textIdStrField, nullptr);
	}

	void SetTextCommonFontColor(Il2CppObject* textCommon, const wchar_t* color)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, unsigned long)>(textCommon->klass, "set_FontColor", 1)->methodPointer(textCommon, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), color)));
	}

	void SetTextCommonOutlineSize(Il2CppObject* textCommon, const wchar_t* size)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, unsigned long)>(textCommon->klass, "set_OutlineSize", 1)->methodPointer(textCommon, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), size)));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(textCommon->klass, "UpdateOutline", 0)->methodPointer(textCommon);
	}

	void SetTextCommonOutlineColor(Il2CppObject* textCommon, const wchar_t* color)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, unsigned long)>(textCommon->klass, "set_OutlineColor", 1)->methodPointer(textCommon, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), color)));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(textCommon->klass, "RebuildOutline", 0)->methodPointer(textCommon);
	}

	Il2CppObject* GetOptionItemTitle(const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemtitle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemTitle = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemTitle->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemTitle, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonText(textCommon, title);

		return optionItemTitle;
	}

	Il2CppObject* GetOptionItemOnOff(const char* name, const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemonoff"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemOnOff = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItemOnOff, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_verticalOverflow(textCommon, 1);
		SetTextCommonText(textCommon, title);

		return optionItemOnOff;
	}

	Il2CppObject* GetOptionItemOnOffQualityRich(const char* name, const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptioniteminfo_qualityrich"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemOnOff = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItemOnOff, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_verticalOverflow(textCommon, 1);
		SetTextCommonText(textCommon, title);

		return optionItemOnOff;
	}

	void SetOptionItemOnOffAction(const char* name, bool isOn, void (*onChange)(Il2CppObject*, bool))
	{
		auto optionItemOnOff = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemOnOff)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "PartsOnOffToggleSwitch")), true, true, false, false, nullptr);

			auto toggleSwitch = array->vector[0];

			auto action = CreateUnityAction(toggleSwitch, onChange);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool, Il2CppDelegate*)>(toggleSwitch->klass, "Setup", 2)->methodPointer(toggleSwitch, isOn, &action->delegate);
		}
	}

	bool GetOptionItemOnOffIsOn(const char* name)
	{
		auto optionItemOnOff = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemOnOff)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionItemOnOff, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "PartsOnOffToggleSwitch")), true, true, false, false, nullptr);

			auto toggleSwitch = array->vector[0];

			return il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(toggleSwitch->klass, "get_IsOn", 0)->methodPointer(toggleSwitch);
		}
		return false;
	}

	Il2CppObject* GetOptionItemButton(const char* name, const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitembutton"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemButton = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItemButton, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemButton->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array1 = getComponents(optionItemButton, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array1->vector[0];

		SetTextCommonText(textCommon, title);

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
			auto array = getComponents(optionItemButton, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "ButtonCommon")), true, true, false, false, nullptr);

			auto buttonCommon = array->vector[0];

			auto action = CreateUnityAction(buttonCommon, onClick);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(buttonCommon->klass, "SetOnClick", 1)->methodPointer(buttonCommon, &action->delegate);
		}
	}

	Il2CppObject* GetOptionItemAttention(const wchar_t* text)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemattention"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemAttention = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemAttention->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemAttention, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonText(textCommon, text);

		return optionItemAttention;
	}

	Il2CppObject* GetOptionItemInfo(const wchar_t* text)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptioniteminfo"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemInfo->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemInfo, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonTextWithCustomTag(textCommon, text);

		auto contentSizeFitter = AddComponent(optionItemInfo, GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto verticalLayoutGroup = AddComponent(optionItemInfo, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 64);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_right", 1)->methodPointer(padding, 64);

		auto _layoutField = il2cpp_class_get_field_from_name_wrap(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		return optionItemInfo;
	}

	Il2CppObject* GetOptionItemSimple(const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemsimple"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemSimple = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemSimple->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItemSimple, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonText(textCommon, title);

		return optionItemSimple;
	}

	Il2CppObject* GetOptionItemSimpleWithButton(const char* name, const wchar_t* title, const wchar_t* text)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemsimple"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItemSimple = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItemSimple, il2cpp_string_new((name + "_simple"s).data()));

		auto rectTransform = GetRectTransform(optionItemSimple);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rectTransform->klass, "set_anchoredPosition", 1)->methodPointer(rectTransform, UnityEngine::Vector2{ 71.583984375, -18 });

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemSimple->klass, "GetComponentsInternal", 6)->methodPointer;

		auto buttonObject = resources_load_hook(il2cpp_string_new("ui/parts/base/buttons00"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto buttons00 = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(buttonObject);

		UnityEngine::Object::Name(buttons00, il2cpp_string_new(name));

		auto array2 = getComponents(buttons00, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ButtonCommon")), true, true, false, false, nullptr);

		auto buttonCommon = array2->vector[0];

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(buttonCommon->klass, "SetInteractable", 1)->methodPointer(buttonCommon, true);

		auto buttonRectTransform = GetRectTransform(buttons00);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(buttonRectTransform->klass, "set_sizeDelta", 1)->methodPointer(buttonRectTransform, UnityEngine::Vector2{ 167, 67 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(buttonRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(buttonRectTransform, UnityEngine::Vector2{ 382.5, 0 });

		AddToLayout(rectTransform, vector{ buttons00 });

		auto array = getComponents(optionItemSimple, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		SetTextCommonText(array->vector[0], text);
		SetTextCommonText(array->vector[1], title);

		return optionItemSimple;
	}

	Il2CppObject* GetOptionItemSimpleWithButtonTextCommon(const char* name)
	{
		auto gameObject = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new((name + "_simple"s).data()));

		if (gameObject)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

			return array->vector[1];
		}
		return nullptr;
	}

	Il2CppObject* GetToggleCommon(const char* name)
	{
		auto toggleObject = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (toggleObject)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(toggleObject->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(toggleObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "ToggleCommon")), true, true, false, false, nullptr);

			auto toggleCommon = array->vector[0];

			return toggleCommon;
		}
		return nullptr;
	}

	Il2CppObject* GetToggleGroupCommon(Il2CppObject* toggleGroupObject)
	{
		if (toggleGroupObject)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(toggleGroupObject->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(toggleGroupObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "ToggleGroupCommon")), true, true, false, false, nullptr);

			auto toggleGroupCommon = array->vector[0];

			return toggleGroupCommon;
		}
		return nullptr;
	}

	Il2CppObject* GetToggleGroupCommon(const char* name)
	{
		auto toggleGroupCommon = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (toggleGroupCommon)
		{
			return GetToggleGroupCommon(toggleGroupCommon);
		}
		return nullptr;
	}

	int GetToggleGroupCommonValue(const char* name)
	{
		auto gameObject = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (gameObject)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "ToggleGroupCommon")), true, true, false, false, nullptr);

			auto toggleGroupCommon = array->vector[0];

			return il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(toggleGroupCommon->klass, "GetOnIndex", 0)->methodPointer(toggleGroupCommon);
		}
		return -1;
	}

	Il2CppObject* GetOptionItem3ToggleVertical(const char* name, const wchar_t* title, const wchar_t* option1, const wchar_t* option2, const wchar_t* option3, int selectedIndex)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem3togglevertical"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItem3ToggleVertical = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItem3ToggleVertical, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItem3ToggleVertical->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItem3ToggleVertical, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		SetTextCommonText(array->vector[0], title);
		SetTextCommonText(array->vector[1], option1);
		SetTextCommonText(array->vector[2], option2);
		SetTextCommonText(array->vector[3], option3);

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem3ToggleVertical);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem3ToggleVertical;
	}

	Il2CppObject* GetOptionItem3Toggle(const char* name, const wchar_t* title, const wchar_t* option1, const wchar_t* option2, const wchar_t* option3, int selectedIndex)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem3toggle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItem3Toggle = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItem3Toggle, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItem3Toggle->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItem3Toggle, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		SetTextCommonText(array->vector[0], title);
		SetTextCommonText(array->vector[1], option1);
		SetTextCommonText(array->vector[2], option2);
		SetTextCommonText(array->vector[3], option3);

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem3Toggle);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem3Toggle;
	}

	Il2CppObject* GetOptionItem2Toggle(const char* name, const wchar_t* title, const wchar_t* option1, const wchar_t* option2, int selectedIndex)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem2toggle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionItem2Toggle = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(optionItem2Toggle, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionItem2Toggle->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionItem2Toggle, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		SetTextCommonText(array->vector[0], title);
		SetTextCommonText(array->vector[1], option1);
		SetTextCommonText(array->vector[2], option2);

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem2Toggle);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem2Toggle;
	}

	Il2CppObject* GetOptionSliderNumText(Il2CppObject* slider)
	{
		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(slider->klass, "get_gameObject", 0)->methodPointer(slider);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		return array->vector[0];
	}

	float GetOptionSliderValue(Il2CppObject* slider)
	{
		return il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(slider->klass, "get_value", 0)->methodPointer(slider);
	}

	float GetOptionSliderValue(const char* name)
	{
		auto optionSlider = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionSlider)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionSlider->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "SliderCommon")), true, true, false, false, nullptr);

			auto sliderCommon = array->vector[0];

			return GetOptionSliderValue(sliderCommon);
		}
		return 0;
	}

	Il2CppObject* GetOptionSlider(const char* name)
	{
		auto optionSlider = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionSlider)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionSlider->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"umamusume.dll", "Gallop", "SliderCommon")), true, true, false, false, nullptr);

			return array->vector[0];
		}
		return nullptr;
	}

	Il2CppObject* GetOptionSlider(const char* name, const wchar_t* title, float value = 0, float min = 0, float max = 10, bool wholeNumbers = true, void (*onChange)(Il2CppObject*) = nullptr)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/optionsoundvolumeslider"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionSlider = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionSlider->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		text_set_verticalOverflow(textCommon, 1);
		SetTextCommonText(textCommon, title);

		auto optionSoundVolumeSliderArray = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "OptionSoundVolumeSlider")), true, true, false, false, nullptr);

		auto optionSoundVolumeSlider = optionSoundVolumeSliderArray->vector[0];

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(optionSoundVolumeSlider);

		auto sliderCommonArray = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "SliderCommon")), true, true, false, false, nullptr);

		auto sliderCommon = sliderCommonArray->vector[0];

		auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sliderCommon->klass, "get_onValueChanged", 0)->methodPointer(sliderCommon);

		auto AddCall = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(onValueChanged->klass, "AddCall", 1);

		auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.single_class));

		auto valueChanged = CreateDelegateWithClass(delegateClass, sliderCommon,
			onChange ? onChange :
			*[](Il2CppObject* _this)
			{
				auto textCommon = GetOptionSliderNumText(_this);

				if (il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(_this->klass, "get_wholeNumbers", 0)->methodPointer(_this))
				{
					auto value = static_cast<int>(il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(_this->klass, "get_value", 0)->methodPointer(_this));

					text_set_text(textCommon, il2cpp_string_new(to_string(value).data()));
				}
				else
				{
					auto value = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(_this->klass, "get_value", 0)->methodPointer(_this);

					value = roundf(value * 100) / 100;

					text_set_text(textCommon, il2cpp_string_new(format("{:.2f}", value).data()));
				}
			});

		auto invokeableCall = il2cpp_object_new(GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "InvokableCall`1"), GetRuntimeType(il2cpp_defaults.single_class)));

		auto delegateField = il2cpp_class_get_field_from_name_wrap(invokeableCall->klass, "Delegate");
		il2cpp_field_set_value(invokeableCall, delegateField, valueChanged);

		AddCall->methodPointer(onValueChanged, invokeableCall);

		try
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(sliderCommon->klass, "set_wholeNumbers", 1)->methodPointer(sliderCommon, wholeNumbers);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_minValue", 1)->methodPointer(sliderCommon, min);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_maxValue", 1)->methodPointer(sliderCommon, max);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_value", 1)->methodPointer(sliderCommon, value);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			cout << e.ex->klass->name << ": ";
			wcout << e.ex->message << endl;
		}

		auto transformArray = GetRectTransformArray(optionSlider);

		vector<Il2CppObject*> destroyTargets;

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == L"ToggleMute"s ||
					UnityEngine::Object::Name(transform)->chars == L"ImageIcon"s ||
					UnityEngine::Object::Name(transform)->chars == L"Line"s)
				{
					destroyTargets.emplace_back(transform);
				}
			}
		}

		for (int i = 0; i < destroyTargets.size(); i++)
		{
			auto transform = destroyTargets[i];
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(transform->klass, "SetParent", 2)->methodPointer(transform, nullptr, false);
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_gameObject", 0)->methodPointer(transform);
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(gameObject);
		}

		destroyTargets.clear();

		transformArray = GetRectTransformArray(optionSlider);

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == L"Slider"s)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_sizeDelta", 1)->methodPointer(transform, UnityEngine::Vector2{ 560, 24 });
					break;
				}
			}
		}

		auto gameObject = CreateGameObject();

		UnityEngine::Object::Name(gameObject, il2cpp_string_new(name));

		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto verticalLayoutGroup = AddComponent(gameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 54);
		/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 32);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 32);*/

		auto sliderTransform = GetRectTransform(optionSlider);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(sliderTransform->klass, "set_sizeDelta", 1)->methodPointer(sliderTransform, UnityEngine::Vector2{ 1000, 86 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(sliderTransform->klass, "SetParent", 2)->methodPointer(sliderTransform, rootTransform, false);

		return gameObject;
	}

	Il2CppObject* GetDropdown(const char* name)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/base/dropdowncommon"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto dropdownGameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(dropdownGameObject->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(dropdownGameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "DropDownCommon")), true, true, false, false, nullptr);

		auto dropDownCommon = array->vector[0];

		auto optionList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dropDownCommon->klass, "get_options", 0)->methodPointer(dropDownCommon);

		auto optionClass = il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.UI", "Dropdown/OptionData");

		auto option1 = il2cpp_object_new(optionClass);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(option1->klass, "set_text", 1)->methodPointer(option1, il2cpp_string_new("Option 1"));

		auto option2 = il2cpp_object_new(optionClass);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(option2->klass, "set_text", 1)->methodPointer(option2, il2cpp_string_new("Option 2"));

		auto array1 = il2cpp_array_new(optionClass, 2);

		il2cpp_array_setref(array1, 0, option1);
		il2cpp_array_setref(array1, 1, option2);

		auto itemField = il2cpp_class_get_field_from_name_wrap(optionList->klass, "_items");
		il2cpp_field_set_value(optionList, itemField, array1);

		auto sizeField = il2cpp_class_get_field_from_name_wrap(optionList->klass, "_size");
		int size = 2;
		il2cpp_field_set_value(optionList, sizeField, &size);


		il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppObject*)>(dropDownCommon->klass, "set_options", 1)->methodPointer(dropDownCommon, optionList);

		auto transform = GetRectTransform(dropdownGameObject);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_sizeDelta", 1)->methodPointer(transform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_anchorMax", 1)->methodPointer(transform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_anchorMin", 1)->methodPointer(transform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_pivot", 1)->methodPointer(transform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(transform->klass, "set_anchoredPosition", 1)->methodPointer(transform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(transform->klass, "set_localPosition", 1)->methodPointer(transform, UnityEngine::Vector3{ 0, 0, -10 });

		return dropdownGameObject;
	}

	Il2CppObject* GetCheckbox(const char* name)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/base/checkbox"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto checkbox = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		return checkbox;
	}

	Il2CppObject* GetCheckboxWithText(const char* name)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/base/checkboxwithtext"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto checkboxWithText = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		return checkboxWithText;
	}

	Il2CppObject* GetRadioButtonWithText(const char* name, const wchar_t* title)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/base/radiobuttonwithtext"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto radioButtonWithText = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Internal_CloneSingle", 1)(object);

		UnityEngine::Object::Name(radioButtonWithText, il2cpp_string_new(name));

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(radioButtonWithText->klass, "GetComponentsInternal", 6)->methodPointer;
		auto array = getComponents(radioButtonWithText, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonText(textCommon, title);

		return radioButtonWithText;
	}

	Il2CppObject* settingsDialog;

	template<typename T>
	void AddOrSet(WDocument& document, wchar_t* name, T value)
	{
		if (document.HasMember(name))
		{
			document[name].Set(value);
		}
		else
		{
			WValue v;
			v.Set(value);

			document.AddMember(rapidjson::StringRef(name), v, document.GetAllocator());
		}
	}

	void AddOrSetString(WDocument& document, wchar_t* name, wchar_t* value)
	{
		wchar_t* copy = new wchar_t[wcslen(value) + 1];
		wcscpy(copy, value);

		if (document.HasMember(name))
		{
			document[name].SetString(rapidjson::StringRef(copy));
		}
		else
		{
			WValue v;
			v.SetString(rapidjson::StringRef(copy));

			document.AddMember(rapidjson::StringRef(name), v, document.GetAllocator());
		}
	}

	Il2CppObject* selectOptionDialog;

	function<void(int)> optionSelected;

	void OpenSelectOption(const wchar_t* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected)
	{
		::optionSelected = optionSelected;

		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
				::optionSelected(GetToggleGroupCommonValue("option_toggle_group_content"));
			});

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
				)(dialogData, il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0003"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto gridLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));

		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;

		for (auto& pair : options)
		{
			toggles.emplace_back(GetRadioButtonWithText(("radio_"s + pair).data(), u8_wide(pair).data()));
		}

		AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());

		for (int i = 0; i < options.size(); i++)
		{
			auto& pair = options[i];
			il2cpp_array_setref(toggleArray, i, GetToggleCommon(("radio_"s + pair).data()));
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		selectOptionDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void OpenSelectFontColorOption(const wchar_t* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected)
	{
		::optionSelected = optionSelected;

		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
				::optionSelected(GetToggleGroupCommonValue("option_toggle_group_content"));
			});

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
				)(dialogData, il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0003"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto gridLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));

		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;

		for (auto& color : options)
		{
			auto colorW = u8_wide(color);
			toggles.emplace_back(GetRadioButtonWithText(("radio_"s + color).data(), colorW.data()));
			SetTextCommonFontColor(GetTextCommon(("radio_"s + color).data()), colorW.data());
		}

		AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());

		for (int i = 0; i < options.size(); i++)
		{
			auto& color = options[i];

			il2cpp_array_setref(toggleArray, i, GetToggleCommon(("radio_"s + color).data()));
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		selectOptionDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void OpenSelectOutlineSizeOption(const wchar_t* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected)
	{
		::optionSelected = optionSelected;

		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
				::optionSelected(GetToggleGroupCommonValue("option_toggle_group_content"));
			});

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
				)(dialogData, il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0003"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto gridLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));

		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;

		for (auto& size : options)
		{
			auto sizeW = u8_wide(size);
			toggles.emplace_back(GetRadioButtonWithText(("radio_"s + size).data(), sizeW.data()));
			SetTextCommonFontColor(GetTextCommon(("radio_"s + size).data()), L"White");
			SetTextCommonOutlineSize(GetTextCommon(("radio_"s + size).data()), sizeW.data());
			SetTextCommonOutlineColor(GetTextCommon(("radio_"s + size).data()), L"Brown");
		}

		AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());

		for (int i = 0; i < options.size(); i++)
		{
			auto& color = options[i];

			il2cpp_array_setref(toggleArray, i, GetToggleCommon(("radio_"s + color).data()));
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		selectOptionDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void OpenSelectOutlineColorOption(const wchar_t* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected)
	{
		::optionSelected = optionSelected;

		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(selectOptionDialog->klass, "Close", 0)->methodPointer(selectOptionDialog);
				::optionSelected(GetToggleGroupCommonValue("option_toggle_group_content"));
			});

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
				)(dialogData, il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0003"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto gridLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));

		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;

		for (auto& color : options)
		{
			auto colorW = u8_wide(color);
			toggles.emplace_back(GetRadioButtonWithText(("radio_"s + color).data(), colorW.data()));
			SetTextCommonOutlineColor(GetTextCommon(("radio_"s + color).data()), colorW.data());
		}

		AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());

		for (int i = 0; i < options.size(); i++)
		{
			auto& color = options[i];

			il2cpp_array_setref(toggleArray, i, GetToggleCommon(("radio_"s + color).data()));
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		selectOptionDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	vector<string> GetFontColorOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "FontColorType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	vector<string> GetOutlineSizeOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "OutlineSizeType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	vector<string> GetOutlineColorOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "OutlineColorType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	vector<string> GetGraphicsQualityOptions()
	{
		vector<string> options = { "Default" };

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings/GraphicsQuality");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				if (name != "Max"s)
				{
					options.emplace_back(name);
				}
			}
		}

		return options;
	}

	void OpenSettings()
	{
		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));

		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(settingsDialog->klass, "Close", 0)->methodPointer(settingsDialog);

				SetNotificationBackgroundAlpha(config::character_system_text_caption_background_alpha);
				SetNotificationPosition(config::character_system_text_caption_position_x, config::character_system_text_caption_position_y);
				SetNotificationFontColor(config::character_system_text_caption_font_color);
				SetNotificationOutlineSize(config::character_system_text_caption_outline_size);
				SetNotificationOutlineColor(config::character_system_text_caption_outline_color);

				config::rollback_config();
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				auto& configDocument = config::config_document;

				AddOrSet(configDocument, L"antiAliasing", vector<int>{ -1, 0, 2, 4, 8 }[GetOptionSliderValue("anti_aliasing")]);

				AddOrSet(configDocument, L"characterSystemTextCaption", GetOptionItemOnOffIsOn("character_system_text_caption"));

				AddOrSet(configDocument, L"characterSystemTextCaptionPositionX", GetOptionSliderValue("character_system_text_caption_position_x") / 10);

				AddOrSet(configDocument, L"characterSystemTextCaptionPositionY", GetOptionSliderValue("character_system_text_caption_position_y") / 10);

				AddOrSet(configDocument, L"characterSystemTextCaptionBackgroundAlpha", GetOptionSliderValue("character_system_text_caption_background_alpha") / 100);

				AddOrSet(configDocument, L"liveSliderAlwaysShow", GetOptionItemOnOffIsOn("live_slider_always_show"));

				AddOrSet(configDocument, L"livePlaybackLoop", GetOptionItemOnOffIsOn("live_playback_loop"));

				AddOrSet(configDocument, L"championsLiveShowText", GetOptionItemOnOffIsOn("champions_live_show_text"));

				AddOrSet(configDocument, L"championsLiveYear", GetToggleGroupCommonValue("champions_live_year") + 2022);

				AddOrSet(configDocument, L"allowDeleteCookie", GetOptionItemOnOffIsOn("allow_delete_cookie"));

				AddOrSet(configDocument, L"cySpringUpdateMode", static_cast<int>(GetOptionSliderValue("cyspring_update_mode")));

				AddOrSet(configDocument, L"uiAnimationScale", static_cast<int>(roundf(GetOptionSliderValue("ui_animation_scale") * 100)) / 100.0f);

				AddOrSet(configDocument, L"resolution3dScale", static_cast<int>(roundf(GetOptionSliderValue("resolution_3d_scale") * 100)) / 100.0f);

				AddOrSet(configDocument, L"notificationTp", GetOptionItemOnOffIsOn("notification_tp"));

				AddOrSet(configDocument, L"notificationRp", GetOptionItemOnOffIsOn("notification_rp"));

				AddOrSet(configDocument, L"dumpMsgPack", GetOptionItemOnOffIsOn("dump_msgpack"));

				AddOrSet(configDocument, L"dumpMsgPackRequest", GetOptionItemOnOffIsOn("dump_msgpack_request"));

#ifdef _DEBUG
				AddOrSet(configDocument, L"unlockLiveChara", GetOptionItemOnOffIsOn("unlock_live_chara"));
#endif
				AddOrSet(configDocument, L"unlockSize", GetOptionItemOnOffIsOn("unlock_size"));

				AddOrSet(configDocument, L"unlockSizeUseSystemResolution", GetOptionItemOnOffIsOn("use_system_resolution"));

				AddOrSet(configDocument, L"uiScale", static_cast<int>(roundf(GetOptionSliderValue("ui_scale") * 100)) / 100.0f);

				AddOrSet(configDocument, L"autoFullscreen", GetOptionItemOnOffIsOn("auto_fullscreen"));

				AddOrSet(configDocument, L"freeFormWindow", GetOptionItemOnOffIsOn("freeform_window"));

				AddOrSet(configDocument, L"freeFormUiScalePortrait", static_cast<int>(roundf(GetOptionSliderValue("ui_scale_portrait") * 100)) / 100.0f);

				AddOrSet(configDocument, L"freeFormUiScaleLandscape", static_cast<int>(roundf(GetOptionSliderValue("ui_scale_landscape") * 100)) / 100.0f);

				config::graphics_quality = configDocument[L"graphicsQuality"].GetInt();

				config::anti_aliasing = configDocument[L"antiAliasing"].GetInt();

				config::character_system_text_caption_background_alpha = configDocument[L"characterSystemTextCaptionBackgroundAlpha"].GetFloat();

				config::character_system_text_caption_position_x = configDocument[L"characterSystemTextCaptionPositionX"].GetFloat();

				config::character_system_text_caption_position_y = configDocument[L"characterSystemTextCaptionPositionY"].GetFloat();

				config::character_system_text_caption_font_color = configDocument[L"characterSystemTextCaptionFontColor"].GetString();

				config::character_system_text_caption_outline_size = configDocument[L"characterSystemTextCaptionOutlineSize"].GetString();

				config::character_system_text_caption_outline_color = configDocument[L"characterSystemTextCaptionOutlineColor"].GetString();

				config::live_slider_always_show = configDocument[L"liveSliderAlwaysShow"].GetBool();

				config::live_playback_loop = configDocument[L"livePlaybackLoop"].GetBool();

				config::champions_live_show_text = configDocument[L"championsLiveShowText"].GetBool();

				config::champions_live_year = configDocument[L"championsLiveYear"].GetInt();

				config::champions_live_resource_id = configDocument[L"championsLiveResourceId"].GetInt();

				config::cyspring_update_mode = configDocument[L"cySpringUpdateMode"].GetInt();

				config::ui_animation_scale = configDocument[L"uiAnimationScale"].GetFloat();

				config::resolution_3d_scale = configDocument[L"resolution3dScale"].GetFloat();

				auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

				if (graphicSettings)
				{
					auto _resolutionScaleField = il2cpp_class_get_field_from_name_wrap(graphicSettings->klass, "_resolutionScale");

					il2cpp_field_set_value(graphicSettings, _resolutionScaleField, &config::resolution_3d_scale);

					auto _resolutionScale2DField = il2cpp_class_get_field_from_name_wrap(graphicSettings->klass, "_resolutionScale2D");

					il2cpp_field_set_value(graphicSettings, _resolutionScale2DField, &config::resolution_3d_scale);
				}

				auto nowLoading = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "NowLoading", "get_Instance", IgnoreNumberOfArguments)();
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(nowLoading->klass, "DeleteMiniCharacter", 0)->methodPointer(nowLoading);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(nowLoading->klass, "CreateMiniCharacter", 0)->methodPointer(nowLoading);

				config::unlock_size_use_system_resolution = configDocument[L"unlockSizeUseSystemResolution"].GetBool();

				config::ui_scale = configDocument[L"uiScale"].GetFloat();

				config::auto_fullscreen = configDocument[L"autoFullscreen"].GetBool();

				config::freeform_ui_scale_portrait = configDocument[L"freeFormUiScalePortrait"].GetFloat();

				config::freeform_ui_scale_landscape = configDocument[L"freeFormUiScaleLandscape"].GetFloat();

				config::notification_tp = configDocument[L"notificationTp"].GetBool();

				if (config::notification_tp)
				{
					MsgPackData::RegisterTPScheduledToast();
				}
				else
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
				}

				config::notification_rp = configDocument[L"notificationRp"].GetBool();

				if (config::notification_rp)
				{
					MsgPackData::RegisterRPScheduledToast();
				}
				else
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
				}

				config::dump_msgpack = configDocument[L"dumpMsgPack"].GetBool();

				config::dump_msgpack_request = configDocument[L"dumpMsgPackRequest"].GetBool();

#ifdef _DEBUG
				config::unlock_live_chara = configDocument[L"unlockLiveChara"].GetBool();
#endif
				config::write_config();

				auto dialogData = il2cpp_object_new(
					il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
				il2cpp_runtime_object_init(dialogData);

				dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
					ULONG headerTextId,
					Il2CppString * message,
					Il2CppDelegate * onClose,
					ULONG closeTextId)>(
						il2cpp_class_get_method_from_name(dialogData->klass, "SetSimpleOneButtonMessage",
							4)->methodPointer
						)(dialogData, GetTextIdByName(L"AccoutDataLink0061"), localize_get_hook(GetTextIdByName(L"Outgame0309")), nullptr, GetTextIdByName(L"Common0007"));

				auto onDestroy = CreateDelegateStatic(*[]()
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(settingsDialog->klass, "Close", 0)->methodPointer(settingsDialog);
						settingsDialog = nullptr;
					});

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(dialogData->klass, "AddDestroyCallback", 1)->methodPointer(dialogData, onDestroy);
				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
			});

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
				)(dialogData, il2cpp_string_new16(LocalifySettings::GetText("title")), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0261"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto verticalLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, -20);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 16);

		int antiAliasing = 0;
		bool characterSystemTextCaption = false;
		bool liveSliderAlwaysShow = false;
		bool livePlaybackLoop = false;
		bool championsLiveShowText = false;
		int championsLiveYear = 2022;
		float characterSystemTextCaptionPositionX = 0;
		float characterSystemTextCaptionPositionY = 0;
		float characterSystemTextCaptionBackgroundAlpha = 0;
		bool allowDeleteCookie = false;
		int cySpringUpdateMode = -1;
		float resolution3dScale = 1;
		float uiAnimationScale = 1;
		bool notificationTp = false;
		bool notificationRp = false;
		bool dumpMsgPack = false;
		bool dumpMsgPackRequest = false;
		bool unlockLiveChara = false;
		bool unlockSize = false;
		bool unlockSizeUseSystemResolution = false;
		float uiScale = 0;
		bool autoFullscreen = false;
		bool freeFormWindow = false;
		float freeFormUiScalePortrait = 0;
		float freeFormUiScaleLandscape = 0;

		if (config::read_config())
		{
			auto& configDocument = config::config_document;

			if (configDocument.HasMember(L"antiAliasing"))
			{
				vector<int> options = { -1, 0, 2, 4, 8 };
				antiAliasing = find(options.begin(), options.end(), configDocument[L"antiAliasing"].GetInt()) - options.begin();
			}

			if (configDocument.HasMember(L"characterSystemTextCaption"))
			{
				characterSystemTextCaption = configDocument[L"characterSystemTextCaption"].GetBool();
			}

			if (configDocument.HasMember(L"championsLiveShowText"))
			{
				championsLiveShowText = configDocument[L"championsLiveShowText"].GetBool();
			}

			if (configDocument.HasMember(L"liveSliderAlwaysShow"))
			{
				liveSliderAlwaysShow = configDocument[L"liveSliderAlwaysShow"].GetBool();
			}

			if (configDocument.HasMember(L"livePlaybackLoop"))
			{
				livePlaybackLoop = configDocument[L"livePlaybackLoop"].GetBool();
			}

			if (configDocument.HasMember(L"championsLiveYear"))
			{
				championsLiveYear = configDocument[L"championsLiveYear"].GetInt();
			}

			if (configDocument.HasMember(L"characterSystemTextCaptionPositionX"))
			{
				characterSystemTextCaptionPositionX = configDocument[L"characterSystemTextCaptionPositionX"].GetFloat();
			}

			if (configDocument.HasMember(L"characterSystemTextCaptionPositionY"))
			{
				characterSystemTextCaptionPositionY = configDocument[L"characterSystemTextCaptionPositionY"].GetFloat();
			}

			if (configDocument.HasMember(L"characterSystemTextCaptionBackgroundAlpha"))
			{
				characterSystemTextCaptionBackgroundAlpha = configDocument[L"characterSystemTextCaptionBackgroundAlpha"].GetFloat();
			}

			if (configDocument.HasMember(L"allowDeleteCookie"))
			{
				allowDeleteCookie = configDocument[L"allowDeleteCookie"].GetBool();
			}

			if (configDocument.HasMember(L"cySpringUpdateMode"))
			{
				cySpringUpdateMode = configDocument[L"cySpringUpdateMode"].GetInt();
			}

			if (configDocument.HasMember(L"resolution3dScale"))
			{
				resolution3dScale = configDocument[L"resolution3dScale"].GetFloat();
			}

			if (configDocument.HasMember(L"uiAnimationScale"))
			{
				uiAnimationScale = configDocument[L"uiAnimationScale"].GetFloat();
			}

			if (configDocument.HasMember(L"notificationTp"))
			{
				notificationTp = configDocument[L"notificationTp"].GetBool();
			}

			if (configDocument.HasMember(L"notificationRp"))
			{
				notificationRp = configDocument[L"notificationRp"].GetBool();
			}

			if (configDocument.HasMember(L"dumpMsgPack"))
			{
				dumpMsgPack = configDocument[L"dumpMsgPack"].GetBool();
			}

			if (configDocument.HasMember(L"dumpMsgPackRequest"))
			{
				dumpMsgPackRequest = configDocument[L"dumpMsgPackRequest"].GetBool();
			}

#ifdef _DEBUG
			if (configDocument.HasMember(L"unlockLiveChara"))
			{
				unlockLiveChara = configDocument[L"unlockLiveChara"].GetBool();
			}
#endif
			if (configDocument.HasMember(L"unlockSize"))
			{
				unlockSize = configDocument[L"unlockSize"].GetBool();
			}

			if (configDocument.HasMember(L"unlockSizeUseSystemResolution"))
			{
				unlockSizeUseSystemResolution = configDocument[L"unlockSizeUseSystemResolution"].GetBool();
			}

			if (configDocument.HasMember(L"uiScale"))
			{
				uiScale = configDocument[L"uiScale"].GetFloat();
			}

			if (configDocument.HasMember(L"autoFullscreen"))
			{
				autoFullscreen = configDocument[L"autoFullscreen"].GetBool();
			}

			if (configDocument.HasMember(L"freeFormWindow"))
			{
				freeFormWindow = configDocument[L"freeFormWindow"].GetBool();
			}

			if (configDocument.HasMember(L"freeFormUiScalePortrait"))
			{
				freeFormUiScalePortrait = configDocument[L"freeFormUiScalePortrait"].GetFloat();
			}

			if (configDocument.HasMember(L"freeFormUiScaleLandscape"))
			{
				freeFormUiScaleLandscape = configDocument[L"freeFormUiScaleLandscape"].GetFloat();
			}
		}

		vector<string> graphicsQualityOptions = GetGraphicsQualityOptions();

		AddToLayout(m_Content,
			{
				GetOptionItemTitle(LocalifySettings::GetText("graphics")),
				GetOptionItemSimpleWithButton("graphics_quality", (LocalifySettings::GetText("graphics_quality") + L": "s + u8_wide(graphicsQualityOptions[config::config_document[L"graphicsQuality"].GetInt() + 1])).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionSlider("anti_aliasing", LocalifySettings::GetText("anti_aliasing"), antiAliasing, 0, 4, true, *[](Il2CppObject* slider) {
					auto numText = GetOptionSliderNumText(slider);
					auto value = GetOptionSliderValue(slider);

					switch (static_cast<int>(value))
					{
					case 0:
						text_set_text(numText, il2cpp_string_new("Default"));
						break;
					case 1:
						text_set_text(numText, il2cpp_string_new("OFF"));
						break;
					case 2:
						text_set_text(numText, il2cpp_string_new("x2"));
						break;
					case 3:
						text_set_text(numText, il2cpp_string_new("x4"));
						break;
					case 4:
						text_set_text(numText, il2cpp_string_new("x8"));
						break;
					}
				}),
				GetOptionSlider("ui_animation_scale", LocalifySettings::GetText("ui_animation_scale"), uiAnimationScale, 0.1, 5.0, false),
				GetOptionSlider("resolution_3d_scale", LocalifySettings::GetText("resolution_3d_scale"), resolution3dScale, 0.1, 2.0, false),
				GetOptionSlider("cyspring_update_mode", LocalifySettings::GetText("cyspring_update_mode"), cySpringUpdateMode, -1, 3, true, *[](Il2CppObject* slider) {
					auto numText = GetOptionSliderNumText(slider);
					auto value = GetOptionSliderValue(slider);

					switch (static_cast<int>(value))
					{
					case -1:
						text_set_text(numText, il2cpp_string_new("Default"));
						break;
					case 0:
						text_set_text(numText, il2cpp_string_new("ModeNormal"));
						break;
					case 1:
						text_set_text(numText, il2cpp_string_new("Mode60FPS"));
						break;
					case 2:
						text_set_text(numText, il2cpp_string_new("SkipFrame"));
						break;
					case 3:
						text_set_text(numText, il2cpp_string_new("SkipFramePostAlways"));
						break;
					}
				}),
				GetOptionItemTitle(LocalifySettings::GetText("screen")),
				GetOptionItemOnOff("unlock_size", LocalifySettings::GetText("unlock_size")),
				GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				GetOptionItemOnOff("use_system_resolution", LocalifySettings::GetText("use_system_resolution")),
				GetOptionSlider("ui_scale", LocalifySettings::GetText("ui_scale"), uiScale, 0.1, 2.0, false),
				GetOptionItemOnOff("auto_fullscreen", LocalifySettings::GetText("auto_fullscreen")),
				GetOptionItemOnOff("freeform_window", LocalifySettings::GetText("freeform_window")),
				GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				GetOptionSlider("ui_scale_portrait", LocalifySettings::GetText("ui_scale_portrait"), freeFormUiScalePortrait, 0.1, 2.0, false),
				GetOptionSlider("ui_scale_landscape", LocalifySettings::GetText("ui_scale_landscape"), freeFormUiScaleLandscape, 0.1, 2.0, false),
				GetOptionItemTitle(localize_get_hook(GetTextIdByName(L"Common0035"))->chars),
				GetOptionItemOnOff("live_slider_always_show", LocalifySettings::GetText("live_slider_always_show")),
				GetOptionItemOnOff("live_playback_loop", LocalifySettings::GetText("live_playback_loop")),
				GetOptionItemOnOff("champions_live_show_text", LocalifySettings::GetText("champions_live_show_text")),
				GetOptionItemSimpleWithButton("champions_live_resource_id", (LocalifySettings::GetText("champions_live_resource_id") + L": "s + u8_wide(MasterDB::GetChampionsResources()[config::config_document[L"championsLiveResourceId"].GetInt() - 1])).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionItem3Toggle("champions_live_year", LocalifySettings::GetText("champions_live_year"), L"2022", L"2023", L"2024", championsLiveYear - 2022),
				GetOptionItemSimple(L""),
				GetOptionItemTitle(LocalifySettings::GetText("character_system_text_caption")),
				GetOptionItemOnOff("character_system_text_caption", LocalifySettings::GetText("character_system_text_caption")),
				GetOptionSlider("character_system_text_caption_position_x", LocalifySettings::GetText("character_system_text_caption_position_x"), characterSystemTextCaptionPositionX * 10, -100, 100, true, *[](Il2CppObject* slider) {
					auto numText = GetOptionSliderNumText(slider);
					auto value = GetOptionSliderValue(slider);
					value = value / 10;

					text_set_text(numText, il2cpp_string_new(format("{:.2f}", value).data()));

					SetNotificationPosition(value, GetOptionSliderValue("character_system_text_caption_position_y") / 10);
					SetNotificationDisplayTime(1);
					ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
				}),
				GetOptionSlider("character_system_text_caption_position_y", LocalifySettings::GetText("character_system_text_caption_position_y"), characterSystemTextCaptionPositionY * 10, -100, 100, true, *[](Il2CppObject* slider) {
					auto numText = GetOptionSliderNumText(slider);
					auto value = GetOptionSliderValue(slider);
					value = value / 10;

					text_set_text(numText, il2cpp_string_new(format("{:.2f}", value).data()));

					SetNotificationPosition(GetOptionSliderValue("character_system_text_caption_position_x") / 10, value);
					SetNotificationDisplayTime(1);
					ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
				}),
				GetOptionSlider("character_system_text_caption_background_alpha", LocalifySettings::GetText("character_system_text_caption_background_alpha"), characterSystemTextCaptionBackgroundAlpha * 100, 0, 100, true, *[](Il2CppObject* slider) {
					auto numText = GetOptionSliderNumText(slider);
					auto value = GetOptionSliderValue(slider);
					value = value / 100;

					text_set_text(numText, il2cpp_string_new(format("{:.2f}", value).data()));

					SetNotificationBackgroundAlpha(value);
					SetNotificationDisplayTime(1);
					ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
				}),
				GetOptionItemSimpleWithButton("character_system_text_caption_font_color", (LocalifySettings::GetText("character_system_text_caption_font_color") + L": "s + config::config_document[L"characterSystemTextCaptionFontColor"].GetString()).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionItemSimpleWithButton("character_system_text_caption_outline_size", (LocalifySettings::GetText("character_system_text_caption_outline_size") + L": "s + config::config_document[L"characterSystemTextCaptionOutlineSize"].GetString()).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionItemSimpleWithButton("character_system_text_caption_outline_color", (LocalifySettings::GetText("character_system_text_caption_outline_color") + L": "s + config::config_document[L"characterSystemTextCaptionOutlineColor"].GetString()).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionItemButton("show_caption", LocalifySettings::GetText("show_caption")),
				GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				GetOptionItemTitle(localize_get_hook(GetTextIdByName(L"Outgame0293"))->chars),
				GetOptionItemOnOff("notification_tp", localize_get_hook(GetTextIdByName(L"Outgame0294"))->chars),
				GetOptionItemOnOff("notification_rp", localize_get_hook(GetTextIdByName(L"Outgame0437"))->chars),
				GetOptionItemButton("show_notification", LocalifySettings::GetText("show_notification")),
				GetOptionItemAttention(localize_get_hook(GetTextIdByName(L"Outgame0297"))->chars),
				GetOptionItemTitle(LocalifySettings::GetText("title")),
				Game::CurrentGameRegion == Game::Region::JAP ?
					GetOptionItemButton("clear_webview_cache", LocalifySettings::GetText("clear_webview_cache")) :
					GetOptionItemOnOff("allow_delete_cookie", LocalifySettings::GetText("allow_delete_cookie")),
				GetOptionItemOnOff("dump_msgpack", LocalifySettings::GetText("dump_msgpack")),
				GetOptionItemOnOff("dump_msgpack_request", LocalifySettings::GetText("dump_msgpack_request")),
#ifdef _DEBUG
				GetOptionItemOnOff("unlock_live_chara", LocalifySettings::GetText("unlock_live_chara")),
				GetOptionItemInfo(LocalifySettings::GetText("unlock_live_chara_info")),
#endif
				GetOptionItemButton("github", L"GitHub"),
				GetOptionItemTitle(LocalifySettings::GetText("experiments")),
				GetOptionItemButton("toggle_vr", L"Toggle VR"),
			// GetOptionItemSimple("Simple"),
			// GetOptionItemOnOff("on_off", "On Off"),
			// GetOptionItem3ToggleVertical("Text"),
			// GetOptionItem3Toggle("Text"),
			// GetOptionItem2Toggle("Text"),
			/*GetDropdown("dropdown"),
			GetCheckbox("checkbox"),
			GetCheckboxWithText("checkbox_with_text"),
			GetRadioButtonWithText("radiobutton_with_text", "Test"),*/
			/*GetSlider("slider"),*/
			/*GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),
			GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),*/
			}
		);

		SetOptionItemOnOffAction("character_system_text_caption", characterSystemTextCaption, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("live_slider_always_show", liveSliderAlwaysShow, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("live_playback_loop", livePlaybackLoop, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("champions_live_show_text", championsLiveShowText, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("notification_tp", notificationTp, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("notification_rp", notificationRp, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("dump_msgpack", dumpMsgPack, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("dump_msgpack_request", dumpMsgPackRequest, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("unlock_size", unlockSize, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("use_system_resolution", unlockSizeUseSystemResolution, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("auto_fullscreen", autoFullscreen, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("freeform_window", freeFormWindow, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("unlock_live_chara", unlockLiveChara, *([](Il2CppObject*, bool isOn)
			{
			}));

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			SetOptionItemOnOffAction("allow_delete_cookie", allowDeleteCookie, *([](Il2CppObject*, bool isOn)
				{
					// TODO
				}));
		}

		SetOptionItemOnOffAction("on_off", false, *([](Il2CppObject*, bool isOn)
			{
				stringstream text;

				text << "Changed to " << (isOn ? "On" : "Off");

				ShowUINotification(il2cpp_string_new(text.str().data()));
			}));

		SetOptionItemButtonAction("show_caption", *([](Il2CppObject*)
			{
				if (config::character_system_text_caption)
				{
					SetNotificationDisplayTime(1);
					ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
				}
				else
				{
					ShowUINotification(il2cpp_string_new16(LocalifySettings::GetText("setting_disabled")));
				}
			}));

		SetOptionItemButtonAction("show_notification", *([](Il2CppObject*)
			{
				auto leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
				auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
				auto contentU8 = MasterDB::GetTextData(163, leader_chara_id);
				replaceAll(contentU8, "\\n", "\n");
				auto content = u8_wide(contentU8);

				DesktopNotificationManagerCompat::ShowToastNotification(title.data(), content.data(), MsgPackData::GetIconPath(leader_chara_id)->chars);
			}));

		SetOptionItemButtonAction("graphics_quality", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("graphics_quality"), GetGraphicsQualityOptions(), config::config_document[L"graphicsQuality"].GetInt() + 1, [](int value) {
					AddOrSet(config::config_document, L"graphicsQuality", value - 1);

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("graphics_quality");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("graphics_quality") + L": "s + u8_wide(GetGraphicsQualityOptions()[config::config_document[L"graphicsQuality"].GetInt() + 1])).data());
					});
			}));

		SetOptionItemButtonAction("champions_live_resource_id", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("champions_live_resource_id"), MasterDB::GetChampionsResources(), config::config_document[L"championsLiveResourceId"].GetInt() - 1, [](int value) {
					AddOrSet(config::config_document, L"championsLiveResourceId", value + 1);

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("champions_live_resource_id");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_resource_id") + L": "s + u8_wide(MasterDB::GetChampionsResources()[config::config_document[L"championsLiveResourceId"].GetInt() - 1])).data());
					});
			}));

		auto fontColorTextCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_font_color");
		SetTextCommonOutlineColor(fontColorTextCommon, L"Brown");
		SetTextCommonFontColor(fontColorTextCommon,
			config::config_document[L"characterSystemTextCaptionFontColor"].GetString());

		SetOptionItemButtonAction("character_system_text_caption_font_color", *([](Il2CppObject*)
			{
				auto options = GetFontColorOptions();
				auto value = wide_u8(config::config_document[L"characterSystemTextCaptionFontColor"].GetString());
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectFontColorOption(LocalifySettings::GetText("character_system_text_caption_font_color"), options, index, [](int value) {
					auto options = GetFontColorOptions();
					wstring color = u8_wide(options[value]);
					AddOrSetString(config::config_document, L"characterSystemTextCaptionFontColor", color.data());

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_font_color");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_font_color") + L": "s + config::config_document[L"characterSystemTextCaptionFontColor"].GetString()).data());
					SetTextCommonFontColor(textCommon, color.data());
					SetNotificationFontColor(color.data());
					});
			}));

		auto outlineSizeTextCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_size");
		SetTextCommonFontColor(outlineSizeTextCommon, L"White");
		SetTextCommonOutlineColor(outlineSizeTextCommon, L"Brown");
		SetTextCommonOutlineSize(outlineSizeTextCommon,
			config::config_document[L"characterSystemTextCaptionOutlineSize"].GetString());

		SetOptionItemButtonAction("character_system_text_caption_outline_size", *([](Il2CppObject*)
			{
				auto options = GetOutlineSizeOptions();
				auto value = wide_u8(config::config_document[L"characterSystemTextCaptionOutlineSize"].GetString());
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectOutlineSizeOption(LocalifySettings::GetText("character_system_text_caption_outline_size"), options, index, [](int value) {
					auto options = GetOutlineSizeOptions();
					wstring color = u8_wide(options[value]);
					AddOrSetString(config::config_document, L"characterSystemTextCaptionOutlineSize", color.data());

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_size");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_outline_size") + L": "s + config::config_document[L"characterSystemTextCaptionOutlineSize"].GetString()).data());
					SetTextCommonOutlineSize(textCommon, color.data());
					SetNotificationOutlineSize(color.data());
					});
			}));

		auto outlineColorTextCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_color");
		SetTextCommonOutlineColor(outlineColorTextCommon,
			config::config_document[L"characterSystemTextCaptionOutlineColor"].GetString());

		SetOptionItemButtonAction("character_system_text_caption_outline_color", *([](Il2CppObject*)
			{
				auto options = GetOutlineColorOptions();
				auto value = wide_u8(config::config_document[L"characterSystemTextCaptionOutlineColor"].GetString());
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectOutlineColorOption(LocalifySettings::GetText("character_system_text_caption_outline_color"), options, index, [](int value) {
					auto options = GetOutlineColorOptions();
					wstring color = u8_wide(options[value]);
					AddOrSetString(config::config_document, L"characterSystemTextCaptionOutlineColor", color.data());

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_color");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_outline_color") + L": "s + config::config_document[L"characterSystemTextCaptionOutlineColor"].GetString()).data());
					SetTextCommonOutlineColor(textCommon, color.data());
					SetNotificationOutlineColor(color.data());
					});
			}));

		SetOptionItemButtonAction("toggle_vr", *([](Il2CppObject*)
			{
				if (!Unity::OpenXR::initialized)
				{
					Unity::OpenXR::InitLibrary(unityInterfaces);
					Unity::OpenXR::Init();
				}

				if (Unity::OpenXR::initialized)
				{
					if (Unity::OpenXR::started)
					{
						Unity::OpenXR::Stop();
					}
					else
					{
						Unity::OpenXR::Start();
					}
				}
			}));

		SetOptionItemButtonAction("github", *([](Il2CppObject*)
			{
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
							localizeextension_text_hook(GetTextIdByName(L"Common0009")),
							localizeextension_text_hook(GetTextIdByName(L"Home0073")),
							CreateDelegateStatic(*[]()
								{
									UnityEngine::Application::OpenURL(il2cpp_string_new("https://github.com/Kimjio/umamusume-localify"));
								}),
							GetTextIdByName(L"Common0004"),
							GetTextIdByName(L"Common0003"),
							nullptr,
							2);

				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*)>(
					"umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
			}));

		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			SetOptionItemButtonAction("clear_webview_cache", *([](Il2CppObject*)
				{
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
								localizeextension_text_hook(GetTextIdByName(L"Race0652")),
								il2cpp_string_new16(LocalifySettings::GetText("clear_webview_cache_confirm")),
								CreateDelegateStatic(*[]()
									{
										PWSTR path;
										SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &path);

										wstring combinedPath = wstring(path).append(L"\\DMMWebView2");

										try
										{
											filesystem::remove_all(combinedPath);
											ShowUINotification(il2cpp_string_new16(LocalifySettings::GetText("deleted")));
										}
										catch (exception& e)
										{
											cout << e.what() << endl;
										}
									}),
								GetTextIdByName(L"Common0004"),
								GetTextIdByName(L"Common0003"),
								nullptr,
								2);

					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*)>(
						"umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
				}));
		}

		auto contentSizeFitter = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto _layoutField = il2cpp_class_get_field_from_name_wrap(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		settingsDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void OpenLiveSettings()
	{
		auto dialogData = il2cpp_object_new(
			il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(settingsDialog->klass, "Close", 0)->methodPointer(settingsDialog);

				config::rollback_config();
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				auto& configDocument = config::config_document;

				AddOrSet(configDocument, L"liveSliderAlwaysShow", GetOptionItemOnOffIsOn("live_slider_always_show"));

				AddOrSet(configDocument, L"livePlaybackLoop", GetOptionItemOnOffIsOn("live_playback_loop"));

				AddOrSet(configDocument, L"championsLiveShowText", GetOptionItemOnOffIsOn("champions_live_show_text"));

				AddOrSet(configDocument, L"championsLiveYear", GetToggleGroupCommonValue("champions_live_year") + 2022);

				config::live_slider_always_show = configDocument[L"liveSliderAlwaysShow"].GetBool();

				config::live_playback_loop = configDocument[L"livePlaybackLoop"].GetBool();

				config::champions_live_show_text = configDocument[L"championsLiveShowText"].GetBool();

				config::champions_live_year = configDocument[L"championsLiveYear"].GetInt();

				config::champions_live_resource_id = configDocument[L"championsLiveResourceId"].GetInt();

				config::write_config();

				auto dialogData = il2cpp_object_new(
					il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
				il2cpp_runtime_object_init(dialogData);

				dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
					ULONG headerTextId,
					Il2CppString * message,
					Il2CppDelegate * onClose,
					ULONG closeTextId)>(
						il2cpp_class_get_method_from_name(dialogData->klass, "SetSimpleOneButtonMessage",
							4)->methodPointer
						)(dialogData, GetTextIdByName(L"AccoutDataLink0061"), localize_get_hook(GetTextIdByName(L"Outgame0309")), nullptr, GetTextIdByName(L"Common0007"));

				auto onDestroy = CreateDelegateStatic(*[]()
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(settingsDialog->klass, "Close", 0)->methodPointer(settingsDialog);
						settingsDialog = nullptr;
					});

				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(dialogData->klass, "AddDestroyCallback", 1)->methodPointer(dialogData, onDestroy);
				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
			});

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
				)(dialogData, il2cpp_string_new16(LocalifySettings::GetText("title")), nullptr, onRight, GetTextIdByName(L"Common0004"), GetTextIdByName(L"Common0261"), onLeft, 10);

		auto DispStackTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "DispStackType");
		int DispStackType = 2;
		il2cpp_field_set_value(dialogData, DispStackTypeField, &DispStackType);

		auto ObjParentTypeField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ObjParentType");
		int ObjParentType = 1;
		il2cpp_field_set_value(dialogData, ObjParentTypeField, &ObjParentType);

		auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
		bool AutoClose = false;
		il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

		auto gameObject = CreateGameObject();
		auto rootTransform = AddComponent(gameObject, GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		auto scrollViewBase = resources_load_hook(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(scrollViewBase->klass, "GetComponentsInternal", 6)->methodPointer;

		auto scrollRectArray = getComponents(scrollViewBase, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "ScrollRectCommon")), true, true, false, false, nullptr);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		auto scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_sizeDelta", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ -24, -12 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMax", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchorMin", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_pivot", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scrollRectTransform->klass, "set_anchoredPosition", 1)->methodPointer(scrollRectTransform, UnityEngine::Vector2{ 0, -6 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(scrollRectTransform->klass, "SetParent", 2)->methodPointer(scrollRectTransform, rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name_wrap(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		auto contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto verticalLayoutGroup = AddComponent(contentGameObject, GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, -20);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 16);

		bool liveSliderAlwaysShow = false;
		bool livePlaybackLoop = false;
		bool championsLiveShowText = false;
		int championsLiveYear = 2023;

		if (config::read_config())
		{
			auto& configDocument = config::config_document;

			if (configDocument.HasMember(L"liveSliderAlwaysShow"))
			{
				liveSliderAlwaysShow = configDocument[L"liveSliderAlwaysShow"].GetBool();
			}

			if (configDocument.HasMember(L"livePlaybackLoop"))
			{
				livePlaybackLoop = configDocument[L"livePlaybackLoop"].GetBool();
			}

			if (configDocument.HasMember(L"championsLiveShowText"))
			{
				championsLiveShowText = configDocument[L"championsLiveShowText"].GetBool();
			}

			if (configDocument.HasMember(L"championsLiveYear"))
			{
				championsLiveYear = configDocument[L"championsLiveYear"].GetInt();
			}
		}

		AddToLayout(m_Content,
			{
				GetOptionItemTitle(localize_get_hook(GetTextIdByName(L"Common0035"))->chars),
				GetOptionItemOnOff("live_slider_always_show", LocalifySettings::GetText("live_slider_always_show")),
				GetOptionItemOnOff("live_playback_loop", LocalifySettings::GetText("live_playback_loop")),
				GetOptionItemOnOff("champions_live_show_text", LocalifySettings::GetText("champions_live_show_text")),
				GetOptionItemSimpleWithButton("champions_live_resource_id", (LocalifySettings::GetText("champions_live_resource_id") + L": "s + u8_wide(MasterDB::GetChampionsResources()[config::config_document[L"championsLiveResourceId"].GetInt() - 1])).data(),
					localize_get_hook(GetTextIdByName(L"Circle0206"))->chars),
				GetOptionItem3Toggle("champions_live_year", LocalifySettings::GetText("champions_live_year"), L"2022", L"2023", L"2024", championsLiveYear - 2022),
				GetOptionItemSimple(L""),
			}
			);

		SetOptionItemOnOffAction("live_slider_always_show", liveSliderAlwaysShow, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("live_playback_loop", livePlaybackLoop, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemOnOffAction("champions_live_show_text", championsLiveShowText, *([](Il2CppObject*, bool isOn)
			{
			}));

		SetOptionItemButtonAction("champions_live_resource_id", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("champions_live_resource_id"), MasterDB::GetChampionsResources(), config::config_document[L"championsLiveResourceId"].GetInt() - 1, [](int value) {
					AddOrSet(config::config_document, L"championsLiveResourceId", value + 1);

					auto textCommon = GetOptionItemSimpleWithButtonTextCommon("champions_live_resource_id");
					SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_resource_id") + L": "s + u8_wide(MasterDB::GetChampionsResources()[config::config_document[L"championsLiveResourceId"].GetInt() - 1])).data());
					});
			}));

		auto contentSizeFitter = AddComponent(contentGameObject, GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto _layoutField = il2cpp_class_get_field_from_name_wrap(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		bool _autoUpdate = true;
		auto _autoUpdateField = il2cpp_class_get_field_from_name_wrap(contentSizeFitter->klass, "_autoUpdate");
		il2cpp_field_set_value(contentSizeFitter, _autoUpdateField, &_autoUpdate);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "ContentsObject");

		il2cpp_field_set_value(dialogData, ContentsObjectField, gameObject);

		settingsDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
	}

	void InitOptionLayout(Il2CppObject* parentRectTransform)
	{
		AddToLayout(parentRectTransform,
			{
				GetOptionItemTitle(LocalifySettings::GetText("title")),
				GetOptionItemButton("open_settings", LocalifySettings::GetText("open_settings")),
			}
			);
	}

	void SetupOptionLayout()
	{
		SetOptionItemButtonAction("open_settings", *([](Il2CppObject*)
			{
				OpenSettings();
			}));
	}

	void SetupLiveOptionLayout()
	{
		SetOptionItemButtonAction("open_settings", *([](Il2CppObject*)
			{
				OpenLiveSettings();
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

			auto ViewerIdTextField = il2cpp_class_get_field_from_name_wrap(view->klass, "ViewerIdText");
			Il2CppObject* ViewerIdText;
			il2cpp_field_get_value(view, ViewerIdTextField, &ViewerIdText);

			auto CopyIdButtonField = il2cpp_class_get_field_from_name_wrap(view->klass, "CopyIdButton");
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
				auto newText = text->chars + L"\n("s + viewerId->chars + L")";
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

							il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("UnityEngine.IMGUIModule.dll", "UnityEngine", "GUIUtility", "set_systemCopyBuffer", 1)(il2cpp_string_new16(to_wstring(viewerId).data()));

							auto dialogData = il2cpp_object_new(
								il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
							il2cpp_runtime_object_init(dialogData);

							dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
								ULONG headerTextId,
								Il2CppString * message,
								Il2CppDelegate * onClose,
								ULONG closeTextId)>(
									il2cpp_class_get_method_from_name(dialogData->klass, "SetSimpleOneButtonMessage",
										4)->methodPointer
									)(dialogData, GetTextIdByName(L"Outgame0031"), il2cpp_string_new16(L"트레이너 ID를 복사했습니다."), nullptr, GetTextIdByName(L"Common0007"));

							il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
						};

					auto playerIdCopyFn = *[](void*)
						{
							auto controller = GetCurrentViewController();
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "OnClickCopyIdButton", 0)->methodPointer(controller);
						};

					auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
					il2cpp_runtime_object_init(dialogData);

					dialogData =
						il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * thisObj,
							Il2CppString * headerTextArg,
							Il2CppString * message,
							Il2CppDelegate * onRight,
							unsigned long leftTextId,
							unsigned long rightTextId,
							Il2CppDelegate * onLeft,
							int dialogFormType)>(dialogData->klass, "SetSimpleTwoButtonMessage", 7)->methodPointer
							(dialogData,
								localizeextension_text_hook(GetTextIdByName(L"Outgame0031")),
								il2cpp_string_new("트레이너 ID를 복사하시겠습니까?"),
								CreateDelegateStatic(viewerIdCopyFn), GetTextIdByName(L"Outgame0002"), GetTextIdByName(L"Common0003"),
								CreateDelegateStatic(playerIdCopyFn), 2);

					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
				};

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(CopyIdButton->klass, "SetOnClick", 1)->methodPointer(CopyIdButton, &CreateUnityActionStatic(fn)->delegate);
			return true;
		}
		return false;
	}

	bool UpdateHomeMenuMainButton()
	{
		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			return true;
		}

		auto dialog = GetFrontDialog();
		auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

		auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(data->klass, "ContentsObject");
		Il2CppObject* ContentsObject;
		il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

		auto homeMenuMain = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogHomeMenuMain"));

		if (homeMenuMain)
		{
			auto _serialButtonField = il2cpp_class_get_field_from_name_wrap(homeMenuMain->klass, "_serialButton");
			Il2CppObject* _serialButton;
			il2cpp_field_get_value(homeMenuMain, _serialButtonField, &_serialButton);

			if (!GetButtonCommonOnClickDelegate(_serialButton))
			{
				return false;
			}

			auto fn = *[](void*)
				{
					auto gallopCpnFn = *[](void*)
						{
							il2cpp_symbols::get_method_pointer<void (*)()>("umamusume.dll", "Gallop", "DialogSerialInput", "CreateDialog", -1)();
						};

					auto kakaoCpnFn = *[](void*)
						{
							auto KakaoManager = il2cpp_symbols::get_class("umamusume.dll", "", "KakaoManager");
							auto managerInstanceField = il2cpp_class_get_field_from_name_wrap(KakaoManager, "instance");

							Il2CppObject* manager;
							il2cpp_field_static_get_value(managerInstanceField, &manager);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*, Il2CppDelegate*)>(manager->klass, "OnKakaoShowCouponPopup", 2)->methodPointer(manager, nullptr, nullptr);
						};

					auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
					il2cpp_runtime_object_init(dialogData);

					dialogData =
						il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * thisObj,
							Il2CppString * headerTextArg,
							Il2CppString * message,
							Il2CppDelegate * onRight,
							unsigned long leftTextId,
							unsigned long rightTextId,
							Il2CppDelegate * onLeft,
							int dialogFormType)>(dialogData->klass, "SetSimpleTwoButtonMessage", 7)->methodPointer
							(dialogData,
								localizeextension_text_hook(GetTextIdByName(L"Menu0136")),
								il2cpp_string_new("Kakao Games 쿠폰 입력 창을 열겠습니까?"),
								CreateDelegateStatic(kakaoCpnFn), GetTextIdByName(L"Common0002"), GetTextIdByName(L"Common0003"),
								CreateDelegateStatic(gallopCpnFn), 2);

					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
				};

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_serialButton->klass, "SetOnClick", 1)->methodPointer(_serialButton, &CreateUnityActionStatic(fn)->delegate);

			return true;
		}
		return false;
	}

	UnityEngine::GameObject GetSlider(const char* name, float value, float min = 0, float max = 10, bool wholeNumbers = true, void (*onChange)(Il2CppObject*) = nullptr)
	{
		auto object = resources_load_hook(il2cpp_string_new("ui/parts/outgame/option/optionsoundvolumeslider"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto optionSlider = UnityEngine::Object::Internal_CloneSingle(object);

		auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(optionSlider->klass, "GetComponentsInternal", 6)->methodPointer;

		auto optionSoundVolumeSliderArray = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "OptionSoundVolumeSlider")), true, true, false, false, nullptr);

		auto optionSoundVolumeSlider = optionSoundVolumeSliderArray->vector[0];

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(optionSoundVolumeSlider);

		auto sliderCommonArray = getComponents(optionSlider, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "SliderCommon")), true, true, false, false, nullptr);

		auto sliderCommon = sliderCommonArray->vector[0];

		if (onChange)
		{
			auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sliderCommon->klass, "get_onValueChanged", 0)->methodPointer(sliderCommon);

			auto AddCall = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(onValueChanged->klass, "AddCall", 1);

			auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.single_class));

			auto valueChanged = CreateDelegateWithClass(delegateClass, sliderCommon, onChange);

			auto invokeableCall = il2cpp_object_new(GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "InvokableCall`1"), GetRuntimeType(il2cpp_defaults.single_class)));

			auto delegateField = il2cpp_class_get_field_from_name_wrap(invokeableCall->klass, "Delegate");
			il2cpp_field_set_value(invokeableCall, delegateField, valueChanged);

			AddCall->methodPointer(onValueChanged, invokeableCall);
		}

		try
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(sliderCommon->klass, "set_wholeNumbers", 1)->methodPointer(sliderCommon, wholeNumbers);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_minValue", 1)->methodPointer(sliderCommon, min);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_maxValue", 1)->methodPointer(sliderCommon, max);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_value", 1)->methodPointer(sliderCommon, value);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			cout << e.ex->klass->name << ": ";
			wcout << e.ex->message << endl;
		}

		auto transformArray = GetRectTransformArray(optionSlider);

		vector<Il2CppObject*> destroyTargets;

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == L"TextName"s ||
					UnityEngine::Object::Name(transform)->chars == L"ToggleMute"s ||
					UnityEngine::Object::Name(transform)->chars == L"ImageIcon"s ||
					UnityEngine::Object::Name(transform)->chars == L"Line"s)
				{
					destroyTargets.emplace_back(transform);
				}
			}
		}

		for (int i = 0; i < destroyTargets.size(); i++)
		{
			auto transform = destroyTargets[i];
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(transform->klass, "SetParent", 2)->methodPointer(transform, nullptr, false);
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_gameObject", 0)->methodPointer(transform);
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(gameObject);
		}

		destroyTargets.clear();

		transformArray = GetRectTransformArray(optionSlider);
		UnityEngine::RectTransform sliderTransform{ nullptr };
		UnityEngine::RectTransform numTransform{ nullptr };

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == L"Slider"s)
				{
					sliderTransform = UnityEngine::RectTransform(transform);
				}

				if (UnityEngine::Object::Name(transform)->chars == L"Num"s)
				{
					numTransform = UnityEngine::RectTransform(transform);
				}

				if (sliderTransform && numTransform)
				{
					break;
				}
			}
		}

		auto array = getComponents(numTransform.gameObject(), reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon = array->vector[0];

		SetTextCommonText(textCommon, L"0:00");

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(textCommon->klass, "set_OutlineSize", 1)->methodPointer(textCommon, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), L"M")));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(textCommon->klass, "set_OutlineColor", 1)->methodPointer(textCommon, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), L"White")));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(textCommon->klass, "UpdateOutline", 0)->methodPointer(textCommon);

		numTransform.anchoredPosition({ -100, 11 });
		numTransform.anchorMax({ 0, 0.5 });
		numTransform.anchorMin({ 0, 0.5 });
		numTransform.sizeDelta({ 80, 50 });
		numTransform.pivot({ 0, 0.5 });

		auto parent = numTransform.GetParent();

		auto clonedText = UnityEngine::GameObject(UnityEngine::Object::Internal_CloneSingle(numTransform.gameObject()));
		UnityEngine::Object::Name(clonedText, il2cpp_string_new((name + "_total"s).data()));
		auto totalNumTransform = static_cast<UnityEngine::RectTransform>(clonedText.transform());

		totalNumTransform.SetParent(parent);

		auto array1 = getComponents(clonedText, reinterpret_cast<Il2CppType*>(GetRuntimeType(
			"umamusume.dll", "Gallop", "TextCommon")), true, true, false, false, nullptr);

		auto textCommon1 = array1->vector[0];

		SetTextCommonText(textCommon1, L"1:00");

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(textCommon1->klass, "set_OutlineSize", 1)->methodPointer(textCommon1, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), L"M")));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(textCommon1->klass, "set_OutlineColor", 1)->methodPointer(textCommon1, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), L"White")));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(textCommon1->klass, "UpdateOutline", 0)->methodPointer(textCommon1);

		totalNumTransform.anchoredPosition({ 100, 11 });
		totalNumTransform.anchorMax({ 1, 0.5 });
		totalNumTransform.anchorMin({ 1, 0.5 });
		totalNumTransform.sizeDelta({ 80, 50 });
		totalNumTransform.pivot({ 1, 0.5 });

		auto gameObject = sliderTransform.gameObject();

		UnityEngine::Object::Name(gameObject, il2cpp_string_new(name));

		return gameObject;
	}

	void* Object_Internal_CloneSingleWithParent_orig = nullptr;
	Il2CppObject* Object_Internal_CloneSingleWithParent_hook(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		auto cloned = reinterpret_cast<decltype(Object_Internal_CloneSingleWithParent_hook)*>(Object_Internal_CloneSingleWithParent_orig)(data, parent, worldPositionStays);

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"ProfileTopView") != wstring::npos && Game::CurrentGameRegion == Game::Region::KOR)
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

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"DialogOptionHome") != wstring::npos)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(cloned->klass, "GetComponentsInternal", 6)->methodPointer;
			auto rectTransformArray = getComponents(cloned, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

			for (int i = 0; i < rectTransformArray->max_length; i++)
			{
				auto rectTransform = rectTransformArray->vector[i];

				if (rectTransform && wide_u8(UnityEngine::Object::Name(rectTransform)->chars) == "Content")
				{
					InitOptionLayout(rectTransform);
					break;
				}
			}

			SetupOptionLayout();
		}

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"DialogOptionLiveTheater") != wstring::npos)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(cloned->klass, "GetComponentsInternal", 6)->methodPointer;
			auto rectTransformArray = getComponents(cloned, reinterpret_cast<Il2CppType*>(GetRuntimeType(
				"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

			for (int i = 0; i < rectTransformArray->max_length; i++)
			{
				auto rectTransform = rectTransformArray->vector[i];

				if (rectTransform && wide_u8(UnityEngine::Object::Name(rectTransform)->chars) == "Content")
				{
					InitOptionLayout(rectTransform);
					break;
				}
			}

			SetupLiveOptionLayout();
		}

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"CharacterHomeTopUI") != wstring::npos)
		{
			auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(cloned->klass, "GetComponent", 1)->methodPointer;
			auto CharacterHomeTopUI = getComponent(cloned, GetRuntimeType("umamusume.dll", "Gallop", "CharacterHomeTopUI"));

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
					}
				}
			}
		}

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"LiveView") != wstring::npos)
		{
			auto gameObject = UnityEngine::GameObject(cloned);
			auto contentsRoot = gameObject.transform().Find(il2cpp_string_new(config::live_slider_always_show ? "ContentsRoot" : "ContentsRoot/MenuRoot"));
			auto slider = GetSlider("live_slider", 0, 0, 180, false,
				*[](Il2CppObject* sliderCommon)
				{
					auto value = GetOptionSliderValue("live_slider");

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

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"LiveChampionsTextController") != wstring::npos)
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

							int unityWidth = UnityEngine::Screen::width();

							int unityHeight = UnityEngine::Screen::height();

							if (Game::CurrentGameRegion == Game::Region::KOR)
							{
								auto _flashCanvasScalerField = il2cpp_class_get_field_from_name_wrap(ChampionsTextController->klass, "_flashCanvasScaler");
								Il2CppObject* _flashCanvasScaler;
								il2cpp_field_get_value(ChampionsTextController, _flashCanvasScalerField, &_flashCanvasScaler);

								if (unityWidth < unityHeight)
								{
									float scale = min(config::freeform_ui_scale_portrait, max(1.0f, unityHeight * config::runtime::ratioVertical) * config::freeform_ui_scale_portrait);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(_flashCanvasScaler->klass, "set_referenceResolution", 1)->methodPointer(_flashCanvasScaler, UnityEngine::Vector2{ static_cast<float>(unityWidth / scale), static_cast<float>(unityHeight / scale) });
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(unityHeight / scale), static_cast<float>(unityHeight / scale) });
								}
								else
								{
									float scale = min(config::freeform_ui_scale_landscape, max(1.0f, unityWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(_flashCanvasScaler->klass, "set_referenceResolution", 1)->methodPointer(_flashCanvasScaler, UnityEngine::Vector2{ static_cast<float>(unityWidth / scale), static_cast<float>(unityHeight / scale) });
									il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(unityHeight / scale), static_cast<float>(unityHeight / scale) });
								}
							}
							else
							{
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(root->klass, "SetScreenReferenceSize", 1)->methodPointer(root, UnityEngine::Vector2{ ratio_16_9 * static_cast<float>(unityHeight), static_cast<float>(unityHeight) });
							}
						}
					}
				}));

			// Delay 50ms
			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, &updateScreenReferenceSize->delegate, true);
		}

		return cloned;
	}

	void* Object_Internal_CloneSingle_orig = nullptr;
	Il2CppObject* Object_Internal_CloneSingle_hook(Il2CppObject* data)
	{
		auto cloned = reinterpret_cast<decltype(Object_Internal_CloneSingle_hook)*>(Object_Internal_CloneSingle_orig)(data);

		if (wstring(UnityEngine::Object::Name(cloned)->chars).find(L"DialogHomeMenuMain") != wstring::npos && Game::CurrentGameRegion == Game::Region::KOR)
		{
			auto homeMenuMain = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(cloned->klass, "GetComponent", 1)->methodPointer(cloned, GetRuntimeType("umamusume.dll", "Gallop", "DialogHomeMenuMain"));

			auto _dataTransButtonField = il2cpp_class_get_field_from_name_wrap(homeMenuMain->klass, "_dataTransButton");
			Il2CppObject* _dataTransButton;
			il2cpp_field_get_value(homeMenuMain, _dataTransButtonField, &_dataTransButton);

			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_dataTransButton->klass, "get_gameObject", 0)->methodPointer(_dataTransButton);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

			static Il2CppDelegate* updateHomeMenuMainButton;
			updateHomeMenuMainButton = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
				{
					if (!UpdateHomeMenuMainButton())
					{
						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateHomeMenuMainButton, true);
					}
				}))->delegate;

			// Delay 50ms
			il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateHomeMenuMainButton, true);
		}

		return cloned;
	}

	Il2CppObject* currentOptionObj = nullptr;

	void* resources_load_orig = nullptr;
	Il2CppObject* resources_load_hook(Il2CppString* path, Il2CppObject* type)
	{
		wstring wName = path->chars;

		if (wName == L"ui/views/titleview"s)
		{
			if (find_if(replaceAssetNames.begin(), replaceAssetNames.end(), [](const wstring& item)
				{
					return item.find(L"utx_obj_title_logo_umamusume") != wstring::npos;
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
					reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D")));
				auto m_TextureField = il2cpp_class_get_field_from_name_wrap(imgCommon->klass->parent, "m_Texture");
				il2cpp_field_set_value(imgCommon, m_TextureField, texture);
				return gameObj;
			}
		}

		if (wName == L"TMP Settings"s && config::replace_to_custom_font && fontAssets)
		{
			auto object = reinterpret_cast<decltype(resources_load_hook)*>(resources_load_orig)(path, type);
			auto fontAssetField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_defaultFontAsset");
			il2cpp_field_set_value(object, fontAssetField, GetCustomTMPFont());
			return object;
		}

		if (wName.ends_with(L"dialogoptionhome"))
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
						/*auto _notityObjListField = il2cpp_class_get_field_from_name_wrap(obj->klass, "_notityObjList");
						Il2CppObject* _notityObjList;
						il2cpp_field_get_value(obj, _notityObjListField, &_notityObjList);

						if (_notityObjList)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_notityObjList->klass, "Clear", 0)->methodPointer(_notityObjList);
						}*/

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

							if (rectTransform && UnityEngine::Object::Name(rectTransform)->chars == L"Content"s)
							{
								// InitOptionLayout(rectTransform);
								break;
							}
						}
					}
				}
			}

			return object;
		}


		if (wName.ends_with(L"dialogoptionlivetheater"))
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
				"umamusume.dll", "Gallop", "PartsOptionPageLive")), true, true, false, false, nullptr);

			if (array)
			{
				for (int i = 0; i < array->max_length; i++)
				{
					auto obj = array->vector[i];

					if (obj && obj->klass->name == "PartsOptionPageLive"s)
					{
						auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(obj->klass, "get_gameObject", 0)->methodPointer(obj);

						auto rectTransformArray = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
							"UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform")), true, true, false, false, nullptr);

						for (int j = 0; j < rectTransformArray->max_length; j++)
						{
							auto rectTransform = rectTransformArray->vector[j];

							if (rectTransform && UnityEngine::Object::Name(rectTransform)->chars == L"Content"s)
							{
								// InitOptionLayout(rectTransform);
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
		auto uobject_name = UnityEngine::Object::Name(texture2D);
		if (!wstring(uobject_name->chars).empty())
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

	void* Material_set_mainTexture_orig = nullptr;
	void Material_set_mainTexture_hook(Il2CppObject* _this, Il2CppObject* texture)
	{
		if (texture)
		{
			if (!wstring(UnityEngine::Object::Name(texture)->chars).empty())
			{
				auto newTexture = GetReplacementAssets(
					UnityEngine::Object::Name(texture),
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
			auto uobject_name = UnityEngine::Object::Name(texture);
			if (!wstring(uobject_name->chars).empty())
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
		if (texture && !wstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
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
		if (texture && !wstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
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
		if (texture && !wstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
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
		if (!wstring(UnityEngine::Object::Name(texture)->chars).empty())
		{
			auto newTexture = GetReplacementAssets(
				UnityEngine::Object::Name(texture),
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
		cout << "GameObject_GetComponentFastPath " << wide_u8(name->chars) << endl;*/
		reinterpret_cast<decltype(GameObject_GetComponentFastPath_hook)*>(GameObject_GetComponentFastPath_orig)(_this, type, oneFurtherThanResultValue);
		auto helper = CastHelper{};
		int objSize = sizeof(helper.obj);
		memmove(&helper, reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), sizeof(CastHelper));
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
			memmove(reinterpret_cast<void*>(oneFurtherThanResultValue - objSize), &helper, sizeof(CastHelper));
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

	void* Screen_RequestOrientation_orig = nullptr;

	void Screen_RequestOrientation_hook(UnityEngine::ScreenOrientation orientation)
	{
		if (config::freeform_window)
		{
			reinterpret_cast<decltype(Screen_RequestOrientation_hook)*>(Screen_RequestOrientation_orig)(UnityEngine::ScreenOrientation::AutoRotation);

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

	void* WaitDeviceOrientation_orig = nullptr;

	Il2CppObject* WaitDeviceOrientation_hook(UnityEngine::ScreenOrientation targetOrientation)
	{
		if (!config::freeform_window)
		{
			return reinterpret_cast<decltype(WaitDeviceOrientation_hook)*>(WaitDeviceOrientation_orig)(
				targetOrientation);
		}

		auto arrayList = il2cpp_object_new(il2cpp_symbols::get_class("mscorlib.dll", "System.Collections", "ArrayList"));
		il2cpp_runtime_object_init(arrayList);
		return il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(arrayList->klass, "GetEnumerator", 0)->methodPointer(arrayList);
	}

	Il2CppObject* UIManager_WaitBootSetup_hook(Il2CppObject* _this);

	void* BootSystem_Awake_orig = nullptr;

	void BootSystem_Awake_hook(Il2CppObject* _this)
	{
		// UnityEngine::Resolution r;
		// get_resolution_stub(&r);
		// last_display_width = r.width;
		// last_display_height = r.height;

		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)()>("Cute.Core.Assembly.dll", "Cute.Core", "Perf/Time", "get_Instance", IgnoreNumberOfArguments)();

		auto StartCoroutine = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "MonoBehaviour", [](const MethodInfo* method)
			{
				return method->name == "StartCoroutine"s && method->parameters[0].parameter_type->type == IL2CPP_TYPE_CLASS;
			});

		StartCoroutine(_this, il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_this->klass, "BootCoroutine", 0)->methodPointer(_this));

		/*auto uiManager = Gallop::UIManager::Instance();

		auto defaultResolution = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)()>(uiManager, "get_DefaultResolution", 0)->methodPointer();

		auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(uiManager, "GetCanvasScalerList", 0)->methodPointer(uiManager);
		for (int i = 0; i < canvasScalerList->max_length; i++)
		{
			auto canvasScaler = canvasScalerList->vector[i];
			if (canvasScaler)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, defaultResolution);

				if (defaultResolution.x < defaultResolution.y)
				{
					float scale = min(1, max(1.0f, defaultResolution.y / 1080) * config::freeform_ui_scale_portrait / (defaultResolution.y / 1080));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
				else
				{
					float scale = min(1, max(1.0f, defaultResolution.x / 1920) * config::freeform_ui_scale_landscape / (defaultResolution.x / 1920));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
			}
		}

		auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(uiManager, "_bgCamera");
		Il2CppObject* _bgCamera;
		il2cpp_field_get_value(uiManager, _bgCameraField, &_bgCamera);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
			il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "AdjustSafeArea", 0)->methodPointer(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "CreateRenderTextureFromScreen", 0)->methodPointer(_this);*/

		if (!config::freeform_window)
		{
			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");

			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			bool isLandscape = width > height;

			auto changedSize = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)(float, float, bool)>(StandaloneWindowResize, "GetChangedSize", 3)->methodPointer(isLandscape ? height : width, isLandscape ? width : height, true);

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
	Il2CppObject* (*MovieManager_GetMovieInfo)(Il2CppObject* _this, Cute::Cri::MoviePlayerHandle playerHandle);

	void* MovieManager_SetImageUvRect_orig = nullptr;

	void MovieManager_SetImageUvRect_hook(Il2CppObject* _this, Cute::Cri::MoviePlayerHandle playerHandle, UnityEngine::Rect uv)
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
		if (width < height && !Gallop::StandaloneWindowResize::IsVirt())
		{
			int rWidth = UnityEngine::Screen::width();
			int rHeight = UnityEngine::Screen::height();
			if (roundf(config::runtime::ratioHorizontal * (max(1.0f, rHeight * config::runtime::ratioVertical) * (config::freeform_window ? config::freeform_ui_scale_landscape : config::ui_scale))) == uv.height)
			{
				uv.height = rWidth;
			}
			uv.width = rHeight;
		}

		reinterpret_cast<decltype(MovieManager_SetImageUvRect_hook)*>(MovieManager_SetImageUvRect_orig)(_this, playerHandle, uv);
	}

	void* MovieManager_SetScreenSize_orig = nullptr;

	void MovieManager_SetScreenSize_hook(Il2CppObject* _this, Cute::Cri::MoviePlayerHandle playerHandle, UnityEngine::Vector2 screenSize)
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
		if (width < height && !Gallop::StandaloneWindowResize::IsVirt())
		{
			int rWidth = UnityEngine::Screen::width();
			int rHeight = UnityEngine::Screen::height();
			if (roundf(config::runtime::ratioHorizontal * (max(1.0f, rHeight * config::runtime::ratioVertical) * (config::freeform_window ? config::freeform_ui_scale_landscape : config::ui_scale))) == screenSize.y)
			{
				screenSize.y = rWidth;
			}
			screenSize.x = rHeight;
		}

		reinterpret_cast<decltype(MovieManager_SetScreenSize_hook)*>(MovieManager_SetScreenSize_orig)(_this, playerHandle, screenSize);
	}

	void* MoviePlayerForUI_AdjustScreenSize_orig = nullptr;

	void MoviePlayerForUI_AdjustScreenSize_hook(Il2CppObject* _this, UnityEngine::Vector2 dispRectWH, bool isPanScan)
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

		// auto ratio = floorf(static_cast<float>(width) / height * 100) / 100;
		auto ratio1 = static_cast<float>(width) / static_cast<float>(height);

		if (!Gallop::StandaloneWindowResize::IsVirt())
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

			/*if (roundf(ratio_horizontal * (max(1.0f, rHeight * config::runtime::ratioVertical) * (config::freeform_window ? config::freeform_ui_scale_landscape : config::ui_scale))) == dispRectWH.y)
			{
				dispRectWH.y = rWidth;
				dispRectWH.x = rHeight;
			}
			else
			{*/
			float scale = min(config::freeform_ui_scale_landscape, max(1.0f, rWidth / config::runtime::ratioHorizontal) * config::freeform_ui_scale_landscape);

			if (roundf(dispRectWH.y * scale) != rHeight)
			{
				dispRectWH.x = dispRectWH.y * ratio_16_9;
			}
			else
			{
				dispRectWH.x = dispRectWH.y * ratio1;
			}
			// }
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
		UnityEngine::Application::targetFrameRate(30);
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
		auto okText = GetTextIdByName(L"Common0009");
		auto errorText = GetTextIdByName(L"Common0071");

		auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
		il2cpp_runtime_object_init(dialogData);
		dialogData =
			il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * _this, unsigned long headerTextId, Il2CppString * message, Il2CppObject * onClickCenterButton, unsigned long closeTextId)>(dialogData->klass, "SetSimpleOneButtonMessage", 4)->methodPointer
			(dialogData, errorText, local::get_localized_string(il2cpp_string_new16(GotoTitleErrorJa.data())), nullptr, okText);
		errorDialog = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * data, bool isEnableOutsideClick)>("umamusume.dll", "Gallop", "DialogManager", "PushSystemDialog", 2)(dialogData, true);
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

									auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(data->klass, "ContentsObject");
									Il2CppObject* ContentsObject;
									il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

									auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

									auto _inputFieldField = il2cpp_class_get_field_from_name_wrap(idSearch->klass, "_inputField");
									Il2CppObject* _inputField;
									il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

									auto text1 = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

									wstring searchId = text1->chars;

									auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

									auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
									auto viewerIdObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredLong(*)(Il2CppObject*)>(workUserData->klass, "get_ViewerId", 0)->methodPointer(workUserData);
									auto viewerId = viewerIdObscured.GetDecrypted();

									if (searchId == to_wstring(viewerId))
									{
										il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FriendViewController", "PushErrorSearchOwn", IgnoreNumberOfArguments)();
										return;
									}

									auto onModifyFriendCloseFn = *[]()
										{
											auto controller = GetCurrentViewController();
											il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "UpdateCurrentTabView", 0)->methodPointer(controller);
										};

									il2cpp_symbols::get_method_pointer<void (*)(int64_t, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppObject*, bool, Il2CppDelegate*, bool)>("umamusume.dll", "Gallop", "DialogTrainerInfo", "PushDialog", 9)
										(stoll(searchId), CreateDelegateStatic(onModifyFriendCloseFn), nullptr, nullptr, nullptr, nullptr, false, nullptr, false);
								};
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "DialogIDSearch", "PushDialog", 1)(CreateDelegateStatic(fn));

							auto dialog = GetFrontDialog();
							auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

							auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(data->klass, "ContentsObject");
							Il2CppObject* ContentsObject;
							il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

							auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

							auto _inputFieldField = il2cpp_class_get_field_from_name_wrap(idSearch->klass, "_inputField");
							Il2CppObject* _inputField;
							il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

							auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_inputField->klass, "get_onValueChanged", 0)->methodPointer(_inputField);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(onValueChanged->klass, "RemoveAllListeners", 0)->methodPointer(onValueChanged);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_inputField->klass, "Start", 0)->methodPointer(_inputField);


							auto valueChangeFn = *[](Il2CppObject* obj)
								{
									auto dialog = GetFrontDialog();
									auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

									auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(data->klass, "ContentsObject");
									Il2CppObject* ContentsObject;
									il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

									auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogIDSearch"));

									auto _inputFieldField = il2cpp_class_get_field_from_name_wrap(idSearch->klass, "_inputField");
									Il2CppObject* _inputField;
									il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

									auto value = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

									Il2CppObject* rightButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, uint64_t)>(dialog->klass, "GetButtonObj", 1)->methodPointer(dialog, 2);

									bool valid = false;

									if (value && value->length >= 9)
									{
										try
										{
											stoll(value->chars);
											valid = true;
										}
										catch (invalid_argument e)
										{
										}
									}

									if (rightButton)
									{
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(rightButton->klass, "set_interactable", 1)->methodPointer(rightButton, valid);
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(rightButton->klass, "SetNotificationMessage", 1)->methodPointer(rightButton, valid ? il2cpp_string_new("") : localizeextension_text_hook(GetTextIdByName(L"Friend0013")));
									}
								};

							auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType("mscorlib.dll", "System", "String"));

							void** params = new void* [1];
							params[0] = CreateDelegateWithClass(delegateClass, _inputField, valueChangeFn);

							Il2CppException* exception;

							il2cpp_runtime_invoke_type<void>(il2cpp_class_get_method_from_name(onValueChanged->klass, "AddListener", 1), onValueChanged, params, &exception);

							auto _myIdTextField = il2cpp_class_get_field_from_name_wrap(idSearch->klass, "_myIdText");
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

					auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
					il2cpp_runtime_object_init(dialogData);

					dialogData =
						il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * thisObj,
							Il2CppString * headerTextArg,
							Il2CppString * message,
							Il2CppDelegate * onRight,
							unsigned long leftTextId,
							unsigned long rightTextId,
							Il2CppDelegate * onLeft,
							int dialogFormType)>(dialogData->klass, "SetSimpleTwoButtonMessage", 7)->methodPointer
							(dialogData,
								localizeextension_text_hook(GetTextIdByName(L"Friend0039")),
								il2cpp_string_new("트레이너 ID로 검색하시겠습니까?"),
								CreateDelegateStatic(viewerIdSearchFn), GetTextIdByName(L"Outgame0002"), GetTextIdByName(L"Common0003"),
								CreateDelegateStatic(playerIdSearchFn), 2);

					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
				};

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(ButtonIDSearch->klass, "SetOnClick", 1)->methodPointer(ButtonIDSearch, &CreateUnityActionStatic(fn)->delegate);
			return true;

		}
		return false;
	}

	Il2CppObject* voiceButtonTarget = nullptr;

	Il2CppDelegate* updateVoiceButton = nullptr;

	void UpdateVoiceButton()
	{
		Il2CppArraySize_t<Il2CppObject*>* voiceButtonList;
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			voiceButtonList = UnityEngine::Object::FindObjectsByType(
				GetRuntimeType("umamusume.dll", "Gallop", "PartsEpisodeExtraVoiceButton"), UnityEngine::FindObjectsInactive::Exclude, UnityEngine::FindObjectsSortMode::None);
		}
		else
		{
			voiceButtonList = UnityEngine::Object::FindObjectsOfType(
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

							auto newFn = *[](Il2CppObject* _this)
								{
									auto storyIdField = il2cpp_class_get_field_from_name_wrap(voiceButtonTarget->klass, "storyId");
									int storyId;
									il2cpp_field_get_value(voiceButtonTarget, storyIdField, &storyId);

									FieldInfo* thisField = nullptr;
									void* iter = nullptr;
									while (FieldInfo* field = il2cpp_class_get_fields(voiceButtonTarget->klass, &iter)) {
										if (string(field->name).find("this") != string::npos) {
											thisField = field;
										}
									}
									Il2CppObject* thisObj;
									il2cpp_field_get_value(voiceButtonTarget, thisField, &thisObj);

									reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(
										thisObj->klass, "StopVoiceIfNeed", 0)->methodPointer)(thisObj);

									auto onLeft = CreateDelegateStatic(*[](void*)
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

											auto action = CreateDelegateStatic(*[]() {});

											il2cpp_symbols::get_method_pointer<void (*)(int,
												Il2CppDelegate*,
												Il2CppDelegate*)>(
													"umamusume.dll", "Gallop",
													"DialogAnnounceEvent", "Open", 3)(announceId, action, action);
										});

									if (storyId < 1005)
									{
										auto onRight = CreateDelegateStatic(*[](void*)
											{
												auto storyIdField = il2cpp_class_get_field_from_name_wrap(voiceButtonTarget->klass, "storyId");
												int storyId;
												il2cpp_field_get_value(voiceButtonTarget, storyIdField, &storyId);

												auto cueSheetNameField = il2cpp_class_get_field_from_name_wrap(voiceButtonTarget->klass, "cueSheetName");
												Il2CppString* cueSheetName;
												il2cpp_field_get_value(voiceButtonTarget, cueSheetNameField, &cueSheetName);

												auto cueNameField = il2cpp_class_get_field_from_name_wrap(voiceButtonTarget->klass, "cueName");
												Il2CppString* cueName;
												il2cpp_field_get_value(voiceButtonTarget, cueNameField, &cueName);

												string optionKey = string("kakaoUmaAnnounceEvent").append(to_string(storyId));

												auto KakaoManager = il2cpp_symbols::get_class("umamusume.dll", "", "KakaoManager");
												auto managerInstanceField = il2cpp_class_get_field_from_name_wrap(KakaoManager, "instance");

												Il2CppObject* manager;
												il2cpp_field_static_get_value(managerInstanceField, &manager);

												auto url = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*, Il2CppString*)>(
													manager->klass, "GetKakaoOptionValue", 1)->methodPointer(manager, il2cpp_string_new(optionKey.data()));


												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, Il2CppDelegate*)>(manager->klass, "OnKakaoShowInAppWebView", 2)->methodPointer(manager, url, CreateDelegateStatic(*[]()
													{
														FieldInfo* thisField = nullptr;
														void* iter = nullptr;
														while (FieldInfo* field = il2cpp_class_get_fields(voiceButtonTarget->klass, &iter)) {
															if (string(field->name).find("this") != string::npos) {
																thisField = field;
															}
														}
														Il2CppObject* thisObj;
														il2cpp_field_get_value(voiceButtonTarget, thisField, &thisObj);

														reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(
															thisObj->klass, "StopVoiceIfNeed", 0)->methodPointer)(thisObj);
													}));

												FieldInfo* thisField;
												void* iter = nullptr;
												while (FieldInfo* field = il2cpp_class_get_fields(voiceButtonTarget->klass, &iter))
												{
													if (string(field->name).find("this") != string::npos)
													{
														thisField = field;
													}
												}

												Il2CppObject* parentObj;
												il2cpp_field_get_value(voiceButtonTarget, thisField, &parentObj);

												il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, Il2CppString*)>(
													parentObj->klass, "PlayAnnounceVoice", 2)->methodPointer(parentObj, cueSheetName, cueName);
											});

										auto dialogData = il2cpp_object_new(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
										il2cpp_runtime_object_init(dialogData);

										dialogData =
											il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject * thisObj,
												Il2CppString * headerTextArg,
												Il2CppString * message,
												Il2CppDelegate * onRight,
												unsigned long leftTextId,
												unsigned long rightTextId,
												Il2CppDelegate * onLeft,
												int dialogFormType)>(dialogData->klass, "SetSimpleTwoButtonMessage", 7)->methodPointer
												(dialogData,
													localizeextension_text_hook(GetTextIdByName(L"StoryEvent0079")),
													il2cpp_string_new("해당 스토리 이벤트는 개최 정보가 누락되어있습니다.\n\n웹 페이지를 보시겠습니까?"),
													onRight, GetTextIdByName(L"Common0002"), GetTextIdByName(L"Common0001"),
													onLeft, 2);

										il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);
									}
									else
									{
										reinterpret_cast<void (*)(void*)>(onLeft->method_ptr)(nullptr);
									}
								};
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button,
								&CreateUnityAction(voiceButtonTarget, newFn)->delegate);
						}
					}
				}
			}
		}
	}

	int currentStoryId = 0;

	void* PartsEpisodeList_SetupStoryExtraEpisodeList_orig = nullptr;

	void PartsEpisodeList_SetupStoryExtraEpisodeList_hook(Il2CppObject* _this, Il2CppObject* extraSubCategory, Il2CppObject* partDataList, Il2CppObject* partData, Il2CppDelegate* onClick)
	{
		reinterpret_cast<decltype(PartsEpisodeList_SetupStoryExtraEpisodeList_hook)*>(PartsEpisodeList_SetupStoryExtraEpisodeList_orig)(_this, extraSubCategory, partDataList, partData, onClick);

		auto partDataIdField = il2cpp_class_get_field_from_name_wrap(partData->klass, "<Id>k__BackingField");
		il2cpp_field_get_value(partData, partDataIdField, &currentStoryId);

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
						Il2CppArraySize_t<Il2CppObject*>* arr;
						il2cpp_field_get_value(bannerList, itemsField, &arr);

						int announceId = -1;

						for (int i = 0; i < arr->max_length; i++)
						{
							auto item = arr->vector[i];
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
									conditionValue == currentStoryId)
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

						if (announceId == -1 && currentStoryId < 1005)
						{
							announceId = currentStoryId - 1002;
						}

						auto action = CreateDelegateStatic(*[]() {});

						il2cpp_symbols::get_method_pointer<void (*)(int,
							Il2CppDelegate*,
							Il2CppDelegate*)>(
								"umamusume.dll", "Gallop",
								"DialogAnnounceEvent", "Open", 3)(announceId, action, action);
					});
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button,
					&CreateUnityAction(il2cpp_value_box(il2cpp_symbols::get_class("mscorlib.dll", "System", "Int32"), &currentStoryId), newFn)->delegate);
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

	void* UIManager_WaitBootSetup_orig = nullptr;

	Il2CppObject* UIManager_WaitBootSetup_hook(Il2CppObject* _this)
	{
		auto enumerator = reinterpret_cast<decltype(UIManager_WaitBootSetup_hook)*>(UIManager_WaitBootSetup_orig)(_this);

		auto move_next = il2cpp_class_get_method_from_name_type<bool (*)(
			Il2CppObject * _this)>(enumerator->klass,
				"MoveNext",
				0)->methodPointer;
		while (move_next(enumerator)) {}

		auto defaultResolution = Gallop::UIManager::DefaultResolution();

		auto canvasScalerList = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*)>(_this->klass, "GetCanvasScalerList", 0)->methodPointer(_this);
		for (int i = 0; i < canvasScalerList->max_length; i++)
		{
			auto canvasScaler = canvasScalerList->vector[i];
			if (canvasScaler)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(canvasScaler->klass, "set_referenceResolution", 1)->methodPointer(canvasScaler, defaultResolution);

				if (defaultResolution.x < defaultResolution.y)
				{
					float scale = min(1.0f, max(1.0f, defaultResolution.y / 1080) * config::freeform_ui_scale_portrait / (defaultResolution.y / 1080));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
				else
				{
					float scale = min(1.0f, max(1.0f, defaultResolution.x / 1920) * config::freeform_ui_scale_landscape / (defaultResolution.x / 1920));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasScaler->klass, "set_scaleFactor", 1)->methodPointer(canvasScaler, scale);
				}
			}
		}

		auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_bgCamera");
		Il2CppObject* _bgCamera;
		il2cpp_field_get_value(_this, _bgCameraField, &_bgCamera);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Color)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
			il2cpp_symbols::get_method_pointer<UnityEngine::Color(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "AdjustSafeArea", 0)->methodPointer(_this);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_this->klass, "CreateRenderTextureFromScreen", 0)->methodPointer(_this);

		return enumerator;
	}

	void* Input_get_mousePosition_Injected_orig = nullptr;

	void Input_get_mousePosition_Injected_hook(UnityEngine::Vector3* out)
	{
		reinterpret_cast<decltype(Input_get_mousePosition_Injected_hook)*>(Input_get_mousePosition_Injected_orig)(out);
	}

	wstring GetLoginURL()
	{
		auto hInternet = InternetOpenW(L"DMMGamePlayer5/5.2.47", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

		auto hConnect = InternetConnectW(hInternet, L"apidgp-gameplayer.games.dmm.com", INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

		LPCWSTR types[] = { L"application/json", NULL };
		auto hReq = HttpOpenRequestW(hConnect, L"GET", L"/v5/loginurl", nullptr, nullptr, types, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

		auto res = HttpSendRequestW(hReq, nullptr, 0, nullptr, 0);

		if (!res)
		{
			return L"";
		}

		DWORD dwSize = 0;
		DWORD dwSizeLen = sizeof(DWORD);

		HttpQueryInfoW(hReq, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwSize, &dwSizeLen, 0);

		char* buffer = new char[dwSize + 1];

		DWORD dwBytesRead;
		BOOL bRead = InternetReadFile(hReq, buffer, dwSize + 1, &dwBytesRead);

		if (!bRead)
		{
			return L"";
		}
		else
		{
			buffer[dwBytesRead] = 0;
		}

		InternetCloseHandle(hReq);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		rapidjson::Document document;

		document.Parse(buffer);
		delete[] buffer;

		if (document.HasParseError())
		{
			cout << "Response JSON parse error: " << GetParseError_En(document.GetParseError()) << " (" << document.GetErrorOffset() << ")" << endl;
			return L"";
		}

		if (document.HasMember("result_code") && document["result_code"].GetInt() == 100)
		{
			return u8_wide(document["data"].GetObjectW()["url"].GetString());
		}

		return L"";
	}

	string GetGameArgs(wstring sessionId, wstring secureId)
	{
		auto hInternet = InternetOpenW(L"DMMGamePlayer5/5.2.47", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

		auto hConnect = InternetConnectW(hInternet, L"apidgp-gameplayer.games.dmm.com", INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

		LPCWSTR types[] = { L"application/json", NULL };
		auto hReq = HttpOpenRequestW(hConnect, L"POST", L"/v5/launch/cl", nullptr, nullptr, types, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

		wstringstream headerStream;
		headerStream << L"Client-App: DMMGamePlayer5" << endl
			<< L"Client-version: 5.2.47" << endl
			<< L"Cookie: login_session_id=" << sessionId << L";login_secure_id=" << secureId << endl;

		auto body = R"({"product_id":"umamusume","game_type":"GCL","launch_type":"LIB","game_os":"win","user_os":"win","mac_address":"null","hdd_serial":"null","motherboard":"null"})"s;
		auto res = HttpSendRequestW(hReq, headerStream.str().data(), 0, body.data(), body.size());

		if (!res)
		{
			return "";
		}

		DWORD dwSize = 0;
		DWORD dwSizeLen = sizeof(DWORD);

		HttpQueryInfoW(hReq, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwSize, &dwSizeLen, 0);

		char* buffer = new char[dwSize + 1];

		DWORD dwBytesRead;
		BOOL bRead = InternetReadFile(hReq, buffer, dwSize + 1, &dwBytesRead);

		if (!bRead)
		{
			return "";
		}
		else
		{
			buffer[dwBytesRead] = 0;
		}

		InternetCloseHandle(hReq);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		rapidjson::Document document;

		document.Parse(buffer);
		delete[] buffer;

		if (document.HasParseError())
		{
			cout << "Response JSON parse error: " << GetParseError_En(document.GetParseError()) << " (" << document.GetErrorOffset() << ")" << endl;
			return "";
		}

		if (document.HasMember("result_code") && document["result_code"].GetInt() == 100)
		{
			return string(document["data"].GetObjectW()["execute_args"].GetString());
		}
		else if (document.HasMember("error") && document["error"].IsString())
		{
			thread([](const char* error)
				{
					auto t = il2cpp_thread_attach(il2cpp_domain_get());

					auto dialogData = il2cpp_object_new(
						il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon/Data"));
					il2cpp_runtime_object_init(dialogData);

					dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
						ULONG headerTextId,
						Il2CppString * message,
						Il2CppDelegate * onClose,
						ULONG closeTextId)>(
							il2cpp_class_get_method_from_name(dialogData->klass, "SetSimpleOneButtonMessage",
								4)->methodPointer
							)(dialogData, GetTextIdByName(L"Error0014"), il2cpp_string_new(error), nullptr, GetTextIdByName(L"Common0007"));

					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject* data)>("umamusume.dll", "Gallop", "DialogManager", "PushDialog", 1)(dialogData);

					il2cpp_thread_detach(t);
				},
				document["error"].GetString()
			).detach();
		}

		return "";
	}

	wil::com_ptr<ICoreWebView2Controller> webviewController;
	wil::com_ptr<ICoreWebView2> webview;

	bool isLoginWebViewOpen = false;

	string viewerId;
	string onetimeToken;

	void* Certification_initDmmPlatformData_orig = nullptr;

	void Certification_initDmmPlatformData_hook()
	{
		reinterpret_cast<decltype(Certification_initDmmPlatformData_hook)*>(Certification_initDmmPlatformData_orig)();

		if (!viewerId.empty() && !onetimeToken.empty())
		{
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmViewerId", 1)(il2cpp_string_new(viewerId.data()));
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmOnetimeToken", 1)(il2cpp_string_new(onetimeToken.data()));
		}
	}

	LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			if (webviewController != nullptr) {
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				webviewController->put_Bounds(bounds);
			};
			break;
		case WM_DESTROY:
			isLoginWebViewOpen = false;
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
			break;
		}

		return NULL;
	}

	DWORD WINAPI WebViewThread(LPVOID)
	{
		IsGUIThread(TRUE);

		WNDCLASSEX wcex = {};

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WebViewWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIconW(hInstance, reinterpret_cast<LPWSTR>(IDI_APP_ICON));
		wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = L"WebViewWindow";
		wcex.hIconSm = LoadIconW(wcex.hInstance, reinterpret_cast<LPWSTR>(IDI_APP_ICON));

		RegisterClassExW(&wcex);

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		const auto hWnd = CreateWindowExW(NULL, L"WebViewWindow", L"Login DMM",
			WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT,
			900, 900,
			nullptr, NULL, hInstance, NULL);

		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		auto envOptions = Make<CoreWebView2EnvironmentOptions>();
#ifdef _DEBUG
		envOptions->put_AdditionalBrowserArguments(L"--enable-logging --v=1");
#endif

		wstring loginUrl = GetLoginURL();

		PWSTR path;
		SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &path);

		wstring combinedPath = path + L"\\DMMWebView2"s;

		CreateCoreWebView2EnvironmentWithOptions(nullptr, combinedPath.data(), envOptions.Get(),
			Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[hWnd, loginUrl](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
				{
					env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[env, hWnd, loginUrl](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
						{
							if (controller != nullptr)
							{
								webviewController = controller;
								webviewController->get_CoreWebView2(&webview);
							}
							else
							{
								return S_OK;
							}

							ICoreWebView2Settings* settings;
							webview->get_Settings(&settings);
							settings->put_IsScriptEnabled(TRUE);
							settings->put_AreDefaultScriptDialogsEnabled(TRUE);
							settings->put_IsWebMessageEnabled(TRUE);

							RECT bounds;
							GetClientRect(hWnd, &bounds);

							webviewController->put_Bounds(bounds);

							webview->Navigate(loginUrl.data());

#ifdef _DEBUG
							webview->OpenDevToolsWindow();
#endif

							EventRegistrationToken token;
							webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
								[hWnd](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
								{
									LPWSTR uri;
									args->get_Uri(&uri);
									std::wstring source(uri);

									if (source == L"dmmgameplayer://showLibrary")
									{
										ICoreWebView2_2* webView2;
										webview->QueryInterface<ICoreWebView2_2>(&webView2);

										ICoreWebView2CookieManager* cookieManager;
										webView2->get_CookieManager(&cookieManager);

										cookieManager->GetCookies(L"https://accounts.dmm.com", Callback<ICoreWebView2GetCookiesCompletedHandler>(
											[hWnd](
												HRESULT result,
												ICoreWebView2CookieList* cookieList)
											{
												UINT count;
												cookieList->get_Count(&count);


												wstring sessionId;
												wstring secureId;


												for (int i = 0; i < count; i++)
												{
													ICoreWebView2Cookie* cookie;
													cookieList->GetValueAtIndex(i, &cookie);

													LPWSTR name;
													cookie->get_Name(&name);

													LPWSTR value;

													if (name == L"login_session_id"s)
													{
														cookie->get_Value(&value);
														sessionId = value;
													}

													if (name == L"login_secure_id"s)
													{
														cookie->get_Value(&value);
														secureId = value;
													}

													if (!sessionId.empty() && !secureId.empty())
													{
														break;
													}
												}

												auto gameArgs = GetGameArgs(sessionId, secureId);

												if (!gameArgs.empty())
												{
													stringstream gameArgsStream(gameArgs);
													string segment;
													vector<string> split;
													while (getline(gameArgsStream, segment, ' '))
													{
														split.emplace_back(segment);
													}

													stringstream singleArgStream1(split[0]);
													vector<string> split1;
													while (getline(singleArgStream1, segment, '='))
													{
														split1.emplace_back(segment);
													}

													viewerId = string(split1.back());

													split1.clear();

													stringstream singleArgStream2(split[1]);
													while (getline(singleArgStream2, segment, '='))
													{
														split1.emplace_back(segment);
													}

													onetimeToken = string(split1.back());

													il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmViewerId", 1)(il2cpp_string_new(viewerId.data()));
													il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmOnetimeToken", 1)(il2cpp_string_new(onetimeToken.data()));
												}

												webviewController->Close();
												PostMessageW(hWnd, WM_CLOSE, NULL, NULL);

												isLoginWebViewOpen = false;

												return S_OK;
											}).Get());
									}

									if (source.substr(0, 5) != L"https")
									{
										args->put_Cancel(true);
									}
									return S_OK;
								}).Get(), &token);

							return S_OK;
						}
					).Get());
					return S_OK;
				}
			).Get());


		MSG msg;
		while (GetMessageW(&msg, nullptr, 0, 0) && isLoginWebViewOpen)
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return msg.wParam;
	}

	void* TitleViewController_OnClickPushStart_orig = nullptr;

	void TitleViewController_OnClickPushStart_hook(Il2CppObject* _this)
	{
		const auto dmmId = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("umamusume.dll", "Gallop", "Certification", "get_dmmViewerId", IgnoreNumberOfArguments)();
		const auto dmmOnetimeToken = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("umamusume.dll", "Gallop", "Certification", "get_dmmOnetimeToken", IgnoreNumberOfArguments)();

		if (dmmId && !wstring(dmmId->chars).empty() &&
			dmmOnetimeToken && !wstring(dmmOnetimeToken->chars).empty())
		{
			reinterpret_cast<decltype(TitleViewController_OnClickPushStart_hook)*>(TitleViewController_OnClickPushStart_orig)(_this);
		}
		else
		{
			const auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(AudioManager->klass, "PlaySe_UIDecide", 0)->methodPointer(AudioManager);

			if (!isLoginWebViewOpen)
			{
				isLoginWebViewOpen = true;

				CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(WebViewThread), NULL, NULL, NULL);
			}

		}
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

	void* UploadHandlerRaw_Create_hook(Il2CppObject* self, Il2CppArraySize_t<int8_t>* data)
	{
		auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

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

#ifdef _DEBUG
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

		return reinterpret_cast<decltype(UploadHandlerRaw_Create_hook)*>(UploadHandlerRaw_Create_orig)(self, data);
	}

	void* DownloadHandler_InternalGetByteArray_orig = nullptr;

	Il2CppArraySize_t<int8_t>* DownloadHandler_InternalGetByteArray_hook(Il2CppObject* self)
	{
		auto data = reinterpret_cast<decltype(DownloadHandler_InternalGetByteArray_hook)*>(DownloadHandler_InternalGetByteArray_orig)(self);

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

#ifdef _DEBUG
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

		return data;
	}

	void* HttpHelper_CompressRequest_orig = nullptr;

	Il2CppArraySize_t<int8_t>* HttpHelper_CompressRequest_hook(Il2CppArraySize_t<int8_t>* data)
	{
		auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

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

#ifdef _DEBUG
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

		return reinterpret_cast<decltype(HttpHelper_CompressRequest_hook)*>(HttpHelper_CompressRequest_orig)(data);
	}

	void* HttpHelper_DecompressResponse_orig = nullptr;

	Il2CppArraySize_t<int8_t>* HttpHelper_DecompressResponse_hook(Il2CppArraySize_t<int8_t>* compressed)
	{
		auto data = reinterpret_cast<decltype(HttpHelper_DecompressResponse_hook)*>(HttpHelper_DecompressResponse_orig)(compressed);

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

#ifdef _DEBUG
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

		auto _horseManagerField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_horseManager");
		Il2CppObject* _horseManager;
		il2cpp_field_get_value(instance, _horseManagerField, &_horseManager);

		return _horseManager;
	}

	wstring GetRaceState()
	{
		auto instance = GetRaceManager();
		if (!instance)
		{
			return L"";
		}

		auto state = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(instance->klass, "get_State", 0)->methodPointer(instance);
		auto stateName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "RaceDefine/RaceState"), state);
		return stateName->chars;
	}

	wstring GetRaceCameraMode()
	{
		auto instance = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "RaceCameraManager"));
		if (!instance)
		{
			return L"";
		}

		auto mode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(instance->klass, "GetCameraMode", 0)->methodPointer(instance);
		auto modeName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "RaceCameraManager/CameraMode"), mode);
		return modeName->chars;
	}

	bool IsRace() {
		auto stateName = GetRaceState();
		if (!stateName.empty())
		{
			return L"Race" == stateName;
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
						name = wide_u8(text_get_text(text)->chars);
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
				auto uiManager = Gallop::UIManager::Instance();
				auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
				if (uiManager && sceneManager)
				{
					string detail;

					auto CommonHeaderTitle = uiManager.CommonHeaderTitle();

					if (CommonHeaderTitle)
					{
						auto _cacheTextField = il2cpp_class_get_field_from_name_wrap(CommonHeaderTitle->klass, "_cacheText");
						Il2CppString* _cacheText;
						il2cpp_field_get_value(CommonHeaderTitle, _cacheTextField, &_cacheText);

						if (_cacheText)
						{
							detail = wide_u8(_cacheText->chars);
						}
					}

					if (detail.empty())
					{
						auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);
						auto viewName = wide_u8(GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "SceneDefine/ViewId"), viewId)->chars);
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

			if (discord)
			{
				discord->RunCallbacks();
			}
		}
	}

	void TickFrame()
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			if (config::cyspring_update_mode != -1)
			{
				auto threadClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "CySpringController/CySpringThread");

				auto instanceField = il2cpp_class_get_field_from_name_wrap(threadClass, "_instance");

				if (instanceField)
				{
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
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(item->klass, "set_UpdateMode", 1)->methodPointer(item, config::cyspring_update_mode);
							}
						}
					}
				}
			}
		}

		if (Unity::OpenXR::started)
		{
			Unity::OpenXR::Internal_PumpMessageLoop();
		}

		if (config::unlock_size || config::freeform_window)
		{
			SetBGCanvasScalerSize();
		}

		if (config::freeform_window)
		{
			ResizeMoviePlayer();
		}

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			UpdateVoiceButton();
		}

		if (config::discord_rich_presence)
		{
			TickDiscord();
		}

		auto active = il2cpp_symbols::get_method_pointer<UnityEngine::SceneManagement::Scene(*)()>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager", "GetActiveScene", IgnoreNumberOfArguments)();

		auto handleName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "Scene", "GetNameInternal", 1)(active.handle);

		if (handleName)
		{
			wstring sceneName = handleName->chars;

			if (sceneName == L"Live")
			{
				auto controller = GetCurrentViewController();

				if (controller && controller->klass->name == "LiveViewController"s)
				{
					auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
					if (director)
					{
						auto LiveCurrentTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveCurrentTime", 0)->methodPointer(director);
						auto LiveTotalTime = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(director->klass, "get_LiveTotalTime", 0)->methodPointer(director);

						auto sliderCommon = GetOptionSlider("live_slider");

						auto textCommon = GetTextCommon("live_slider");

						if (textCommon)
						{
							auto timeMin = static_cast<int>(LiveCurrentTime / 60);
							auto timeSec = static_cast<int>(fmodf(LiveCurrentTime, 60));

							wostringstream str;
							str << setw(2) << setfill(L'0') << timeSec;

							SetTextCommonText(textCommon, (to_wstring(timeMin) + L":" + str.str()).data());
						}

						auto textCommonTotal = GetTextCommon("live_slider_total");

						if (textCommonTotal)
						{
							auto timeMin = static_cast<int>(LiveTotalTime / 60);
							auto timeSec = static_cast<int>(fmodf(LiveTotalTime, 60));

							wostringstream str;
							str << setw(2) << setfill(L'0') << timeSec;

							SetTextCommonText(textCommonTotal, (to_wstring(timeMin) + L":" + str.str()).data());
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
				if (sceneName == L"Home")
				{
					bool hasSetList = false;

					auto hubViewController = GetCurrentHubViewChildController();

					if (hubViewController && hubViewController->klass->name == "HomeViewController"s)
					{
						auto topUi = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(hubViewController->klass, "GetTopUI", 1)->methodPointer(hubViewController, 10);
						if (topUi)
						{
							auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(topUi->klass, "get_TempSetListPlayingData", 0)->methodPointer(topUi);

							auto IsPlayingField = il2cpp_class_get_field_from_name_wrap(data->klass, "IsPlaying");
							bool IsPlaying;
							il2cpp_field_get_value(data, IsPlayingField, &IsPlaying);

							hasSetList = IsPlaying;

							if (IsPlaying)
							{
								SystemMediaTransportControlsManager::instance.PlaybackStatus(winrt::Windows::Media::MediaPlaybackStatus::Playing);

								auto SetListIndexField = il2cpp_class_get_field_from_name_wrap(data->klass, "SetListIndex");
								int SetListIndex;
								il2cpp_field_get_value(data, SetListIndexField, &SetListIndex);

								int MusicListCount = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(data->klass, "GetMusicListCount", 0)->methodPointer(data);

								SystemMediaTransportControlsManager::instance.IsPreviousEnabled(SetListIndex > 0);
								SystemMediaTransportControlsManager::instance.IsNextEnabled(SetListIndex < MusicListCount - 1);

								auto musicData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(data->klass, "GetMasterSetListMusicData", 0)->methodPointer(data);

								auto MusicIdField = il2cpp_class_get_field_from_name_wrap(musicData->klass, "MusicId");
								int MusicId;
								il2cpp_field_get_value(musicData, MusicIdField, &MusicId);

								SystemMediaTransportControlsManager::UpdateMetadata(MusicId);
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

				if (sceneName == L"Live")
				{
					auto controller = GetCurrentViewController();

					if (controller && controller->klass->name == "LiveViewController"s)
					{
						auto _stateField = il2cpp_class_get_field_from_name_wrap(controller->klass, "_state");
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

	void StartTickFrame()
	{
		static auto tickFrameDelegate = CreateDelegateStatic(TickFrame);

		try
		{
			auto uiManager = Gallop::UIManager::Instance();
			if (uiManager)
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrame", 2)(uiManager, tickFrameDelegate);
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
			if (lParam != SW_RESTORE) {
				if (UnityEngine::Screen::fullScreen()) {
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
		wprintf(L"%s\n", sceneName->chars);
		return reinterpret_cast<decltype(load_scene_internal_hook)*>(load_scene_internal_orig)(sceneName, sceneBuildIndex, parameters, mustCompleteNextFrame);
	}

	void dump_all_entries()
	{
		vector<wstring> static_entries;
		vector<pair<const wstring, const wstring>> text_id_static_entries;
		vector<pair<const wstring, const wstring>> text_id_not_matched_entries;
		// 0 is None
		for (int i = 1;; i++)
		{
			auto* str = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(i);

			if (str && *str->chars)
			{
				if (config::static_entries_use_text_id_name)
				{
					wstring textIdName = GetTextIdNameById(i);
					text_id_static_entries.emplace_back(textIdName, str->chars);
					if (local::get_localized_string(textIdName) == nullptr ||
						local::get_localized_string(textIdName)->chars == str->chars)
					{
						text_id_not_matched_entries.emplace_back(textIdName, str->chars);
					}
				}
				else if (config::static_entries_use_hash)
				{
					static_entries.emplace_back(str->chars);
				}
				else
				{
					logger::write_entry(i, str->chars);
				}
			}
			else
			{
				// check next string, if it's still empty, then we are done!
				auto* nextStr = reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(i + 1);
				if (!(nextStr && *nextStr->chars))
					break;
			}
		}
		if (config::static_entries_use_text_id_name)
		{
			logger::write_text_id_static_dict(text_id_static_entries, text_id_not_matched_entries);
		}
		else if (config::static_entries_use_hash)
		{
			logger::write_static_dict(static_entries);
		}
	}

	void patch_game_assembly()
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

		if (config::dump_il2cpp)
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
		load_asset = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject * _this, Il2CppString * name, Il2CppObject * runtimeType)>(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine",
			"AssetBundle", "LoadAsset", 2);

		get_all_asset_names = il2cpp_symbols::get_method_pointer<Il2CppArraySize_t<Il2CppString*> *(*)(Il2CppObject * _this)>(
			"UnityEngine.AssetBundleModule.dll", "UnityEngine",
			"AssetBundle", "GetAllAssetNames", 0);

		uobject_IsNativeObjectAlive = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject * uObject)>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Object", "IsNativeObjectAlive", 1);

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

		auto Screen_SetResolution_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "SetResolution", 4
		);

		auto Screen_IsCurrentOrientation_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "Screen", "IsCurrentOrientation", 1
		);

		auto GallopInput_mousePosition_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll", "Gallop", "GallopInput", "mousePosition", IgnoreNumberOfArguments
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

		auto story_timeline_controller_play_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryTimelineController", "Play", 0);

		auto story_race_textasset_load_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryRaceTextAsset", "Load", 0);

		auto get_modified_string_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GetModifiedString", IgnoreNumberOfArguments);

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

		auto rendertexture_set_anti_aliasing_addr = il2cpp_resolve_icall("UnityEngine.RenderTexture::set_antiAliasing(System.Int32)");

		auto set_vSyncCount_addr = il2cpp_resolve_icall("UnityEngine.QualitySettings::set_vSyncCount()");

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
			"Gallop", "NowLoading", "Show", 8);

		auto NowLoading_Hide_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "NowLoading", "Hide", 4);

		auto BootSystem_Awake_addr = il2cpp_symbols::get_method_pointer(
			"umamusume.dll",
			"Gallop", "BootSystem", "Awake", 0);

		MoviePlayerBase_get_MovieInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerBase", "get_MovieInfo", 0);

		MovieManager_GetMovieInfo = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Cute::Cri::MoviePlayerHandle)>(
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

		Material_HasProperty = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Material", "HasProperty", 1);

		auto FrameRateController_OverrideByNormalFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByNormalFrameRate", 1);

		auto FrameRateController_OverrideByMaxFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "OverrideByMaxFrameRate", 1);

		auto FrameRateController_ResetOverride_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ResetOverride", 1);

		auto FrameRateController_ReflectionFrameRate_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "FrameRateController", "ReflectionFrameRate", 0);

		auto BGManager_CalcBgScale_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "BGManager", "CalcBgScale", 4);

		auto GallopUtil_GotoTitleOnError_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "GallopUtil", "GotoTitleOnError", 1);

		auto DialogCommon_Close_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCommon", "Close", 0);

		auto StoryViewController_ctor_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "StoryViewController", ".ctor", 0);

		auto DialogCircleItemDonate_Initialize_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "DialogCircleItemDonate", "Initialize", 2);

		auto UIManager_WaitBootSetup_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "UIManager", "WaitBootSetup", 0);

		auto Object_Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");

		auto Object_Internal_CloneSingle_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingle()");

		auto Input_get_mousePosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.Input::get_mousePosition_Injected(UnityEngine.Vector3&)");

		auto TitleViewController_OnClickPushStart_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "OnClickPushStart", 0);

		auto Certification_initDmmPlatformData_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "Certification", "initDmmPlatformData", IgnoreNumberOfArguments);

		auto UploadHandlerRaw_Create_addr = il2cpp_resolve_icall("UnityEngine.Networking.UploadHandlerRaw::Create()");

		auto DownloadHandler_InternalGetByteArray_addr = il2cpp_resolve_icall("UnityEngine.Networking.DownloadHandler::InternalGetByteArray()");

		auto HttpHelper_CompressRequest_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "HttpHelper", "CompressRequest", 1);

		auto HttpHelper_DecompressResponse_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "HttpHelper", "DecompressResponse", 1);

		auto load_scene_internal_addr = il2cpp_resolve_icall("UnityEngine.SceneManagement.SceneManager::LoadSceneAsyncNameIndexInternal_Injected(System.String,System.Int32,UnityEngine.SceneManagement.LoadSceneParameters&,System.bool)");

#pragma endregion

		ADD_HOOK(Certification_initDmmPlatformData, "Gallop.Certification::initDmmPlatformData at %p\n");

		ADD_HOOK(TitleViewController_OnClickPushStart, "Gallop.TitleViewController::OnClickPushStart at %p\n");

		ADD_HOOK(Object_Internal_CloneSingleWithParent, "UnityEngine.Object::Internal_CloneSingleWithParent at %p\n");

		ADD_HOOK(Object_Internal_CloneSingle, "UnityEngine.Object::Internal_CloneSingle at %p\n");

		// ADD_HOOK(UIManager_WaitBootSetup, "Gallop.UIManager::WaitBootSetup at %p\n");

		ADD_HOOK(PartsEpisodeList_SetupStoryExtraEpisodeList, "Gallop.PartsEpisodeList::SetupStoryExtraEpisodeList at %p\n");

		ADD_HOOK(DialogCircleItemDonate_Initialize, "Gallop.DialogCircleItemDonate::Initialize at %p\n");

		// ADD_HOOK(DialogCommon_Close, "Gallop.DialogCommon.Close() at %p\n");

		// ADD_HOOK(GallopUtil_GotoTitleOnError, "Gallop.GallopUtil.GotoTitleOnError() at %p\n");

		ADD_HOOK(set_shadowResolution, "UnityEngine.QualitySettings.set_shadowResolution(ShadowResolution) at %p\n");

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

		ADD_HOOK(set_shadows, "UnityEngine.QualitySettings.set_shadows(UnityEngine.ShadowQuality) at %p\n");

		ADD_HOOK(set_softVegetation, "UnityEngine.QualitySettings.set_softVegetation(System.Boolean) at %p\n");

		ADD_HOOK(set_realtimeReflectionProbes, "UnityEngine.QualitySettings.set_realtimeReflectionProbes(System.Boolean) at %p\n");

		ADD_HOOK(Light_set_shadowResolution, "UnityEngine.Light.set_shadowResolution(UnityEngine.Rendering.LightShadowResolution) at %p\n");

		// TODO Gallop.NowLoading::ShowCustomWipeFlash
		ADD_HOOK(NowLoading_Show, "Gallop.NowLoading::Show at %p\n");

		ADD_HOOK(NowLoading_Hide, "Gallop.NowLoading::Hide at %p\n");

		// ADD_HOOK(PathResolver_GetLocalPath, "Cyan.Loader.PathResolver::GetLocalPath at %p\n");

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

		if (!config::replace_text_db_path.empty())
		{
			try
			{
				MasterDB::InitReplacementMasterDB(wide_u8(config::replace_text_db_path.data()));
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

		if (config::cyspring_update_mode != -1)
		{
			ADD_HOOK(CySpringUpdater_set_SpringUpdateMode, "CySpringUpdater::set_SpringUpdateMode at %p\n");
			ADD_HOOK(CySpringUpdater_get_SpringUpdateMode, "CySpringUpdater::get_SpringUpdateMode at %p\n");
		}

		// ADD_HOOK(load_scene_internal, "SceneManager::LoadSceneAsyncNameIndexInternal at %p\n");

		// ADD_HOOK(BootSystem_Awake, "Gallop.BootSystem::Awake at %p\n");

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

		if (config::unlock_size || config::freeform_window)
		{
			// ADD_HOOK(canvas_scaler_setres, "UnityEngine.UI.CanvasScaler::set_referenceResolution at %p\n");
			// ADD_HOOK(UIManager_UpdateCanvasScaler, "Gallop.UIManager::UpdateCanvasScaler at %p\n");
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
			ADD_HOOK(FrameRateController_OverrideByNormalFrameRate, "Gallop.FrameRateController::OverrideByNormalFrameRate at %p\n");
			ADD_HOOK(FrameRateController_OverrideByMaxFrameRate, "Gallop.FrameRateController::OverrideByMaxFrameRate at %p\n");
			ADD_HOOK(FrameRateController_ResetOverride, "Gallop.FrameRateController::ResetOverride at %p\n");
			ADD_HOOK(FrameRateController_ReflectionFrameRate, "Gallop.FrameRateController::ReflectionFrameRate at %p\n");
			// ADD_HOOK(set_fps, "UnityEngine.Application.set_targetFrameRate at %p\n");
		}

		if (config::unlock_size || config::freeform_window)
		{
			ADD_HOOK(GallopFrameBuffer_Initialize, "Gallop.GallopFrameBuffer::Initialize at %p\n");
			ADD_HOOK(GallopFrameBuffer_Release, "Gallop.GallopFrameBuffer::Release at %p\n");
			ADD_HOOK(GallopFrameBuffer_ResizeRenderTexture, "Gallop.GallopFrameBuffer::ResizeRenderTexture at %p\n");
		}

		if (config::freeform_window)
		{
			ADD_HOOK(StandaloneWindowResize_DisableMaximizebox, "Gallop.StandaloneWindowResize::DisableMaximizebox at %p\n");
			ADD_HOOK(StandaloneWindowResize_ReshapeAspectRatio, "Gallop.StandaloneWindowResize::ReshapeAspectRatio at %p\n");
			ADD_HOOK(StandaloneWindowResize_KeepAspectRatio, "Gallop.StandaloneWindowResize::KeepAspectRatio at %p\n");
			ADD_HOOK(Screen_SetResolution, "Gallop.Screen::SetResolution at %p\n");
			ADD_HOOK(Screen_IsCurrentOrientation, "Gallop.Screen::IsCurrentOrientation at %p\n");
			ADD_HOOK(GallopInput_mousePosition, "Gallop.GallopInput::mousePosition at %p\n");

			auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
			il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			Gallop::StandaloneWindowResize::IsVirt(width <= height);

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

			if (SCREEN_ORIENTATION_CATEGORIES)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);
			}

			auto Camera_set_orthographicSize_addr = il2cpp_resolve_icall("UnityEngine.Camera::set_orthographicSize(System.Single)");

			auto RectTransform_get_rect_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_rect_Injected(UnityEngine.Rect&)");

			auto WaitDeviceOrientation_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"Gallop", "Screen", "WaitDeviceOrientation", 1);

			auto Screen_RequestOrientation_addr = il2cpp_resolve_icall("UnityEngine.Screen::RequestOrientation(UnityEngine.ScreenOrientation)");

			auto DeviceOrientationGuide_Show_addr = il2cpp_symbols::get_method_pointer(
				"umamusume.dll",
				"Gallop", "DeviceOrientationGuide", "Show", 2);

			auto MoviePlayerForUI_AdjustScreenSize_addr = il2cpp_symbols::get_method_pointer(
				"Cute.Cri.Assembly.dll", "Cute.Cri", "MoviePlayerForUI", "AdjustScreenSize", 2);

			ADD_HOOK(Camera_set_orthographicSize, "UnityEngine.Camera::set_orthographicSize at %p\n");
			ADD_HOOK(RectTransform_get_rect_Injected, "UnityEngine.RectTransform::get_rect_Injected at %p\n");

			// ADD_HOOK(WaitDeviceOrientation, "Gallop.Screen::WaitDeviceOrientation at %p\n");
			ADD_HOOK(Screen_RequestOrientation, "UnityEngine.Screen::RequestOrientation at %p\n");
			ADD_HOOK(DeviceOrientationGuide_Show, "DeviceOrientationGuide::Show at %p\n");
			ADD_HOOK(MoviePlayerForUI_AdjustScreenSize, "MoviePlayerForUI::AdjustScreenSize at %p\n");
		}

		if (config::graphics_quality != -1)
		{
			ADD_HOOK(apply_graphics_quality, "Gallop.GraphicSettings.ApplyGraphicsQuality at %p\n");
		}

		if (config::freeform_window || config::unlock_size || config::resolution_3d_scale != 1.0f)
		{
			// ADD_HOOK(BGManager_CalcBgScale, "Gallop.BGManager::CalcBgScale at %p\n");
		}

		if (config::resolution_3d_scale != 1.0f || config::freeform_window)
		{
			// ADD_HOOK(GraphicSettings_GetVirtualResolution3D, "Gallop.GraphicSettings.GetVirtualResolution3D at %p\n");
			// ADD_HOOK(GraphicSettings_GetVirtualResolution, "Gallop.GraphicSettings.GetVirtualResolution at %p\n");
		}

		if (config::anti_aliasing != -1)
		{
			ADD_HOOK(set_anti_aliasing, "UnityEngine.QualitySettings.set_antiAliasing(int) at %p\n");
			ADD_HOOK(rendertexture_set_anti_aliasing, "UnityEngine.RenderTexture.set_antiAliasing(int) at %p\n");
		}

		if (!config::external_dlls_path.empty())
		{
			for (int i = 0; i < config::external_dlls_path.size(); i++)
			{
				wcout << L"Loading " << config::external_dlls_path[i] << L": ";
				auto dll = LoadLibraryW(config::external_dlls_path[i].data());
				wcout << dll << endl;
			}
		}

		const auto nameArray = reinterpret_cast<Il2CppArraySize_t<Il2CppString*>*(*)()>(il2cpp_resolve_icall("UnityEngine.QualitySettings::get_names()"))();
		reinterpret_cast<void(*)(int)>(il2cpp_resolve_icall("UnityEngine.QualitySettings::SetQualityLevel()"))(nameArray->max_length - 1);
	}

	void InitNotification()
	{
		if (!isRequiredInitNotification)
		{
			return;
		}

		/*if (notification)
		{
			if (uobject_IsNativeObjectAlive(notification))
			{
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "Destroy", 1)(notification);
			}
			notification = nullptr;
		}*/

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _noticeCanvas = uiManager._noticeCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_noticeCanvas->klass, "get_transform", 0)->methodPointer(_noticeCanvas);

		auto object = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppString*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Resources", "Load", 2)(
			il2cpp_string_new("UI/Parts/Notification"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		auto instantiated = UnityEngine::Object::Internal_CloneSingleWithParent(object, transform, false);
		auto helper = new CastHelper{};
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*, uintptr_t*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentFastPath", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "Notification"), &helper->oneFurtherThanResultValue);
		notification = helper->obj;
		delete helper;

		auto canvasGroupField = il2cpp_class_get_field_from_name_wrap(notification->klass, "canvasGroup");
		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);

		auto _LabelField = il2cpp_class_get_field_from_name_wrap(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_fontSize", 1)->methodPointer(_Label, config::character_system_text_caption_font_size);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_FontColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), config::character_system_text_caption_font_color.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineSize", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), config::character_system_text_caption_outline_size.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), config::character_system_text_caption_outline_color.data())));

		auto background = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentInChildren", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(background->klass, "SetAlpha", 1)->methodPointer(background, config::character_system_text_caption_background_alpha);

		auto canvasGroupTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasGroup->klass, "get_transform", 0)->methodPointer(canvasGroup);

		auto position = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(canvasGroupTransform->klass, "get_position", 0)->methodPointer(canvasGroupTransform);

		position.x = config::character_system_text_caption_position_x;
		position.y = config::character_system_text_caption_position_y;

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(canvasGroupTransform->klass, "set_position", 1)->methodPointer(canvasGroupTransform, position);

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
		auto amuid = wstring(UnityEngine::Application::companyName()->chars) + L".Gallop";

		DesktopNotificationManagerCompat::RegisterAumidAndComServer(amuid.data(), localize_get_hook(GetTextIdByName(L"Outgame0028"))->chars);

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

		auto Screen_set_orientation_addr = il2cpp_symbols::get_method_pointer(
			"UnityEngine.CoreModule.dll",
			"UnityEngine",
			"Screen", "set_orientation", 1);

		auto set_resolution_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution(System.Int32,System.Int32,UnityEngine.FullScreenMode,System.Int32)");

		auto MovieManager_SetImageUvRect_addr = il2cpp_symbols::get_method_pointer(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "SetImageUvRect", 2);

		auto MovieManager_SetScreenSize_addr = il2cpp_symbols::get_method_pointer(
			"Cute.Cri.Assembly.dll", "Cute.Cri", "MovieManager", "SetScreenSize", 2);

		auto assetbundle_LoadFromFile_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadFromFile_Internal(System.String,System.UInt32,System.UInt64)");

		auto AssetBundleRequest_GetResult_addr = il2cpp_resolve_icall("UnityEngine.AssetBundleRequest::GetResult()");

		auto assetbundle_load_asset_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)");

		auto assetbundle_load_asset_async_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAssetAsync_Internal(System.String,System.Type)");

		auto assetbundle_unload_addr = il2cpp_symbols::get_method_pointer("UnityEngine.AssetBundleModule.dll", "UnityEngine", "AssetBundle", "Unload", 1);

		auto resources_load_addr = il2cpp_resolve_icall("UnityEngine.ResourcesAPIInternal::Load()");

		auto Sprite_get_texture_addr = il2cpp_resolve_icall("UnityEngine.Sprite::get_texture(UnityEngine.Sprite)");

		auto Renderer_get_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_material", 0);

		auto Renderer_get_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_materials", 0);

		auto Renderer_get_sharedMaterial_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterial", 0);

		auto Renderer_get_sharedMaterials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "get_sharedMaterials", 0);

		auto Renderer_set_material_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_material", 1);

		auto Renderer_set_materials_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer", "set_materials", 1);

		auto Material_get_mainTexture_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Material", "get_mainTexture", 0);

		auto Material_set_mainTexture_addr = il2cpp_symbols::get_method_pointer("UnityEngine.CoreModule.dll", "UnityEngine", "Material", "set_mainTexture", 1);

		auto Material_GetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::GetTextureImpl(System.String,System.Int32)");

		auto Material_SetTextureImpl_addr = il2cpp_resolve_icall("UnityEngine.Material::SetTextureImpl(System.String,System.Int32,UnityEngine.Texture)");

		auto CharaPropRendererAccessor_SetTexture_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "CharaPropRendererAccessor", "SetTexture", 1);

		auto GameObject_GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent(System.Type)");

		auto GameObject_GetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentFastPath(System.Type,System.IntPtr)");

#pragma region LOAD_ASSETBUNDLE
		if (!fontAssets && !config::font_assetbundle_path.empty() && config::replace_to_custom_font)
		{
			wstring assetbundlePath = config::font_assetbundle_path;
			if (PathIsRelativeW(assetbundlePath.data()))
			{
				assetbundlePath.insert(0, filesystem::current_path().wstring().append(L"/"));
			}

			wcout << L"Loading font asset: " << assetbundlePath << L"... ";

			DesktopNotificationManagerCompat::ShowToastNotification(L"Loading font AssetBundle", assetbundlePath.data(), nullptr, true);

			fontAssets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

			if (!fontAssets && filesystem::exists(assetbundlePath))
			{
				wcout << L"Asset founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
			}

			wcout << L"OK: " << fontAssets << endl;
		}

		if (!config::replace_assetbundle_file_path.empty())
		{
			wstring assetbundlePath = config::replace_assetbundle_file_path;
			if (PathIsRelativeW(assetbundlePath.data()))
			{
				assetbundlePath.insert(0, filesystem::current_path().wstring().append(L"/"));
			}

			wcout << L"Loading replacement AssetBundle: " << assetbundlePath << L"... ";

			DesktopNotificationManagerCompat::ShowToastNotification(L"Loading replacement AssetBundle", assetbundlePath.data(), nullptr, true);

			auto assets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

			if (!assets && filesystem::exists(assetbundlePath))
			{
				wcout << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
			}
			else
			{
				wcout << L"OK: " << assets << endl;
				replaceAssets.emplace_back(assets);
			}
		}

		if (!config::replace_assetbundle_file_paths.empty())
		{
			for (auto it = config::replace_assetbundle_file_paths.begin(); it != config::replace_assetbundle_file_paths.end(); it++)
			{
				wstring assetbundlePath = *it;
				if (PathIsRelativeW(assetbundlePath.data()))
				{
					assetbundlePath.insert(0, filesystem::current_path().wstring().append(L"/"));
				}

				wcout << L"Loading replacement AssetBundle: " << assetbundlePath << L"... ";

				DesktopNotificationManagerCompat::ShowToastNotification(L"Loading replacement AssetBundle", assetbundlePath.data(), nullptr, true);

				auto assets = load_from_file(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

				if (!assets && filesystem::exists(assetbundlePath))
				{
					wcout << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
				}
				else if (assets)
				{
					wcout << L"OK: " << assets << endl;
					replaceAssets.emplace_back(assets);
				}
			}
		}

		history->Clear();

		if (!replaceAssets.empty())
		{
			for (auto it = replaceAssets.begin(); it != replaceAssets.end(); it++)
			{
				auto names = get_all_asset_names(*it);
				for (int i = 0; i < names->max_length; i++)
				{
					replaceAssetNames.emplace_back(names->vector[i]->chars);
				}
			}
		}
#pragma endregion

		ADD_HOOK(assetbundle_LoadFromFile, "UnityEngine.AssetBundle::LoadFromFile at %p\n");

		ADD_HOOK(assetbundle_unload, "UnityEngine.AssetBundle::Unload at %p\n");

		ADD_HOOK(resources_load, "UnityEngine.Resources::Load at %p\n");

		if (!replaceAssets.empty())
		{
			ADD_HOOK(AssetBundleRequest_GetResult, "UnityEngine.AssetBundleRequest::GetResult at %p\n");

			ADD_HOOK(assetbundle_load_asset, "UnityEngine.AssetBundle::LoadAsset at %p\n");

			ADD_HOOK(assetbundle_load_asset_async, "UnityEngine.AssetBundle::LoadAssetAsync at %p\n");

			ADD_HOOK(GameObject_GetComponent, "UnityEngine.GameObject::GetComponent at %p\n");

			// ADD_HOOK(GameObject_GetComponentFastPath, "UnityEngine.GameObject::GetComponentFastPath at %p\n");

			ADD_HOOK(Sprite_get_texture, "UnityEngine.Sprite::get_texture at %p\n");

			ADD_HOOK(Renderer_get_material, "UnityEngine.Renderer::get_material at %p\n");

			ADD_HOOK(Renderer_get_materials, "UnityEngine.Renderer::get_materials at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterial, "UnityEngine.Renderer::get_sharedMaterial at %p\n");

			ADD_HOOK(Renderer_get_sharedMaterials, "UnityEngine.Renderer::get_sharedMaterials at %p\n");

			ADD_HOOK(Renderer_set_material, "UnityEngine.Renderer::set_material at %p\n");

			ADD_HOOK(Renderer_set_materials, "UnityEngine.Renderer::set_materials at %p\n");

			ADD_HOOK(Material_get_mainTexture, "UnityEngine.Material::get_mainTexture at %p\n");

			ADD_HOOK(Material_set_mainTexture, "UnityEngine.Material::set_mainTexture at %p\n");

			ADD_HOOK(Material_GetTextureImpl, "UnityEngine.Material::GetTextureImpl at %p\n");

			ADD_HOOK(Material_SetTextureImpl, "UnityEngine.Material::SetTextureImpl at %p\n");

			ADD_HOOK(CharaPropRendererAccessor_SetTexture, "Gallop.CharaPropRendererAccessor::SetTexture at %p\n");
		}

		if (config::dump_entries)
		{
			dump_all_entries();
		}

		if (config::freeform_window)
		{
			int width = UnityEngine::Screen::width();
			int height = UnityEngine::Screen::height();

			auto GallopScreen = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "Screen");

			auto SCREEN_ORIENTATION_CATEGORIES_Field = il2cpp_class_get_field_from_name_wrap(GallopScreen, "SCREEN_ORIENTATION_CATEGORIES");
			Il2CppObject* SCREEN_ORIENTATION_CATEGORIES;
			il2cpp_field_static_get_value(SCREEN_ORIENTATION_CATEGORIES_Field, &SCREEN_ORIENTATION_CATEGORIES);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Clear", 0)->methodPointer(SCREEN_ORIENTATION_CATEGORIES);

			if (width < height)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::Portrait, UnityEngine::ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::PortraitUpsideDown, UnityEngine::ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::LandscapeLeft, UnityEngine::ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::LandscapeRight, UnityEngine::ScreenOrientation::Portrait);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::AutoRotation, UnityEngine::ScreenOrientation::Portrait);
			}
			else
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::Portrait, UnityEngine::ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::PortraitUpsideDown, UnityEngine::ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::LandscapeLeft, UnityEngine::ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::LandscapeRight, UnityEngine::ScreenOrientation::LandscapeLeft);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation, UnityEngine::ScreenOrientation)>(SCREEN_ORIENTATION_CATEGORIES->klass, "Add", 2)->methodPointer(SCREEN_ORIENTATION_CATEGORIES, UnityEngine::ScreenOrientation::AutoRotation, UnityEngine::ScreenOrientation::LandscapeLeft);
			}
		}

		if (config::auto_fullscreen || config::unlock_size || config::freeform_window)
		{
			if (!set_resolution_orig)
			{
				ADD_HOOK(set_resolution, "UnityEngine.Screen.SetResolution(int, int, FullScreenMode, int) at %p\n");
			}
			// ADD_HOOK(UIManager_ChangeResizeUIForPC, "Gallop.UIManager::ChangeResizeUIForPC at %p\n");
		}

		if (config::unlock_size || config::freeform_window)
		{
			// remove fixed 1080p render resolution
			ADD_HOOK(gallop_get_screenheight, "Gallop.Screen::get_Height at %p\n");
			ADD_HOOK(gallop_get_screenwidth, "Gallop.Screen::get_Width at %p\n");

			auto display = display_get_main();
			if (config::initial_width > 72 && config::initial_height > 72)
			{
				auto StandaloneWindowResize = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StandaloneWindowResize");
				il2cpp_class_get_method_from_name_type<void (*)(bool)>(StandaloneWindowResize, "set_IsPreventReShape", 1)->methodPointer(true);

				auto ratio = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);

				if (config::initial_width < config::initial_height)
				{
					config::runtime::ratioVertical = static_cast<float>(config::initial_width) / static_cast<float>(config::initial_height);
					config::runtime::ratioHorizontal = static_cast<float>(config::initial_height) / static_cast<float> (config::initial_width);

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

		/*auto uiManager = Gallop::UIManager::Instance();
		auto mainCanvas = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "UIManager", "get_MainCanvas", IgnoreNumberOfArguments)();
		auto camera = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(mainCanvas->klass, "get_worldCamera", 0)->methodPointer(mainCanvas);

		cout << "Camera " << camera << endl;

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(camera->klass, "get_gameObject", 0)->methodPointer(camera);
		wcout << il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(gameObject->klass, "get_tag", 0)->methodPointer(gameObject)->chars << endl;
		il2cpp_class_get_method_from_name_type<Il2CppString* (*)(Il2CppObject*, Il2CppString*)>(gameObject->klass, "set_tag", 1)->methodPointer(gameObject, il2cpp_string_new("MainCamera"));*/

		fullScreenFl = UnityEngine::Screen::fullScreen();

		if (config::discord_rich_presence)
		{
			discord::Core::Create(1080397170215223367, static_cast<uint64_t>(discord::CreateFlags::NoRequireDiscord), &discord);
			startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		}

		auto sceneManagerClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager");

		auto activeSceneChangedField = il2cpp_class_get_field_from_name_wrap(sceneManagerClass, "activeSceneChanged");

		auto action = CreateDelegateWithClassStatic(il2cpp_class_from_type(activeSceneChangedField->type), *([](void*, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::Scene scene1)
			{
				auto hWnd = GetHWND();

				if (config::freeform_window)
				{
					long style = GetWindowLongW(hWnd, GWL_STYLE);
					style |= WS_MAXIMIZEBOX;
					SetWindowLongPtrW(hWnd, GWL_STYLE, style);
				}

				isPortraitBeforeFullscreen = false;

				auto uiManager = Gallop::UIManager::Instance();

				if (config::resolution_3d_scale != 1.0f)
				{
					auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

					if (graphicSettings)
					{
						auto _resolutionScaleField = il2cpp_class_get_field_from_name_wrap(graphicSettings->klass, "_resolutionScale");

						il2cpp_field_set_value(graphicSettings, _resolutionScaleField, &config::resolution_3d_scale);

						auto _resolutionScale2DField = il2cpp_class_get_field_from_name_wrap(graphicSettings->klass, "_resolutionScale2D");

						il2cpp_field_set_value(graphicSettings, _resolutionScale2DField, &config::resolution_3d_scale);
					}
				}

				if (config::graphics_quality > -1)
				{
					auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));
					if (graphicSettings)
					{
						apply_graphics_quality_hook(graphicSettings, config::graphics_quality, true);
					}
				}

				auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

				if (graphicSettings)
				{
					const auto get_RenderingManager = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(graphicSettings->klass, "get_RenderingManager", 0);
					if (get_RenderingManager)
					{
						auto renderingManager = get_RenderingManager->methodPointer(graphicSettings);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(renderingManager->klass, "set_IsOverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, true);

						if (config::anti_aliasing == 0)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderingManager->klass, "set_OverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, 1);
						}
						else
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(renderingManager->klass, "set_OverrideAntiAliasingLevel", 1)->methodPointer(renderingManager, 0);
						}
					}
				}

				auto active = il2cpp_symbols::get_method_pointer<UnityEngine::SceneManagement::Scene(*)()>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "SceneManager", "GetActiveScene", IgnoreNumberOfArguments)();

				auto handleName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int)>("UnityEngine.CoreModule.dll", "UnityEngine.SceneManagement", "Scene", "GetNameInternal", 1)(active.handle);

				if (!handleName)
				{
					return;
				}

				/*Il2CppArraySize_t<Il2CppObject*>* CriWareInitializerList;
				if (Game::CurrentGameRegion == Game::Region::KOR)
				{
					CriWareInitializerList = UnityEngine::Object::FindObjectsByType(
						GetRuntimeType("CriMw.CriWare.Runtime.dll", "CriWare", "CriWareInitializer"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
				}
				else
				{
					CriWareInitializerList = UnityEngine::Object::FindObjectsOfType(
						GetRuntimeType("CriMw.CriWare.Runtime.dll", "CriWare", "CriWareInitializer"), true);
				}

				if (CriWareInitializerList && CriWareInitializerList->max_length)
				{
					auto obj = CriWareInitializerList->vector[0];
					auto useDecrypterField = il2cpp_class_get_field_from_name_wrap(obj->klass, "useDecrypter");
					bool useDecrypter;
					il2cpp_field_get_value(obj, useDecrypterField, &useDecrypter);

					cout << "useDecrypter " << boolalpha << useDecrypter << noboolalpha << endl;

					if (useDecrypter)
					{
						auto decrypterConfigField = il2cpp_class_get_field_from_name_wrap(obj->klass, "decrypterConfig");
						Il2CppObject* decrypterConfig;
						il2cpp_field_get_value(obj, decrypterConfigField, &decrypterConfig);

						if (decrypterConfig)
						{
							auto keyField = il2cpp_class_get_field_from_name_wrap(decrypterConfig->klass, "key");
							Il2CppString* key;
							il2cpp_field_get_value(decrypterConfig, keyField, &key);

							cout << "key: " << wide_u8(key->chars) << endl;
						}
					}
				}*/

				wstring sceneName = handleName->chars;

				//if (sceneName == L"Race")
				//{
				//	auto delayCallback = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
				//		{
				//			auto horseManager = GetRaceHorseManager();

				//			cout << "horseManager " << horseManager << endl;
				//		}))->delegate;

				//	// Delay 50ms
				//	il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, delayCallback, true);
				//}
				if (sceneName == L"Title")
				{
					StartTickFrame();
				}

				if (sceneName == L"OutGame" && Game::CurrentGameRegion == Game::Region::KOR)
				{
					auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));
					auto viewId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(sceneManager->klass, "GetCurrentViewId", 0)->methodPointer(sceneManager);
					wstring viewName = GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "SceneDefine/ViewId"), viewId)->chars;

					if (viewName == L"Friend")
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

				if (sceneName == L"Live")
				{
					SystemMediaTransportControlsManager::instance.IsEnabled(true);

					auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
					auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

					SystemMediaTransportControlsManager::UpdateMetadata(musicId);
					SystemMediaTransportControlsManager::instance.IsPreviousEnabled(false);
					SystemMediaTransportControlsManager::instance.IsNextEnabled(true);
				}
				else if (sceneName == L"Home")
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

				if (sceneName == L"Title")
				{
					if (Game::CurrentGameRegion == Game::Region::KOR)
					{
						auto uncheater = GetSingletonInstanceByMethod(il2cpp_symbols::get_class("umamusume.dll", "", "UncheaterInit"));

						if (uncheater)
						{
							auto flagField = il2cpp_class_get_field_from_name_wrap(uncheater->klass, "flag");
							bool flag = true;
							il2cpp_field_set_value(uncheater, flagField, &flag);

							auto systemField = il2cpp_class_get_field_from_name_wrap(uncheater->klass, "uncheater");
							Il2CppObject* system;
							il2cpp_field_get_value(uncheater, systemField, &system);

							if (system)
							{
								auto pUncSendCommandVa_Field = il2cpp_class_get_field_from_name_wrap(system->klass, "pUncSendCommandVa_");
								auto fn = CreateDelegateStatic(*[](void*, uint32_t cmd, va_list va)
									{
										cout << cmd << endl;
										return 0;
									});
								il2cpp_field_set_value(system, pUncSendCommandVa_Field, fn);
							}
						}
					}

					if (config::freeform_window && Game::CurrentGameRegion == Game::Region::KOR)
					{
						static bool initialResize = false;

						if (!initialResize) {
							initialResize = true;

							auto hWnd = GetHWND();

							RECT windowRect;
							GetWindowRect(hWnd, &windowRect);
							int windowWidth = windowRect.right - windowRect.left,
								windowHeight = windowRect.bottom - windowRect.top;
							resizeWindow(hWnd, windowWidth, windowHeight);
						}
					}

					if (config::character_system_text_caption)
					{
						isRequiredInitNotification = true;
						notification = nullptr;
					}

					if (config::max_fps > -1 || config::unlock_size || config::freeform_window)
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
							auto oldWndProcPtr2 = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
							il2cpp_field_static_set_value(oldWndProcPtrField, &oldWndProcPtr2);

							if ((config::unlock_size || config::freeform_window) && config::initial_width > 72 && config::initial_height > 72)
							{
								if (config::initial_width < config::initial_height)
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
							Il2CppObject* _bgCamera = uiManager._bgCamera();

							/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Color_t)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
								il2cpp_symbols::get_method_pointer<Color_t(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());*/
						}
					}

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

				if (sceneName == L"Home")
				{
					if (config::character_system_text_caption)
					{
						InitNotification();
					}

					auto readDisclaimer = il2cpp_symbols::get_method_pointer<int (*)(Il2CppString*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "GetInt", 2)(il2cpp_string_new("ReadDisclaimer"), 0);

					if (!readDisclaimer)
					{
						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppString*, Il2CppString*, Il2CppString*, uint64_t, Il2CppDelegate*, Il2CppDelegate*, bool, bool, Il2CppString*)>
							("umamusume.dll", "Gallop", "DialogSimpleCheckNoWarning", "OpenMiddleOneButton", 9)(localize_get_hook(GetTextIdByName(L"Common0081")), il2cpp_string_new16(
								(LocalifySettings::GetText("initial_disclaimer_1") + wstring(localize_get_hook(GetTextIdByName(L"Common187002"))->chars) + LocalifySettings::GetText("initial_disclaimer_2")).data()),
								localize_get_hook(GetTextIdByName(L"Common187002")), GetTextIdByName(L"Common0007"),
								CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
									{
										il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "DialogCommon", "Close", 0)(GetFrontDialog());
										il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "SetInt", 2)(il2cpp_string_new("ReadDisclaimer"), 1);
										il2cpp_symbols::get_method_pointer<void (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "Save", IgnoreNumberOfArguments);
									}), nullptr, false, true, nullptr);
					}

					if (config::unlock_live_chara)
					{
						auto charaList = MsgPackModify::GetCharaList();

						auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "WorkDataManager"));

						auto workCharaData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_CharaData", 0)->methodPointer(workDataManager);

						auto UserCharaClass = il2cpp_symbols::get_class("umamusume.Http.dll", "Gallop", "UserChara");

						for (auto& chara : charaList)
						{
							auto userChara = il2cpp_object_new(UserCharaClass);

							auto chara_id_field = il2cpp_class_get_field_from_name_wrap(userChara->klass, "chara_id");
							int chara_id = chara["chara_id"].int32_value();
							il2cpp_field_set_value(userChara, chara_id_field, &chara_id);

							auto training_num_field = il2cpp_class_get_field_from_name_wrap(userChara->klass, "training_num");
							int training_num = chara["training_num"].int32_value();
							il2cpp_field_set_value(userChara, training_num_field, &training_num);

							auto love_point_field = il2cpp_class_get_field_from_name_wrap(userChara->klass, "love_point");
							int love_point = chara["love_point"].int32_value();
							il2cpp_field_set_value(userChara, love_point_field, &love_point);

							auto love_point_pool_field = il2cpp_class_get_field_from_name_wrap(userChara->klass, "love_point_pool");
							int love_point_pool = chara["love_point_pool"].int32_value();
							il2cpp_field_set_value(userChara, love_point_pool_field, &love_point_pool);

							auto fan_field = il2cpp_class_get_field_from_name_wrap(userChara->klass, "fan");
							uint64_t fan = chara["fan"].uint64_value();
							il2cpp_field_set_value(userChara, fan_field, &fan);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(workCharaData->klass, "UpdateCharaData", 1)->methodPointer(workCharaData, userChara);
						}
					}
				}

				if (sceneName == L"Live" && config::champions_live_show_text)
				{
					auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
					auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

					if (musicId == 1054)
					{
						auto raceInfo = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_raceInfo", 0)->methodPointer(loadSettings);

						auto resourceId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(raceInfo->klass, "get_ChampionsMeetingResourceId", 0)->methodPointer(raceInfo);

						if (resourceId == 0)
						{
							auto charaNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_symbols::get_class("mscorlib.dll", "System", "String"), 9);
							auto trainerNameArray = il2cpp_array_new_type<Il2CppString*>(il2cpp_symbols::get_class("mscorlib.dll", "System", "String"), 9);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArray", 1)->methodPointer(raceInfo, charaNameArray);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArray", 1)->methodPointer(raceInfo, trainerNameArray);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_CharacterNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppString*>*)>(raceInfo->klass, "set_TrainerNameArrayForChampionsText", 1)->methodPointer(raceInfo, nullptr);

							auto charaInfoList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(loadSettings->klass, "get_CharacterInfoList", 0)->methodPointer(loadSettings);

							FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(charaInfoList->klass, "_items");
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
					auto detail = GetSceneName(wide_u8(sceneName));

					discord::Activity activity{};
					activity.GetAssets().SetLargeImage("umamusume");
					activity.SetDetails(detail.data());
					activity.GetTimestamps().SetStart(startTime);
					if (sceneName == L"Live"s)
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

		SystemMediaTransportControlsManager::CreateShortcutForSMTC(reinterpret_cast<decltype(localize_get_hook)*>(localize_get_orig)(GetTextIdByName(L"Outgame0028"))->chars);
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

										auto SetListIdField = il2cpp_class_get_field_from_name_wrap(data->klass, "SetListId");
										int SetListId;
										il2cpp_field_get_value(data, SetListIdField, &SetListId);

										if (SetListId > 0)
										{
											auto _jukeboxBgmSelectorField = il2cpp_class_get_field_from_name_wrap(topUi->klass, "_jukeboxBgmSelector");
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

									auto IsPlayingField = il2cpp_class_get_field_from_name_wrap(data->klass, "IsPlaying");
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

void* MessageBoxA_orig = nullptr;

int
WINAPI
MessageBoxA_hook(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType)
{
	if (string(lpCaption).starts_with("AppSign"))
	{
		lpText = "Wellbia AppSign이 의도치 않은 동작을 감지하여 프로그램이 종료됩니다.\n\n문제가 지속되는 경우 https://wellbia.com/ 에 방문하거나, support@wellbia.com 으로 문의하시기 바랍니다.";
		lpCaption = "Warning";
		uType = MB_ICONWARNING;
	}
	return reinterpret_cast<decltype(MessageBoxA)*>(MessageBoxA_orig)(hWnd, lpText, lpCaption, uType);
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
				doc.GetObjectW()["os"].SetString(rapidjson::StringRef("android"));

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

constexpr int MAX_DLL_COUNT = 23;
constexpr int MAX_ROOT_FILE_COUNT = 8 + /* self (.) */1 + /* parent (..) */1;

HANDLE currentFindHandle;
HANDLE currentRootFindHandle;

int dllCount;
int rootFileCount;

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

void* GetModuleHandleW_orig = nullptr;
static HMODULE
WINAPI
GetModuleHandleW_hook(
	_In_opt_ LPCWSTR lpModuleName
)
{
	if (lpModuleName && lpModuleName == L"version.dll"s)
	{
		return proxy::version;
	}
	return reinterpret_cast<decltype(GetModuleHandleW)*>(GetModuleHandleW_orig)(lpModuleName);
}

void* WinVerifyTrust_addr = nullptr;
void* WinVerifyTrust_orig = nullptr;
static LONG WINAPI WinVerifyTrust_hook(HWND hwnd, GUID* pgActionID, LPVOID pWVTData)
{
	auto data = reinterpret_cast<WINTRUST_DATA*>(pWVTData);

	if (data->pFile)
	{
		if (data->pFile->pcwszFilePath)
		{
			wstring path = data->pFile->pcwszFilePath;
			transform(path.begin(), path.end(), path.begin(), [](auto c)
				{
					return tolower(c);
				}
			);
			if (path.find(L"version.dll") != wstring::npos)
			{
				wstring dll_path;
				dll_path.resize(MAX_PATH);
				dll_path.resize(GetSystemDirectoryW(dll_path.data(), MAX_PATH));

				dll_path += LR"(\version.dll)"s;
				data->pFile->pcwszFilePath = dll_path.data();

				MH_DisableHook(WinVerifyTrust_addr);

				return reinterpret_cast<decltype(WinVerifyTrust)*>(WinVerifyTrust_addr)(hwnd, pgActionID, pWVTData);
			}
		}
	}

	return reinterpret_cast<decltype(WinVerifyTrust)*>(WinVerifyTrust_orig)(hwnd, pgActionID, pWVTData);
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

	MH_CreateHook(MessageBoxA, MessageBoxA_hook, &MessageBoxA_orig);
	MH_EnableHook(MessageBoxA);

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
#ifdef _DEBUG
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

	MH_CreateHook(GetModuleHandleW, GetModuleHandleW_hook, &GetModuleHandleW_orig);
	MH_EnableHook(GetModuleHandleW);

	if (!config::allow_delete_cookie && Game::CurrentGameRegion == Game::Region::KOR)
	{
		MH_CreateHook(LoadLibraryExW, load_library_ex_w_hook, &load_library_ex_w_orig);
		MH_EnableHook(LoadLibraryExW);
	}

	MH_CreateHook(LoadLibraryW, load_library_w_hook, &load_library_w_orig);
	MH_EnableHook(LoadLibraryW);

	auto wintrust = LoadLibraryW(L"wintrust.dll");

	WinVerifyTrust_addr = GetProcAddress(wintrust, "WinVerifyTrust");

	MH_CreateHook(WinVerifyTrust_addr, WinVerifyTrust_hook, &WinVerifyTrust_orig);
	MH_EnableHook(WinVerifyTrust_addr);

	auto UnityPlayer = GetModuleHandleW(L"UnityPlayer.dll");
	auto UnityMain_addr = GetProcAddress(UnityPlayer, "UnityMain");

	MH_CreateHook(UnityMain_addr, UnityMain_hook, &UnityMain_orig);
	MH_EnableHook(UnityMain_addr);

	return true;
}

bool init_hook()
{
	fullScreenFl = config::auto_fullscreen && !config::freeform_window;

	if (config::freeform_window)
	{
		MH_CreateHook(SetWindowLongPtrW, SetWindowLongPtrW_hook, &SetWindowLongPtrW_orig);
		MH_EnableHook(SetWindowLongPtrW);
		MH_CreateHook(SetWindowLongPtrA, SetWindowLongPtrA_hook, &SetWindowLongPtrA_orig);
		MH_EnableHook(SetWindowLongPtrA);
	}

	return true;
}

void uninit_hook()
{
	if (!mh_inited)
		return;

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
