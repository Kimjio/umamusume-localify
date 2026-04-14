#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GraphicSettings.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2Int.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "game.hpp"
#include "config/config.hpp"

namespace
{
	void* GraphicSettings_ApplyGraphicsQuality_addr = nullptr;
	void* GraphicSettings_ApplyGraphicsQuality_orig = nullptr;

	void* GraphicSettings_Get3DAntiAliasingLevel_addr = nullptr;
	void* GraphicSettings_Get3DAntiAliasingLevel_orig = nullptr;

	void* GraphicSettings_Update3DRenderTexture_addr = nullptr;

	void* GraphicSettings_GetVirtualResolution_addr = nullptr;
	void* GraphicSettings_GetVirtualResolution_orig = nullptr;

	void* GraphicSettings_GetVirtualResolution3D_addr = nullptr;
	void* GraphicSettings_GetVirtualResolution3D_orig = nullptr;

	void* GraphicSettings_GetVirtualResolution3D2_addr = nullptr;
	void* GraphicSettings_GetVirtualResolution3D2_orig = nullptr;
}

static void GraphicSettings_ApplyGraphicsQuality_hook(Il2CppObject* self, int quality, bool force)
{
	reinterpret_cast<decltype(GraphicSettings_ApplyGraphicsQuality_hook)*>(GraphicSettings_ApplyGraphicsQuality_orig)(self, config::graphics_quality, true);
}

static int GraphicSettings_Get3DAntiAliasingLevel_hook(Il2CppObject* self, bool allowMSAA)
{
	if (config::anti_aliasing < 0)
	{
		return reinterpret_cast<decltype(GraphicSettings_Get3DAntiAliasingLevel_hook)*>(GraphicSettings_Get3DAntiAliasingLevel_orig)(self, allowMSAA);
	}

	if (!allowMSAA)
	{
		return 1;
	}

	return config::anti_aliasing;
}

static UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution_hook(Il2CppObject* self)
{
	if (config::freeform_window)
	{
		int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
		int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

		return UnityEngine::Vector2Int{ width, height };
	}

	auto resolution = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution_hook)*>(GraphicSettings_GetVirtualResolution_orig)(self);
	if (config::unlock_size)
	{
		int width = Gallop::Screen::Width();
		int height = Gallop::Screen::Height();
		if (resolution.m_X > resolution.m_Y)
		{
			resolution.m_X = width;
			resolution.m_Y = height;
		}
		else
		{
			resolution.m_X = height;
			resolution.m_Y = width;
		}
	}
	resolution.m_X *= config::resolution_3d_scale;
	resolution.m_Y *= config::resolution_3d_scale;
	return resolution;
}

static UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution3D_hook(Il2CppObject* self, bool isForcedWideAspect)
{
	if (config::freeform_window)
	{
		int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
		int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

		return UnityEngine::Vector2Int{ width, height };
	}

	auto resolution = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution3D_hook)*>(GraphicSettings_GetVirtualResolution3D_orig)(self, isForcedWideAspect);
	if (config::unlock_size)
	{
		int width = Gallop::Screen::Width();
		int height = Gallop::Screen::Height();
		if (resolution.m_X > resolution.m_Y)
		{
			resolution.m_X = width;
			resolution.m_Y = height;
		}
		else
		{
			resolution.m_X = height;
			resolution.m_Y = width;
		}
	}
	resolution.m_X *= config::resolution_3d_scale;
	resolution.m_Y *= config::resolution_3d_scale;
	return resolution;
}


static UnityEngine::Vector2Int GraphicSettings_GetVirtualResolution3D2_hook(Il2CppObject* self, UnityEngine::Vector2Int resolution, bool isForcedWideAspect)
{
	if (config::freeform_window)
	{
		int width = UnityEngine::Screen::width() * config::resolution_3d_scale;
		int height = UnityEngine::Screen::height() * config::resolution_3d_scale;

		return UnityEngine::Vector2Int{ width, height };
	}

	auto resolution2 = reinterpret_cast<decltype(GraphicSettings_GetVirtualResolution3D2_hook)*>(GraphicSettings_GetVirtualResolution3D2_orig)(self, resolution, isForcedWideAspect);
	if (config::unlock_size)
	{
		int width = Gallop::Screen::Width();
		int height = Gallop::Screen::Height();
		if (resolution2.m_X > resolution2.m_Y)
		{
			resolution2.m_X = width;
			resolution2.m_Y = height;
		}
		else
		{
			resolution2.m_X = height;
			resolution2.m_Y = width;
		}
	}
	resolution2.m_X *= config::resolution_3d_scale;
	resolution2.m_Y *= config::resolution_3d_scale;
	return resolution2;
}

static void InitAddress()
{
	auto GraphicSettings_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "GraphicSettings");
	GraphicSettings_ApplyGraphicsQuality_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "ApplyGraphicsQuality", 2);
	GraphicSettings_Get3DAntiAliasingLevel_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "Get3DAntiAliasingLevel", 1);
	GraphicSettings_Update3DRenderTexture_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "Update3DRenderTexture", 0);
	GraphicSettings_GetVirtualResolution3D_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "GetVirtualResolution3D", 1);
	GraphicSettings_GetVirtualResolution3D2_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "GetVirtualResolution3D", 2);
	GraphicSettings_GetVirtualResolution_addr = il2cpp_symbols::get_method_pointer(GraphicSettings_klass, "GetVirtualResolution", 0);
}

static void HookMethods()
{
	if (config::graphics_quality != -1)
	{
		ADD_HOOK(GraphicSettings_ApplyGraphicsQuality, "Gallop.GraphicSettings::ApplyGraphicsQuality at %p\n");
	}

	if (config::resolution_3d_scale != 1.0f || config::freeform_window)
	{
		ADD_HOOK(GraphicSettings_GetVirtualResolution3D, "Gallop.GraphicSettings::GetVirtualResolution3D at %p\n");
		ADD_HOOK(GraphicSettings_GetVirtualResolution3D2, "Gallop.GraphicSettings::GetVirtualResolution3D2 at %p\n");
		ADD_HOOK(GraphicSettings_GetVirtualResolution, "Gallop.GraphicSettings::GetVirtualResolution at %p\n");
	}

	if (config::anti_aliasing != -1)
	{
		ADD_HOOK(GraphicSettings_Get3DAntiAliasingLevel, "Gallop.GraphicSettings::Get3DAntiAliasingLevel at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* GraphicSettings::namespaze = "Gallop";
	const char* GraphicSettings::klassName = "GraphicSettings";

	void GraphicSettings::Update3DRenderTexture()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(GraphicSettings_Update3DRenderTexture_addr)(instance);
	}
}
