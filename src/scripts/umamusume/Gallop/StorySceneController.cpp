#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StorySceneController.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Material.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Shader.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppMethodPointer StorySceneController_UpdateScene_addr = nullptr;
	void* StorySceneController_UpdateScene_orig = nullptr;

	Il2CppMethodPointer StorySceneController_UpdateFovFactor_addr = nullptr;
	void* StorySceneController_UpdateFovFactor_orig = nullptr;

	Il2CppMethodPointer StorySceneController_SetMotionSeEnable_addr = nullptr;
	Il2CppMethodPointer StorySceneController_get_BgTempMaterialArray_addr = nullptr;

	FieldInfo* StorySceneController__isPausedField = nullptr;
	FieldInfo* StorySceneController__storyManager = nullptr;
	FieldInfo* StorySceneController__isFlowBgEnabledField = nullptr;
	FieldInfo* StorySceneController__flowBgIndex = nullptr;
	FieldInfo* StorySceneController__flowBgTextureList = nullptr;

	float _flowBgTimer = 0.f;
	const float FLOW_BG_INTERVAL = 1.f / 30.f;
}

static void StorySceneController_UpdateFovFactor_hook(Il2CppObject* self, float fovFactor)
{
	reinterpret_cast<decltype(StorySceneController_UpdateFovFactor_hook)*>(StorySceneController_UpdateFovFactor_orig)(self, 1);
}

static void StorySceneController_UpdateScene_hook(Il2CppObject* self)
{
	bool _isPaused;
	il2cpp_field_get_value(self, StorySceneController__isPausedField, &_isPaused);

	if (_isPaused)
	{
		return;
	}

	auto StoryChoiceController = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryChoiceController"));
	il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(StoryChoiceController->klass, "CheckChoiceAutoTap", 0)(StoryChoiceController);
	Gallop::StorySceneController(self).UpdateFlowBackground();

	Il2CppObject* _storyManager;
	il2cpp_field_get_value(self, StorySceneController__storyManager, &_storyManager);

	auto _viewField = il2cpp_class_get_field_from_name(_storyManager->klass, "_view");
	Il2CppObject* _view;
	il2cpp_field_get_value(_storyManager, _viewField, &_view);

	auto FadeLayer = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(_view->klass, "get_FadeLayer", 0)(_view);

	if (!il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>(FadeLayer->klass, "IsMotionSeEnable", 0)(FadeLayer))
	{
		reinterpret_cast<void (*)(Il2CppObject*, bool)>(StorySceneController_SetMotionSeEnable_addr)(self, false);
		return;
	}

	auto TimelineController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(_storyManager->klass, "get_TimelineController", 0)(_storyManager);

	auto WipeControllerField = il2cpp_class_get_field_from_name(TimelineController->klass, "WipeController");
	Il2CppObject* wipeController;
	il2cpp_field_get_value(TimelineController, WipeControllerField, &wipeController);

	auto IsMotionSeEnable = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>(wipeController->klass, "IsMotionSeEnable", 0)(wipeController);
	reinterpret_cast<void (*)(Il2CppObject*, bool)>(StorySceneController_SetMotionSeEnable_addr)(self, IsMotionSeEnable);
}

static void InitAddress()
{
	auto StorySceneController_klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StorySceneController");
	StorySceneController_UpdateScene_addr = il2cpp_symbols::get_method_pointer(StorySceneController_klass, "UpdateScene", 0);
	StorySceneController_UpdateFovFactor_addr = il2cpp_symbols::find_method(StorySceneController_klass, [](const MethodInfo* info)
		{
			if (Game::CurrentUnityVersion == Game::UnityVersion::Unity20)
			{
				auto info2020 = reinterpret_cast<const MethodInfo2020*>(info);
				return info2020->name == "UpdateFovFactor"s && info2020->parameters[0].parameter_type->type == Il2CppTypeEnum::IL2CPP_TYPE_R4;
			}

			return info->name == "UpdateFovFactor"s && info->parameters[0]->type == Il2CppTypeEnum::IL2CPP_TYPE_R4;
		}
	);
	StorySceneController_SetMotionSeEnable_addr = il2cpp_symbols::get_method_pointer(StorySceneController_klass, "SetMotionSeEnable", 1);
	StorySceneController_get_BgTempMaterialArray_addr = il2cpp_symbols::get_method_pointer(StorySceneController_klass, "get_BgTempMaterialArray", 0);

	StorySceneController__isPausedField = il2cpp_class_get_field_from_name(StorySceneController_klass, "_isPaused");
	StorySceneController__storyManager = il2cpp_class_get_field_from_name(StorySceneController_klass, "_storyManager");
	StorySceneController__isFlowBgEnabledField = il2cpp_class_get_field_from_name(StorySceneController_klass, "_isFlowBgEnabled");
	StorySceneController__flowBgIndex = il2cpp_class_get_field_from_name(StorySceneController_klass, "_flowBgIndex");
	StorySceneController__flowBgTextureList = il2cpp_class_get_field_from_name(StorySceneController_klass, "_flowBgTextureList");
}

static void HookMethods()
{
	if (config::max_fps > -1)
	{
		ADD_HOOK(StorySceneController_UpdateScene, "Gallop.StorySceneController::UpdateScene at %p\n");
	}

	if (config::freeform_window)
	{
		ADD_HOOK(StorySceneController_UpdateFovFactor, "Gallop.StorySceneController::UpdateFovFactor at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	void StorySceneController::UpdateFlowBackground()
	{
		bool _isFlowBgEnabled;
		il2cpp_field_get_value(instance, StorySceneController__isFlowBgEnabledField, &_isFlowBgEnabled);

		if (_isFlowBgEnabled)
		{
			_flowBgTimer += il2cpp_resolve_icall_type<float (*)()>("UnityEngine.Time::get_deltaTime()")();

			while (_flowBgTimer >= FLOW_BG_INTERVAL)
			{
				_flowBgTimer -= FLOW_BG_INTERVAL;
				int _flowBgIndex;
				il2cpp_field_get_value(instance, StorySceneController__flowBgIndex, &_flowBgIndex);

				_flowBgIndex++;
				if (_flowBgIndex >= 10)
				{
					_flowBgIndex = 0;
				}
				il2cpp_field_set_value(instance, StorySceneController__flowBgIndex, &_flowBgIndex);

				auto BgTempMaterialArray = reinterpret_cast<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*)>(StorySceneController_get_BgTempMaterialArray_addr)(instance);
				Il2CppObject* _flowBgTextureList;
				il2cpp_field_get_value(instance, StorySceneController__flowBgTextureList, &_flowBgTextureList);

				auto _itemsField = il2cpp_class_get_field_from_name(_flowBgTextureList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* _items;
				il2cpp_field_get_value(_flowBgTextureList, _itemsField, &_items);

				UnityEngine::Material BgTempMaterial = BgTempMaterialArray->vector[0];

				BgTempMaterial.SetTextureImpl(UnityEngine::Shader::PropertyToID(il2cpp_string_new("_MainTex")), _items->vector[_flowBgIndex]);
			}
		}
	}
}
