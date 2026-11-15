#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LandscapeModeSplashVisual.hpp"

#include "LandscapeUIManager.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer LandscapeModeSplashVisual_Awake_addr = nullptr;
	void* LandscapeModeSplashVisual_Awake_orig = nullptr;

	FieldInfo* LandscapeModeSplashVisual__sourceImageField = nullptr;
	FieldInfo* LandscapeModeSplashVisual__targetRawImageField = nullptr;
	FieldInfo* LandscapeModeSplashVisual__targetRectTransformField = nullptr;
	FieldInfo* LandscapeModeSplashVisual__canvasScalerField = nullptr;
}

static void LandscapeModeSplashVisual_Awake_hook(Il2CppObject* self)
{
	if (UnityEngine::Screen::width() < UnityEngine::Screen::height())
	{
		return;
	}

	Il2CppObject* _sorceImage;
	il2cpp_field_get_value(self, LandscapeModeSplashVisual__sourceImageField, &_sorceImage);

	Il2CppObject* _targetRawImage;
	il2cpp_field_get_value(self, LandscapeModeSplashVisual__targetRawImageField, &_targetRawImage);

	Il2CppObject* _targetRectTransform;
	il2cpp_field_get_value(self, LandscapeModeSplashVisual__targetRectTransformField, &_targetRectTransform);

	Il2CppObject* _canvasScaler;
	il2cpp_field_get_value(self, LandscapeModeSplashVisual__canvasScalerField, &_canvasScaler);

	if (_canvasScaler)
	{
		auto size = UnityEngine::Vector2{ .y = static_cast<float>(Gallop::LandscapeUIManager::GetWindowsHeightMax()) };
		size.x = size.y * ratio_16_9;

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, UnityEngine::Vector2)>(_canvasScaler->klass, "set_referenceResolution", 1)(_canvasScaler, size);
		UnityEngine::RectTransform(_targetRectTransform).sizeDelta(size);
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>(_targetRawImage->klass, "set_texture", 1)(_targetRawImage, _sorceImage);
	}
}

static void InitAddress()
{
	auto LandscapeModeSplashVisual_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LandscapeModeSplashVisual");
	LandscapeModeSplashVisual_Awake_addr = il2cpp_symbols::get_method_pointer(LandscapeModeSplashVisual_klass, "Awake", 0);
	LandscapeModeSplashVisual__sourceImageField = il2cpp_class_get_field_from_name(LandscapeModeSplashVisual_klass, "_sourceImage");
	LandscapeModeSplashVisual__targetRawImageField = il2cpp_class_get_field_from_name(LandscapeModeSplashVisual_klass, "_targetRawImage");
	LandscapeModeSplashVisual__targetRectTransformField = il2cpp_class_get_field_from_name(LandscapeModeSplashVisual_klass, "_targetRectTransform");
	LandscapeModeSplashVisual__canvasScalerField = il2cpp_class_get_field_from_name(LandscapeModeSplashVisual_klass, "_canvasScaler");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(LandscapeModeSplashVisual_Awake, "Gallop.LandscapeModeSplashVisual::Awake at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
