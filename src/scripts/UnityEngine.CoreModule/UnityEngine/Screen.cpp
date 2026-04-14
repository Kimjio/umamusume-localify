#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Screen.hpp"

#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"

#include "config/config.hpp"

namespace
{
	void* get_width_addr = nullptr;

	void* get_height_addr = nullptr;

	void* get_fullScreen_addr = nullptr;

	void* get_currentResolution_Injected_addr = nullptr;

	void* RequestOrientation_addr = nullptr;
	void* RequestOrientation_orig = nullptr;

	void* SetResolution_Injected_addr = nullptr;
	void* SetResolution_Injected_orig = nullptr;
}

static void RequestOrientation_hook(UnityEngine::ScreenOrientation orientation)
{
	if (config::freeform_window)
	{
		reinterpret_cast<decltype(RequestOrientation_hook)*>(RequestOrientation_orig)(UnityEngine::ScreenOrientation::AutoRotation);
#ifdef _MSC_VER
		auto hWnd = GetHWND();

		long style = GetWindowLongW(hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLongPtrW(hWnd, GWL_STYLE, style);
#endif
	}
	else
	{
		reinterpret_cast<decltype(RequestOrientation_hook)*>(RequestOrientation_orig)(orientation);
	}
}



void SetResolution_Injected_hook(int width, int height, int fullscreenMode, UnityEngine::RefreshRate* perferredRefreshRate)
{
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

	reinterpret_cast<decltype(SetResolution_Injected_hook)*>(SetResolution_Injected_orig)(width, height, fullscreenMode, perferredRefreshRate);
}

static void InitAddress()
{
	get_width_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_width()");
	get_height_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_height()");
	get_fullScreen_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_fullScreen()");
	get_currentResolution_Injected_addr = il2cpp_resolve_icall("UnityEngine.Screen::get_currentResolution_Injected(UnityEngine.Resolution&)");
	RequestOrientation_addr = il2cpp_resolve_icall("UnityEngine.Screen::RequestOrientation()");
	SetResolution_Injected_addr = il2cpp_resolve_icall("UnityEngine.Screen::SetResolution_Injected(System.Int32,System.Int32,UnityEngine.FullScreenMode,UnityEngine.RefreshRate)");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(RequestOrientation, "UnityEngine.Screen::RequestOrientation at %p\n");
	}

	if (config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(SetResolution_Injected, "UnityEngine.Screen.SetResolution_Injected(int, int, FullScreenMode, RefreshRate) at %p\n");
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
		reinterpret_cast<void (*)(UnityEngine::Resolution*)>(get_currentResolution_Injected_addr)(&resolution);
		return resolution;
	}
	
	void Screen::RequestOrientation(ScreenOrientation orientation)
	{
		return reinterpret_cast<decltype(RequestOrientation)*>(RequestOrientation_addr)(orientation);
	}

	void Screen::SetResolution_Injected(int width, int height, UnityEngine::FullScreenMode fullscreenMode, UnityEngine::RefreshRate* perferredRefreshRate)
	{
		return reinterpret_cast<decltype(SetResolution_Injected)*>(SetResolution_Injected_addr)(width, height, fullscreenMode, perferredRefreshRate);
	}
}
