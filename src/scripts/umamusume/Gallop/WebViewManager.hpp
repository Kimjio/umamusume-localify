#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"
#include "WebViewDefine.hpp"
#include "../../Cute.Core.Assembly/Cute/Core/WebViewManager.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/Rect.hpp"

namespace Gallop
{
	class WebViewManager : public MonoSingleton<WebViewManager>
	{
	public:
		class WebViewInfo : public Object
		{
		public:
			WebViewInfo(Il2CppString* url, Il2CppString* partsPath = il2cpp_string_new(""), bool isVueJs = true, WebViewDefine::DmmUrlType = WebViewDefine::DmmUrlType::Cygames);

			Il2CppString* _url();
			bool _isVueJs();

			using UnityEngine::Object::Object;
		};

		void Open(Il2CppString* url, Il2CppObject* dialogData, Il2CppDelegate* onLoadedCallback = nullptr, Il2CppDelegate* onErrorCallback = nullptr, bool isDirect = false);

		Il2CppObject* _currentWebViewDialog();
		void _currentWebViewDialog(Il2CppObject*);
		void _errorCallback(Il2CppObject*);
		Il2CppArraySize_t<Il2CppString*>* _fontFilePaths();
		void SetErrorCallback();
		void OpenErrorDialog();
		void OpenWebView(Il2CppString* url, Il2CppDelegate* onLoadedCallback);
		void SetMargin(UnityEngine::Rect offsetRect);

		Cute::Core::WebViewManager CuteWebView();

		static void SettingUIEffectOnOpen();
		static void SettingUIEffectOnClose();

		static Il2CppString* GetGachaUrl(int gachaId);

		static Il2CppString* GetGachaURLProperty(int gachaId);

		static Il2CppString* GetProductUrl(int productMasterId);

		static Il2CppString* GetProductURLProperty(int productMasterId);

		static bool TryGetWebViewInfo(WebViewDefine::Url urlType, WebViewInfo* out);

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
