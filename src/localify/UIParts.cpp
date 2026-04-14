#include "UIParts.hpp"
#include "scripts/ScriptInternal.hpp" 
#include <vector>
#include <string>

#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ResourcesAPIInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Color.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RenderTexture.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector4.hpp"

#include "masterdb/masterdb.hpp"

#include "string_utils.hpp"

using namespace std;

namespace Localify
{
	static Il2CppArraySize_t<Il2CppObject*>* GetRectTransformArray(UnityEngine::GameObject object)
	{
		return object.GetComponentsInChildren(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"), false);
	}

	static UnityEngine::RectTransform GetRectTransform(UnityEngine::GameObject object)
	{
		auto rectTransformArray = GetRectTransformArray(object);

		if (rectTransformArray->max_length)
		{
			return rectTransformArray->vector[0];
		}

		return nullptr;
	}

	void UIParts::SetImageCommonAtlasSprite(Il2CppObject* imageCommon, const Il2CppChar* atlasName, const Il2CppChar* spriteName)
	{
		auto _atlasNameField = il2cpp_class_get_field_from_name(imageCommon->klass, "_atlasName");
		il2cpp_field_set_value(imageCommon, _atlasNameField, il2cpp_string_new16(atlasName));

		auto _spriteNameField = il2cpp_class_get_field_from_name(imageCommon->klass, "_spriteName");
		il2cpp_field_set_value(imageCommon, _spriteNameField, il2cpp_string_new16(spriteName));
	}

	UnityEngine::GameObject UIParts::GetOptionItemTitle(const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemtitle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemTitle = UnityEngine::Object::Internal_CloneSingle(object);

		auto array = optionItemTitle.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.text(il2cpp_string_new16(title));

		return optionItemTitle;
	}

	UnityEngine::GameObject UIParts::GetOptionItemOnOff(const char* name, const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemonoff"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemOnOff = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItemOnOff, il2cpp_string_new(name));

		auto array = optionItemOnOff.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.verticalOverflow(UnityEngine::VerticalWrapMode::Overflow);
		textCommon.text(il2cpp_string_new16(title));

		return optionItemOnOff;
	}

	UnityEngine::GameObject UIParts::GetOptionItemOnOffQualityRich(const char* name, const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptioniteminfo_qualityrich"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemOnOff = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItemOnOff, il2cpp_string_new(name));

		auto array = optionItemOnOff.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.verticalOverflow(UnityEngine::VerticalWrapMode::Overflow);
		textCommon.text(il2cpp_string_new16(title));

		return optionItemOnOff;
	}

	void UIParts::SetOptionItemOnOffAction(const char* name, bool isOn, void (*onChange)(Il2CppObject*, bool))
	{
		auto optionItemOnOff = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemOnOff)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppReflectionType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionItemOnOff, GetRuntimeType("umamusume.dll", "Gallop", "PartsOnOffToggleSwitch"), true, true, false, false, nullptr);

			auto toggleSwitch = array->vector[0];

			auto action = CreateUnityAction(toggleSwitch, onChange);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool, Il2CppDelegate*)>(toggleSwitch->klass, "Setup", 2)->methodPointer(toggleSwitch, isOn, &action->delegate);
		}
	}

	bool UIParts::GetOptionItemOnOffIsOn(const char* name)
	{
		auto optionItemOnOff = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(il2cpp_resolve_icall("UnityEngine.GameObject::Find()"))(il2cpp_string_new(name));

		if (optionItemOnOff)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppReflectionType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemOnOff->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionItemOnOff, GetRuntimeType("umamusume.dll", "Gallop", "PartsOnOffToggleSwitch"), true, true, false, false, nullptr);

			auto toggleSwitch = array->vector[0];

			return il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(toggleSwitch->klass, "get_IsOn", 0)->methodPointer(toggleSwitch);
		}
		return false;
	}

	UnityEngine::GameObject UIParts::GetOptionItemButton(const char* name, const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitembutton"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemButton = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItemButton, il2cpp_string_new(name));

		auto array1 = optionItemButton.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array1->vector[0]);
		textCommon.text(il2cpp_string_new16(title));

		auto array2 = optionItemButton.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ButtonCommon"), false);

		auto buttonCommon = array2->vector[0];

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(buttonCommon->klass, "SetInteractable", 1)->methodPointer(buttonCommon, true);

		return optionItemButton;
	}

	void UIParts::SetOptionItemButtonAction(const char* name, void (*onClick)(Il2CppObject*))
	{
		auto optionItemButton = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();

		if (optionItemButton)
		{
			auto getComponents = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppReflectionType*, bool, bool, bool, bool, Il2CppObject*)>(optionItemButton->klass, "GetComponentsInternal", 6)->methodPointer;
			auto array = getComponents(optionItemButton, GetRuntimeType("umamusume.dll", "Gallop", "ButtonCommon"), true, true, false, false, nullptr);

			auto buttonCommon = array->vector[0];

			auto action = CreateUnityAction(buttonCommon, onClick);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(buttonCommon->klass, "SetOnClick", 1)->methodPointer(buttonCommon, &action->delegate);
		}
	}

	UnityEngine::GameObject UIParts::GetOptionItemAttention(const Il2CppChar* text)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemattention"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemAttention = UnityEngine::Object::Internal_CloneSingle(object);

		auto array = optionItemAttention.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.text(il2cpp_string_new16(text));

		return optionItemAttention;
	}

	UnityEngine::GameObject UIParts::GetOptionItemInfo(const char* name, const Il2CppChar* text)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptioniteminfo"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemInfo = UnityEngine::Object::Internal_CloneSingle(object);

		if (name)
		{
			UnityEngine::Object::Name(optionItemInfo, il2cpp_string_new((string(name) + "_info"s).data()));
		}

		auto array = optionItemInfo.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).SetTextWithCustomTag(il2cpp_string_new16(text));

		auto contentSizeFitter = optionItemInfo.AddComponent(GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto verticalLayoutGroup = optionItemInfo.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 64);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_right", 1)->methodPointer(padding, 64);

		auto _layoutField = il2cpp_class_get_field_from_name(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		return optionItemInfo;
	}

	UnityEngine::GameObject UIParts::GetOptionItemSimple(const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemsimple"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemSimple = UnityEngine::Object::Internal_CloneSingle(object);

		auto array = optionItemSimple.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));

		return optionItemSimple;
	}

	UnityEngine::GameObject UIParts::GetOptionItemSimpleWithButton(const char* name, const Il2CppChar* title, const Il2CppChar* text)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitemsimple"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItemSimple = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItemSimple, il2cpp_string_new((string(name) + "_simple"s).data()));

		auto rectTransform = GetRectTransform(optionItemSimple);

		rectTransform.anchoredPosition({ 71.583984375, -18 });

		UnityEngine::GameObject buttonObject = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/buttons00"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject buttons00 = UnityEngine::Object::Internal_CloneSingle(buttonObject);

		UnityEngine::Object::Name(buttons00, il2cpp_string_new(name));

		auto array2 = buttons00.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ButtonCommon"), false);

		auto buttonCommon = array2->vector[0];

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(buttonCommon->klass, "SetInteractable", 1)->methodPointer(buttonCommon, true);

		auto buttonRectTransform = GetRectTransform(buttons00);

		buttonRectTransform.sizeDelta({ 167, 67 });

		buttonRectTransform.anchoredPosition({ 382.5, 0 });

		AddToLayout(rectTransform, vector{ buttons00.NativeObject() });

		auto array = optionItemSimple.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(text));
		Gallop::TextCommon(array->vector[1]).text(il2cpp_string_new16(title));

		return optionItemSimple;
	}

	Gallop::TextCommon UIParts::GetOptionItemSimpleWithButtonTextCommon(const char* name)
	{
		UnityEngine::GameObject gameObject = UnityEngine::GameObject::Find(il2cpp_string_new((string(name) + "_simple"s).data())).NativeObject();

		if (gameObject)
		{
			auto array = gameObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

			return array->vector[1];
		}
		return nullptr;
	}

	// Toggles
	Il2CppObject* UIParts::GetToggleCommon(const char* name)
	{
		UnityEngine::GameObject toggleObject = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();
		if (toggleObject)
		{
			auto array = toggleObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ToggleCommon"), false);
			return array->vector[0];
		}
		return nullptr;
	}

	Il2CppObject* UIParts::GetToggleGroupCommon(UnityEngine::GameObject toggleGroupObject)
	{
		if (toggleGroupObject)
		{
			auto array = toggleGroupObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"), false);
			return array->vector[0];
		}
		return nullptr;
	}

	Il2CppObject* UIParts::GetToggleGroupCommon(const char* name)
	{
		UnityEngine::GameObject toggleGroupCommon = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();
		if (toggleGroupCommon)
		{
			return GetToggleGroupCommon(toggleGroupCommon);
		}
		return nullptr;
	}

	int UIParts::GetToggleGroupCommonValue(const char* name)
	{
		UnityEngine::GameObject gameObject = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();
		if (gameObject)
		{
			auto array = gameObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"), false);
			auto toggleGroupCommon = array->vector[0];
			return il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(toggleGroupCommon->klass, "GetOnIndex", 0)->methodPointer(toggleGroupCommon);
		}
		return -1;
	}

	void UIParts::AddToLayout(Il2CppObject* parentRectTransform, vector<Il2CppObject*> objects, bool setAsFirstSibling)
	{
		for (int i = objects.size() - 1; i >= 0; i--)
		{
			if (objects[i])
			{
				auto rectTransform = GetRectTransform(objects[i]);
				if (rectTransform)
				{
					rectTransform.SetParent(parentRectTransform, false);

					if (setAsFirstSibling)
					{
						rectTransform.SetAsFirstSibling();
					}
				}
			}
		}
	}

	Gallop::TextCommon UIParts::GetTextCommon(const char* name)
	{
		UnityEngine::GameObject gameObject = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();
		if (gameObject)
		{
			auto array = gameObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);
			return array->vector[0];
		}
		return nullptr;
	}

	Gallop::TextCommon UIParts::GetOptionSliderNumText(Il2CppObject* slider)
	{
		UnityEngine::GameObject gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(slider->klass, "get_gameObject", 0)->methodPointer(slider);
		auto array = gameObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);
		return array->vector[0];
	}

	float UIParts::GetOptionSliderValue(Il2CppObject* slider)
	{
		return il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(slider->klass, "get_value", 0)->methodPointer(slider);
	}

	float UIParts::GetOptionSliderValue(const char* name)
	{
		UnityEngine::GameObject optionSlider = UnityEngine::GameObject::Find(il2cpp_string_new(name));
		if (optionSlider)
		{
			auto array = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "SliderCommon"), false);
			auto sliderCommon = array->vector[0];
			return GetOptionSliderValue(sliderCommon);
		}
		return 0;
	}

	Il2CppObject* UIParts::GetOptionSlider(const char* name)
	{
		UnityEngine::GameObject optionSlider = UnityEngine::GameObject::Find(il2cpp_string_new(name));
		if (optionSlider)
		{
			auto array = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "SliderCommon"), false);
			return array->vector[0];
		}
		return nullptr;
	}

	UnityEngine::GameObject UIParts::GetOptionSlider(const char* name, const Il2CppChar* title, float value, float min, float max, bool wholeNumbers, void (*onChange)(Il2CppObject*))
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/optionsoundvolumeslider"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionSlider = UnityEngine::Object::Internal_CloneSingle(object);

		auto array = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.verticalOverflow(UnityEngine::VerticalWrapMode::Overflow);
		textCommon.text(il2cpp_string_new16(title));

		auto optionSoundVolumeSliderArray = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "OptionSoundVolumeSlider"), false);

		auto optionSoundVolumeSlider = optionSoundVolumeSliderArray->vector[0];

		UnityEngine::Object::Destroy(optionSoundVolumeSlider);

		auto sliderCommonArray = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "SliderCommon"), false);

		auto sliderCommon = sliderCommonArray->vector[0];

		auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sliderCommon->klass, "get_onValueChanged", 0)->methodPointer(sliderCommon);

		auto AddCall = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(onValueChanged->klass, "AddCall", 1);

		auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.single_class));

		auto valueChanged = CreateDelegateWithClass(delegateClass, sliderCommon,
			onChange ? onChange :
			*[](Il2CppObject* self)
			{
				auto textCommon = GetOptionSliderNumText(self);

				if (il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(self->klass, "get_wholeNumbers", 0)->methodPointer(self))
				{
					auto value = static_cast<int>(il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(self->klass, "get_value", 0)->methodPointer(self));

					textCommon.text(il2cpp_string_new16(u8_il2cpp(to_string(value)).data()));
				}
				else
				{
					auto value = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(self->klass, "get_value", 0)->methodPointer(self);

					value = roundf(value * 100) / 100;

					textCommon.text(il2cpp_string_new16(format(IL2CPP_STRING("{:.2f}"), value).data()));
				}
			});

		auto invokeableCall = il2cpp_object_new(GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "InvokableCall`1"), GetRuntimeType(il2cpp_defaults.single_class)));

		auto delegateField = il2cpp_class_get_field_from_name(invokeableCall->klass, "Delegate");
		il2cpp_field_set_value(invokeableCall, delegateField, valueChanged);

		AddCall->methodPointer(onValueChanged, invokeableCall);

		try
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(sliderCommon->klass, "set_wholeNumbers", 1)->methodPointer(sliderCommon, wholeNumbers);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_minValue", 1)->methodPointer(sliderCommon, min);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_maxValue", 1)->methodPointer(sliderCommon, max);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_value", 1)->methodPointer(sliderCommon, value);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			cout << e.ex->klass->name << ": ";
			wcout << e.ex->message << endl;
		}

		auto transformArray = GetRectTransformArray(optionSlider);

		vector<Il2CppObject*> destroyTargets;

		for (int i = 0; i < transformArray->max_length; i++)
		{
			UnityEngine::RectTransform transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("ToggleMute")) ||
					UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("ImageIcon")) ||
					UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Line")))
				{
					destroyTargets.emplace_back(transform);
				}
			}
		}

		for (int i = 0; i < destroyTargets.size(); i++)
		{
			UnityEngine::RectTransform transform = destroyTargets[i];
			transform.SetParent(nullptr, false);
			UnityEngine::Object::Destroy(transform.gameObject());
		}

		destroyTargets.clear();

		transformArray = GetRectTransformArray(optionSlider);

		for (int i = 0; i < transformArray->max_length; i++)
		{
			UnityEngine::RectTransform transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Slider")))
				{
					transform.sizeDelta({ 560, 24 });
					break;
				}
			}
		}

		auto gameObject = UnityEngine::GameObject();

		UnityEngine::Object::Name(gameObject, il2cpp_string_new(name));

		UnityEngine::RectTransform rootTransform = gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		rootTransform.sizeDelta({ 0, 0 });

		rootTransform.anchorMax({ 0, 0 });

		rootTransform.anchorMin({ 0, 0 });

		rootTransform.pivot({ 0.5, 1 });

		rootTransform.anchoredPosition({ 0, 0 });

		auto verticalLayoutGroup = gameObject.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 54);
		/*il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 32);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 32);*/

		auto sliderTransform = GetRectTransform(optionSlider);

		sliderTransform.sizeDelta({ 1000, 86 });
		sliderTransform.SetParent(rootTransform, false);

		return gameObject;
	}

	void UIParts::SetOptionSliderAction(const char* name, void (*onChange)(Il2CppObject*))
	{
		UnityEngine::GameObject optionSlider = UnityEngine::GameObject::Find(il2cpp_string_new(name)).NativeObject();

		if (optionSlider)
		{
			auto array = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "SliderCommon"), false);

			auto sliderCommon = array->vector[0];

			auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sliderCommon->klass, "get_onValueChanged", 0)->methodPointer(sliderCommon);

			auto AddCall = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(onValueChanged->klass, "AddCall", 1);

			auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.single_class));

			auto valueChanged = CreateDelegateWithClass(delegateClass, sliderCommon, onChange);

			auto invokeableCall = il2cpp_object_new(GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "InvokableCall`1"), GetRuntimeType(il2cpp_defaults.single_class)));

			auto delegateField = il2cpp_class_get_field_from_name(invokeableCall->klass, "Delegate");
			il2cpp_field_set_value(invokeableCall, delegateField, valueChanged);

			AddCall->methodPointer(onValueChanged, invokeableCall);
		}
	}

	UnityEngine::GameObject UIParts::GetCheckbox(const char* name)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/checkbox"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		UnityEngine::GameObject checkbox = UnityEngine::Object::Internal_CloneSingle(object);
		return checkbox;
	}

	UnityEngine::GameObject UIParts::GetCheckboxWithText(const char* name)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/checkboxwithtext"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		UnityEngine::GameObject checkboxWithText = UnityEngine::Object::Internal_CloneSingle(object);
		return checkboxWithText;
	}

	UnityEngine::GameObject UIParts::GetRadioButtonWithText(const char* name, const Il2CppChar* title)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/radiobuttonwithtext"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		UnityEngine::GameObject radioButtonWithText = UnityEngine::Object::Internal_CloneSingle(object);
		UnityEngine::Object::Name(radioButtonWithText, il2cpp_string_new(name));
		auto array = radioButtonWithText.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);
		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));
		return radioButtonWithText;
	}

	UnityEngine::GameObject UIParts::GetRadioButtonWithImageAndText(const char* name, const Il2CppChar* title, const Il2CppChar* atlasName, const Il2CppChar* spriteName)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/radiobuttonwithimageandtext"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		UnityEngine::GameObject radioButtonWithText = UnityEngine::Object::Internal_CloneSingle(object);
		UnityEngine::Object::Name(radioButtonWithText, il2cpp_string_new(name));
		auto imageArray = radioButtonWithText.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), false);
		SetImageCommonAtlasSprite(imageArray->vector[0], atlasName, spriteName);

		auto array = radioButtonWithText.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);
		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));
		return radioButtonWithText;
	}

	UnityEngine::GameObject UIParts::GetOptionItem3ToggleVertical(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, const Il2CppChar* option3, int selectedIndex)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem3togglevertical"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItem3ToggleVertical = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItem3ToggleVertical, il2cpp_string_new(name));

		auto array = optionItem3ToggleVertical.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));
		Gallop::TextCommon(array->vector[1]).text(il2cpp_string_new16(option1));
		Gallop::TextCommon(array->vector[2]).text(il2cpp_string_new16(option2));
		Gallop::TextCommon(array->vector[3]).text(il2cpp_string_new16(option3));

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem3ToggleVertical);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem3ToggleVertical;
	}

	UnityEngine::GameObject UIParts::GetOptionItem3Toggle(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, const Il2CppChar* option3, int selectedIndex)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem3toggle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItem3Toggle = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItem3Toggle, il2cpp_string_new(name));

		auto array = optionItem3Toggle.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));
		Gallop::TextCommon(array->vector[1]).text(il2cpp_string_new16(option1));
		Gallop::TextCommon(array->vector[2]).text(il2cpp_string_new16(option2));
		Gallop::TextCommon(array->vector[3]).text(il2cpp_string_new16(option3));

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem3Toggle);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem3Toggle;
	}

	UnityEngine::GameObject UIParts::GetOptionItem2Toggle(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, int selectedIndex)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/partsoptionitem2toggle"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionItem2Toggle = UnityEngine::Object::Internal_CloneSingle(object);

		UnityEngine::Object::Name(optionItem2Toggle, il2cpp_string_new(name));

		auto array = optionItem2Toggle.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		Gallop::TextCommon(array->vector[0]).text(il2cpp_string_new16(title));
		Gallop::TextCommon(array->vector[1]).text(il2cpp_string_new16(option1));
		Gallop::TextCommon(array->vector[2]).text(il2cpp_string_new16(option2));

		auto toggleGroupCommon = GetToggleGroupCommon(optionItem2Toggle);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);

		return optionItem2Toggle;
	}

	UnityEngine::GameObject UIParts::GetDropdown(const char* name)
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/dropdowncommon"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject dropdownGameObject = UnityEngine::Object::Internal_CloneSingle(object);

		auto array = dropdownGameObject.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "DropDownCommon"), false);

		auto dropDownCommon = array->vector[0];

		auto optionList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dropDownCommon->klass, "get_options", 0)->methodPointer(dropDownCommon);

		auto optionClass = il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.UI", "Dropdown/OptionData");

		auto option1 = il2cpp_object_new(optionClass);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(option1->klass, "set_text", 1)->methodPointer(option1, il2cpp_string_new("Option 1"));

		auto option2 = il2cpp_object_new(optionClass);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(option2->klass, "set_text", 1)->methodPointer(option2, il2cpp_string_new("Option 2"));

		auto array1 = il2cpp_array_new(optionClass, 2);

		il2cpp_array_setref(array1, 0, option1);
		il2cpp_array_setref(array1, 1, option2);

		auto itemField = il2cpp_class_get_field_from_name(optionList->klass, "_items");
		il2cpp_field_set_value(optionList, itemField, array1);

		auto sizeField = il2cpp_class_get_field_from_name(optionList->klass, "_size");
		int size = 2;
		il2cpp_field_set_value(optionList, sizeField, &size);


		il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppObject*)>(dropDownCommon->klass, "set_options", 1)->methodPointer(dropDownCommon, optionList);

		auto transform = GetRectTransform(dropdownGameObject);
		transform.sizeDelta({ 0, 0 });
		transform.anchorMax({ 1, 1 });
		transform.anchorMin({ 0, 0 });
		transform.pivot({ 0.5, 0.5 });
		transform.anchoredPosition({ 0, 0 });
		transform.localPosition({ 0, 0, -10 });

		return dropdownGameObject;
	}

	UnityEngine::GameObject UIParts::GetLiveSlider(const char* name, float value, float min, float max, bool wholeNumbers, void (*onChange)(Il2CppObject*))
	{
		UnityEngine::GameObject object = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/outgame/option/optionsoundvolumeslider"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		UnityEngine::GameObject optionSlider = UnityEngine::Object::Internal_CloneSingle(object);

		auto optionSoundVolumeSliderArray = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "OptionSoundVolumeSlider"), false);

		auto optionSoundVolumeSlider = optionSoundVolumeSliderArray->vector[0];

		UnityEngine::Object::Destroy(optionSoundVolumeSlider);

		auto sliderCommonArray = optionSlider.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "SliderCommon"), false);

		auto sliderCommon = sliderCommonArray->vector[0];

		if (onChange)
		{
			auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sliderCommon->klass, "get_onValueChanged", 0)->methodPointer(sliderCommon);

			auto AddCall = il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(onValueChanged->klass, "AddCall", 1);

			auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.single_class));

			auto valueChanged = CreateDelegateWithClass(delegateClass, sliderCommon, onChange);

			auto invokeableCall = il2cpp_object_new(GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "InvokableCall`1"), GetRuntimeType(il2cpp_defaults.single_class)));

			auto delegateField = il2cpp_class_get_field_from_name(invokeableCall->klass, "Delegate");
			il2cpp_field_set_value(invokeableCall, delegateField, valueChanged);

			AddCall->methodPointer(onValueChanged, invokeableCall);
		}

		try
		{
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(sliderCommon->klass, "set_wholeNumbers", 1)->methodPointer(sliderCommon, wholeNumbers);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_minValue", 1)->methodPointer(sliderCommon, min);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_maxValue", 1)->methodPointer(sliderCommon, max);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(sliderCommon->klass, "set_value", 1)->methodPointer(sliderCommon, value);
		}
		catch (const Il2CppExceptionWrapper& e)
		{
			cout << e.ex->klass->name << ": ";
			wcout << e.ex->message << endl;
		}

		auto transformArray = GetRectTransformArray(optionSlider);

		vector<Il2CppObject*> destroyTargets;

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("TextName")) ||
					UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("ToggleMute")) ||
					UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("ImageIcon")) ||
					UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Line")))
				{
					destroyTargets.emplace_back(transform);
				}
				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Fill")))
				{
					auto imageCommonArray = UnityEngine::Transform(transform).gameObject().GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon"), false);

					for (int j = 0; j < imageCommonArray->max_length; j++)
					{
						auto imageCommon = imageCommonArray->vector[j];
						Il2CppObject* sprite = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(imageCommon->klass, "get_sprite", 0)->methodPointer(imageCommon);

						if (sprite)
						{
							auto loadSettings = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop.Live", "Director", "get_LoadSettings", IgnoreNumberOfArguments)();
							auto musicId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(loadSettings->klass, "get_MusicId", 0)->methodPointer(loadSettings);

							string topColor = MasterDB::GetLiveTitleColorTop(musicId);
							string bottomColor = MasterDB::GetLiveTitleColorBottom(musicId);
							UnityEngine::Color titleColorTop;
							UnityEngine::Color titleColorBottom;

							auto TryParseHtmlString = il2cpp_symbols::get_method_pointer<bool (*)(Il2CppString*, UnityEngine::Color*)>("UnityEngine.CoreModule.dll", "UnityEngine", "ColorUtility", "TryParseHtmlString", 2);
							if (TryParseHtmlString(il2cpp_string_new(topColor.data()), &titleColorTop) && TryParseHtmlString(il2cpp_string_new(bottomColor.data()), &titleColorBottom))
							{
								auto texture = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(sprite->klass, "get_texture", 0)->methodPointer(sprite);

								int width = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_width", 0)->methodPointer(texture);
								int height = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_height", 0)->methodPointer(texture);

								auto renderTexture = UnityEngine::RenderTexture::GetTemporary(width, height);

								il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Graphics", "Blit", 2)(texture, renderTexture);

								auto previous = UnityEngine::RenderTexture::GetActive();

								UnityEngine::RenderTexture::SetActive(renderTexture);

								auto readableTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(readableTexture->klass, ".ctor", 2)->methodPointer(readableTexture, width, height);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Rect, int, int)>(readableTexture->klass, "ReadPixels", 3)->methodPointer(readableTexture, UnityEngine::Rect{ 0, 0, static_cast<float>(width), static_cast<float>(height) }, 0, 0);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(readableTexture->klass, "Apply", 0)->methodPointer(readableTexture);

								UnityEngine::RenderTexture::SetActive(previous);

								UnityEngine::RenderTexture::ReleaseTemporary(renderTexture);

								auto pixels = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<UnityEngine::Color>*(*)(Il2CppObject*)>(readableTexture->klass, "GetPixels", 0)->methodPointer(readableTexture);

								for (int k = 0; k < pixels->max_length; k++)
								{
									auto color = pixels->vector[k];
									il2cpp_array_setref_type(pixels, UnityEngine::Color, k, UnityEngine::Color(1, 1, 1, color.a));
								}

								auto newTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(newTexture->klass, ".ctor", 2)->methodPointer(newTexture, width, height);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<UnityEngine::Color>*)>(newTexture->klass, "SetPixels", 1)->methodPointer(newTexture, pixels);
								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(newTexture->klass, "Apply", 0)->methodPointer(newTexture);

								auto rect = il2cpp_class_get_method_from_name_type<UnityEngine::Rect(*)(Il2CppObject*)>(sprite->klass, "get_rect", 0)->methodPointer(sprite);
								auto pivot = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)(Il2CppObject*)>(sprite->klass, "get_pivot", 0)->methodPointer(sprite);
								auto pixelsPerUnit = il2cpp_class_get_method_from_name_type<float (*)(Il2CppObject*)>(sprite->klass, "get_pixelsPerUnit", 0)->methodPointer(sprite);
								auto border = il2cpp_class_get_method_from_name_type<UnityEngine::Vector4(*)(Il2CppObject*)>(sprite->klass, "get_border", 0)->methodPointer(sprite);

								auto newSprite = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, UnityEngine::Rect, UnityEngine::Vector2, float, uint32_t, int, UnityEngine::Vector4, bool)>("UnityEngine.CoreModule.dll", "UnityEngine", "Sprite", "Create", 8)
									(newTexture, rect, pivot, pixelsPerUnit, 0, 0, border, false);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(imageCommon->klass, "set_sprite", 1)->methodPointer(imageCommon, newSprite);

								auto color = il2cpp_symbols::get_method_pointer<UnityEngine::Color(*)(UnityEngine::Color, UnityEngine::Color, float)>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "Lerp", 3)(titleColorTop, titleColorBottom, 0.5f);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Color)>(imageCommon->klass, "SetMulColor", 1)->methodPointer(imageCommon, color);
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < destroyTargets.size(); i++)
		{
			auto transform = destroyTargets[i];
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*, bool)>(transform->klass, "SetParent", 2)->methodPointer(transform, nullptr, false);
			auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_gameObject", 0)->methodPointer(transform);
			UnityEngine::Object::Destroy(gameObject);
		}

		destroyTargets.clear();

		transformArray = GetRectTransformArray(optionSlider);
		UnityEngine::RectTransform sliderTransform{ nullptr };
		UnityEngine::RectTransform numTransform{ nullptr };

		for (int i = 0; i < transformArray->max_length; i++)
		{
			auto transform = transformArray->vector[i];

			if (transform)
			{
				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Slider")))
				{
					sliderTransform = UnityEngine::RectTransform(transform);
				}

				if (UnityEngine::Object::Name(transform)->chars == il2cppstring(IL2CPP_STRING("Num")))
				{
					numTransform = UnityEngine::RectTransform(transform);
				}

				if (sliderTransform && numTransform)
				{
					break;
				}
			}
		}

		auto array = numTransform.gameObject().GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon = Gallop::TextCommon(array->vector[0]);
		textCommon.text(il2cpp_string_new16(IL2CPP_STRING("0:00")));
		textCommon.OutlineSize(IL2CPP_STRING("M"));
		textCommon.OutlineColor(IL2CPP_STRING("White"));
		textCommon.UpdateOutline();

		numTransform.anchoredPosition({ -100, 11 });
		numTransform.anchorMax({ 0, 0.5 });
		numTransform.anchorMin({ 0, 0.5 });
		numTransform.sizeDelta({ 80, 50 });
		numTransform.pivot({ 0, 0.5 });

		auto parent = numTransform.GetParent();

		auto clonedText = UnityEngine::GameObject(UnityEngine::Object::Internal_CloneSingle(numTransform.gameObject()));
		UnityEngine::Object::Name(clonedText, il2cpp_string_new((name + "_total"s).data()));
		auto totalNumTransform = static_cast<UnityEngine::RectTransform>(clonedText.transform());

		totalNumTransform.SetParent(parent);

		auto array1 = clonedText.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "TextCommon"), false);

		auto textCommon1 = Gallop::TextCommon(array1->vector[0]);
		textCommon1.text(il2cpp_string_new16(IL2CPP_STRING("1:00")));
		textCommon1.OutlineSize(IL2CPP_STRING("M"));
		textCommon1.OutlineColor(IL2CPP_STRING("White"));
		textCommon1.UpdateOutline();

		totalNumTransform.anchoredPosition({ 100, 11 });
		totalNumTransform.anchorMax({ 1, 0.5 });
		totalNumTransform.anchorMin({ 1, 0.5 });
		totalNumTransform.sizeDelta({ 80, 50 });
		totalNumTransform.pivot({ 1, 0.5 });

		auto gameObject = sliderTransform.gameObject();

		UnityEngine::Object::Name(gameObject, il2cpp_string_new(name));

		return gameObject;
	}
}
