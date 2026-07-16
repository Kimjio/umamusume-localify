#include "../../Cute.Core.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "WebViewManager.hpp"
#include "../../../mscorlib/System/Collections/Generic/Dictionary.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "../../../umamusume/Gallop/Screen.hpp"
#include "../../../umamusume/Gallop/UIManager.hpp"

#ifdef _MSC_VER
#include <WebView2.h>
#include <wrl.h>
#include <wil/com.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <WebView2EnvironmentOptions.h>
#endif

#include <sstream>

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "config/config.hpp"

#include "string_utils.hpp"

#ifdef _MSC_VER
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
#endif

namespace
{
	Il2CppClass* Cute_Core_WebViewManager = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_Awake_addr = nullptr;
	void* Cute_Core_WebViewManager_Awake_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_OpenWeb_addr = nullptr;
	void* Cute_Core_WebViewManager_OpenWeb_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_SetMargins_addr = nullptr;
	void* Cute_Core_WebViewManager_SetMargins_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_EvaluateJS_addr = nullptr;
	void* Cute_Core_WebViewManager_EvaluateJS_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_CanGoBack_addr = nullptr;
	void* Cute_Core_WebViewManager_CanGoBack_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_SetVisible_addr = nullptr;
	void* Cute_Core_WebViewManager_SetVisible_orig = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_get_Callback_addr = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_set_Callback_addr = nullptr;

	Il2CppMethodPointer Cute_Core_WebViewManager_OnLoadedCallback_addr = nullptr;
	void* Cute_Core_WebViewManager_OnLoadedCallback_orig = nullptr;

	FieldInfo* Cute_Core_WebViewManager_marginNow = nullptr;

#ifdef _MSC_VER
	RECT webViewBounds{};
#endif

	il2cppstring CurrentUrlString;
}

#ifdef _MSC_VER
wil::com_ptr<ICoreWebView2> webview;
#endif

static void Cute_Core_WebViewManager_Awake_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(Cute_Core_WebViewManager_Awake_hook)*>(Cute_Core_WebViewManager_Awake_orig)(self);
#ifdef _MSC_VER
	auto path = UnityEngine::Application::persistentDataPath()->chars;

	auto combinedPath = path + il2cppstring(IL2CPP_STRING("\\WebView2"));

	auto envOptions = Make<CoreWebView2EnvironmentOptions>();

	ComPtr<ICoreWebView2EnvironmentOptions4> options4;
	envOptions.As(&options4);

	const wchar_t* allowedOrigins[1] = { L"*" };

	auto customSchemeRegistration = Make<CoreWebView2CustomSchemeRegistration>(L"unity");
	customSchemeRegistration->SetAllowedOrigins(1, allowedOrigins);
	customSchemeRegistration->put_TreatAsSecure(TRUE);
	customSchemeRegistration->put_HasAuthorityComponent(FALSE);

	ICoreWebView2CustomSchemeRegistration* registrations[1] = { customSchemeRegistration.Get() };

	options4->SetCustomSchemeRegistrations(1, registrations);

	CreateCoreWebView2EnvironmentWithOptions(nullptr, combinedPath.data(), envOptions.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{
				env->CreateCoreWebView2Controller(GetHWND(), Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[env](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
					{
						if (controller != nullptr)
						{
							Cute::Core::WebViewManager::webviewController = controller;
							Cute::Core::WebViewManager::webviewController->get_CoreWebView2(&webview);
						}
						else
						{
							return S_OK;
						}

						controller->put_IsVisible(FALSE);

						ICoreWebView2Controller2* webviewController2;
						Cute::Core::WebViewManager::webviewController->QueryInterface(&webviewController2);
						webviewController2->put_DefaultBackgroundColor({ 0xff, 0xf9, 0xf9, 0xf9 });

						ICoreWebView2Settings* settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);
						settings->put_AreDefaultContextMenusEnabled(FALSE);
						settings->put_IsStatusBarEnabled(FALSE);

						if (Game::CurrentGameRegion == Game::Region::KOR)
						{
							ICoreWebView2Settings2* settings2;
							settings->QueryInterface<ICoreWebView2Settings2>(&settings2);

							wil::unique_cotaskmem_string userAgent;
							settings2->get_UserAgent(&userAgent);
							settings2->put_UserAgent(wstring(userAgent.get()).append(L" KakaoGameSDK/99.99.99").data());
						}

						ICoreWebView2Settings4* settings4;
						settings->QueryInterface<ICoreWebView2Settings4>(&settings4);
						settings4->put_IsGeneralAutofillEnabled(FALSE);

						ICoreWebView2Settings6* settings6;
						settings->QueryInterface<ICoreWebView2Settings6>(&settings6);
						settings6->put_IsSwipeNavigationEnabled(FALSE);

						webview->CallDevToolsProtocolMethod(L"Emulation.setEmitTouchEventsForMouse", LR"({"enabled":true})",
							Callback<ICoreWebView2CallDevToolsProtocolMethodCompletedHandler>(
								[](HRESULT errorCode, LPCWSTR result)
								{
									return S_OK;
								}
							).Get());

						ICoreWebView2_4* webview4;
						webview->QueryInterface(&webview4);

						ICoreWebView2_22* webview22;
						webview->QueryInterface(&webview22);

						EventRegistrationToken token;

						webview4->add_FrameCreated(Callback<ICoreWebView2FrameCreatedEventHandler>(
							[](ICoreWebView2* sender, ICoreWebView2FrameCreatedEventArgs* args)
							{
								ICoreWebView2Frame* frame;
								args->get_Frame(&frame);

								ICoreWebView2Frame2* frame2;
								frame->QueryInterface(&frame2);

								EventRegistrationToken token;
								frame2->add_NavigationStarting(Callback<ICoreWebView2FrameNavigationStartingEventHandler>(
									[](ICoreWebView2Frame* sender, ICoreWebView2NavigationStartingEventArgs* args)
									{
										wil::unique_cotaskmem_string uri;
										args->get_Uri(&uri);

										wstring wuri = uri.get();
										if (wuri.starts_with(L"unity:"))
										{
											auto callback = Cute::Core::WebViewManager::Instance().Callback();

											if (callback)
											{
												reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(
													callback->method_ptr
													)(callback->target, il2cpp_string_new16(wuri.substr(6).data()));
											}
										}

										return S_OK;
									}
								).Get(), &token);

								return S_OK;
							}
						).Get(), &token);

						webview22->AddWebResourceRequestedFilterWithRequestSourceKinds(
							L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_FONT,
							COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_DOCUMENT);

						webview22->AddWebResourceRequestedFilterWithRequestSourceKinds(
							L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE,
							COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_DOCUMENT);

						webview22->AddWebResourceRequestedFilterWithRequestSourceKinds(
							L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT,
							COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_DOCUMENT);

						webview22->AddWebResourceRequestedFilterWithRequestSourceKinds(
							L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_XML_HTTP_REQUEST,
							COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_DOCUMENT);

						webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
							{
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);

								BOOL isVisible;
								Cute::Core::WebViewManager::webviewController->get_IsVisible(&isVisible);

								if (isVisible && uri.get() == L"about:blank"s)
								{
									args->put_Cancel(TRUE);
									return S_OK;
								}

								if (uri.get() != L"about:blank"s)
								{
									if (CurrentUrlString != uri.get())
									{
										auto callback = Cute::Core::WebViewManager::Instance().Callback();

										if (callback)
										{
											reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(
												callback->method_ptr
												)(callback->target, il2cpp_string_new("showBackButton"));
										}
									}
								}

								return S_OK;
							}
						).Get(), &token);

						webview22->add_DOMContentLoaded(Callback<ICoreWebView2DOMContentLoadedEventHandler>(
							[](ICoreWebView2* sender, ICoreWebView2DOMContentLoadedEventArgs* args)
							{
								auto script = LR"(
const style = document.createElement('style');
style.type = 'text/css';
style.innerHTML = '::-webkit-scrollbar { display: none; }';
document.head.appendChild(style);
)";

								webview->ExecuteScript(script, Callback<ICoreWebView2ExecuteScriptCompletedHandler>([](HRESULT errorCode, LPCWSTR result) { return S_OK; }).Get());

								auto serverUrl = GetApplicationServerUrl()->chars;

								wil::unique_cotaskmem_string uri;
								sender->get_Source(&uri);
								if (!il2cppstring(uri.get()).starts_with(serverUrl))
								{
									auto script1 = LR"(
window.Unity = {
	call: function(t) {
        const e = document.createElement("IFRAME");
        e.setAttribute("src", "unity:" + t);
        document.documentElement.appendChild(e);
		e.parentNode.removeChild(e);
	}
};
window.onclick = () => { Unity.call('snd_sfx_UI_decide_m_01'); };
const zoom = (window.innerWidth || window.screen.width) / 528;
let { viewport } = document.head.getElementsByTagName('meta');
if (!viewport) {
    viewport = document.createElement('meta');
    viewport.name = 'viewport';
    document.head.appendChild(viewport);
}
viewport.content = `width=device-width, initial-scale=${zoom}, user-scalable=no`;
)";

									webview->ExecuteScript(script1, Callback<ICoreWebView2ExecuteScriptCompletedHandler>([](HRESULT errorCode, LPCWSTR result) { return S_OK; }).Get());
								}

								return S_OK;
							}
						).Get(), &token);

						webview->AddScriptToExecuteOnDocumentCreated(LR"(
Object.defineProperty(CSSStyleDeclaration.prototype, 'zoom', {
		get: function() { return this.getPropertyValue('zoom'); },
		set: function(val) {
			if (this.parentRule?.parentStyleSheet?.ownerNode === document.documentElement ||
				this === document.documentElement?.style) {
				this.setProperty('zoom', (window.innerWidth || window.screen.width) / (window.document.body.offsetWidth || 1));
			} else {
				this.setProperty('zoom', val);
			}
		},
		configurable: true,
		enumerable: true
	}
);
document.documentElement.style.zoom = 1;
)", nullptr);

						webview->add_WebResourceRequested(Callback<ICoreWebView2WebResourceRequestedEventHandler>(
							[env](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args)
							{
								COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;
								args->get_ResourceContext(&resourceContext);

								ICoreWebView2WebResourceRequest* req;
								args->get_Request(&req);

								wil::unique_cotaskmem_string uri;
								req->get_Uri(&uri);

								if (resourceContext == COREWEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT)
								{
									wstring wuri = uri.get();
									if (wuri.starts_with(L"unity:"))
									{
										auto callback = Cute::Core::WebViewManager::Instance().Callback();

										if (callback)
										{
											reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(
												callback->method_ptr
												)(callback->target, il2cpp_string_new16(wuri.substr(6).data()));
										}
										return E_INVALIDARG;
									}
								}

								if (resourceContext == COREWEBVIEW2_WEB_RESOURCE_CONTEXT_FONT)
								{
									wstringstream pathStream(uri.get());
									wstring segment;
									vector<wstring> splited;
									while (getline(pathStream, segment, L'/'))
									{
										splited.emplace_back(segment);
									}
									il2cppstring fileName = splited.back();

									if (!config::web_font_path.empty() && filesystem::exists(config::web_font_path))
									{
										IStream* stream;
										SHCreateStreamOnFileEx(config::web_font_path.data(), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &stream);

										ICoreWebView2WebResourceResponse* response;
										env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: font/otf", &response);
										args->put_Response(response);

										return S_OK;

									}

									if (Cute::Core::WebViewManager::customFontMap.contains(fileName))
									{
										IStream* stream;
										SHCreateStreamOnFileEx(Cute::Core::WebViewManager::customFontMap.at(fileName).data(), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &stream);

										ICoreWebView2WebResourceResponse* response;
										env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: font/otf", &response);
										args->put_Response(response);

										return S_OK;
									}
								}

								if (resourceContext == COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE)
								{
									if (wstring(uri.get()).find(L"v_sprites/icon/sprite.png") != il2cppstring::npos)
									{
										wstringstream pathStream(uri.get());
										wstring segment;
										vector<wstring> splited;
										while (getline(pathStream, segment, L'='))
										{
											splited.emplace_back(segment);
										}

										wstring version = splited.back();

										if (!config::web_icon_sprite_path.empty() &&
											filesystem::exists(config::web_icon_sprite_path))
										{
											if (!config::web_icon_sprite_version.empty() && version != config::web_icon_sprite_version)
											{
												return E_INVALIDARG;
											}

											IStream* stream;
											SHCreateStreamOnFileEx(config::web_icon_sprite_path.data(), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &stream);

											ICoreWebView2WebResourceResponse* response;
											env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: image/png", &response);
											args->put_Response(response);

											return S_OK;
										}
									}
								}

								if (resourceContext == COREWEBVIEW2_WEB_RESOURCE_CONTEXT_XML_HTTP_REQUEST)
								{
									if (wstring(uri.get()).find(L"/ajax/faq_index") != wstring::npos)
									{
										if (!config::faq_index.IsArray())
										{
											return E_INVALIDARG;
										}

										rapidjson::StringBuffer buffer;
										buffer.Clear();
										rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
										config::faq_index.Accept(writer);

										string responseJson = R"({"response_code": 1,"faq_list":)"s + buffer.GetString() + "}";

										IStream* stream;
										stream = SHCreateMemStream(reinterpret_cast<const BYTE*>(responseJson.data()), static_cast<UINT>(responseJson.size()));

										ICoreWebView2WebResourceResponse* response;
										env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: application/json", &response);
										args->put_Response(response);

										return S_OK;
									}

									if (wstring(uri.get()).find(L"/ajax/glossary_index") != wstring::npos)
									{
										if (!config::glossary_index.IsArray())
										{
											return E_INVALIDARG;
										}

										rapidjson::StringBuffer buffer;
										buffer.Clear();
										rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
										config::glossary_index.Accept(writer);

										string responseJson = R"({"response_code": 1,"glossary_list":)"s + buffer.GetString() + "}";

										IStream* stream;
										stream = SHCreateMemStream(reinterpret_cast<const BYTE*>(responseJson.data()), static_cast<UINT>(responseJson.size()));

										ICoreWebView2WebResourceResponse* response;
										env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: application/json", &response);
										args->put_Response(response);

										return S_OK;
									}

									if (wstring(uri.get()).find(L"/ajax/glossary_search") != wstring::npos)
									{
										if (!config::glossary_index.IsArray())
										{
											return E_INVALIDARG;
										}

										IStream* requestStream;
										req->get_Content(&requestStream);
										STATSTG stat;
										requestStream->Stat(&stat, STATFLAG_DEFAULT);
										requestStream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);

										string streamData;
										streamData.resize(stat.cbSize.QuadPart);

										requestStream->Read(streamData.data(), stat.cbSize.QuadPart, nullptr);

										rapidjson::Document requestParams;
										requestParams.Parse(streamData.data());

										if (requestParams.HasParseError() || !requestParams.HasMember("keyword"))
										{
											return E_INVALIDARG;
										}

										string keyword = requestParams["keyword"].GetString();

										rapidjson::Document searchResult;
										searchResult.SetArray();

										auto groupArray = config::glossary_index.GetArray();

										for (auto& group : groupArray)
										{
											if (!group.HasMember("pages"))
											{
												continue;
											}

											auto itemArray = group["pages"].GetArray();
											for (auto& item : itemArray)
											{
												if (!item.HasMember("title") || !item.HasMember("description"))
												{
													continue;
												}
												auto title = item["title"].GetString();
												auto description = item["description"].GetString();
												if (string(title).find(keyword) != string::npos || string(description).find(keyword) != string::npos)
												{
													rapidjson::Value item(rapidjson::kObjectType);
													item.AddMember("title", rapidjson::Value(title, searchResult.GetAllocator()), searchResult.GetAllocator());
													item.AddMember("description", rapidjson::Value(description, searchResult.GetAllocator()), searchResult.GetAllocator());
													searchResult.PushBack(item, searchResult.GetAllocator());
												}
											}
										}

										rapidjson::StringBuffer buffer;
										buffer.Clear();
										rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
										searchResult.Accept(writer);

										string responseJson = R"({"response_code": 1,"glossary_list":)"s + buffer.GetString() + "}";

										IStream* stream;
										stream = SHCreateMemStream(reinterpret_cast<const BYTE*>(responseJson.data()), static_cast<UINT>(responseJson.size()));

										ICoreWebView2WebResourceResponse* response;
										env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: application/json", &response);
										args->put_Response(response);

										return S_OK;
									}
								}

								return E_INVALIDARG;
							}
						).Get(), &token);
#ifdef _DEBUG
						webview->OpenDevToolsWindow();
#endif
						return S_OK;
					}
				).Get());
				return S_OK;
			}
		).Get());
#endif
}

static void Cute_Core_WebViewManager_OpenWeb_hook(Il2CppObject* self, Il2CppString* url)
{
#ifdef _MSC_VER
	CurrentUrlString = url->chars;
	if (webview)
	{
		webview->Navigate(CurrentUrlString.data());
	}
	Cute::Core::WebViewManager(self).SetVisible(true);
#endif
}

static void Cute_Core_WebViewManager_SetMargins_hook(Il2CppObject* self, int leftMargin, int topMargin, int rightMargin, int bottomMargin)
{
	reinterpret_cast<decltype(Cute_Core_WebViewManager_SetMargins_hook)*>(Cute_Core_WebViewManager_SetMargins_orig)(self, leftMargin, topMargin, rightMargin, bottomMargin);
#ifdef _MSC_VER
	GetClientRect(GetHWND(), &webViewBounds);

	float scale = 1;

	if (!config::freeform_window)
	{
		if (!Gallop::Screen::IsSplitWindow())
		{
			scale = Gallop::Screen::OriginalScreenWidth() / static_cast<float>(UnityEngine::Screen::width());
		}
	}

	webViewBounds.left += static_cast<LONG>(ceilf(leftMargin / scale));
	webViewBounds.top += static_cast<LONG>(ceilf(topMargin / scale));
	webViewBounds.right -= static_cast<LONG>(ceilf(rightMargin / scale));
	webViewBounds.bottom -= static_cast<LONG>(ceilf(bottomMargin / scale));

	if (Cute::Core::WebViewManager::webviewController)
	{
		Cute::Core::WebViewManager::webviewController->put_Bounds(webViewBounds);

		ICoreWebView2* webview;
		Cute::Core::WebViewManager::webviewController->get_CoreWebView2(&webview);

		webview->ExecuteScript(L"document.documentElement.style.zoom = (window.innerWidth || window.screen.width) / 528", Callback<ICoreWebView2ExecuteScriptCompletedHandler>([](HRESULT errorCode, LPCWSTR result) { return S_OK; }).Get());
	}
#endif
}

static void Cute_Core_WebViewManager_SetVisible_hook(Il2CppObject* self, bool visible)
{
	reinterpret_cast<decltype(Cute_Core_WebViewManager_SetVisible_hook)*>(Cute_Core_WebViewManager_SetVisible_orig)(self, visible);
#ifdef _MSC_VER
	if (Cute::Core::WebViewManager::webviewController)
	{
		Cute::Core::WebViewManager::webviewController->put_IsVisible(visible);
	}
#endif
}

static void Cute_Core_WebViewManager_OnLoadedCallback_hook(Il2CppObject* self, Il2CppString* msg)
{
	auto serverUrl = GetApplicationServerUrl();
	if (msg && serverUrl && il2cppstring(msg->chars).find(serverUrl->chars) == il2cppstring::npos)
	{
		const auto WebViewInitScript = R"(
window.onclick = () => { Unity.call('snd_sfx_UI_decide_m_01'); };
window.zoomScale = (window.innerWidth || window.screen.width) / 528;
let { viewport } = document.head.getElementsByTagName('meta');
if (!viewport) {
    viewport = document.createElement('meta');
    viewport.name = 'viewport';
    document.head.appendChild(viewport);
}
viewport.content = `width=device-width, initial-scale=${window.zoomScale}, user-scalable=no`;
)";
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppString*)>(self->klass, "EvaluateJS", 1)(self, il2cpp_string_new(WebViewInitScript));
	}
	reinterpret_cast<decltype(Cute_Core_WebViewManager_OnLoadedCallback_hook)*>(Cute_Core_WebViewManager_OnLoadedCallback_orig)(self, msg);
}

static void InitAddress()
{
	Cute_Core_WebViewManager = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Cute.Core", "WebViewManager");
	Cute_Core_WebViewManager_Awake_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "Awake", 0);
	Cute_Core_WebViewManager_OpenWeb_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "OpenWeb", 1);
	Cute_Core_WebViewManager_SetMargins_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "SetMargins", 4);
	Cute_Core_WebViewManager_SetVisible_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "SetVisible", 1);
	Cute_Core_WebViewManager_get_Callback_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "get_Callback", 0);
	Cute_Core_WebViewManager_set_Callback_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "set_Callback", 1);
	Cute_Core_WebViewManager_OnLoadedCallback_addr = il2cpp_symbols::get_method_pointer(Cute_Core_WebViewManager, "OnLoadedCallback", 1);
	Cute_Core_WebViewManager_marginNow = il2cpp_class_get_field_from_name(Cute_Core_WebViewManager, "marginNow");
}

static void HookMethods()
{
#ifdef _MSC_VER
	ADD_HOOK(Cute_Core_WebViewManager_Awake, "Cute.Core.WebViewManager::Awake at %p\n");
	ADD_HOOK(Cute_Core_WebViewManager_OpenWeb, "Cute.Core.WebViewManager::OpenWeb at %p\n");
	ADD_HOOK(Cute_Core_WebViewManager_SetMargins, "Cute.Core.WebViewManager::SetMargins at %p\n");
	ADD_HOOK(Cute_Core_WebViewManager_SetVisible, "Cute.Core.WebViewManager::SetVisible at %p\n");
#else
	ADD_HOOK(Cute_Core_WebViewManager_OnLoadedCallback, "Cute.Core.WebViewManager::OnLoadedCallback at %p\n");
#endif
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Cute::Core
{
#ifdef _MSC_VER
	wil::com_ptr<ICoreWebView2Controller> WebViewManager::webviewController;

	unordered_map<il2cppstring, il2cppstring> WebViewManager::customFontMap;
#endif

	WebViewManager WebViewManager::Instance()
	{
		return WebViewManager(GetSingletonInstance(Cute_Core_WebViewManager));
	}

	UnityEngine::Vector4 WebViewManager::marginNow()
	{
		UnityEngine::Vector4 value;
		il2cpp_field_get_value(instance, Cute_Core_WebViewManager_marginNow, &value);

		return value;
	}

	void WebViewManager::OpenWeb(Il2CppString* url)
	{
		reinterpret_cast<decltype(Cute_Core_WebViewManager_OpenWeb_hook)*>(Cute_Core_WebViewManager_OpenWeb_addr)(instance, url);
	}

	void WebViewManager::SetVisible(bool visible)
	{
		reinterpret_cast<decltype(Cute_Core_WebViewManager_SetVisible_hook)*>(Cute_Core_WebViewManager_SetVisible_addr)(instance, visible);
	}

	void WebViewManager::SetMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin)
	{
		reinterpret_cast<decltype(Cute_Core_WebViewManager_SetMargins_hook)*>(Cute_Core_WebViewManager_SetMargins_addr)(instance, leftMargin, topMargin, rightMargin, bottomMargin);
	}

	void WebViewManager::GoBack()
	{
#ifdef _MSC_VER
		if (webview)
		{
			webview->GoBack();
		}
#endif
	}

	Il2CppDelegate* WebViewManager::Callback()
	{
		return reinterpret_cast<Il2CppDelegate * (*)(Il2CppObject*)>(Cute_Core_WebViewManager_get_Callback_addr)(instance);
	}

	void WebViewManager::Callback(Il2CppDelegate* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*)>(Cute_Core_WebViewManager_set_Callback_addr)(instance, value);
	}
}
