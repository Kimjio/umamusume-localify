#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "WebViewManager.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/Vector4.hpp"
#include "../../Cute.Core.Assembly/Cute/Core/WebViewManager.hpp"

#include <WebView2.h>
#include <wrl.h>
#include <wil/com.h>
#include <ShlObj.h>
#include <WebView2EnvironmentOptions.h>

#include "config/config.hpp"

#include "string_utils.hpp"

#include "jwt/jwt.hpp"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;

void* Gallop_WebViewManager_cctor_addr = nullptr;
void* Gallop_WebViewManager_cctor_orig = nullptr;

void* Gallop_WebViewManager_GetUrl_addr = nullptr;
void* Gallop_WebViewManager_GetUrl_orig = nullptr;

void* Gallop_WebViewManager_Open_addr = nullptr;
void* Gallop_WebViewManager_Open_orig = nullptr;

void* Gallop_WebViewManager_SetCustomFont_addr = nullptr;
void* Gallop_WebViewManager_SetCustomFont_orig = nullptr;

void* Gallop_WebViewManager_OpenWebView_addr = nullptr;

void* Gallop_WebViewManager_SetMargin_addr = nullptr;

void* Gallop_WebViewManager_get_CuteWebView_addr = nullptr;

void* Gallop_WebViewManager_SetErrorCallback_addr = nullptr;

void* Gallop_WebViewManager_OpenErrorDialog_addr = nullptr;

void* Gallop_WebViewManager_GetGachaUrl_addr = nullptr;
void* Gallop_WebViewManager_GetGachaUrl_orig = nullptr;

void* Gallop_WebViewManager_GetProductUrl_addr = nullptr;
void* Gallop_WebViewManager_GetProductUrl_orig = nullptr;

void* Gallop_WebViewManager_SettingUIEffectOnOpen_addr = nullptr;

void* Gallop_WebViewManager_SettingUIEffectOnClose_addr = nullptr;

void* Gallop_WebViewManager_GetGachaURLProperty_addr = nullptr;
void* Gallop_WebViewManager_GetGachaURLProperty_orig = nullptr;

void* Gallop_WebViewManager_GetProductURLProperty_addr = nullptr;

const MethodInfo* Gallop_WebViewManager_TryGetWebViewInfo = nullptr;
void* Gallop_WebViewManager_TryGetWebViewInfo_addr = nullptr;

Il2CppClass* Gallop_WebViewManager_WebViewInfo = nullptr;

FieldInfo* Gallop_WebViewManager__currentWebViewDialog = nullptr;

FieldInfo* Gallop_WebViewManager__errorCallback = nullptr;

FieldInfo* Gallop_WebViewManager__fontFilePaths = nullptr;

void* Gallop_WebViewManager_WebViewInfo_ctor_addr = nullptr;

FieldInfo* Gallop_WebViewManager_WebViewInfo__url = nullptr;

FieldInfo* Gallop_WebViewManager_WebViewInfo__isVueJs = nullptr;

void* Gallop_WebViewManager_OpenGachaDetail_addr = nullptr;
void* Gallop_WebViewManager_OpenGachaDetail_orig = nullptr;

void* Gallop_WebViewManager_OpenPurchaseItemDetail_addr = nullptr;
void* Gallop_WebViewManager_OpenPurchaseItemDetail_orig = nullptr;

void* DialogHomeMenuMain_SetupTrainer_callback = nullptr;

void* DialogHomeMenuMain_SetupTrainer_addr = nullptr;
void* DialogHomeMenuMain_SetupTrainer_orig = nullptr;

void* DialogHomeMenuMain_SetupOther_callback = nullptr;

void* DialogHomeMenuMain_SetupOther_addr = nullptr;
void* DialogHomeMenuMain_SetupOther_orig = nullptr;

void* DialogHomeMenuSupport_OnSelectMenu_addr = nullptr;
void* DialogHomeMenuSupport_OnSelectMenu_orig = nullptr;

void* DialogTitleMenu_OnSelectMenu_addr = nullptr;
void* DialogTitleMenu_OnSelectMenu_orig = nullptr;

void* DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_addr = nullptr;
void* DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_orig = nullptr;

void* DialogTutorialGuide_OnPushHelpButton_addr = nullptr;
void* DialogTutorialGuide_OnPushHelpButton_orig = nullptr;

void* DialogSingleModeTopMenu_Setup_help_callback = nullptr;

void* DialogSingleModeTopMenu_Setup_guide_callback = nullptr;

void* DialogSingleModeTopMenu_Setup_addr = nullptr;
void* DialogSingleModeTopMenu_Setup_orig = nullptr;

void* ChampionsInfoWebViewButton_OnClick_addr = nullptr;
void* ChampionsInfoWebViewButton_OnClick_orig = nullptr;

void* StoryEventTopViewController_OnClickHelpButton_addr = nullptr;
void* StoryEventTopViewController_OnClickHelpButton_orig = nullptr;

void* PartsNewsButton_Setup_callback = nullptr;

void* PartsNewsButton_Setup_addr = nullptr;
void* PartsNewsButton_Setup_orig = nullptr;

void* BannerUI_OnClickBannerItem_addr = nullptr;
void* BannerUI_OnClickBannerItem_orig = nullptr;

void* KakaoManager_OnKakaoShowInAppWebView_addr = nullptr;
void* KakaoManager_OnKakaoShowInAppWebView_orig = nullptr;

bool _isShowWebView = false;

extern wil::com_ptr<ICoreWebView2Controller> webviewController;
extern wil::com_ptr<ICoreWebView2> webview;

static void OpenWebViewDialog(Il2CppString* url, Il2CppString* headerTextArg, uint64_t closeTextId,
	Il2CppDelegate* onClose = nullptr) {
	auto data = il2cpp_object_new(
		il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommon/Data"));
	il2cpp_runtime_object_init(data);

	data = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * self,
		Il2CppString * headerTextArg,
		Il2CppString * message,
		Il2CppDelegate * onClickCenterButton,
		unsigned long closeTextId, int dialogFormType)>(il2cpp_class_get_method_from_name(data->klass, "SetSimpleOneButtonMessage", 5)->methodPointer)
			(data, headerTextArg, nullptr, onClose, closeTextId, 9);

	auto webViewManager = GetSingletonInstance(
		il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"));
	reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, Il2CppObject*, Il2CppDelegate*,
		Il2CppDelegate*, bool)>(il2cpp_class_get_method_from_name(
			webViewManager->klass, "Open", 5)->methodPointer)(webViewManager, url, data,
				nullptr, nullptr, false);
}

static Il2CppString* LocalizeExtention_Text(uint64_t id)
{
	return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(uint64_t)>(
		ASSEMBLY_NAME, "Gallop", "LocalizeExtention", "Text", 1
	)(id);
}

static void OpenNewsDialog() {
	if (config::use_third_party_news)
	{
		OpenWebViewDialog(il2cpp_string_new("https://m.cafe.daum.net/umamusume-kor/Z4os"),
			LocalizeExtention_Text(GetTextIdByName(L"Common0081")),
			GetTextIdByName(L"Common0007"));
	}
	else
	{
		auto webViewManager = GetSingletonInstance(
			il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"));
		reinterpret_cast<void (*)(Il2CppObject*,
			Il2CppDelegate*)>(il2cpp_class_get_method_from_name(
				webViewManager->klass, "OpenNews", 1)->methodPointer)(webViewManager, nullptr);
	}
}

static void OpenHelpDialog() {
	auto webViewManager = GetSingletonInstance(
		il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"));
	reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(
		webViewManager->klass, "OpenHelp", 0)->methodPointer)(webViewManager);
}

static void OpenStoryEventHelpDialog() {
	auto webViewManager = GetSingletonInstance(
		il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"));
	reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(
		webViewManager->klass, "OpenStoryEventHelp", 0)->methodPointer)(webViewManager);
}

static string GetOqupieToken()
{
	if (Game::CurrentGameRegion != Game::Region::KOR) {
		cout << "GetOqupieToken: Not korean version... returning empty string." << endl;
		return "";
	}
	const auto oqupieAccessKey = "a66427394118bc5e";
	const auto jwtToken = "f2c9ea20a25a94b7885d75f220cfcbcf";

	auto Application = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine",
		"Application");

	auto systemLanguage = reinterpret_cast<int (*)()>(il2cpp_class_get_method_from_name(Application,
		"get_systemLanguage",
		0)->methodPointer)();

	auto SystemInfo = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine",
		"SystemInfo");

	auto deviceId = reinterpret_cast<Il2CppString * (*)()>(il2cpp_class_get_method_from_name(
		SystemInfo, "get_deviceUniqueIdentifier", 0)->methodPointer)();
	auto deviceIdU8 = wide_u8(deviceId->chars);

	auto deviceModel = reinterpret_cast<Il2CppString * (*)()>(il2cpp_class_get_method_from_name(
		SystemInfo, "get_deviceModel", 0)->methodPointer)();
	auto deviceModelU8 = wide_u8(deviceModel->chars);

	auto systemMemorySize = reinterpret_cast<int (*)()>(il2cpp_class_get_method_from_name(
		SystemInfo, "get_systemMemorySize", 0)->methodPointer)();

	auto operatingSystem = reinterpret_cast<Il2CppString * (*)()>(il2cpp_class_get_method_from_name(
		SystemInfo, "get_operatingSystem", 0)->methodPointer)();
	auto operatingSystemU8 = wide_u8(operatingSystem->chars);

	auto manager = GetSingletonInstanceByMethod(il2cpp_symbols::get_class(ASSEMBLY_NAME, "", "KakaoManager"));
	Il2CppString* playerId = reinterpret_cast<Il2CppString * (*)(
		Il2CppObject*)>(il2cpp_class_get_method_from_name(manager->klass, "get_PlayerID",
			0)->methodPointer)(manager);
	auto playerIdU8 = wide_u8(playerId->chars);

	auto AppVersionName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "DeviceHelper", "GetAppVersionName", 0)();

	auto payload = "{"s;
	payload += R"("access_key":")";
	payload += oqupieAccessKey;
	payload += R"(",)";
	payload += R"("brand_key1":"inquirykr",)";

	payload += R"("userId":")";
	payload += playerIdU8;
	payload += R"(",)";

	payload += R"("deviceId":")";
	payload += deviceIdU8;
	payload += R"(",)";

	payload += R"("deviceModel":")";
	payload += deviceModelU8;
	payload += R"(",)";

	payload += R"("systemMemorySize":)";
	payload += to_string(systemMemorySize);
	payload += R"(,)";

	payload += R"("systemLanguage":)";
	payload += to_string(systemLanguage);
	payload += R"(,)";

	payload += R"("operatingSystem":")";
	payload += operatingSystemU8;
	payload += R"(",)";

	payload += R"("version_client":")";
	payload += wide_u8(AppVersionName->chars).data();
	payload += R"(",)";
	payload += R"("exp":)";
	auto nowSec = chrono::duration_cast<chrono::seconds>(
		chrono::system_clock::now().time_since_epoch()).count();
	payload += to_string(nowSec + 3600);
	payload += "}";

	auto token = jwt(jwtToken);
	return token.encodeJWT(payload);
}

static void Gallop_WebViewManager_cctor_hook()
{
	reinterpret_cast<decltype(Gallop_WebViewManager_cctor_hook)*>(Gallop_WebViewManager_cctor_orig)();

	auto WebViewUrlDictField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class(ASSEMBLY_NAME, Gallop::WebViewManager::namespaze, Gallop::WebViewManager::klassName), "WebViewUrlDict");
	Il2CppObject* WebViewUrlDict;
	il2cpp_field_static_get_value(WebViewUrlDictField, &WebViewUrlDict);

	auto WebViewUrlDict_Add = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Gallop::WebViewDefine::Url, Il2CppObject*, const MethodInfo*)>(WebViewUrlDict->klass, "Add", 2);
	void (*WebViewUrlDict_Add_methodPointer)(Il2CppObject*, Gallop::WebViewDefine::Url, Il2CppObject*, const MethodInfo*) = WebViewUrlDict_Add->methodPointer;

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		WebViewUrlDict_Add_methodPointer = reinterpret_cast<decltype(WebViewUrlDict_Add_methodPointer)>(reinterpret_cast<const MethodInfo2020*>(WebViewUrlDict_Add)->methodPointer);
	}

	auto methodInfo = reinterpret_cast<const MethodInfo*>(WebViewUrlDict_Add);

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(WebViewUrlDict->klass, "Clear", 0)->methodPointer(WebViewUrlDict);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::Update,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("info?p=2&c=0")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::Event,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("info?p=2&c=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::Bug,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("info?p=2&c=3")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::Help,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::NewsDetail,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("info_detail?transition_id={0}")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::StoryEventHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("story_event_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::TrainingChallengeHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("training_challenge_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::ChallengeMatchHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("racing_carnival_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::FanRaidHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("fan_collect_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::CollectRaidHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("collect_raid_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::TeamBuildingHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("team_building_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::FactorResearchHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("factor_research_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::HeroesHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("heroes_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::UltimateRaceHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("ultimate_race_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::RatingRaceHelp,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("rating_race_help?p=2")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::TermOfService,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("terms_of_service/")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::Tokusho,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("commercial_transaction/")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::ShopPayment,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("fund_settlement/")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::PrivacyPolicy,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("privacy_policy/")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::License,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("rights/")), methodInfo);
	WebViewUrlDict_Add_methodPointer(WebViewUrlDict, Gallop::WebViewDefine::Url::AppleId,
		Gallop::WebViewManager::WebViewInfo(il2cpp_string_new("")), methodInfo);
}

static Il2CppString* Gallop_WebViewManager_GetUrl_hook(Il2CppObject* self, Gallop::WebViewDefine::Url urlType)
{
	Gallop::WebViewManager::WebViewInfo webViewInfo = nullptr;
	if (!Gallop::WebViewManager::TryGetWebViewInfo(urlType, &webViewInfo))
	{
		return il2cpp_string_new("");
	}


	string path = "/contents/";

	if (webViewInfo._isVueJs())
	{
		path += "v/index.html#/";
	}

	if (urlType == Gallop::WebViewDefine::Url::AppleId)
	{
		auto SocialServiceUtility = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>(ASSEMBLY_NAME, "Gallop", "SocialServiceUtility", "get_Instance", 0)();

		auto SIGN_IN_WITH_APPLE_12_Field = il2cpp_class_get_field_from_name(SocialServiceUtility->klass, "SIGN_IN_WITH_APPLE_12");
		Il2CppString* SIGN_IN_WITH_APPLE_12;
		il2cpp_field_get_value(SocialServiceUtility, SIGN_IN_WITH_APPLE_12_Field, &SIGN_IN_WITH_APPLE_12);

		return SIGN_IN_WITH_APPLE_12;
	}

	auto url = webViewInfo._url();

	if (urlType == Gallop::WebViewDefine::Update)
	{
		auto ViewerId = il2cpp_symbols::get_method_pointer<int64_t(*)()>(ASSEMBLY_NAME, "Gallop", "Certification", "get_ViewerId", 0)();

		auto ViewerIdObj = il2cpp_value_box(il2cpp_defaults.int64_class, &ViewerId);

		auto params = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppObject*)>("mscorlib.dll", "System", "String", "Format", 2)
			(il2cpp_string_new("&v={0}"), ViewerIdObj);

		url = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 2)(url, params);
	}

	auto serverUrl = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "GameDefine", "get_ApplicationServerUrl", 0)();

	return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 3)(serverUrl, il2cpp_string_new(path.data()), url);
}

static void Gallop_WebViewManager_Open_hook(Il2CppObject* self, Il2CppString* url, Il2CppObject* dialogData, Il2CppDelegate* onLoadedCallback, Il2CppDelegate* onErrorCallback, bool isDirect)
{
	auto webViewManager = Gallop::WebViewManager(self);

	if (!_isShowWebView)
	{
		il2cpp_class_get_field_from_name_wrap(self->klass, "_errorCallback");
		webViewManager.SetErrorCallback();

		auto internetReachability = UnityEngine::Application::internetReachability();

		if (internetReachability == UnityEngine::NetworkReachability::NotReachable)
		{
			webViewManager.OpenErrorDialog();
			return;
		}

		_isShowWebView = true;

		auto actionClass = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "Action`1"), GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogCommon"));
		auto beginOpenCallback = CreateDelegateWithClassStatic(actionClass, *[](void*, Il2CppObject* dialog)
			{
			}
		);
		auto BeginOpenCallbackField = il2cpp_class_get_field_from_name(dialogData->klass, "BeginOpenCallback");
		il2cpp_field_set_value(dialogData, BeginOpenCallbackField, beginOpenCallback);

		auto openCallback = CreateDelegateWithClassStatic(actionClass, *[](void*, Il2CppObject* dialog)
			{
				if (Cute::Core::WebViewManager::webviewController)
				{
					Cute::Core::WebViewManager::Instance().SetVisible(true);
				}
				Gallop::WebViewManager::SettingUIEffectOnOpen();
			}
		);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(dialogData->klass, "AddOpenCallback", 1)->methodPointer(dialogData, &openCallback->delegate);

		//AddDestroyCallback
		auto destroyCallback = CreateDelegateWithClassStatic(il2cpp_symbols::get_class("mscorlib.dll", "System", "Action"), *[](void*)
			{
				_isShowWebView = false;
				Gallop::WebViewManager::Instance()._currentWebViewDialog(nullptr);

				if (Cute::Core::WebViewManager::webviewController)
				{
					webview->Navigate(L"about:blank");
				}
			}
		);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(dialogData->klass, "AddDestroyCallback", 1)->methodPointer(dialogData, &destroyCallback->delegate);


		//AddBeginCloseCallback
		auto action1Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "Action`1"), GetRuntimeType(il2cpp_defaults.boolean_class));
		auto beginCloseCallback = CreateDelegateWithClassStatic(action1Class, *[](void*, bool needDestroy)
			{
				if (Cute::Core::WebViewManager::webviewController)
				{
					Cute::Core::WebViewManager::Instance().SetVisible(false);
				}
				Gallop::WebViewManager::SettingUIEffectOnClose();
			}
		);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(dialogData->klass, "AddBeginCloseCallback", 1)->methodPointer(dialogData, &beginCloseCallback->delegate);

		auto dialogCommon = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, bool)>(
			ASSEMBLY_NAME, "Gallop", "DialogManager", "PushSystemDialog", 2)(dialogData, true);

		webViewManager._currentWebViewDialog(dialogCommon);

		webViewManager.OpenWebView(url, onLoadedCallback);

		UnityEngine::Rect WebViewRectOffset;
		auto WebViewRectOffsetField = il2cpp_class_get_field_from_name(dialogData->klass, "WebViewRectOffset");
		il2cpp_field_get_value(dialogData, WebViewRectOffsetField, &WebViewRectOffset);
		Gallop::WebViewManager(self).SetMargin(WebViewRectOffset);
	}
}

static void Gallop_WebViewManager_SetCustomFont_hook(Il2CppObject* self, Gallop::WebViewDefine::FontNameDefine fontName)
{
	auto fontFilePaths = Gallop::WebViewManager(self)._fontFilePaths();

	if (fontName < fontFilePaths->max_length)
	{
		auto filePath = fontFilePaths->vector[fontName];
		wstringstream pathStream(filePath->chars);
		wstring segment;
		vector<wstring> splited;
		while (getline(pathStream, segment, L'/'))
		{
			splited.emplace_back(segment);
		}

		if (!Cute::Core::WebViewManager::customFontMap.contains(splited.back()))
		{
			auto LocalFile = il2cpp_symbols::get_method_pointer<Il2CppObject* (*)()>(ASSEMBLY_NAME, "Gallop", "AssetManager", "get_LocalFile", 0)();
			auto pathAllowUnknown = il2cpp_class_get_method_from_name_type<Il2CppString* (*)(Il2CppObject*, Il2CppString*)>(LocalFile->klass, "GetPathAllowUnknown", 1)->methodPointer(LocalFile, filePath);
	
			if (filesystem::exists(wstring(pathAllowUnknown->chars)))
			{
				Cute::Core::WebViewManager::customFontMap.emplace(splited.back(), pathAllowUnknown->chars);
			}
		}
	}

	Gallop::WebViewManager(self).CuteWebView();
}

static Il2CppString* Gallop_WebViewManager_GetGachaURLProperty_hook(int gachaId)
{
	auto AppVersionName = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "DeviceHelper", "GetAppVersionName", 0)();
	auto SafeResourceVer = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetSafeResourceVer", 0)();

	auto gachaIdObj = il2cpp_value_box(il2cpp_defaults.int32_class, &gachaId);

	auto params = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*, Il2CppString*, Il2CppObject*)>("mscorlib.dll", "System", "String", "Format", 4)(
		il2cpp_string_new("?v={0}&r={1}&g={2}"), AppVersionName, SafeResourceVer, gachaIdObj);

	uint64_t gachaType;
	if (il2cpp_symbols::get_method_pointer<bool (*)(int, uint64_t*)>(ASSEMBLY_NAME, "Gallop", "WebViewManager", "IsSelectPickupGacha", 2)(gachaId, &gachaType))
	{
		auto SelectPickupGachaURLProperty = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, uint64_t)>(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetSelectPickupGachaURLProperty", 2)(gachaId, gachaType);

		params = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 2)(params, SelectPickupGachaURLProperty);
	}

	return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 2)(params, il2cpp_string_new("&p=2"));
}

static Il2CppString* Gallop_WebViewManager_GetGachaUrl_hook(int gachaId, int stepupId)
{
	auto serverUrl = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "GameDefine", "get_ApplicationServerUrl", 0)();
	auto res = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 3)(serverUrl, il2cpp_string_new("/contents/v/index.html#/gacha"),
		Gallop::WebViewManager::GetGachaURLProperty(gachaId));
	return res;
}

static Il2CppString* Gallop_WebViewManager_GetProductUrl_hook(int productMasterId)
{
	auto serverUrl = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(ASSEMBLY_NAME, "Gallop", "GameDefine", "get_ApplicationServerUrl", 0)();
	auto res = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppString*, Il2CppString*, Il2CppString*)>("mscorlib.dll", "System", "String", "Concat", 3)(serverUrl, il2cpp_string_new("/contents/v/index.html#/product"),
		Gallop::WebViewManager::GetProductURLProperty(productMasterId));
	return res;
}

static void DialogHomeMenuMain_SetupTrainer_hook(Il2CppObject* self, Il2CppObject* dialog) {
	reinterpret_cast<decltype(DialogHomeMenuMain_SetupTrainer_hook)*>(DialogHomeMenuMain_SetupTrainer_orig)(
		self, dialog);
	auto guideButtonField = il2cpp_class_get_field_from_name(self->klass, "_guideButton");
	Il2CppObject* guideButton;
	il2cpp_field_get_value(self, guideButtonField, &guideButton);

	auto guideCallback = GetButtonCommonOnClickDelegate(guideButton);
	if (guideCallback)
	{
		if (!DialogHomeMenuMain_SetupTrainer_callback)
		{
			auto newFn = *([]()
				{
					OpenWebViewDialog(il2cpp_string_new("https://guide.umms.kakaogames.com"),
						LocalizeExtention_Text(GetTextIdByName(L"Menu900001")),
						GetTextIdByName(L"Common0007"));
				});
			MH_CreateHook(reinterpret_cast<void*>(guideCallback->method_ptr),
				reinterpret_cast<void*>(newFn), &DialogHomeMenuMain_SetupTrainer_callback);
			MH_EnableHook(reinterpret_cast<void*>(guideCallback->method_ptr));
		}
	}
}

static void DialogHomeMenuMain_SetupOther_hook(Il2CppObject* self) {
	reinterpret_cast<decltype(DialogHomeMenuMain_SetupOther_hook)*>(DialogHomeMenuMain_SetupOther_orig)(
		self);
	auto helpButtonField = il2cpp_class_get_field_from_name(self->klass, "_helpButton");
	Il2CppObject* helpButton;
	il2cpp_field_get_value(self, helpButtonField, &helpButton);

	auto helpCallback = GetButtonCommonOnClickDelegate(helpButton);
	if (helpCallback)
	{
		if (!DialogHomeMenuMain_SetupOther_callback)
		{
			auto newFn = *([]()
				{
					OpenHelpDialog();
				});
			MH_CreateHook(reinterpret_cast<void*>(helpCallback->method_ptr),
				reinterpret_cast<void*>(newFn), &DialogHomeMenuMain_SetupOther_callback);
			MH_EnableHook(reinterpret_cast<void*>(helpCallback->method_ptr));
		}
	}
}

static void DialogHomeMenuSupport_OnSelectMenu_hook(int menu) {
	switch (menu)
	{
	case 0:
	{
		// FAQ
		auto closeText = GetTextIdByName(L"Common0007");
		auto faqText = GetTextIdByName(L"Menu0013");
		auto url = string(
			" https://kakaogames.oqupie.com/portals/1576/categories/3438?jwt=").append(
				GetOqupieToken());
		OpenWebViewDialog(il2cpp_string_new(url.data()), LocalizeExtention_Text(faqText),
			closeText);
		return;
	}
	case 1:
	{
		// QNA
		auto closeText = GetTextIdByName(L"Common0007");
		auto qnaText = GetTextIdByName(L"Common0050");
		auto url = string("https://kakaogames.oqupie.com/portals/finder?jwt=").append(
			GetOqupieToken());
		OpenWebViewDialog(il2cpp_string_new(url.data()), LocalizeExtention_Text(qnaText),
			closeText);
		return;
	}
	case 2:
	{
		// Term of service
		auto closeText = GetTextIdByName(L"Common0007");
		auto termOfService = GetTextIdByName(L"Outgame0082");
		OpenWebViewDialog(il2cpp_string_new(
			"https://web-data-game.kakaocdn.net/real/www/html/terms/index.html?service=S0001&type=T001&country=kr&lang=ko"),
			LocalizeExtention_Text(termOfService), closeText);
	}
	case 3:
	{
		// Privacy policy
		auto closeText = GetTextIdByName(L"Common0007");
		auto privacyPolicy = GetTextIdByName(L"AccoutDataLink0087");
		OpenWebViewDialog(il2cpp_string_new(
			"https://web-data-game.kakaocdn.net/real/www/html/terms/index.html?service=S0001&type=T003&country=kr&lang=ko"),
			LocalizeExtention_Text(privacyPolicy), closeText);
	}
	default:
		reinterpret_cast<decltype(DialogHomeMenuSupport_OnSelectMenu_hook)*>(DialogHomeMenuSupport_OnSelectMenu_orig)(
			menu);
	}
}

static void DialogTitleMenu_OnSelectMenu_hook(int menu)
{
	switch (menu)
	{
	case 0:
		OpenNewsDialog();
		return;
	case 2:
	{
		auto closeText = GetTextIdByName(L"Common0007");
		auto qnaText = GetTextIdByName(L"Common0050");
		auto url = string("https://kakaogames.oqupie.com/portals/finder?jwt=").append(
			GetOqupieToken());
		OpenWebViewDialog(il2cpp_string_new(url.data()), LocalizeExtention_Text(qnaText),
			closeText);
		return;
	}
	default:
		reinterpret_cast<decltype(DialogTitleMenu_OnSelectMenu_hook)*>(DialogTitleMenu_OnSelectMenu_orig)(
			menu);
	}
}

static void DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_hook(int menu)
{
	if (menu == 0)
	{
		OpenNewsDialog();
		return;
	}

	reinterpret_cast<decltype(DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_hook)*>(DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_orig)(
		menu);
}

static void DialogTutorialGuide_OnPushHelpButton_hook(Il2CppObject*)
{
	OpenHelpDialog();
}

static void DialogSingleModeTopMenu_Setup_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(DialogSingleModeTopMenu_Setup_hook)*>(DialogSingleModeTopMenu_Setup_orig)(self);
	auto helpButtonField = il2cpp_class_get_field_from_name(self->klass, "_helpButton");
	Il2CppObject* helpButton;
	il2cpp_field_get_value(self, helpButtonField, &helpButton);
	auto helpCallback = GetButtonCommonOnClickDelegate(helpButton);
	if (helpCallback) {
		if (!DialogSingleModeTopMenu_Setup_help_callback)
		{
			auto newFn = *([]()
				{
					OpenHelpDialog();
				});
			MH_CreateHook(reinterpret_cast<void*>(helpCallback->method_ptr),
				reinterpret_cast<void*>(newFn),
				&DialogSingleModeTopMenu_Setup_help_callback);
			MH_EnableHook(reinterpret_cast<void*>(helpCallback->method_ptr));
		}
	}

	auto guideButtonField = il2cpp_class_get_field_from_name(self->klass, "_guideButton");
	Il2CppObject* guideButton;
	il2cpp_field_get_value(self, guideButtonField, &guideButton);
	auto guideCallback = GetButtonCommonOnClickDelegate(guideButton);
	if (guideCallback)
	{
		auto newFn = *([]()
			{
				OpenWebViewDialog(il2cpp_string_new("https://guide.umms.kakaogames.com"),
					LocalizeExtention_Text(GetTextIdByName(L"Menu900001")),
					GetTextIdByName(L"Common0007"));
			});
		if (!DialogSingleModeTopMenu_Setup_guide_callback)
		{
			MH_CreateHook(reinterpret_cast<void*>(guideCallback->method_ptr),
				reinterpret_cast<void*>(newFn),
				&DialogSingleModeTopMenu_Setup_guide_callback);
			MH_EnableHook(reinterpret_cast<void*>(guideCallback->method_ptr));
		}
	}
}

static void ChampionsInfoWebViewButton_OnClick_hook(Il2CppObject*)
{
	auto KakaoManager = il2cpp_symbols::get_class(ASSEMBLY_NAME, "", "KakaoManager");
	auto managerInstanceField = il2cpp_class_get_field_from_name(KakaoManager, "instance");
	Il2CppObject* manager;
	il2cpp_field_static_get_value(managerInstanceField, &manager);

	auto url = reinterpret_cast<Il2CppString * (*)(Il2CppObject*, Il2CppString*)>(
		il2cpp_class_get_method_from_name(manager->klass, "GetKakaoOptionValue",
			1)->methodPointer
		)(manager, il2cpp_string_new("kakaoUmaChampion"));

	OpenWebViewDialog(url, LocalizeExtention_Text(GetTextIdByName(L"Common0161")),
		GetTextIdByName(L"Common0007"));
}

static void StoryEventTopViewController_OnClickHelpButton_hook(Il2CppObject*)
{
	OpenStoryEventHelpDialog();
}

static void PartsNewsButton_Setup_hook(Il2CppObject* self, Il2CppDelegate* onUpdateBadge)
{
	reinterpret_cast<decltype(PartsNewsButton_Setup_hook)*>(PartsNewsButton_Setup_orig)(self,
		onUpdateBadge);

	auto buttonField = il2cpp_class_get_field_from_name(self->klass, "_button");
	Il2CppObject* button;
	il2cpp_field_get_value(self, buttonField, &button);

	if (button)
	{
		auto callback = GetButtonCommonOnClickDelegate(button);
		if (callback)
		{
			if (!PartsNewsButton_Setup_callback)
			{
				auto newFn = *([](Il2CppObject*)
					{
						OpenNewsDialog();
					});
				MH_CreateHook(reinterpret_cast<void*>(callback->method_ptr),
					reinterpret_cast<void*>(newFn), &PartsNewsButton_Setup_callback);
				MH_EnableHook(reinterpret_cast<void*>(callback->method_ptr));
			}
		}
	}
}

static void BannerUI_OnClickBannerItem_hook(Il2CppObject* self, Il2CppObject* buttonInfo)
{
	auto master = reinterpret_cast<Il2CppObject * (*)(
		Il2CppObject*)>(il2cpp_class_get_method_from_name(buttonInfo->klass, "get_Master",
			0)->methodPointer)(buttonInfo);
	auto masterTypeField = il2cpp_class_get_field_from_name(master->klass, "Type");
	int masterType;
	il2cpp_field_get_value(master, masterTypeField, &masterType);

	auto masterTransitionField = il2cpp_class_get_field_from_name(master->klass, "Transition");
	int masterTransition;
	il2cpp_field_get_value(master, masterTransitionField, &masterTransition);

	if (masterType == 6)
	{
		OpenWebViewDialog(il2cpp_string_new("https://m.cafe.daum.net/umamusume-kor/ZBhv"),
			LocalizeExtention_Text(GetTextIdByName(L"Common0161")),
			GetTextIdByName(L"Common0007"));
		return;
	}

	reinterpret_cast<decltype(BannerUI_OnClickBannerItem_hook)*>(BannerUI_OnClickBannerItem_orig)(self, buttonInfo);
}

static void KakaoManager_OnKakaoShowInAppWebView_hook(Il2CppObject* self, Il2CppString* url, Il2CppDelegate* isSuccess)
{
	if (url->chars == L"https://m.cafe.daum.net/umamusume-kor/Z4os?boardType=notice"s) {
		auto NewsDialogInfo = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop",
			"HomeStartCheckSequence/NewsDialogInfo");
		auto instance = il2cpp_object_new(NewsDialogInfo);
		il2cpp_runtime_object_init(instance);
		auto newsOpened = reinterpret_cast<bool (*)(
			Il2CppObject*)>(il2cpp_class_get_method_from_name(instance->klass, "Check",
				0)->methodPointer)(instance);
		if (!newsOpened)
		{
			OpenNewsDialog();
		}
		return;
	}

	auto closeText = GetTextIdByName(L"Common0007");

	OpenWebViewDialog(url, il2cpp_string_new(""), closeText);
}

static void Gallop_WebViewManager_OpenGachaDetail_hook(Il2CppObject* self, int gachaId, int stepupId, Il2CppDelegate* onClose)
{
	auto webViewManager = Gallop::WebViewManager(self);

	auto data = il2cpp_object_new(
		il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCommon/Data"));
	il2cpp_runtime_object_init(data);

	data = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * self,
		Il2CppString * headerTextArg,
		Il2CppString * message,
		Il2CppDelegate * onClickCenterButton,
		unsigned long closeTextId, int dialogFormType)>(il2cpp_class_get_method_from_name(data->klass, "SetSimpleOneButtonMessage", 5)->methodPointer)
		(data, LocalizeExtention_Text(GetTextIdByName(L"Gacha0010")), nullptr, onClose, GetTextIdByName(L"Common0007"), 9);

	auto gachaUrl = Gallop::WebViewManager::GetGachaUrl(gachaId);

	webViewManager.Open(gachaUrl, data);
}

static void Gallop_WebViewManager_OpenPurchaseItemDetail_hook(Il2CppObject* self, Il2CppObject* purchaseItem, Il2CppDelegate* onClose)
{
	auto webViewManager = Gallop::WebViewManager(self);

	auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppDelegate*)>(self->klass, "CreatePurchaseItemDetailDialogData", 1)->methodPointer(self, onClose);
	auto productMasterId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(purchaseItem->klass, "get_Id", 0)->methodPointer(purchaseItem);
	auto productUrl = Gallop::WebViewManager::GetProductUrl(productMasterId);

	webViewManager.Open(productUrl, data);
}

static void InitAddress()
{
	Gallop_WebViewManager_cctor_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", ".cctor", 0);
	Gallop_WebViewManager_GetUrl_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetUrl", 1);
	Gallop_WebViewManager_Open_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "Open", 5);
	Gallop_WebViewManager_OpenWebView_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "OpenWebView", 2);
	Gallop_WebViewManager_SetMargin_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "SetMargin", 1);
	Gallop_WebViewManager_SetCustomFont_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "SetCustomFont", 1);
	Gallop_WebViewManager_get_CuteWebView_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "get_CuteWebView", 0);
	Gallop_WebViewManager_SetErrorCallback_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "SetErrorCallback", 0);
	Gallop_WebViewManager_OpenErrorDialog_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "OpenErrorDialog", 0);
	Gallop_WebViewManager_GetGachaUrl_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetGachaUrl", 2);
	Gallop_WebViewManager_GetProductUrl_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetProductUrl", 1);
	Gallop_WebViewManager_SettingUIEffectOnOpen_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "SettingUIEffectOnOpen", 0);
	Gallop_WebViewManager_SettingUIEffectOnClose_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "SettingUIEffectOnClose", 0);
	Gallop_WebViewManager_GetGachaURLProperty_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetGacahURLProperty", 1);
	if (!Gallop_WebViewManager_GetGachaURLProperty_addr)
	{
		Gallop_WebViewManager_GetGachaURLProperty_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetGachaURLProperty", 1);
	}
	Gallop_WebViewManager_GetProductURLProperty_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "GetProductURLProperty", 1);
	Gallop_WebViewManager_TryGetWebViewInfo = il2cpp_symbols::get_method(ASSEMBLY_NAME, "Gallop", "WebViewManager", "TryGetWebViewInfo", 2);
	Gallop_WebViewManager_TryGetWebViewInfo_addr = Gallop_WebViewManager_TryGetWebViewInfo->methodPointer;
	Gallop_WebViewManager__currentWebViewDialog = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"), "_currentWebViewDialog");
	Gallop_WebViewManager__errorCallback = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"), "_errorCallback");
	Gallop_WebViewManager__fontFilePaths = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WebViewManager"), "_fontFilePaths");

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		Gallop_WebViewManager_WebViewInfo = il2cpp_class_from_type(reinterpret_cast<const MethodInfo2020*>(Gallop_WebViewManager_TryGetWebViewInfo)->parameters[1].parameter_type);
	}
	else
	{
		Gallop_WebViewManager_WebViewInfo = il2cpp_class_from_type(Gallop_WebViewManager_TryGetWebViewInfo->parameters[1]);
	}

	Gallop_WebViewManager_WebViewInfo_ctor_addr = il2cpp_class_get_method_from_name(Gallop_WebViewManager_WebViewInfo, ".ctor", 4)->methodPointer;
	Gallop_WebViewManager_WebViewInfo__url = il2cpp_class_get_field_from_name(Gallop_WebViewManager_WebViewInfo, "_url");
	Gallop_WebViewManager_WebViewInfo__isVueJs = il2cpp_class_get_field_from_name(Gallop_WebViewManager_WebViewInfo, "_isVueJs");

	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		Gallop_WebViewManager_OpenGachaDetail_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "OpenGachaDetail", 3);
		Gallop_WebViewManager_OpenPurchaseItemDetail_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "WebViewManager", "OpenPurchaseItemDetail", 2);
		DialogHomeMenuMain_SetupTrainer_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogHomeMenuMain", "SetupTrainer", 1);
		DialogHomeMenuMain_SetupOther_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogHomeMenuMain", "SetupOther", 0);
		DialogHomeMenuSupport_OnSelectMenu_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogHomeMenuSupport", "OnSelectMenu", 1);
		DialogTitleMenu_OnSelectMenu_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogTitleMenu", "OnSelectMenu", 1);

		DialogTitleMenu_OnSelectMenu_KaKaoNotLogin_addr = il2cpp_symbols::find_method(
			ASSEMBLY_NAME, "Gallop", "DialogTitleMenu", [](const MethodInfo* method)
			{
				auto method2020 = reinterpret_cast<const MethodInfo2020*>(method);
				return method2020->name == "OnSelectMenu"s &&
					il2cpp_type_get_name(method2020->parameters->parameter_type) ==
					"Gallop.DialogTitleMenu.KaKaoNotLoginMenu"s;
			});

		DialogTutorialGuide_OnPushHelpButton_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogTutorialGuide", "OnPushHelpButton", 0);
		DialogSingleModeTopMenu_Setup_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "DialogSingleModeTopMenu", "Setup", 0);
		ChampionsInfoWebViewButton_OnClick_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ChampionsInfoWebViewButton", "OnClick", 0);
		StoryEventTopViewController_OnClickHelpButton_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "StoryEventTopViewController", "OnClickHelpButton", 0);
		PartsNewsButton_Setup_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PartsNewsButton", "Setup", 1);
		BannerUI_OnClickBannerItem_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "BannerUI", "OnClickBannerItem", 1);
		KakaoManager_OnKakaoShowInAppWebView_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "KakaoManager", "OnKakaoShowInAppWebView", 2);
	}
}

static void HookMethods()
{
	ADD_HOOK(Gallop_WebViewManager_cctor, "Gallop.WebViewManager::.cctor at %p\n");
	ADD_HOOK(Gallop_WebViewManager_GetUrl, "Gallop.WebViewManager::GetUrl at %p\n");
	ADD_HOOK(Gallop_WebViewManager_Open, "Gallop.WebViewManager::Open at %p\n");
	ADD_HOOK(Gallop_WebViewManager_SetCustomFont, "Gallop.WebViewManager::SetCustomFont at %p\n");
	ADD_HOOK(Gallop_WebViewManager_GetGachaURLProperty, "Gallop.WebViewManager::GetGachaURLProperty at %p\n");
	ADD_HOOK(Gallop_WebViewManager_GetGachaUrl, "Gallop.WebViewManager::GetGachaUrl at %p\n");
	ADD_HOOK(Gallop_WebViewManager_GetProductUrl, "Gallop.WebViewManager::GetProductUrl at %p\n");


	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(Gallop_WebViewManager_OpenGachaDetail, "Gallop.WebViewManager::OpenGachaDetail at %p\n");
		ADD_HOOK(Gallop_WebViewManager_OpenPurchaseItemDetail, "Gallop.WebViewManager::OpenPurchaseItemDetail at %p\n");
		ADD_HOOK(DialogHomeMenuMain_SetupTrainer, "Gallop.DialogHomeMenuMain::SetupTrainer at %p\n");
		ADD_HOOK(DialogHomeMenuMain_SetupOther, "Gallop.DialogHomeMenuMain::SetupOther at %p\n");
		ADD_HOOK(DialogHomeMenuSupport_OnSelectMenu, "Gallop.DialogHomeMenuSupport::OnSelectMenu at %p\n");
		ADD_HOOK(DialogTitleMenu_OnSelectMenu, "Gallop.DialogTitleMenu::OnSelectMenu at %p\n");
		ADD_HOOK(DialogTitleMenu_OnSelectMenu_KaKaoNotLogin, "Gallop.DialogTitleMenu::OnSelectMenu_KaKaoNotLogin at %p\n");
		ADD_HOOK(DialogTutorialGuide_OnPushHelpButton, "Gallop.DialogTutorialGuide::OnPushHelpButton at %p\n");
		ADD_HOOK(DialogSingleModeTopMenu_Setup, "Gallop.DialogSingleModeTopMenu::Setup at %p\n");
		ADD_HOOK(ChampionsInfoWebViewButton_OnClick, "Gallop.ChampionsInfoWebViewButton::OnClick at %p\n");
		ADD_HOOK(StoryEventTopViewController_OnClickHelpButton, "Gallop.StoryEventTopViewController::OnClickHelpButton at %p\n");
		ADD_HOOK(PartsNewsButton_Setup, "Gallop.PartsNewsButton::Setup at %p\n");
		ADD_HOOK(BannerUI_OnClickBannerItem, "Gallop.BannerUI::OnClickBannerItem at %p\n");
		ADD_HOOK(KakaoManager_OnKakaoShowInAppWebView, "KakaoManager::OnKakaoShowInAppWebView at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}


namespace Gallop
{
	const char* WebViewManager::namespaze = "Gallop";
	const char* WebViewManager::klassName = "WebViewManager";

	void WebViewManager::Open(Il2CppString* url, Il2CppObject* dialogData, Il2CppDelegate* onLoadedCallback, Il2CppDelegate* onErrorCallback, bool isDirect)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, Il2CppObject*, Il2CppDelegate*, Il2CppDelegate*, bool)>(Gallop_WebViewManager_Open_addr)(instance, url, dialogData, onLoadedCallback, onErrorCallback, isDirect);
	}

	void WebViewManager::SetErrorCallback()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(Gallop_WebViewManager_SetErrorCallback_addr)(instance);
	}

	void WebViewManager::OpenErrorDialog()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(Gallop_WebViewManager_OpenErrorDialog_addr)(instance);
	}

	void WebViewManager::OpenWebView(Il2CppString* url, Il2CppDelegate* onLoadedCallback)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, Il2CppDelegate*)>(Gallop_WebViewManager_OpenWebView_addr)(instance, url, onLoadedCallback);
	}

	void WebViewManager::SetMargin(UnityEngine::Rect offsetRect)
	{
		reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Rect)>(Gallop_WebViewManager_SetMargin_addr)(instance, offsetRect);
	}

	Il2CppObject* WebViewManager::_currentWebViewDialog()
	{
		Il2CppObject* value;
		il2cpp_field_get_value(instance, Gallop_WebViewManager__currentWebViewDialog, &value);
		return value;
	}

	void WebViewManager::_currentWebViewDialog(Il2CppObject* value)
	{
		il2cpp_field_set_value(instance, Gallop_WebViewManager__currentWebViewDialog, value);
	}

	void WebViewManager::_errorCallback(Il2CppObject* value)
	{
		il2cpp_field_set_value(instance, Gallop_WebViewManager__errorCallback, value);
	}

	Il2CppArraySize_t<Il2CppString*>* WebViewManager::_fontFilePaths()
	{
		Il2CppArraySize_t<Il2CppString*>* value;
		il2cpp_field_get_value(instance, Gallop_WebViewManager__fontFilePaths, &value);
		return value;
	}

	Cute::Core::WebViewManager WebViewManager::CuteWebView()
	{
		return Cute::Core::WebViewManager(reinterpret_cast<Il2CppObject * (*)()>(Gallop_WebViewManager_get_CuteWebView_addr)());
	}

	void WebViewManager::SettingUIEffectOnOpen()
	{
		reinterpret_cast<decltype(SettingUIEffectOnOpen)*>(Gallop_WebViewManager_SettingUIEffectOnOpen_addr)();
	}

	void WebViewManager::SettingUIEffectOnClose()
	{
		reinterpret_cast<decltype(SettingUIEffectOnOpen)*>(Gallop_WebViewManager_SettingUIEffectOnClose_addr)();
	}

	Il2CppString* WebViewManager::GetGachaUrl(int gachaId)
	{
		return reinterpret_cast<decltype(GetGachaUrl)*>(Gallop_WebViewManager_GetGachaUrl_addr)(gachaId);
	}

	Il2CppString* WebViewManager::GetGachaURLProperty(int gachaId)
	{
		return reinterpret_cast<decltype(GetGachaURLProperty)*>(Gallop_WebViewManager_GetGachaURLProperty_addr)(gachaId);
	}

	Il2CppString* WebViewManager::GetProductUrl(int productMasterId)
	{
		return reinterpret_cast<decltype(GetProductUrl)*>(Gallop_WebViewManager_GetProductUrl_addr)(productMasterId);
	}

	Il2CppString* WebViewManager::GetProductURLProperty(int productMasterId)
	{
		return reinterpret_cast<decltype(GetProductURLProperty)*>(Gallop_WebViewManager_GetProductURLProperty_addr)(productMasterId);
	}

	bool WebViewManager::TryGetWebViewInfo(WebViewDefine::Url urlType, WebViewInfo* out)
	{
		Il2CppObject* object;
		auto result = reinterpret_cast<bool (*)(WebViewDefine::Url, Il2CppObject**)>(Gallop_WebViewManager_TryGetWebViewInfo_addr)(urlType, &object);
		*out = WebViewInfo(object);

		return result;
	}

	WebViewManager::WebViewInfo::WebViewInfo(Il2CppString* url, Il2CppString* partsPath, bool isVueJs, WebViewDefine::DmmUrlType urlType) : UnityEngine::Object::Object(instance)
	{
		instance = il2cpp_object_new(Gallop_WebViewManager_WebViewInfo);
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, Il2CppString*, bool, WebViewDefine::DmmUrlType)>(Gallop_WebViewManager_WebViewInfo_ctor_addr)
			(instance, url, partsPath, isVueJs, urlType);
	}

	Il2CppString* WebViewManager::WebViewInfo::_url()
	{
		Il2CppString* value;
		il2cpp_field_get_value(instance, Gallop_WebViewManager_WebViewInfo__url, &value);

		return value;
	}

	bool WebViewManager::WebViewInfo::_isVueJs()
	{
		bool value;
		il2cpp_field_get_value(instance, Gallop_WebViewManager_WebViewInfo__isVueJs, &value);

		return value;
	}
}
