#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Vector4.hpp"

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

			void OpenWeb(Il2CppString* url);

			void SetVisible(bool visible);

			Il2CppDelegate* Callback();

			UnityEngine::Vector4 marginNow();

			using UnityEngine::MonoBehaviour::MonoBehaviour;
		};
	}
}
