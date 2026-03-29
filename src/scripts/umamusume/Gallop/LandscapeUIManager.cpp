#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LandscapeUIManager.hpp"

#include "UIManager.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* LandscapeUIManager_Initialize_addr = nullptr;
	void* LandscapeUIManager_Initialize_orig = nullptr;

	void* LandscapeUIManager_get_WindowScaleRate_addr = nullptr;
	void* LandscapeUIManager_get_WindowScaleRate_orig = nullptr;

	void* LandscapeUIManager_GetWindowsWidthMax_addr = nullptr;
	void* LandscapeUIManager_GetWindowsWidthMax_orig = nullptr;

	void* LandscapeUIManager_GetWindowsHeightMax_addr = nullptr;
	void* LandscapeUIManager_GetWindowsHeightMax_orig = nullptr;

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

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_displayOrientationButton->klass, "SetOnClick", 1)->methodPointer(_displayOrientationButton,
			&CreateUnityAction(self, *[](Il2CppObject* self)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickDisplayOrientationButton", 0)->methodPointer(self);
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
	return UnityEngine::Screen::width() / LandscapeUIManager_GetWindowsWidthMax_hook();
}

static void InitAddress()
{
	auto LandscapeUIManager = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "LandscapeUIManager");
	if (!LandscapeUIManager)
	{
		LandscapeUIManager = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SteamUIManager");
	}
	LandscapeUIManager_Initialize_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager, "Initialize", 2);
	LandscapeUIManager_get_WindowScaleRate_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager, "get_WindowScaleRate", 0);
	LandscapeUIManager_GetWindowsWidthMax_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager, "GetWindowsWidthMax", 0);
	LandscapeUIManager_GetWindowsHeightMax_addr = il2cpp_symbols::get_method_pointer(LandscapeUIManager, "GetWindowsHeightMax", 0);
	LandscapeUIManager__displayOrientationButtonField = il2cpp_class_get_field_from_name(LandscapeUIManager, "_displayOrientationButton");
}

static void HookMethods()
{
	ADD_HOOK(LandscapeUIManager_Initialize, "Gallop.LandscapeUIManager::Initialize at %p\n");
	ADD_HOOK(LandscapeUIManager_get_WindowScaleRate, "Gallop.LandscapeUIManager::get_WindowScaleRate at %p\n");
	ADD_HOOK(LandscapeUIManager_GetWindowsWidthMax, "Gallop.LandscapeUIManager::Initialize at %p\n");
	ADD_HOOK(LandscapeUIManager_GetWindowsHeightMax, "Gallop.LandscapeUIManager::Initialize at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
