#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "UIManager.hpp"
#include "StandaloneWindowResize.hpp"
#include "Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/SceneManagement/Scene.hpp"

#include "game.hpp"
#include "config/config.hpp"

#include <algorithm>

namespace
{
	void* ShowNotification_addr = nullptr;

	void* ShowNotification2_addr = nullptr;

	void* get_UITexture_addr = nullptr;

	void* get_CommonHeaderTitle_addr = nullptr;

	void* get_LandscapeUIManager_addr = nullptr;

	void* get_IsLandscapeMode_addr = nullptr;
	void* get_IsLandscapeMode_orig = nullptr;

	void* SetCameraSizeByOrientation_addr = nullptr;

	void* CheckUIToFrameBufferBlitInstance_addr = nullptr;

	void* AdjustSafeArea_addr = nullptr;

	void* AdjustMissionClearContentsRootRect_addr = nullptr;

	void* AdjustSafeAreaToAnnounceRect_addr = nullptr;

	void* ReleaseRenderTexture_addr = nullptr;

	void* LockGameCanvas_addr = nullptr;
	void* LockGameCanvas_orig = nullptr;

	void* UnlockGameCanvas_addr = nullptr;
	void* UnlockGameCanvas_orig = nullptr;

	void* UnlockAllCanvas_addr = nullptr;

	void* ChangeResolution_addr = nullptr;

	void* WaitResizeUI_addr = nullptr;
	void* WaitResizeUI_orig = nullptr;

	void* GetCanvasScalerList_addr = nullptr;

	void* GetCameraSizeByOrientation_addr = nullptr;
	void* GetCameraSizeByOrientation_orig = nullptr;

	void* get_DefaultResolution_addr = nullptr;
	void* get_DefaultResolution_orig = nullptr;

#ifdef _MSC_VER
	void* ChangeResizeUIForPC_addr = nullptr;
	void* ChangeResizeUIForPC_orig = nullptr;
#endif

	float ratio_vertical = 0.5625f;
	float ratio_horizontal = 1.7777778f;

	constexpr float ratio_4_3 = 1.3333f;
	constexpr float ratio_3_4 = 0.75f;
}

static void SetBGCanvasScalerSize()
{
	auto bgManager = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "BGManager", "get_Instance", 0)();
	if (bgManager)
	{
		auto _mainBgField = il2cpp_class_get_field_from_name_wrap(bgManager->klass, "_mainBg");
		Il2CppObject* _mainBg;
		il2cpp_field_get_value(bgManager, _mainBgField, &_mainBg);

		if (_mainBg)
		{
			auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainBg->klass, "get_transform", 0)->methodPointer(_mainBg);

			int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", IgnoreNumberOfArguments)();
			int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", IgnoreNumberOfArguments)();

			if (width > height)
			{
				auto pos = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(transform->klass, "get_localPosition", 0)->methodPointer(transform);

				if (pos.y == 0)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(transform->klass, "set_localPosition", 1)->methodPointer(transform, UnityEngine::Vector3{ 0, 0, 0 });
				}
			}
		}

		auto _bgCanvasScalerField = il2cpp_class_get_field_from_name_wrap(bgManager->klass, "_bgCanvasScaler");
		Il2CppObject* _bgCanvasScaler;
		il2cpp_field_get_value(bgManager, _bgCanvasScalerField, &_bgCanvasScaler);

		if (_bgCanvasScaler)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(_bgCanvasScaler->klass, "set_scaleFactor", 1)->methodPointer(_bgCanvasScaler, 1);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_bgCanvasScaler->klass, "set_uiScaleMode", 1)->methodPointer(_bgCanvasScaler, 1);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_bgCanvasScaler->klass, "set_screenMatchMode", 1)->methodPointer(_bgCanvasScaler, 0);
		}
	}
}

static void ChangeResizeUIForPC_hook(Il2CppObject* self, int width, int height)
{
	if (!config::unlock_size && !config::freeform_window)
	{
		reinterpret_cast<decltype(ChangeResizeUIForPC_hook)*>(ChangeResizeUIForPC_orig)(self, width, height);
		return;
	}

	Il2CppArraySize_t<Il2CppObject*>* scalers = Gallop::UIManager(self).gameObject().GetComponentsInChildren(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), true);

	for (int i = 0; i < scalers->max_length; i++)
	{
		auto scaler = scalers->vector[i];
		if (scaler)
		{
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(scaler->klass, "get_gameObject", 0)->methodPointer(scaler);

			bool keepActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(gameObject->klass, "get_activeSelf", 0)->methodPointer(gameObject);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

			auto scaleMode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(scaler->klass, "get_uiScaleMode", 0)->methodPointer(scaler);

			if (UnityEngine::Object::Name(scaler)->chars == u"SystemCanvas"s ||
				UnityEngine::Object::Name(scaler)->chars == u"GameCanvas"s ||
				UnityEngine::Object::Name(scaler)->chars == u"BGCanvas"s ||
				UnityEngine::Object::Name(scaler)->chars == u"NoImageEffectGameCanvas"s)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_uiScaleMode", 1)->methodPointer(scaler, 0);

				scaleMode = 0;
			}

			if (config::freeform_window)
			{
				if (Gallop::Screen::IsSplitWindow())
				{
					width = height * ratio_3_4;
				}


				if (scaleMode == 1)
				{
					if (width < height)
					{
						float scale = min(config::freeform_ui_scale_portrait, max(1.0f, height * ratio_vertical) * config::freeform_ui_scale_portrait);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, UnityEngine::Vector2{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
					}
					else
					{
						float scale = min(config::freeform_ui_scale_landscape, max(1.0f, width / ratio_horizontal) * config::freeform_ui_scale_landscape);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, UnityEngine::Vector2{ static_cast<float>(width / scale), static_cast<float>(height / scale) });

					}
				}

				if (scaleMode == 0)
				{
					if (width < height)
					{
						float scale = min(config::freeform_ui_scale_portrait, max(1.0f, height * ratio_vertical) * config::freeform_ui_scale_portrait);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
					}
					else
					{
						float scale = min(config::freeform_ui_scale_landscape, max(1.0f, width / ratio_horizontal) * config::freeform_ui_scale_landscape);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
					}
				}
			}
			else
			{
				if (scaleMode == 1)
				{
					if (width < height)
					{
						float scale = min(config::ui_scale, max(1.0f, height * ratio_vertical) * config::ui_scale);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, UnityEngine::Vector2{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
					}
					else
					{
						float scale = min(config::ui_scale, max(1.0f, width / ratio_horizontal) * config::ui_scale);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(scaler->klass, "set_referenceResolution", 1)->methodPointer(scaler, UnityEngine::Vector2{ static_cast<float>(width / scale), static_cast<float>(height / scale) });
					}
				}
				if (scaleMode == 0)
				{
					// set scale factor to make ui bigger on hi-res screen
					if (width < height)
					{
						float scale = min(config::ui_scale, max(1.0f, height * ratio_vertical) * config::ui_scale);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
					}
					else
					{
						float scale = min(config::ui_scale, max(1.0f, width / ratio_horizontal) * config::ui_scale);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(scaler->klass, "set_scaleFactor", 1)->methodPointer(scaler, scale);
					}
				}
			}

			// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_uiScaleMode", 1)->methodPointer(scaler, 0);

			// il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_screenMatchMode", 1)->methodPointer(scaler, 0);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, keepActive);
		}
	}

	if (config::unlock_size || config::freeform_window)
	{
		SetBGCanvasScalerSize();
	}
}

static Il2CppObject* WaitResizeUI_hook(Il2CppObject* self, bool isPortrait, bool isShowOrientationGuide)
{
	if (config::freeform_window)
	{
		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}
	return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, bool, bool)>(WaitResizeUI_orig)(self, config::freeform_window ? Gallop::StandaloneWindowResize::IsVirt() : isPortrait, config::ui_loading_show_orientation_guide ? false : isShowOrientationGuide);
}

static float GetCameraSizeByOrientation_hook(int orientation)
{
	return 5;
}

static UnityEngine::Vector2 get_DefaultResolution_hook()
{
	int width = UnityEngine::Screen::width();
	int height = UnityEngine::Screen::height();

	if (Gallop::Screen::IsSplitWindow())
	{
		width = height * ratio_3_4;
	}

	return UnityEngine::Vector2{ static_cast<float>(width), static_cast<float>(height) };
}

static bool get_IsLandscapeMode_hook()
{
	return false;
}

static void InitAddress()
{
	ShowNotification_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ShowNotification", 1);
	ShowNotification2_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ShowNotification", 2);
	get_UITexture_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_UITexture", 0);
	get_CommonHeaderTitle_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_CommonHeaderTitle", 0);
	get_LandscapeUIManager_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_LandscapeUIManager", 0);
	get_IsLandscapeMode_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_IsLandscapeMode", 0);
	SetCameraSizeByOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "SetCameraSizeByOrientation", 1);
	CheckUIToFrameBufferBlitInstance_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "CheckUIToFrameBufferBlitInstance", 0);
	AdjustSafeArea_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustSafeArea", 0);
	AdjustMissionClearContentsRootRect_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustMissionClearContentsRootRect", 0);
	AdjustSafeAreaToAnnounceRect_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustSafeAreaToAnnounceRect", 0);
	ReleaseRenderTexture_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ReleaseRenderTexture", 0);
	LockGameCanvas_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "LockGameCanvas", 0);
	UnlockGameCanvas_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "UnlockGameCanvas", 0);
	UnlockAllCanvas_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "UnlockAllCanvas", 0);
	ChangeResolution_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ChangeResolution", 0);
	WaitResizeUI_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "WaitResizeUI", 2);
	GetCanvasScalerList_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "GetCanvasScalerList", 0);
	GetCameraSizeByOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "GetCameraSizeByOrientation", 1);
	get_DefaultResolution_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_DefaultResolution", 0);
#ifdef _MSC_VER
	ChangeResizeUIForPC_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ChangeResizeUIForPC", 2);
#endif
}

static void HookMethods()
{
#ifdef _MSC_VER
	if (config::auto_fullscreen || config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(ChangeResizeUIForPC, "Gallop.UIManager::ChangeResizeUIForPC at %p\n");
	}
#endif

	if (config::freeform_window || config::ui_loading_show_orientation_guide)
	{
		ADD_HOOK(WaitResizeUI, "Gallop.UIManager::WaitResizeUI at %p\n");
	}


	if (config::freeform_window)
	{
		ADD_HOOK(GetCameraSizeByOrientation, "Gallop.UIManager::GetCameraSizeByOrientation at %p\n");
		ADD_HOOK(get_DefaultResolution, "Gallop.UIManager::get_DefaultResolution at %p\n");
		// ADD_HOOK(get_IsLandscapeMode, "Gallop.UIManager::get_IsLandscapeMode at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* UIManager::namespaze = "Gallop";
	const char* UIManager::klassName = "UIManager";

	Il2CppObject* UIManager::_noticeCanvas()
	{
		auto _noticeCanvasField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_noticeCanvas");
		Il2CppObject* _noticeCanvas;
		il2cpp_field_get_value(instance, _noticeCanvasField, &_noticeCanvas);

		return _noticeCanvas;
	}

	Il2CppObject* UIManager::_bgCamera()
	{
		auto _bgCameraField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_bgCamera");
		Il2CppObject* _bgCamera;
		il2cpp_field_get_value(instance, _bgCameraField, &_bgCamera);

		return _bgCamera;
	}

	Il2CppObject* UIManager::_uiCamera()
	{
		auto _uiCameraField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_uiCamera");
		Il2CppObject* _uiCamera;
		il2cpp_field_get_value(instance, _uiCameraField, &_uiCamera);

		return _uiCamera;
	}

	Il2CppObject* UIManager::_bgManager()
	{
		auto _bgManagerField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_bgManager");
		Il2CppObject* _bgManager;
		il2cpp_field_get_value(instance, _bgManagerField, &_bgManager);

		return _bgManager;
	}

	Il2CppObject* UIManager::_uiToFrameBufferRenderCameraData()
	{
		auto _uiToFrameBufferRenderCameraDataField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_uiToFrameBufferRenderCameraData");
		if (_uiToFrameBufferRenderCameraDataField)
		{
			Il2CppObject* _uiToFrameBufferRenderCameraData;
			il2cpp_field_get_value(instance, _uiToFrameBufferRenderCameraDataField, &_uiToFrameBufferRenderCameraData);
			return _uiToFrameBufferRenderCameraData;
		}
		return nullptr;
	}

	Il2CppObject* UIManager::_uiCommandBuffer()
	{
		auto _uiCommandBufferField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_uiCommandBuffer");
		if (_uiCommandBufferField)
		{
			Il2CppObject* _uiCommandBuffer;
			il2cpp_field_get_value(instance, _uiCommandBufferField, &_uiCommandBuffer);
			return _uiCommandBuffer;
		}
		return nullptr;
	}

	Il2CppObject* UIManager::_blitToFrameMaterial()
	{
		auto _blitToFrameMaterialField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_blitToFrameMaterial");
		if (_blitToFrameMaterialField)
		{
			Il2CppObject* _blitToFrameMaterial;
			il2cpp_field_get_value(instance, _blitToFrameMaterialField, &_blitToFrameMaterial);
			return _blitToFrameMaterial;
		}
		return nullptr;
	}

	Il2CppObject* UIManager::_noImageEffectUICamera()
	{
		auto _noImageEffectUICameraField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_noImageEffectUICamera");
		Il2CppObject* _noImageEffectUICamera;
		il2cpp_field_get_value(instance, _noImageEffectUICameraField, &_noImageEffectUICamera);

		return _noImageEffectUICamera;
	}

	Il2CppObject* UIManager::_noImageEffectCanvas()
	{
		auto _noImageEffectCanvasField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_noImageEffectCanvas");
		Il2CppObject* _noImageEffectCanvas;
		il2cpp_field_get_value(instance, _noImageEffectCanvasField, &_noImageEffectCanvas);

		return _noImageEffectCanvas;
	}

	Il2CppObject* UIManager::_uiToFrameBufferBlitCamera()
	{
		auto _uiToFrameBufferBlitCameraField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_uiToFrameBufferBlitCamera");
		Il2CppObject* _uiToFrameBufferBlitCamera;
		il2cpp_field_get_value(instance, _uiToFrameBufferBlitCameraField, &_uiToFrameBufferBlitCamera);

		return _uiToFrameBufferBlitCamera;
	}

	Il2CppObject* UIManager::_mainCanvas()
	{
		auto _mainCanvasField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_mainCanvas");
		Il2CppObject* _mainCanvas;
		il2cpp_field_get_value(instance, _mainCanvasField, &_mainCanvas);

		return _mainCanvas;
	}

	Il2CppObject* UIManager::_gameCanvas()
	{
		auto _gameCanvasField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_gameCanvas");
		Il2CppObject* _gameCanvas;
		il2cpp_field_get_value(instance, _gameCanvasField, &_gameCanvas);

		return _gameCanvas;
	}

	Il2CppObject* UIManager::_systemCanvas()
	{
		auto _systemCanvasField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_systemCanvas");
		Il2CppObject* _systemCanvas;
		il2cpp_field_get_value(instance, _systemCanvasField, &_systemCanvas);

		return _systemCanvas;
	}

	Il2CppObject* UIManager::UITexture()
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_UITexture_addr)(instance);
	}

	void UIManager::UITexture(Il2CppObject* value)
	{
		auto _uiTextureField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_uiTexture");
		il2cpp_field_set_value(instance, _uiTextureField, value);
	}

	Il2CppObject* UIManager::CommonHeaderTitle()
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_CommonHeaderTitle_addr)(instance);
	}

	Il2CppObject* UIManager::LandscapeUIManager()
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_LandscapeUIManager_addr)(instance);
	}

	void UIManager::ShowNotification(Il2CppString* text)
	{
		if (ShowNotification2_addr)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*, uint64_t)>(ShowNotification2_addr)(instance, text, 0);
			return;
		}
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(ShowNotification_addr)(instance, text);
	}

	void UIManager::SetCameraSizeByOrientation(UnityEngine::ScreenOrientation orientation)
	{
		reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::ScreenOrientation)>(SetCameraSizeByOrientation_addr)(instance, orientation);
	}

	void UIManager::CheckUIToFrameBufferBlitInstance()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(CheckUIToFrameBufferBlitInstance_addr)(instance);
	}

	void UIManager::AdjustSafeArea()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(AdjustSafeArea_addr)(instance);
	}

	void UIManager::AdjustMissionClearContentsRootRect()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(AdjustMissionClearContentsRootRect_addr)(instance);
	}

	void UIManager::AdjustSafeAreaToAnnounceRect()
	{
		if (AdjustSafeAreaToAnnounceRect_addr)
		{
			reinterpret_cast<void (*)(Il2CppObject*)>(AdjustSafeAreaToAnnounceRect_addr)(instance);
		}
	}

	void UIManager::ReleaseRenderTexture()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(ReleaseRenderTexture_addr)(instance);
	}

	void UIManager::LockGameCanvas()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(LockGameCanvas_addr)(instance);
	}

	void UIManager::UnlockGameCanvas()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(UnlockGameCanvas_addr)(instance);
	}

	void UIManager::UnlockAllCanvas()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(UnlockAllCanvas_addr)(instance);
	}

	void UIManager::ChangeResolution()
	{
		if (!ChangeResolution_addr)
		{
			return;
		}

		reinterpret_cast<void (*)(Il2CppObject*)>(ChangeResolution_addr)(instance);
	}

	Il2CppObject* UIManager::WaitResizeUI(bool isPortrait, bool isShowOrientationGuide)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, bool, bool)>(WaitResizeUI_addr)(instance, isPortrait, isShowOrientationGuide);
	}

	Il2CppArraySize_t<Il2CppObject*>* UIManager::GetCanvasScalerList()
	{
		return reinterpret_cast<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*)>(GetCanvasScalerList_addr)(instance);
	}

	float UIManager::GetCameraSizeByOrientation(ScreenOrientation orientation)
	{
		return reinterpret_cast<decltype(GetCameraSizeByOrientation)*>(GetCameraSizeByOrientation_addr)(orientation);
	}

	UnityEngine::Vector2 UIManager::DefaultResolution()
	{
		return reinterpret_cast<decltype(DefaultResolution)*>(get_DefaultResolution_addr)();
	}

	bool UIManager::IsLandscapeMode()
	{
		if (get_IsLandscapeMode_addr)
		{
			return reinterpret_cast<bool (*)()>(get_IsLandscapeMode_addr)();
		}

		if (Game::CurrentGameStore == Game::Store::Steam)
		{
			return true;
		}

		return false;
	}

#ifdef _MSC_VER
	void UIManager::ChangeResizeUIForPC(int width, int height)
	{
		if (ChangeResizeUIForPC_orig)
		{
			ChangeResizeUIForPC_hook(instance, width, height);
			return;
		}

		reinterpret_cast<void (*)(Il2CppObject*, int, int)>(ChangeResizeUIForPC_addr)(instance, width, height);
	}
#endif
}
