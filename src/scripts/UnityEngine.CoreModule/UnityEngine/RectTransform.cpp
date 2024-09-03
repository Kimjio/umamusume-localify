#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "RectTransform.hpp"

void* set_anchoredPosition_Injected_addr = nullptr;

void* set_anchorMax_Injected_addr = nullptr;

void* set_anchorMin_Injected_addr = nullptr;

void* set_pivot_Injected_addr = nullptr;

void* set_sizeDelta_Injected_addr = nullptr;

static void InitAddress()
{
	set_anchoredPosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchoredPosition_Injected()");
	set_anchorMax_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchorMax_Injected()");
	set_anchorMin_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchorMin_Injected()");
	set_pivot_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_pivot_Injected()");
	set_sizeDelta_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_sizeDelta_Injected()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	void RectTransform::anchoredPosition(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_anchoredPosition_Injected_addr)(instance, &value);
	}

	void RectTransform::anchorMax(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_anchorMax_Injected_addr)(instance, &value);
	}

	void RectTransform::anchorMin(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_anchorMin_Injected_addr)(instance, &value);
	}

	void RectTransform::pivot(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_pivot_Injected_addr)(instance, &value);
	}

	void RectTransform::sizeDelta(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_sizeDelta_Injected_addr)(instance, &value);
	}
}
