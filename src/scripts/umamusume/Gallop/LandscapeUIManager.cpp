#ifdef _MSC_VER
#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LandscapeUIManager.hpp"

#include "UIManager.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer LandscapeUIManager_Initialize_addr = nullptr;
	void* LandscapeUIManager_Initialize_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_get_WindowScaleRate_addr = nullptr;
	void* LandscapeUIManager_get_WindowScaleRate_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_GetWindowsWidthMax_addr = nullptr;
	void* LandscapeUIManager_GetWindowsWidthMax_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_GetWindowsHeightMax_addr = nullptr;
	void* LandscapeUIManager_GetWindowsHeightMax_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_OnPushBandUIButton_addr = nullptr;
	void* LandscapeUIManager_OnPushBandUIButton_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_RestorePrevSelectedBandMenu_addr = nullptr;
	void* LandscapeUIManager_RestorePrevSelectedBandMenu_orig = nullptr;

	Il2CppMethodPointer LandscapeUIManager_GetTouchPositionCameraType_addr = nullptr;

	Il2CppMethodPointer LandscapeUIManager_GetTouchPositionOffsetX_addr = nullptr;

	FieldInfo* LandscapeUIManager__displayOrientationButtonField = nullptr;
}

static void LandscapeUIManager_Initialize_hook(Il2CppObject* self, float leftUiOffsetX, float rightUiOffsetX)
{
	reinterpret_cast<void (*)(Il2CppObject*, float, float)>(LandscapeUIManager_Initialize_orig)(self, leftUiOffsetX, rightUiOffsetX);

	if (LandscapeUIManager__displayOrientationButtonField)
	{
		Il2CppObject* _displayOrientationButton;
		il2cpp_field_get_value(self, LandscapeUIManager__displayOrientationButtonField, &_displayOrientationButton);

		UnityEngine::MonoBehaviour(_displayOrientationButton).gameObject().SetActive(true);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(_displayOrientationButton->klass, "SetOnClick", 1)(_displayOrientationButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(self->klass, "OnClickDisplayOrientationButton", 0)(self);
				})->delegate
		);
	}
}

static int LandscapeUIManager_GetWindowsWidthMax_hook()
{
	if (config::freeform_window || Gallop::UIManager::IsLandscapeMode() || !Gallop::Screen::IsVertical())
	{
		return Gallop::Screen::Width();
	}

	return Gallop::Screen::Height();
}

static int LandscapeUIManager_GetWindowsHeightMax_hook()
{
	if (config::freeform_window || Gallop::UIManager::IsLandscapeMode() || !Gallop::Screen::IsVertical())
	{
		return Gallop::Screen::Height();
	}

	return Gallop::Screen::Width();
}

static float LandscapeUIManager_get_WindowScaleRate_hook()
{
	if (config::freeform_window)
	{
		return 1.0f;
	}

	return static_cast<float>(UnityEngine::Screen::width()) / LandscapeUIManager_GetWindowsWidthMax_hook();
}

static void LandscapeUIManager_OnPushBandUIButton_hook(Il2CppObject* self, uint64_t buttonType)
{
	// no-op
}

static void LandscapeUIManager_RestorePrevSelectedBandMenu_hook(Il2CppObject* self)
{
	// no-op
}

static void InitAddress()
{
	auto LandscapeUIManager_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LandscapeUIManager");
	if (!LandscapeUIManager_klass)
	{
		LandscapeUIManager_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SteamUIManager");
	}

	if (!LandscapeUIManager_klass)
	{
		return;
	}

	LandscapeUIManager_Initialize_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "Initialize", 2);
	LandscapeUIManager_get_WindowScaleRate_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "get_WindowScaleRate", 0);
	LandscapeUIManager_GetWindowsWidthMax_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "GetWindowsWidthMax", 0);
	LandscapeUIManager_GetWindowsHeightMax_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "GetWindowsHeightMax", 0);
	LandscapeUIManager_GetTouchPositionCameraType_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "GetTouchPositionCameraType", 1);
	LandscapeUIManager_GetTouchPositionOffsetX_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "GetTouchPositionOffsetX", 1);
	LandscapeUIManager__displayOrientationButtonField = il2cpp_class_get_field_from_name(LandscapeUIManager_klass, "_displayOrientationButton");
	LandscapeUIManager_OnPushBandUIButton_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "OnPushBandUIButton", 1);
	LandscapeUIManager_RestorePrevSelectedBandMenu_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager_klass, "RestorePrevSelectedBandMenu", 0);
}

static void HookMethods()
{
	ADD_HOOK(LandscapeUIManager_Initialize, "Gallop.LandscapeUIManager::Initialize at %p\n");

	if (config::freeform_window)
	{
		ADD_HOOK(LandscapeUIManager_GetWindowsWidthMax, "Gallop.LandscapeUIManager::GetWindowsWidthMax at %p\n");
		ADD_HOOK(LandscapeUIManager_GetWindowsHeightMax, "Gallop.LandscapeUIManager::GetWindowsHeightMax at %p\n");
		ADD_HOOK(LandscapeUIManager_get_WindowScaleRate, "Gallop.LandscapeUIManager::get_WindowScaleRate at %p\n");
		ADD_HOOK(LandscapeUIManager_OnPushBandUIButton, "Gallop.LandscapeUIManager::OnPushBandUIButton at %p\n");
		ADD_HOOK(LandscapeUIManager_RestorePrevSelectedBandMenu, "Gallop.LandscapeUIManager::RestorePrevSelectedBandMenu at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	float LandscapeUIManager::WindowScaleRate()
	{
		return reinterpret_cast<float (*)()>(LandscapeUIManager_get_WindowScaleRate_addr)();
	}


	LandscapeUIManager::CameraType LandscapeUIManager::GetTouchPositionCameraType(float touchPositionX)
	{
		return reinterpret_cast<CameraType(*)(float)>(LandscapeUIManager_GetTouchPositionCameraType_addr)(touchPositionX);
	}

	float LandscapeUIManager::GetTouchPositionOffsetX(CameraType cameraType)
	{
		return reinterpret_cast<float (*)(CameraType)>(LandscapeUIManager_GetTouchPositionOffsetX_addr)(cameraType);
	}
}
#endif
