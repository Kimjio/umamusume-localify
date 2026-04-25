#include "NotificationManager.hpp"
#include "scripts/ScriptInternal.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/CastHelper.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"
#include "scripts/umamusume/Gallop/ImageCommon.hpp"
#include "scripts/umamusume/Gallop/TextFontManager.hpp"

#include "config/config.hpp"

using namespace std;
using namespace UnityEngine;

namespace Localify
{
	set<void*> NotificationManager::currentPlayerHandles;
	set<void*> NotificationManager::currentAcbHandles;
	void* NotificationManager::currentPlayerHandle;
	Il2CppObject* NotificationManager::currentElem;
	Il2CppObject* NotificationManager::notification = nullptr;
	bool NotificationManager::isRequiredInit = true;

	void NotificationManager::Init()
	{
		if (!isRequiredInit)
		{
			return;
		}

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _noticeCanvas = uiManager._noticeCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_noticeCanvas->klass, "get_transform", 0)->methodPointer(_noticeCanvas);

		auto object = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppString*, Il2CppReflectionType*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Resources", "Load", 2)(
			il2cpp_string_new("UI/Parts/Notification"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		auto instantiated = UnityEngine::Object::Internal_CloneSingleWithParent(object, transform, false);
		auto helper = new UnityEngine::CastHelper{};
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppReflectionType*, uintptr_t*)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentFastPath", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "Notification"), &helper->oneFurtherThanResultValue);
		notification = helper->obj;
		delete helper;

		auto canvasGroupField = il2cpp_class_get_field_from_name(notification->klass, "canvasGroup");
		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_fontSize", 1)->methodPointer(_Label, config::character_system_text_caption_font_size);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_FontColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), config::character_system_text_caption_font_color.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineSize", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), config::character_system_text_caption_outline_size.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), config::character_system_text_caption_outline_color.data())));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_Label->klass, "RebuildOutline", 0)->methodPointer(_Label);

		auto background = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentInChildren", 2)(instantiated, GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(background->klass, "SetAlpha", 1)->methodPointer(background, config::character_system_text_caption_background_alpha);

		auto canvasGroupTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasGroup->klass, "get_transform", 0)->methodPointer(canvasGroup);

		auto position = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(canvasGroupTransform->klass, "get_position", 0)->methodPointer(canvasGroupTransform);

		position.x = config::character_system_text_caption_position_x;
		position.y = config::character_system_text_caption_position_y;

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(canvasGroupTransform->klass, "set_position", 1)->methodPointer(canvasGroupTransform, position);

		auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
		if (gameObject)
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, false);
			isRequiredInit = false;
		}
		else
		{
			notification = nullptr;
			isRequiredInit = true;
		}
	}

	void NotificationManager::Cleanup()
	{
		if (notification && UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
			if (gameObject)
			{
				auto _tweenerField = il2cpp_class_get_field_from_name(notification->klass, "_tweener");
				Il2CppObject* _tweener;
				il2cpp_field_get_value(notification, _tweenerField, &_tweener);
				if (_tweener)
				{
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
					_tweener = nullptr;
					il2cpp_field_set_value(notification, _tweenerField, _tweener);
				}
			}
		}
	}

	void NotificationManager::Reset()
	{
		isRequiredInit = true;
		notification = nullptr;
	}

	void NotificationManager::Show(Il2CppString* text)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto canvasGroupField = il2cpp_class_get_field_from_name(notification->klass, "canvasGroup");
		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);
		
		if (!canvasGroup)
		{
			return;
		}

		auto _tweenerField = il2cpp_class_get_field_from_name(notification->klass, "_tweener");
		Il2CppObject* _tweener;
		il2cpp_field_get_value(notification, _tweenerField, &_tweener);

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		auto _displayTimeField = il2cpp_class_get_field_from_name(notification->klass, "_displayTime");
		float  _displayTime;
		il2cpp_field_get_value(notification, _displayTimeField, &_displayTime);

		auto _fadeOutTimeField = il2cpp_class_get_field_from_name(notification->klass, "_fadeOutTime");
		float _fadeOutTime;
		il2cpp_field_get_value(notification, _fadeOutTimeField, &_fadeOutTime);

		if (_tweener)
		{
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>("DOTween.dll", "DG.Tweening", "TweenExtensions", "Complete", 2)(_tweener, true);
			_tweener = nullptr;
			il2cpp_field_set_value(notification, _tweenerField, _tweener);
		}
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(_Label->klass, "set_text", 1)->methodPointer(_Label, text);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(canvasGroup->klass, "set_alpha", 1)->methodPointer(canvasGroup, 1);

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(notification->klass, "get_gameObject", 0)->methodPointer(notification);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

		_tweener = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, float, float)>("Plugins.dll", "DG.Tweening", "DOTweenModuleUI", "DOFade", 3)(canvasGroup, 0, _fadeOutTime);

		auto delayField = il2cpp_class_get_field_from_name(_tweener->klass, "delay");
		float delay = _displayTime;
		il2cpp_field_set_value(_tweener, delayField, &delay);

		auto delayCompleteField = il2cpp_class_get_field_from_name(_tweener->klass, "delayComplete");
		bool delayComplete = delay <= 0;
		il2cpp_field_set_value(_tweener, delayCompleteField, &delayComplete);

		auto onCompleteField = il2cpp_class_get_field_from_name(_tweener->klass, "onComplete");
		auto callback = CreateDelegateWithClass(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), notification, *([](Il2CppObject* self)
			{
				auto gameObject = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_gameObject", 0)(notification);
				if (gameObject)
				{
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, false);

					auto _tweenerField = il2cpp_class_get_field_from_name(notification->klass, "_tweener");
					il2cpp_field_set_value(notification, _tweenerField, nullptr);
				}
			}));
		il2cpp_field_set_value(_tweener, onCompleteField, callback);

		il2cpp_field_set_value(notification, _tweenerField, _tweener);
	}

	void NotificationManager::SetDisplayTime(float time)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _displayTimeField = il2cpp_class_get_field_from_name(notification->klass, "_displayTime");
		il2cpp_field_set_value(notification, _displayTimeField, &time);
	}

	void NotificationManager::SetFontSize(int size)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_fontSize", 1)->methodPointer(_Label, size);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_resizeTextMaxSize", 1)->methodPointer(_Label, size);
	}

	void NotificationManager::SetFontColor(il2cppstring color)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_FontColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "FontColorType"), color)));
	}

	void NotificationManager::SetOutlineSize(il2cppstring size)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineSize", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineSizeType"), size)));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_Label->klass, "UpdateOutline", 0)->methodPointer(_Label);
	}

	void NotificationManager::SetOutlineColor(il2cppstring color)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto _LabelField = il2cpp_class_get_field_from_name(notification->klass, "_Label");
		Il2CppObject* _Label;
		il2cpp_field_get_value(notification, _LabelField, &_Label);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_Label->klass, "set_OutlineColor", 1)->methodPointer(_Label, GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "OutlineColorType"), color)));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_Label->klass, "RebuildOutline", 0)->methodPointer(_Label);
	}

	void NotificationManager::SetBackgroundAlpha(float alpha)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(notification->klass, "get_gameObject", 0)->methodPointer(notification);
		auto background = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponentInChildren", 2)(gameObject, GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(background->klass, "SetAlpha", 1)->methodPointer(background, alpha);
	}

	void NotificationManager::SetPosition(float x, float y)
	{
		if (!notification || !UnityEngine::Object::IsNativeObjectAlive(notification))
		{
			return;
		}

		auto canvasGroupField = il2cpp_class_get_field_from_name(notification->klass, "canvasGroup");

		if (!canvasGroupField)
		{
			return;
		}

		Il2CppObject* canvasGroup;
		il2cpp_field_get_value(notification, canvasGroupField, &canvasGroup);

		if (!canvasGroup || !UnityEngine::Object::IsNativeObjectAlive(canvasGroup))
		{
			return;
		}

		auto canvasGroupTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(canvasGroup->klass, "get_transform", 0)->methodPointer(canvasGroup);

		auto position = il2cpp_class_get_method_from_name_type<UnityEngine::Vector3(*)(Il2CppObject*)>(canvasGroupTransform->klass, "get_position", 0)->methodPointer(canvasGroupTransform);

		position.x = x;
		position.y = y;

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector3)>(canvasGroupTransform->klass, "set_position", 1)->methodPointer(canvasGroupTransform, position);
	}
}
