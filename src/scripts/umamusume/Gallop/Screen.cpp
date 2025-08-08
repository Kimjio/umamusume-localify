#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Screen.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "config/config.hpp"

Il2CppClass* ScreenClass = nullptr;

void* get_Width_addr = nullptr;
void* get_Width_orig = nullptr;

void* get_Height_addr = nullptr;
void* get_Height_orig = nullptr;

void* get_OriginalScreenWidth_addr = nullptr;
void* get_OriginalScreenWidth_orig = nullptr;

void* set_OriginalScreenWidth_addr = nullptr;
void* set_OriginalScreenWidth_orig = nullptr;

void* get_OriginalScreenHeight_addr = nullptr;
void* get_OriginalScreenHeight_orig = nullptr;

void* set_OriginalScreenHeight_addr = nullptr;
void* set_OriginalScreenHeight_orig = nullptr;

void* get_IsSplitWindow_addr = nullptr;

void* ChangeScreenOrientation_addr = nullptr;
void* ChangeScreenOrientation_orig = nullptr;

void* ChangeScreenOrientationLandscapeAsync_addr = nullptr;
void* ChangeScreenOrientationLandscapeAsync_orig = nullptr;

void* ChangeScreenOrientationPortraitAsync_addr = nullptr;
void* ChangeScreenOrientationPortraitAsync_orig = nullptr;

void* get_IsVertical_addr = nullptr;
void* get_IsVertical_orig = nullptr;

static int get_OriginalScreenWidth_hook()
{
	/*auto widthField = il2cpp_class_get_field_from_name_wrap(ScreenClass, "_originalScreenWidth");
	int _originalScreenWidth;
	il2cpp_field_static_get_value(widthField, &_originalScreenWidth);
	return _originalScreenWidth;*/

	return UnityEngine::Screen::width();
}

static void set_OriginalScreenWidth_hook(int value)
{
	auto widthField = il2cpp_class_get_field_from_name_wrap(ScreenClass, "_originalScreenWidth");
	il2cpp_field_static_set_value(widthField, &value);
}

static int get_OriginalScreenHeight_hook()
{
	/*auto heightField = il2cpp_class_get_field_from_name_wrap(ScreenClass, "_originalScreenHeight");
	int _originalScreenHeight;
	il2cpp_field_static_get_value(heightField, &_originalScreenHeight);
	return _originalScreenHeight;*/

	return UnityEngine::Screen::height();
}

static void set_OriginalScreenHeight_hook(int value)
{
	auto heightField = il2cpp_class_get_field_from_name_wrap(ScreenClass, "_originalScreenHeight");
	il2cpp_field_static_set_value(heightField, &value);
}

static Il2CppObject* ChangeScreenOrientation_hook(UnityEngine::ScreenOrientation targetOrientation, bool isForce)
{
	if (config::freeform_window)
	{
		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}

	return reinterpret_cast<decltype(ChangeScreenOrientation_hook)*>(ChangeScreenOrientation_orig)(targetOrientation, isForce);
}

static Il2CppObject* ChangeScreenOrientationLandscapeAsync_hook()
{
	if (config::freeform_window)
	{
		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}

	return reinterpret_cast<decltype(ChangeScreenOrientationLandscapeAsync_hook)*>(ChangeScreenOrientationLandscapeAsync_orig)();
}

static Il2CppObject* ChangeScreenOrientationPortraitAsync_hook()
{
	if (config::freeform_window)
	{
		auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)->methodPointer(yield, CreateDelegateStatic(*[]() { return false; }));
		return yield;
	}

	return reinterpret_cast<decltype(ChangeScreenOrientationPortraitAsync_hook)*>(ChangeScreenOrientationPortraitAsync_orig)();
}

static void InitAddress()
{
	ScreenClass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "Screen");
	get_Width_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_Width", 0);
	get_Height_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_Height", 0);
	get_OriginalScreenWidth_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_OriginalScreenWidth", 0);
	set_OriginalScreenWidth_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "set_OriginalScreenWidth", 1);
	get_OriginalScreenHeight_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_OriginalScreenHeight", 0);
	set_OriginalScreenHeight_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "set_OriginalScreenHeight", 1);
	get_IsSplitWindow_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_IsSplitWindow", 0);
	get_IsVertical_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "get_IsVertical", 0);
	ChangeScreenOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "ChangeScreenOrientation", 2);
	ChangeScreenOrientationLandscapeAsync_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "ChangeScreenOrientationLandscapeAsync", 0);
	ChangeScreenOrientationPortraitAsync_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Screen", "ChangeScreenOrientationPortraitAsync", 0);
}

static void HookMethods()
{

	if (config::unlock_size || config::freeform_window)
	{
		// remove fixed 1080p render resolution
		// ADD_HOOK(get_Width, "Gallop.Screen::get_Width at %p\n");
		// ADD_HOOK(get_Height, "Gallop.Screen::get_Height at %p\n");
	}

	if (config::freeform_window)
	{
		ADD_HOOK(get_OriginalScreenWidth, "Gallop.Screen::get_OriginalScreenWidth at %p\n");
		ADD_HOOK(set_OriginalScreenWidth, "Gallop.Screen::set_OriginalScreenWidth at %p\n");
		ADD_HOOK(get_OriginalScreenHeight, "Gallop.Screen::get_OriginalScreenHeight at %p\n");
		ADD_HOOK(set_OriginalScreenHeight, "Gallop.Screen::set_OriginalScreenHeight at %p\n");
		ADD_HOOK(ChangeScreenOrientation, "Gallop.Screen::ChangeScreenOrientation at %p\n");
		ADD_HOOK(ChangeScreenOrientationLandscapeAsync, "Gallop.Screen::ChangeScreenOrientationLandscapeAsync at %p\n");
		ADD_HOOK(ChangeScreenOrientationPortraitAsync, "Gallop.Screen::ChangeScreenOrientationPortraitAsync at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	int Screen::OriginalScreenWidth()
	{
		return reinterpret_cast<int (*)()>(get_OriginalScreenWidth_addr)();
	}

	void Screen::OriginalScreenWidth(int value)
	{
		reinterpret_cast<void (*)(int)>(set_OriginalScreenWidth_addr)(value);
	}

	int Screen::OriginalScreenHeight()
	{
		return reinterpret_cast<int (*)()>(get_OriginalScreenHeight_addr)();
	}

	void Screen::OriginalScreenHeight(int value)
	{
		reinterpret_cast<void (*)(int)>(set_OriginalScreenHeight_addr)(value);
	}

	int Screen::Width()
	{
		return reinterpret_cast<int (*)()>(get_Width_addr)();
	}

	int Screen::Height()
	{
		return reinterpret_cast<int (*)()>(get_Height_addr)();
	}

	bool Screen::IsSplitWindow()
	{
		if (!get_IsSplitWindow_addr)
		{
			return false;
		}


		return reinterpret_cast<bool (*)()>(get_IsSplitWindow_addr)();
	}

	bool Screen::IsVertical()
	{
		return reinterpret_cast<bool (*)()>(get_IsVertical_addr)();
	}

	Il2CppObject* Screen::ChangeScreenOrientation(UnityEngine::ScreenOrientation targetOrientation, bool isForce)
	{
		return reinterpret_cast<decltype(ChangeScreenOrientation)*>(ChangeScreenOrientation_addr)(targetOrientation, isForce);
	}

	Il2CppObject* Screen::ChangeScreenOrientationLandscapeAsync()
	{
		return reinterpret_cast<decltype(ChangeScreenOrientationLandscapeAsync)*>(ChangeScreenOrientationLandscapeAsync_addr)();
	}

	Il2CppObject* Screen::ChangeScreenOrientationPortraitAsync()
	{
		return reinterpret_cast<decltype(ChangeScreenOrientationPortraitAsync)*>(ChangeScreenOrientationPortraitAsync_addr)();
	}
}
