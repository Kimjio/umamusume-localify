#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TapEffectController.hpp"
#include "../../UnityEngine.InputLegacyModule/UnityEngine/Input.hpp"

#include "config/config.hpp"

namespace
{
	void* TapEffectController_Update_addr = nullptr;
	void* TapEffectController_Update_orig = nullptr;
}

static void TapEffectController_Update_hook(Il2CppObject* self)
{
	auto _isInitializeField = il2cpp_class_get_field_from_name(self->klass, "_isInitialize");
	bool _isInitialize;
	il2cpp_field_get_value(self, _isInitializeField, &_isInitialize);

	if (!_isInitialize)
	{
		return;
	}

	auto _isPressField = il2cpp_class_get_field_from_name(self->klass, "_isPress");
	bool _isPress;
	il2cpp_field_get_value(self, _isPressField, &_isPress);

	auto _touchPosField = il2cpp_class_get_field_from_name(self->klass, "_touchPos");

	auto _cameraField = il2cpp_class_get_field_from_name(self->klass, "_camera");
	auto _cameraTransformField = il2cpp_class_get_field_from_name(self->klass, "_cameraTransform");
	Il2CppObject* _camera;
	Il2CppObject* _cameraTransform;
	il2cpp_field_get_value(self, _cameraField, &_camera);
	il2cpp_field_get_value(self, _cameraTransformField, &_cameraTransform);

	//if (!_isPress)
	//{
	//	if (UnityEngine::Input::GetMouseButtonDown(0))
	//	{
	//		_isPress = true;
	//		il2cpp_field_set_value(self, _isPressField, &_isPress);
	//		Vector3 _touchPos = UnityEngine::Input::mousePosition();
	//		il2cpp_field_set_value(self, _touchPosField, &_touchPos);
	//	}

	//	if (_isPress)
	//	{
	//		Vector3 _touchPos;
	//		il2cpp_field_get_value(self, _touchPosField, &_touchPos);
	//		_touchPos = il2cpp_class_get_method_from_name_type<Vector3(*)(Il2CppObject*, Vector3)>(_camera->klass, "ScreenToWorldPoint", 1)->methodPointer(_camera, _touchPos);
	//		// _touchPos = UnityEngine::Transform(_cameraTransform).InverseTransformPoint(_touchPos);
	//		il2cpp_field_set_value(self, _touchPosField, &_touchPos);
	//		Il2CppObject* tapEffectParticle = nullptr;
	//		if (il2cpp_class_get_method_from_name_type<bool(*)(Il2CppObject*)>(self->klass, "IsTapReactionObject", 0)->methodPointer(self))
	//		{
	//			int num = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*, bool)>(self->klass, "SetTargetIndex", 1)->methodPointer(self, true);

	//			auto _tapEffectButtonArrayField = il2cpp_class_get_field_from_name(self->klass, "_tapEffectButtonArray");
	//			Il2CppArraySize_t<Il2CppObject*>* _tapEffectButtonArray;
	//			il2cpp_field_get_value(self, _tapEffectButtonArrayField, &_tapEffectButtonArray);

	//			tapEffectParticle = _tapEffectButtonArray->vector[num];
	//		}
	//		else
	//		{
	//			int num2 = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*, bool)>(self->klass, "SetTargetIndex", 1)->methodPointer(self, false);

	//			auto _tapEffectNormalArrayField = il2cpp_class_get_field_from_name(self->klass, "_tapEffectNormalArray");
	//			Il2CppArraySize_t<Il2CppObject*>* _tapEffectNormalArray;
	//			il2cpp_field_get_value(self, _tapEffectNormalArrayField, &_tapEffectNormalArray);

	//			tapEffectParticle = _tapEffectNormalArray->vector[num2];
	//		}
	//		if (tapEffectParticle == nullptr)
	//		{
	//			return;
	//		}
	//		auto _cacheTransformField = il2cpp_class_get_field_from_name(tapEffectParticle->klass, "_cacheTransform");
	//		Il2CppObject* cacheTransform;
	//		il2cpp_field_get_value(tapEffectParticle, _cacheTransformField, &cacheTransform);

	//		UnityEngine::Transform(cacheTransform).position(_touchPos);
	//		il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(tapEffectParticle->klass, "Play", 0)->methodPointer(tapEffectParticle);
	//		il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(self->klass, "UpdateTrailEffect", 0)->methodPointer(self);
	//		il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(self->klass, "PlayTrailEffect", 0)->methodPointer(self);
	//	}
	//	return;
	//}

	//if (UnityEngine::Input::GetMouseButton(0) || UnityEngine::Input::GetMouseButtonUp(0))
	//{
	//	Vector3 _touchPos = UnityEngine::Input::mousePosition();
	//	_touchPos = il2cpp_class_get_method_from_name_type<Vector3(*)(Il2CppObject*, Vector3)>(_camera->klass, "ScreenToWorldPoint", 1)->methodPointer(_camera, _touchPos);
	//	// _touchPos = UnityEngine::Transform(_cameraTransform).InverseTransformPoint(_touchPos);
	//	il2cpp_field_set_value(self, _touchPosField, &_touchPos);
	//	il2cpp_field_set_value(self, _touchPosField, &_touchPos);
	//	il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(self->klass, "UpdateTrailEffect", 0)->methodPointer(self);
	//	return;
	//}

	//_isPress = false;
	//il2cpp_field_set_value(self, _isPressField, &_isPress);
	//il2cpp_class_get_method_from_name_type<void(*)(Il2CppObject*)>(self->klass, "StopTrailEffect", 0)->methodPointer(self);
	reinterpret_cast<void (*)(Il2CppObject*)>(TapEffectController_Update_orig)(self);

}

static void InitAddress()
{
	TapEffectController_Update_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "TapEffectController", "Update", 0);
}

static void HookMethods()
{
	ADD_HOOK(TapEffectController_Update, "Gallop.TapEffectController::Update at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	// il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* TapEffectController::namespaze = "Gallop";
	const char* TapEffectController::klassName = "TapEffectController";

	void TapEffectController::Update()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(TapEffectController_Update_addr)(instance);
	}
}
