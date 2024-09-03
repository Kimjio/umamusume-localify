#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "UIManager.hpp"
#include "StandaloneWindowResize.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "game.hpp"
#include "config/config.hpp"

#include <algorithm>

void* ShowNotification_addr = nullptr;

void* get_UITexture_addr = nullptr;

void* get_CommonHeaderTitle_addr = nullptr;

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

static void ChangeResizeUIForPC_hook(Il2CppObject* _this, int width, int height)
{
	if (!config::unlock_size && !config::freeform_window)
	{
		reinterpret_cast<decltype(ChangeResizeUIForPC_hook)*>(ChangeResizeUIForPC_orig)(_this, width, height);
		return;
	}

	Il2CppArraySize_t<Il2CppObject*>* scalers;
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		scalers = UnityEngine::Object::FindObjectsByType(
			GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), UnityEngine::FindObjectsInactive::Include, UnityEngine::FindObjectsSortMode::None);
	}
	else
	{
		scalers = UnityEngine::Object::FindObjectsOfType(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "CanvasScaler"), true);
	}

	// auto scalers = UIManager_GetCanvasScalerList(_this);
	for (int i = 0; i < scalers->max_length; i++)
	{
		auto scaler = scalers->vector[i];
		if (scaler)
		{
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(scaler->klass, "get_gameObject", 0)->methodPointer(scaler);

			bool keepActive = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(gameObject->klass, "get_activeSelf", 0)->methodPointer(gameObject);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

			auto scaleMode = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(scaler->klass, "get_uiScaleMode", 0)->methodPointer(scaler);

			if (UnityEngine::Object::Name(scaler)->chars == L"SystemCanvas"s ||
				UnityEngine::Object::Name(scaler)->chars == L"GameCanvas"s ||
				UnityEngine::Object::Name(scaler)->chars == L"NoImageEffectGameCanvas"s)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(scaler->klass, "set_uiScaleMode", 1)->methodPointer(scaler, 0);

				scaleMode = 0;
			}

			if (config::freeform_window)
			{
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

static void LockGameCanvas_hook(Il2CppObject* _this)
{
	cout << "LockGameCanvas" << endl;
	PrintStackTrace();
}

static void UnlockGameCanvas_hook(Il2CppObject* _this)
{
	cout << "UnlockGameCanvas" << endl;
	PrintStackTrace();
}

static Il2CppObject* WaitResizeUI_hook(Il2CppObject* _this, bool isPortrait, bool isShowOrientationGuide)
{
	cout << "WaitResizeUI_hook" << endl;
	PrintStackTrace();
	if (config::freeform_window)
	{
		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}
	return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, bool, bool)>(WaitResizeUI_orig)(_this, config::freeform_window ? Gallop::StandaloneWindowResize::IsVirt() : isPortrait, config::ui_loading_show_orientation_guide ? false : isShowOrientationGuide);
}

static float GetCameraSizeByOrientation_hook(int orientation)
{
	return 5;
}

static UnityEngine::Vector2 get_DefaultResolution_hook()
{
	int width = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", IgnoreNumberOfArguments)();
	int height = il2cpp_symbols::get_method_pointer<int (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", IgnoreNumberOfArguments)();
	return UnityEngine::Vector2{ static_cast<float>(width), static_cast<float>(height) };
}

static void InitAddress()
{
	ShowNotification_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ShowNotification", 1);
	get_UITexture_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_UITexture", 0);
	get_CommonHeaderTitle_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "get_CommonHeaderTitle", 0);
	SetCameraSizeByOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "SetCameraSizeByOrientation", 1);
	CheckUIToFrameBufferBlitInstance_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "CheckUIToFrameBufferBlitInstance", 0);
	AdjustSafeArea_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustSafeArea", 0);
	AdjustMissionClearContentsRootRect_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustMissionClearContentsRootRect", 0);
	AdjustSafeAreaToAnnounceRect_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "AdjustSafeAreaToAnnounceRect", 0);
	ReleaseRenderTexture_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "ReleaseRenderTexture", 0);
	LockGameCanvas_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "LockGameCanvas", 0);
	UnlockGameCanvas_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "UIManager", "UnlockGameCanvas", 0);
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
		// ADD_HOOK(LockGameCanvas, "Gallop.UIManager::LockGameCanvas at %p\n");
		// ADD_HOOK(UnlockGameCanvas, "Gallop.UIManager::UnlockGameCanvas at %p\n");
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
		Il2CppObject* _uiCommandBuffer;
		il2cpp_field_get_value(instance, _uiCommandBufferField, &_uiCommandBuffer);

		return _uiCommandBuffer;
	}

	Il2CppObject* UIManager::_blitToFrameMaterial()
	{
		auto _blitToFrameMaterialField = il2cpp_class_get_field_from_name_wrap(instance->klass, "_blitToFrameMaterial");
		Il2CppObject* _blitToFrameMaterial;
		il2cpp_field_get_value(instance, _blitToFrameMaterialField, &_blitToFrameMaterial);

		return _blitToFrameMaterial;
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

	void UIManager::ShowNotification(Il2CppString* text)
	{
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
		reinterpret_cast<void (*)(Il2CppObject*)>(AdjustSafeAreaToAnnounceRect_addr)(instance);
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

#ifdef _MSC_VER
	void UIManager::ChangeResizeUIForPC(int width, int height)
	{
		reinterpret_cast<void (*)(Il2CppObject*, int, int)>(ChangeResizeUIForPC_addr)(instance, width, height);
	}
#endif
}
