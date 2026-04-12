#ifdef _MSC_VER

#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StandaloneWindowResize.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"

#include "config/config.hpp"

namespace
{
	void* StandaloneWindowResize_GetLimitSize_addr = nullptr;
	void* StandaloneWindowResize_GetLimitSize_orig = nullptr;

	void* StandaloneWindowResize_SaveChangedWidth_addr = nullptr;

	void* StandaloneWindowResize_EnableWindowHitTest_addr = nullptr;

	void* StandaloneWindowResize_DisableWindowHitTest_addr = nullptr;

	void* StandaloneWindowResize_DisableMaximizebox_addr = nullptr;
	void* StandaloneWindowResize_DisableMaximizebox_orig = nullptr;

	void* StandaloneWindowResize_ReshapeAspectRatio_addr = nullptr;
	void* StandaloneWindowResize_ReshapeAspectRatio_orig = nullptr;

	void* StandaloneWindowResize_KeepAspectRatio_addr = nullptr;
	void* StandaloneWindowResize_KeepAspectRatio_orig = nullptr;

	void* StandaloneWindowResize_GetChangedSize_addr = nullptr;
	void* StandaloneWindowResize_GetChangedSize_orig = nullptr;

	void* StandaloneWindowResize_ClearStopFlagAfterWhile_addr = nullptr;

	FieldInfo* StandaloneWindowResize_windowLastWidthField = nullptr;
	FieldInfo* StandaloneWindowResize_windowLastHeightField = nullptr;
	FieldInfo* StandaloneWindowResize__aspectRatioField = nullptr;
	FieldInfo* StandaloneWindowResize__isPreventReShapeField = nullptr;
	FieldInfo* StandaloneWindowResize__isVirtField = nullptr;
	FieldInfo* StandaloneWindowResize__isEnableWindowHitTestField = nullptr;
	FieldInfo* StandaloneWindowResize__isWindowSizeChangingField = nullptr;
	FieldInfo* StandaloneWindowResize__isWindowDraggingField = nullptr;
}

static UnityEngine::Vector2 StandaloneWindowResize_GetLimitSize_hook()
{
	return { FLT_MAX, FLT_MAX };
}

static void StandaloneWindowResize_DisableMaximizebox_hook()
{
	auto hWnd = GetHWND();

	long style = GetWindowLongW(hWnd, GWL_STYLE);
	if (!Gallop::Screen::IsLandscapeMode() && !config::freeform_window)
	{
		style &= ~WS_MAXIMIZEBOX;
	}
	else
	{
		style |= WS_MAXIMIZEBOX;
	}
	SetWindowLongPtrW(hWnd, GWL_STYLE, style);
}

static void StandaloneWindowResize_ReshapeAspectRatio_hook()
{

}

static void StandaloneWindowResize_KeepAspectRatio_hook(float currentWidth, float currentHeight)
{
	auto hWnd = GetHWND();

	long style = GetWindowLongW(hWnd, GWL_STYLE);
	style |= WS_MAXIMIZEBOX;
	SetWindowLongPtrW(hWnd, GWL_STYLE, style);
}

static void InitAddress()
{
	auto StandaloneWindowResize_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StandaloneWindowResize");
	StandaloneWindowResize_GetLimitSize_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "GetLimitSize", 0);
	StandaloneWindowResize_SaveChangedWidth_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "SaveChangedWidth", 2);
	StandaloneWindowResize_EnableWindowHitTest_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "EnableWindowHitTest", 0);
	StandaloneWindowResize_DisableWindowHitTest_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "DisableWindowHitTest", 0);
	StandaloneWindowResize_DisableMaximizebox_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "DisableMaximizebox", 0);
	StandaloneWindowResize_ReshapeAspectRatio_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "ReshapeAspectRatio", 0);
	StandaloneWindowResize_KeepAspectRatio_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "KeepAspectRatio", 2);
	StandaloneWindowResize_ClearStopFlagAfterWhile_addr = il2cpp_symbols::get_method_pointer(StandaloneWindowResize_klass, "ClearStopFlagAfterWhile", 0);
	StandaloneWindowResize_windowLastWidthField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "windowLastWidth");
	StandaloneWindowResize_windowLastHeightField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "windowLastHeight");
	StandaloneWindowResize__aspectRatioField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_aspectRatio");
	StandaloneWindowResize__isPreventReShapeField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_isPreventReShape");
	StandaloneWindowResize__isVirtField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_isVirt");
	StandaloneWindowResize__isEnableWindowHitTestField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_isEnableWindowHitTest");
	StandaloneWindowResize__isWindowSizeChangingField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_isWindowSizeChanging");
	StandaloneWindowResize__isWindowDraggingField = il2cpp_class_get_field_from_name(StandaloneWindowResize_klass, "_isWindowDragging");
}

static void HookAddress()
{
	if (config::freeform_window || Game::CurrentGameStore == Game::Store::Steam)
	{
		ADD_HOOK(StandaloneWindowResize_DisableMaximizebox, "Gallop.StandaloneWindowResize::DisableMaximizebox at %p\n");
	}

	if (config::freeform_window)
	{
		ADD_HOOK(StandaloneWindowResize_ReshapeAspectRatio, "Gallop.StandaloneWindowResize::ReshapeAspectRatio at %p\n");
		ADD_HOOK(StandaloneWindowResize_KeepAspectRatio, "Gallop.StandaloneWindowResize::KeepAspectRatio at %p\n");
		ADD_HOOK(StandaloneWindowResize_GetLimitSize, "Gallop.StandaloneWindowResize::GetLimitSize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookAddress);
}

namespace Gallop
{
	int StandaloneWindowResize::windowLastWidth()
	{
		int value;
		il2cpp_field_static_get_value(StandaloneWindowResize_windowLastWidthField, &value);
		return value;
	}

	void StandaloneWindowResize::windowLastWidth(int value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize_windowLastWidthField, &value);
	}

	int StandaloneWindowResize::windowLastHeight()
	{
		int value;
		il2cpp_field_static_get_value(StandaloneWindowResize_windowLastHeightField, &value);
		return value;
	}

	void StandaloneWindowResize::windowLastHeight(int value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize_windowLastHeightField, &value);
	}

	float StandaloneWindowResize::AspectRatio()
	{
		float value;
		il2cpp_field_static_get_value(StandaloneWindowResize__aspectRatioField, &value);
		return value;
	}

	void StandaloneWindowResize::AspectRatio(float value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__aspectRatioField, &value);
	}

	bool StandaloneWindowResize::IsPreventReShape()
	{
		bool value;
		il2cpp_field_static_get_value(StandaloneWindowResize__isPreventReShapeField, &value);
		return value;
	}

	void StandaloneWindowResize::IsPreventReShape(bool value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__isPreventReShapeField, &value);
	}

	bool StandaloneWindowResize::IsVirt()
	{
		bool value;
		il2cpp_field_static_get_value(StandaloneWindowResize__isVirtField, &value);
		return value;
	}

	void StandaloneWindowResize::IsVirt(bool value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__isVirtField, &value);
	}

	bool StandaloneWindowResize::IsEnableWindowHitTest()
	{
		bool value;
		il2cpp_field_static_get_value(StandaloneWindowResize__isEnableWindowHitTestField, &value);
		return value;
	}

	void StandaloneWindowResize::IsEnableWindowHitTest(bool value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__isEnableWindowHitTestField, &value);
	}

	bool StandaloneWindowResize::_isWindowSizeChanging()
	{
		bool value;
		il2cpp_field_static_get_value(StandaloneWindowResize__isWindowSizeChangingField, &value);
		return value;
	}

	void StandaloneWindowResize::_isWindowSizeChanging(bool value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__isWindowSizeChangingField, &value);
	}

	bool StandaloneWindowResize::_isWindowDragging()
	{
		bool value;
		il2cpp_field_static_get_value(StandaloneWindowResize__isWindowDraggingField, &value);
		return value;
	}

	void StandaloneWindowResize::_isWindowDragging(bool value)
	{
		il2cpp_field_static_set_value(StandaloneWindowResize__isWindowDraggingField, &value);
	}

	void StandaloneWindowResize::SaveChangedWidth(float width, float height)
	{
		reinterpret_cast<void (*)(float, float)>(StandaloneWindowResize_SaveChangedWidth_addr)(width, height);
	}

	void StandaloneWindowResize::EnableWindowHitTest()
	{
		reinterpret_cast<void (*)()>(StandaloneWindowResize_EnableWindowHitTest_addr)();
	}

	void StandaloneWindowResize::DisableWindowHitTest()
	{
		reinterpret_cast<void (*)()>(StandaloneWindowResize_DisableWindowHitTest_addr)();
	}

	void StandaloneWindowResize::DisableMaximizebox()
	{
		reinterpret_cast<void (*)()>(StandaloneWindowResize_DisableMaximizebox_addr)();
	}

	Il2CppObject* StandaloneWindowResize::ClearStopFlagAfterWhile()
	{
		return reinterpret_cast<Il2CppObject * (*)()>(StandaloneWindowResize_ClearStopFlagAfterWhile_addr)();
	}
}
#endif
