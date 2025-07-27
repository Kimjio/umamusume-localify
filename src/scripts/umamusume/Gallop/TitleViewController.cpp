#include "../../ScriptInternal.hpp"
#include "TitleViewController.hpp"
#include "../../Cute.Cri.Assembly/Cute/Cri/AudioPlayback.hpp"

#include "game.hpp"
#include "settings_text.hpp"

#include <string>

#include <wininet.h>

#include <wrl.h>
#include <wil/com.h>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>

#include "stdinclude.hpp"

#include "taskbar/TaskbarManager.hpp"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

void* TitleViewController_OnClickPushStart_addr = nullptr;
void* TitleViewController_OnClickPushStart_orig = nullptr;

void* TitleViewController_UpdateView_addr = nullptr;
void* TitleViewController_UpdateView_orig = nullptr;

namespace
{
	wil::com_ptr<ICoreWebView2Controller> webviewController;
	wil::com_ptr<ICoreWebView2> webview;
}

bool isLoginWebViewOpen = false;

static string GetGameArgs(wstring sessionId, wstring secureId)
{
	auto hInternet = InternetOpenW(L"DMMGamePlayer5/5.3.22", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

	auto hConnect = InternetConnectW(hInternet, L"apidgp-gameplayer.games.dmm.com", INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, NULL);

	LPCWSTR types[] = { L"application/json", NULL };
	auto hReq = HttpOpenRequestW(hConnect, L"POST", L"/v5/launch/cl", nullptr, nullptr, types, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_COOKIES, NULL);

	wstringstream headerStream;
	headerStream << L"Client-App: DMMGamePlayer5" << endl
		<< L"Client-version: 5.3.22" << endl
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

	wstring loginUrl = L"https://accounts.dmm.com/service/login/password/=/";

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

								if (source == L"https://www.dmm.com/")
								{
									args->put_Cancel(true);

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

											return S_OK;
										}).Get());
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
	il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Cute::Cri::AudioPlayback*, Il2CppObject*, uint64_t, bool, float, Il2CppObject*,
		float, float, float, float, float, float, float, float, float, bool, float, uint64_t, int
		)>(AudioManager->klass, "PlaySe", 17)->methodPointer(&res, AudioManager, 200000000L, 0, 0.0, 0L, 0.0, 10.0, 100.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0, 1.0, 0, INT_MAX);
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

static void InitAddress()
{
	TitleViewController_OnClickPushStart_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "OnClickPushStart", 0);
	TitleViewController_UpdateView_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "Gallop", "TitleViewController", "UpdateView", 0);
}

static void HookMethods()
{
	ADD_HOOK(TitleViewController_OnClickPushStart, "Gallop.TitleViewController::OnClickPushStart at %p\n");
	ADD_HOOK(TitleViewController_UpdateView, "Gallop.TitleViewController::UpdateView at %p\n");
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
