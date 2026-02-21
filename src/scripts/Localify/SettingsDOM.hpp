#pragma once
#include <stdinclude.hpp>
#include "il2cpp/il2cpp_symbols.hpp"

#include "scripts/UnityEngine.CoreModule//UnityEngine/GameObject.hpp"

namespace Localify
{
	class SettingsDOM
	{
	public:
		static void SetTextCommonText(Il2CppObject* textCommon, const Il2CppChar* text);
		static void SetTextCommonTextWithCustomTag(Il2CppObject* textCommon, const Il2CppChar* text);
		static void SetTextCommonFontColor(Il2CppObject* textCommon, const Il2CppChar* color);
		static void SetTextCommonOutlineSize(Il2CppObject* textCommon, const Il2CppChar* size);
		static void SetTextCommonOutlineColor(Il2CppObject* textCommon, const Il2CppChar* color);
		static UnityEngine::GameObject GetOptionItemTitle(const Il2CppChar* title);
		static UnityEngine::GameObject GetOptionItemOnOff(const char* name, const Il2CppChar* title);
		static UnityEngine::GameObject GetOptionItemOnOffQualityRich(const char* name, const Il2CppChar* title);
		static void SetOptionItemOnOffAction(const char* name, bool isOn, void (*onChange)(Il2CppObject*, bool));
		static bool GetOptionItemOnOffIsOn(const char* name);
		static UnityEngine::GameObject GetOptionItemButton(const char* name, const Il2CppChar* title);
		static void SetOptionItemButtonAction(const char* name, void (*onClick)(Il2CppObject*));
		static UnityEngine::GameObject GetOptionItemAttention(const Il2CppChar* text);
		static UnityEngine::GameObject GetOptionItemInfo(const char* name, const Il2CppChar* text);
		static UnityEngine::GameObject GetOptionItemSimple(const Il2CppChar* title);
		static UnityEngine::GameObject GetOptionItemSimpleWithButton(const char* name, const Il2CppChar* title, const Il2CppChar* text);
		static Il2CppObject* GetOptionItemSimpleWithButtonTextCommon(const char* name);
		static Il2CppObject* GetToggleCommon(const char* name);
		static Il2CppObject* GetToggleGroupCommon(UnityEngine::GameObject toggleGroupObject);
		static Il2CppObject* GetToggleGroupCommon(const char* name);
		static int GetToggleGroupCommonValue(const char* name);
		static UnityEngine::GameObject GetOptionItem3ToggleVertical(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, const Il2CppChar* option3, int selectedIndex);
		static UnityEngine::GameObject GetOptionItem3Toggle(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, const Il2CppChar* option3, int selectedIndex);
		static UnityEngine::GameObject GetOptionItem2Toggle(const char* name, const Il2CppChar* title, const Il2CppChar* option1, const Il2CppChar* option2, int selectedIndex);
		static Il2CppObject* GetOptionSliderNumText(Il2CppObject* slider);
		static float GetOptionSliderValue(Il2CppObject* slider);
		static float GetOptionSliderValue(const char* name);
		static Il2CppObject* GetOptionSlider(const char* name);
		static UnityEngine::GameObject GetOptionSlider(const char* name, const Il2CppChar* title, float value = 0, float min = 0, float max = 10, bool wholeNumbers = true, void (*onChange)(Il2CppObject*) = nullptr);
		static void SetOptionSliderAction(const char* name, void (*onChange)(Il2CppObject*));
		static UnityEngine::GameObject GetDropdown(const char* name);
		static UnityEngine::GameObject GetCheckbox(const char* name);
		static UnityEngine::GameObject GetCheckboxWithText(const char* name);
		static UnityEngine::GameObject GetRadioButtonWithText(const char* name, const Il2CppChar* title);
		static UnityEngine::GameObject GetSlider(const char* name, float value, float min = 0, float max = 10, bool wholeNumbers = true, void (*onChange)(Il2CppObject*) = nullptr);
		
		static void AddToLayout(Il2CppObject* layout, vector<Il2CppObject*> objects);
		static Il2CppObject* GetTextCommon(const char* name);
	};
}
