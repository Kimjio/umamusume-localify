#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Vector4.hpp"

#include <unordered_map>

#include <WebView2.h>
#include <wil/com.h>

namespace Cute
{
	namespace Core
	{
		class WebViewManager : public UnityEngine::MonoBehaviour
		{
		public:
			static WebViewManager Instance();

			static wil::com_ptr<ICoreWebView2Controller> webviewController;

			static unordered_map<wstring, wstring> customFontMap;

			void OpenWeb(Il2CppString* url);

			void SetVisible(bool visible);

			void SetMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin);

			Il2CppDelegate* Callback();

			UnityEngine::Vector4 marginNow();

			using UnityEngine::MonoBehaviour::MonoBehaviour;
		};
	}
}
