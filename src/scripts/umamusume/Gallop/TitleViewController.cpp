#include "../../ScriptInternal.hpp"
#include "TitleViewController.hpp"
#include "DialogCommon.hpp"
#include "DialogManager.hpp"
#include "Screen.hpp"
#include "UIManager.hpp"
#include "../../Cute.Cri.Assembly/Cute/Cri/AudioPlayback.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "../../Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredInt.hpp"
#include "../../Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredLong.hpp"

#include "game.hpp"
#include "settings_text.hpp"

#include <string>

#include <wininet.h>

#include <wrl.h>
#include <wil/com.h>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "stdinclude.hpp"

#include "taskbar/TaskbarManager.hpp"

#include "string_utils.hpp"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

namespace
{
	void* TitleViewController_OnClickPushStart_addr = nullptr;
	void* TitleViewController_OnClickPushStart_orig = nullptr;

	void* TitleViewController_UpdateView_addr = nullptr;
	void* TitleViewController_UpdateView_orig = nullptr;

	void* TitleViewController_InitializeView_addr = nullptr;
	void* TitleViewController_InitializeView_orig = nullptr;

	wil::com_ptr<ICoreWebView2Controller> webviewController;
	wil::com_ptr<ICoreWebView2> webview;

	bool isLoginWebViewOpen = false;

	constexpr auto DMM_UA = L"DMMGamePlayer5-Win/5.4.17";
	constexpr auto DMM_API = L"apidgp-gameplayer.games.dmm.com";
	constexpr auto BASE_HEADER = L"Client-App: DMMGamePlayer5\nClient-version: 5.4.17";
	const wchar_t* ACCEPT_TYPE[] = {L"application/json", nullptr};
}

static void PrintParseError(rapidjson::Document& document)
{
	cout << "Response JSON parse error: " << GetParseError_En(document.GetParseError()) << " (" << document.GetErrorOffset() << ")" << endl;
}

static void ShowErrorDialog(string error)
{
	auto t = il2cpp_thread_attach(il2cpp_domain_get());

	auto dialogData = Gallop::DialogCommon::Data();
	dialogData.SetSimpleOneButtonMessage(GetTextIdByName(L"Error0014"), il2cpp_string_new(error.data()), nullptr, GetTextIdByName(L"Common0007"));

	Gallop::DialogManager::PushDialog(dialogData);

	il2cpp_thread_detach(t);
}

static string GetLoginUrl()
{
	auto hInternet = InternetOpenW(DMM_UA, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

	auto hConnect = InternetConnectW(hInternet, DMM_API, INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

	auto hReq = HttpOpenRequestW(hConnect, L"POST", L"/v5/auth/login/url", nullptr, nullptr, ACCEPT_TYPE, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

	auto body = R"({"prompt":""})"s;
	auto res = HttpSendRequestW(hReq, BASE_HEADER, 0, body.data(), body.size());

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
		PrintParseError(document);
		return "";
	}

	if (document.HasMember("result_code") && document["result_code"].GetInt() == 100)
	{
		return string(document["data"].GetObjectW()["url"].GetString());
	}
	else if (document.HasMember("error") && document["error"].IsString())
	{
		ShowErrorDialog(document["error"].GetString());
	}

	return "";
}

static string GetAccessToken(wstring code)
{
	auto hInternet = InternetOpenW(DMM_UA, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

	auto hConnect = InternetConnectW(hInternet, DMM_API, INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

	auto hReq = HttpOpenRequestW(hConnect, L"POST", L"/v5/auth/accesstoken/issue", nullptr, nullptr, ACCEPT_TYPE, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

	auto body = R"({"code":")"s + wide_u8(code) + R"("})"s;
	auto res = HttpSendRequestW(hReq, BASE_HEADER, 0, body.data(), body.size());

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
		PrintParseError(document);
		return "";
	}

	if (document.HasMember("result_code") && document["result_code"].GetInt() == 100)
	{
		return string(document["data"].GetObjectW()["access_token"].GetString());
	}
	else if (document.HasMember("error") && document["error"].IsString())
	{
		ShowErrorDialog(document["error"].GetString());
	}

	return "";
}

static string GetGameArgs(wstring actAuth)
{
	auto hInternet = InternetOpenW(DMM_UA, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

	auto hConnect = InternetConnectW(hInternet, DMM_API, INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

	auto hReq = HttpOpenRequestW(hConnect, L"POST", L"/v5/r2/launch/cl", nullptr, nullptr, ACCEPT_TYPE, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

	wstringstream headerStream;
	headerStream << BASE_HEADER << endl
		<< L"ActAuth: " << actAuth << endl;

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
		PrintParseError(document);
		return "";
	}

	if (document.HasMember("result_code") && document["result_code"].GetInt() == 100)
	{
		return string(document["data"].GetObjectW()["execute_args"].GetString());
	}
	else if (document.HasMember("error") && document["error"].IsString())
	{
		ShowErrorDialog(document["error"].GetString());
	}

	return "";
}

static LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

static DWORD WINAPI WebViewThread(LPVOID)
{
	wstring loginUrl = u8_wide(GetLoginUrl());

	if (loginUrl.empty())
	{
		return 0;
	}

	IsGUIThread(TRUE);

	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WebViewWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hIcon = LoadIconW(wcex.hInstance, reinterpret_cast<LPWSTR>(IDI_APP_ICON));
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
		nullptr, NULL, wcex.hInstance, NULL);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	auto envOptions = Make<CoreWebView2EnvironmentOptions>();
#ifdef _DEBUG
	envOptions->put_AdditionalBrowserArguments(L"--enable-logging --v=1");
#endif

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

						ICoreWebView2Settings4* settings4;
						settings->QueryInterface<ICoreWebView2Settings4>(&settings4);
						settings4->put_IsGeneralAutofillEnabled(FALSE);

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
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);
								wstring source(uri.get());

								if (source.starts_with(L"https://webdgp-gameplayer.games.dmm.com/login/success"))
								{
									args->put_Cancel(true);

									wstringstream pathStream(source);
									wstring segment;
									vector<wstring> splited;
									while (getline(pathStream, segment, L'='))
									{
										splited.emplace_back(segment);
									}

									ICoreWebView2_2* webView2;
									webview->QueryInterface<ICoreWebView2_2>(&webView2);

									auto actAuth = GetAccessToken(splited.back());

									auto gameArgs = GetGameArgs(u8_wide(actAuth));

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

										Gallop::TitleViewController::viewerId = string(split1.back());

										split1.clear();

										stringstream singleArgStream2(split[1]);
										while (getline(singleArgStream2, segment, '='))
										{
											split1.emplace_back(segment);
										}

										Gallop::TitleViewController::onetimeToken = string(split1.back());

										auto t = il2cpp_thread_attach(il2cpp_domain_get());

										il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmViewerId", 1)(il2cpp_string_new(Gallop::TitleViewController::viewerId.data()));
										il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("umamusume.dll", "Gallop", "Certification", "set_dmmOnetimeToken", 1)(il2cpp_string_new(Gallop::TitleViewController::onetimeToken.data()));

										isLoginWebViewOpen = false;

										auto viewController = GetCurrentViewController();
										il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(viewController->klass, "OnClickPushStart", 0)->methodPointer(viewController);

										il2cpp_thread_detach(t);
									}

									webviewController->Close();
									PostMessageW(hWnd, WM_CLOSE, NULL, NULL);
								}

								if (source.substr(0, 5) != L"https")
								{
									args->put_Cancel(true);
								}
								return S_OK;
							}
						).Get(), &token);
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

static void PlaySfxUiDecideL01()
{
	const auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));

	Cute::Cri::AudioPlayback res{};

	if (auto legacy = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Cute::Cri::AudioPlayback*, Il2CppObject*, uint64_t, bool, float, Il2CppObject*,
		float, float, float, float, float, float, bool, float, uint64_t, int
		)>(AudioManager->klass, "PlaySe", 14))
	{
		legacy->methodPointer(&res, AudioManager, 200000000L, false, 0.0, nullptr, 0.0, 10.0, 100.0, 0.0, 0.0, 1.0, false, 1.0, 0, INT_MAX);
		return;
	}

	il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Cute::Cri::AudioPlayback*, Il2CppObject*, uint64_t, bool, float, Il2CppObject*,
		float, float, float, float, float, float, float, float, float, bool, float, uint64_t, int
		)>(AudioManager->klass, "PlaySe", 17)->methodPointer(&res, AudioManager, 200000000L, false, 0.0, nullptr, 0.0, 10.0, 100.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, false, 1.0, 0, INT_MAX);
}

static void TitleViewController_OnClickPushStart_hook(Il2CppObject* self)
{
	const auto dmmId = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("umamusume.dll", "Gallop", "Certification", "get_dmmViewerId", IgnoreNumberOfArguments)();
	const auto dmmOnetimeToken = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("umamusume.dll", "Gallop", "Certification", "get_dmmOnetimeToken", IgnoreNumberOfArguments)();

	if ((dmmId && !wstring(dmmId->chars).empty() &&
		dmmOnetimeToken && !wstring(dmmOnetimeToken->chars).empty()) ||
		Game::CurrentGameRegion != Game::Region::JPN ||
		Game::CurrentGameStore == Game::Store::Steam)
	{
		auto readDisclaimer = il2cpp_symbols::get_method_pointer<int (*)(Il2CppString*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "GetInt", 2)(il2cpp_string_new("ReadDisclaimer"), 0);

		if (!readDisclaimer)
		{
			PlaySfxUiDecideL01();

			auto localizeextension_text = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int id)>(
				"umamusume.dll", "Gallop", "LocalizeExtention", "Text", 1
			);

			if (il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogSimpleCheckNoWarning"))
			{
				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppString*, Il2CppString*, Il2CppString*, uint64_t, Il2CppDelegate*, Il2CppDelegate*, bool, bool, Il2CppString*)>
					("umamusume.dll", "Gallop", "DialogSimpleCheckNoWarning", "OpenMiddleOneButton", 9)(localizeextension_text(GetTextIdByName(L"Common0081")), il2cpp_string_new16(
						(LocalifySettings::GetText("initial_disclaimer_1") + wstring(localizeextension_text(GetTextIdByName(L"Common187002"))->chars) + LocalifySettings::GetText("initial_disclaimer_2")).data()),
						localizeextension_text(GetTextIdByName(L"Common187002")), GetTextIdByName(L"Common0007"),
						CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
							{
								il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "DialogCommon", "Close", 0)(GetFrontDialog());
								il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "SetInt", 2)(il2cpp_string_new("ReadDisclaimer"), 1);
								il2cpp_symbols::get_method_pointer<void (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "Save", IgnoreNumberOfArguments)();
							}), nullptr, false, true, nullptr);
			}
			else
			{
				auto dialogData = Gallop::DialogCommon::Data();
				dialogData.SetSimpleOneButtonMessage(
					GetTextIdByName(L"Common0081"),
					il2cpp_string_new16(
						(LocalifySettings::GetText("initial_disclaimer_1") + wstring(localizeextension_text(GetTextIdByName(L"Common0150"))->chars) + LocalifySettings::GetText("initial_disclaimer_2")).data()),
					CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
						{
							Gallop::DialogCommon(GetFrontDialog()).Close();
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "SetInt", 2)(il2cpp_string_new("ReadDisclaimer"), 1);
							il2cpp_symbols::get_method_pointer<void (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "PlayerPrefs", "Save", IgnoreNumberOfArguments)();
						}),
					GetTextIdByName(L"Common0150")
				);

				dialogData.AutoClose(false);

				Gallop::DialogManager::PushDialog(dialogData);
			}
			return;
		}

		reinterpret_cast<decltype(TitleViewController_OnClickPushStart_hook)*>(TitleViewController_OnClickPushStart_orig)(self);
	}
	else
	{
		PlaySfxUiDecideL01();

		if (!isLoginWebViewOpen)
		{
			isLoginWebViewOpen = true;

			CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(WebViewThread), NULL, NULL, NULL);
		}

	}
}

static void TitleViewController_UpdateView_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(TitleViewController_UpdateView_hook)*>(TitleViewController_UpdateView_orig)(self);
	auto bootProgress = il2cpp_symbols::get_method_pointer<float (*)()>("umamusume.dll", "Gallop", "MainGameInitializer", "GetBootProgress", 0)();

	if (config::taskbar_show_progress_on_download)
	{
		if (bootProgress > -1)
		{
			if (bootProgress == 1)
			{
				TaskbarManager::SetProgressState(TBPF_NOPROGRESS);
			}
			else
			{
				TaskbarManager::SetProgressValue(bootProgress * 100, 100);
			}
		}
	}
}

static Il2CppObject* TitleViewController_InitializeView_hook(Il2CppObject* self)
{
	auto res = reinterpret_cast<decltype(TitleViewController_InitializeView_hook)*>(TitleViewController_InitializeView_orig)(self);

	if (!Gallop::Screen::IsLandscapeMode())
	{
		return res;
	}

	auto viewBase = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(self->klass, "GetViewBase", 0)->methodPointer(self);
	auto TitleLogoTransformField = il2cpp_class_get_field_from_name_wrap(viewBase->klass, "TitleLogoTransform");
	Il2CppObject* TitleLogoTransform;
	il2cpp_field_get_value(viewBase, TitleLogoTransformField, &TitleLogoTransform);

	auto ProgressRootObjectField = il2cpp_class_get_field_from_name(viewBase->klass, "ProgressRootObject");
	Il2CppObject* ProgressRootObject;
	il2cpp_field_get_value(viewBase, ProgressRootObjectField, &ProgressRootObject);

	auto StartTapObiectField = il2cpp_class_get_field_from_name(viewBase->klass, "StartTapObiect");
	Il2CppObject* StartTapObiect;
	il2cpp_field_get_value(viewBase, StartTapObiectField, &StartTapObiect);

	auto transform = UnityEngine::RectTransform(TitleLogoTransform);
	auto sizeDelta = transform.sizeDelta();
	auto anchoredPosition = transform.anchoredPosition();
	Gallop::UIManager::Instance().StartCoroutineManaged2(res);

	UnityEngine::GameObject(ProgressRootObject).transform().localScale({ 1.0f, 1.0f, 1.0f });
	UnityEngine::GameObject(StartTapObiect).transform().localScale({ 1.0f, 1.0f, 1.0f });

	auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SaveDataManager"));
	auto SaveLoader = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)->methodPointer(SaveDataManager);
	auto CampaignTitleLogoChangeId = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredInt(*)(Il2CppObject*)>(SaveLoader->klass, "get_CampaignTitleLogoChangeId", 0)->methodPointer(SaveLoader);

	if (CampaignTitleLogoChangeId.GetDecrypted() > 0)
	{
		if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			transform.sizeDelta(Vector2{ 1200.0f, 600.0f });
			transform.anchoredPosition(Vector2{ -56.0f, 410.0f });
		}
		else
		{
			transform.sizeDelta(Vector2{ 744.0f, 632.0f });
			transform.anchoredPosition(Vector2{ -14.0f, 610.0f });
		}
	}
	else
	{
		if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			transform.sizeDelta(Vector2{ 1440.0f, 360.0f });
			transform.anchoredPosition(Vector2{ 0.0f, 400.0f });
		}
		else
		{
			transform.sizeDelta(sizeDelta);
			transform.anchoredPosition(anchoredPosition);
		}
	}

	return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(nullptr);
}

static void InitAddress()
{
	TitleViewController_OnClickPushStart_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "OnClickPushStart", 0);
	TitleViewController_UpdateView_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "UpdateView", 0);
	TitleViewController_InitializeView_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "InitializeView", 0);
}

static void HookMethods()
{
	ADD_HOOK(TitleViewController_OnClickPushStart, "Gallop.TitleViewController::OnClickPushStart at %p\n");
	ADD_HOOK(TitleViewController_UpdateView, "Gallop.TitleViewController::UpdateView at %p\n");

	if (config::freeform_window)
	{
		ADD_HOOK(TitleViewController_InitializeView, "Gallop.TitleViewController::InitializeView at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	string TitleViewController::viewerId;
	string TitleViewController::onetimeToken;
}
