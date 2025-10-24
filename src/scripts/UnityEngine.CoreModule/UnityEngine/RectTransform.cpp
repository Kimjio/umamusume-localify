#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "RectTransform.hpp"

namespace
{
	void* set_anchoredPosition_Injected_addr = nullptr;

	void* get_anchoredPosition_Injected_addr = nullptr;

	void* set_anchorMax_Injected_addr = nullptr;

	void* get_anchorMax_Injected_addr = nullptr;

	void* set_anchorMin_Injected_addr = nullptr;

	void* get_anchorMin_Injected_addr = nullptr;

	void* set_pivot_Injected_addr = nullptr;

	void* get_pivot_Injected_addr = nullptr;

	void* set_sizeDelta_Injected_addr = nullptr;

	void* get_sizeDelta_Injected_addr = nullptr;

	void* set_offsetMin_addr = nullptr;

	void* get_offsetMin_addr = nullptr;

	void* set_offsetMax_addr = nullptr;

	void* get_offsetMax_addr = nullptr;

	void* get_rect_Injected_addr = nullptr;
}

static void InitAddress()
{
	set_anchoredPosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchoredPosition_Injected()");
	get_anchoredPosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_anchoredPosition_Injected()");
	set_anchorMax_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchorMax_Injected()");
	get_anchorMax_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_anchorMax_Injected()");
	set_anchorMin_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_anchorMin_Injected()");
	get_anchorMin_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_anchorMin_Injected()");
	set_pivot_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_pivot_Injected()");
	get_pivot_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_pivot_Injected()");
	set_sizeDelta_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::set_sizeDelta_Injected()");
	get_sizeDelta_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_sizeDelta_Injected()");
	set_offsetMin_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "RectTransform", "set_offsetMin", 1);
	get_offsetMin_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "RectTransform", "get_offsetMin", 0);
	set_offsetMax_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "RectTransform", "set_offsetMax", 1);
	get_offsetMax_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "RectTransform", "get_offsetMax", 0);
	get_rect_Injected_addr = il2cpp_resolve_icall("UnityEngine.RectTransform::get_rect_Injected()");
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

	Vector2 RectTransform::anchoredPosition()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_anchoredPosition_Injected_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::anchorMax(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_anchorMax_Injected_addr)(instance, &value);
	}

	Vector2 RectTransform::anchorMax()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_anchorMax_Injected_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::anchorMin(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_anchorMin_Injected_addr)(instance, &value);
	}

	Vector2 RectTransform::anchorMin()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_anchorMin_Injected_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::pivot(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_pivot_Injected_addr)(instance, &value);
	}

	Vector2 RectTransform::pivot()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_pivot_Injected_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::sizeDelta(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_sizeDelta_Injected_addr)(instance, &value);
	}

	Vector2 RectTransform::sizeDelta()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_sizeDelta_Injected_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::offsetMin(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_offsetMin_addr)(instance, &value);
	}

	Vector2 RectTransform::offsetMin()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_offsetMin_addr)(instance, &vector);
		return vector;
	}

	void RectTransform::offsetMax(Vector2 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(set_offsetMax_addr)(instance, &value);
	}

	Vector2 RectTransform::offsetMax()
	{
		Vector2 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector2*)>(get_offsetMax_addr)(instance, &vector);
		return vector;
	}

	Rect RectTransform::rect()
	{
		Rect rect;
		reinterpret_cast<void (*)(Il2CppObject*, Rect*)>(get_rect_Injected_addr)(instance, &rect);
		return rect;
	}
}
