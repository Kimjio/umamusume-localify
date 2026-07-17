#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Screen.hpp"

#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer get_width_addr = nullptr;

	Il2CppMethodPointer get_height_addr = nullptr;

	Il2CppMethodPointer get_fullScreen_addr = nullptr;

	Il2CppMethodPointer get_currentResolution_Injected_addr = nullptr;

    Il2CppMethodPointer SetOrientationEnabled_addr = nullptr;

	Il2CppMethodPointer RequestOrientation_addr = nullptr;

	Il2CppMethodPointer SetResolution_Injected_addr = nullptr;

	Il2CppMethodPointer SetResolution_addr = nullptr;
}

static void RequestOrientation_hook(UnityEngine::ScreenOrientation orientation)
{
	if (config::freeform_window)
	{
		reinterpret_cast<decltype(RequestOrientation_hook)*>(RequestOrientation_addr)(UnityEngine::ScreenOrientation::AutoRotation);
#ifdef _MSC_VER
		auto hWnd = GetHWND();

		long style = GetWindowLongW(hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
#endif
	}
	else
	{
		reinterpret_cast<decltype(RequestOrientation_hook)*>(RequestOrientation_addr)(orientation);
	}
}

void SetResolution_Injected_hook(int width, int height, int fullscreenMode, UnityEngine::RefreshRate* perferredRefreshRate)
{
#ifdef _MSC_VER
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

		Gallop::StandaloneWindowResize::IsPreventReShape(true);
		return;
	}
#endif
	reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_addr)(width, height, fullscreenMode, perferredRefreshRate);
}

static void InitAddress()
{
	get_width_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_width");
	get_height_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_height");
	get_fullScreen_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_fullScreen");
	get_currentResolution_Injected_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_currentResolution_Injected");
	RequestOrientation_addr = il2cpp_resolve_icall("UnityEngine.Screen::RequestOrientation");
    SetOrientationEnabled_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetOrientationEnabled");
	SetResolution_Injected_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution_Injected");
	SetResolution_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		il2cpp_add_internal_call("UnityEngine.Screen::RequestOrientation", reinterpret_cast<Il2CppMethodPointer>(RequestOrientation_hook));
	}

	if (config::unlock_size || config::freeform_window)
	{
        il2cpp_add_internal_call("UnityEngine.Screen.SetResolution_Injected", reinterpret_cast<Il2CppMethodPointer>(SetResolution_Injected_hook));
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	int Screen::width()
	{
		return reinterpret_cast<decltype(width)*>(get_width_addr)();
	}

	int Screen::height()
	{
		return reinterpret_cast<decltype(height)*>(get_height_addr)();
	}
	
	bool Screen::fullScreen()
	{
		return reinterpret_cast<decltype(fullScreen)*>(get_fullScreen_addr)();
	}

	Resolution Screen::currentResolution()
	{
		Resolution resolution;
		reinterpret_cast<void (*)(Resolution*)>(get_currentResolution_Injected_addr)(&resolution);
		return resolution;
	}

    void Screen::SetOrientationEnabled(EnabledOrientation orient, bool enabled)
    {
        reinterpret_cast<decltype(SetOrientationEnabled)*>(SetOrientationEnabled_addr)(orient, enabled);
    }

    void Screen::autorotateToPortrait(bool value)
    {
        SetOrientationEnabled(EnabledOrientation::kAutorotateToPortrait, value);
    }

	void Screen::autorotateToPortraitUpsideDown(bool value)
	{
		SetOrientationEnabled(EnabledOrientation::kAutorotateToPortraitUpsideDown, value);
	}

    void Screen::autorotateToLandscapeLeft(bool value)
	{
        SetOrientationEnabled(EnabledOrientation::kAutorotateToLandscapeLeft, value);
	}

	void Screen::autorotateToLandscapeRight(bool value)
	{
        SetOrientationEnabled(EnabledOrientation::kAutorotateToLandscapeRight, value);
	}
	
	void Screen::RequestOrientation(ScreenOrientation orientation)
	{
		reinterpret_cast<decltype(RequestOrientation)*>(RequestOrientation_addr)(orientation);
	}

	void Screen::SetResolution_Injected(int width, int height, FullScreenMode fullscreenMode, RefreshRate* perferredRefreshRate)
	{
        if (SetResolution_addr)
        {
            reinterpret_cast<void (*)(int, int, FullScreenMode, int)>(SetResolution_addr)(width, height, fullscreenMode, perferredRefreshRate->value());
            return;
        }
		reinterpret_cast<decltype(SetResolution_Injected)*>(SetResolution_Injected_addr)(width, height, fullscreenMode, perferredRefreshRate);
	}
}
