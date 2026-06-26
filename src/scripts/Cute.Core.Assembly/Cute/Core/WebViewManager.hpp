#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "../../../UnityEngine.CoreModule/UnityEngine/Vector4.hpp"

#include <unordered_map>

#ifdef _MSC_VER
#include <WebView2.h>
#include <wil/com.h>
#endif

namespace Cute::Core
{
	class WebViewManager : public UnityEngine::MonoBehaviour
	{
	public:
		static WebViewManager Instance();
#ifdef _MSC_VER
		static wil::com_ptr<ICoreWebView2Controller> webviewController;
#endif

		static unordered_map<il2cppstring, il2cppstring> customFontMap;

		void OpenWeb(Il2CppString* url);

		void SetVisible(bool visible);

		void SetMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin);

		Il2CppDelegate* Callback();

		UnityEngine::Vector4 marginNow();

		using MonoBehaviour::MonoBehaviour;
	};
}
