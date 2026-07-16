#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "StoryViewController.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Quaternion.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Transform.hpp"

namespace
{
	Il2CppMethodPointer StoryViewController_ApplyBackgroundOffset_addr = nullptr;
	void* StoryViewController_ApplyBackgroundOffset_orig = nullptr;

	Il2CppMethodPointer StoryViewController_get_IsSingleModeOrGallery_addr = nullptr;

	Il2CppMethodPointer StoryViewController_SetDisplayMode_addr = nullptr;

	Il2CppMethodPointer StoryViewController_SetupUIOnChangeOrientation_addr = nullptr;

	Il2CppMethodPointer StoryViewController_ShowChoiceRewardInfoButton_addr = nullptr;
	void* StoryViewController_ShowChoiceRewardInfoButton_orig = nullptr;

	FieldInfo* StoryViewController__use3DBgField = nullptr;
	FieldInfo* StoryViewController__controllerField = nullptr;
	FieldInfo* StoryViewController__eyeLevelOffsetField = nullptr;
}

static void StoryViewController_ApplyBackgroundOffset_hook(Il2CppObject* self, UnityEngine::Vector3 position, UnityEngine::Quaternion rotation, UnityEngine::Vector3 localScale)
{
	if (StoryViewController__use3DBgField)
	{
		bool _use3DBg;
		il2cpp_field_get_value(self, StoryViewController__use3DBgField, &_use3DBg);

		if (_use3DBg)
		{
			return;
		}
	}

	Il2CppObject* _controller;
	il2cpp_field_get_value(self, StoryViewController__controllerField, &_controller);

	auto CameraController = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(_controller->klass, "get_CameraController", 0)(_controller);
	UnityEngine::Transform bgOffset = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(CameraController->klass, "get_BgOffset", 0)(CameraController);

	UnityEngine::Vector3 _eyeLevelOffset;
	il2cpp_field_get_value(self, StoryViewController__eyeLevelOffsetField, &_eyeLevelOffset);

	bgOffset.position(position + _eyeLevelOffset);
	bgOffset.rotation(rotation);
	bgOffset.localScale(localScale);
}

static void StoryViewController_ShowChoiceRewardInfoButton_hook(Il2CppObject* self, Il2CppObject* activeChoiceParamList, int currentBlockIndex)
{
	reinterpret_cast<decltype(StoryViewController_ShowChoiceRewardInfoButton_hook)*>(StoryViewController_ShowChoiceRewardInfoButton_orig)(self, activeChoiceParamList, currentBlockIndex);

	auto StoryChoiceController = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StoryChoiceController"));
	auto _onCloseButtonField = il2cpp_class_get_field_from_name(StoryChoiceController->klass, "_onCloseButton");

	auto _onCloseButton = CreateDelegate(self,
		*[](Il2CppObject* self)
		{
			auto view = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(self->klass, "GetViewBase", 0)(self);
			auto ChoiceRewardInfoButton = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(view->klass, "get_ChoiceRewardInfoButton", 0)(view);

			auto _buttonRootField = il2cpp_class_get_field_from_name(ChoiceRewardInfoButton->klass, "_buttonRoot");
			Il2CppObject* _buttonRoot;
			il2cpp_field_get_value(ChoiceRewardInfoButton, _buttonRootField, &_buttonRoot);

			if (UnityEngine::Object::IsNativeObjectAlive(_buttonRoot))
			{
				UnityEngine::GameObject(_buttonRoot).SetActive(false);
			}
		}
	);
	il2cpp_field_set_value(StoryChoiceController, _onCloseButtonField, _onCloseButton);
}

static void InitAddress()
{
	auto StoryViewController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "StoryViewController");
	StoryViewController_ApplyBackgroundOffset_addr = il2cpp_symbols::get_method_pointer(StoryViewController_klass, "ApplyBackgroundOffset", 3);
	StoryViewController_get_IsSingleModeOrGallery_addr = il2cpp_symbols::get_method_pointer(StoryViewController_klass, "get_IsSingleModeOrGallery", 0);
	StoryViewController_SetDisplayMode_addr = il2cpp_symbols::get_method_pointer(StoryViewController_klass, "SetDisplayMode", 1);
	StoryViewController_SetupUIOnChangeOrientation_addr = il2cpp_symbols::get_method_pointer(StoryViewController_klass, "SetupUIOnChangeOrientation", 0);
	StoryViewController_ShowChoiceRewardInfoButton_addr = il2cpp_symbols::get_method_pointer(StoryViewController_klass, "ShowChoiceRewardInfoButton", 2);
	StoryViewController__use3DBgField = il2cpp_class_get_field_from_name(StoryViewController_klass, "_use3DBg");
	StoryViewController__controllerField = il2cpp_class_get_field_from_name(StoryViewController_klass, "_controller");
	StoryViewController__eyeLevelOffsetField = il2cpp_class_get_field_from_name(StoryViewController_klass, "_eyeLevelOffset");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(StoryViewController_ApplyBackgroundOffset, "Gallop.StoryViewController::ApplyBackgroundOffset at %p\n");
		ADD_HOOK(StoryViewController_ShowChoiceRewardInfoButton, "Gallop.StoryViewController::ShowChoiceRewardInfoButton at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	bool StoryViewController::IsSingleModeOrGallery()
	{
		return reinterpret_cast<bool (*)(Il2CppObject*)>(StoryViewController_get_IsSingleModeOrGallery_addr)(instance);
	}

	void StoryViewController::SetDisplayMode(uint64_t displayMode)
	{
		reinterpret_cast<void (*)(Il2CppObject*, uint64_t)>(StoryViewController_SetDisplayMode_addr)(instance, displayMode);
	}

	void StoryViewController::SetupUIOnChangeOrientation()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(StoryViewController_SetupUIOnChangeOrientation_addr)(instance);
	}
}
