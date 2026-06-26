#include "Cute.Core.Assembly.hpp"
#include "scripts/ScriptInternal.hpp"
#include "WebViewObject.hpp"

#ifdef __ANDROID__
#include <bits/get_device_api_level_inlines.h>

namespace
{
    Il2CppMethodPointer WebViewObject_Init_addr = nullptr;
    void* WebViewObject_Init_orig = nullptr;
}

static void WebViewObject_Init_hook(Il2CppObject* self, Il2CppDelegate* cb, Il2CppDelegate* err, Il2CppDelegate* httpErr, Il2CppDelegate* ld, Il2CppDelegate* started, Il2CppDelegate* hooked, Il2CppDelegate* cookies, bool transparent, bool zoom, Il2CppString* ua, int radius, int androidForceDarkMode, bool enableWKWebView, int wkContentMode, bool wkAllowsLinkPreview, bool wkAllowsBackForwardNavigationGestures, bool separated) {
    string uaU8;
    if (ua) {
        uaU8 = il2cpp_u8(ua->chars);
    }
    uaU8.append(" Android ").append(to_string(android_get_device_api_level())).append(" KakaoGameSDK/99.99.99");
    reinterpret_cast<decltype(WebViewObject_Init_hook) *>(WebViewObject_Init_orig)(self, cb, err, httpErr, ld, started, hooked, cookies, transparent, zoom, il2cpp_string_new(uaU8.data()), radius, androidForceDarkMode, enableWKWebView, wkContentMode, wkAllowsLinkPreview, wkAllowsBackForwardNavigationGestures, separated);
}


static void InitAddress()
{
    WebViewObject_Init_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "WebViewObject", "Init", 17);
}

static void HookMethods()
{
    if (Game::CurrentGameRegion == Game::Region::KOR)
    {
        ADD_HOOK(WebViewObject_Init, "WebViewObject::Init at %p\n");
    }
}

STATIC
{
    il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
    il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
#endif

namespace Cute::Core
{
}
