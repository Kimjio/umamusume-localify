#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "WindowsGamepadControl.hpp"
#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/RenderTextureDescriptor.hpp"

#include "config/config.hpp"

namespace
{
	Il2CppClass* WindowsGamepadControl_klass = nullptr;

	void* WindowsGamepadControl_UpdateInputControls_addr = nullptr;
	
	void* WindowsGamepadControl_ReleaseRenderTexture_addr = nullptr;

	void* WindowsGamepadControl_CreateRenderTextureFromScreen_addr = nullptr;
	void* WindowsGamepadControl_CreateRenderTextureFromScreen_orig = nullptr;

	FieldInfo* WindowsGamepadControl__softwareCursorUiTexture = nullptr;
	FieldInfo* WindowsGamepadControl__softwareCursorUiCamera = nullptr;
}

static void WindowsGamepadControl_CreateRenderTextureFromScreen_hook(Il2CppObject* self)
{
	auto control = Gallop::WindowsGamepadControl(self);
	control.ReleaseRenderTexture();
	
	auto descriptor = UnityEngine::RenderTextureDescriptor{};
	il2cpp_symbols::get_method_pointer<void (*)(UnityEngine::RenderTextureDescriptor*, int, int, int, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTextureDescriptor", ".ctor", 4)(&descriptor, Gallop::Screen::Width(), Gallop::Screen::Height(), 8, 24);
	
	auto renderTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture"));
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::RenderTextureDescriptor*)>(renderTexture->klass, ".ctor", 1)->methodPointer(renderTexture, &descriptor);

	if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(renderTexture->klass, "Create", 0)->methodPointer(renderTexture))
	{
		return;
	}

	il2cpp_field_set_value(control, WindowsGamepadControl__softwareCursorUiTexture, renderTexture);
	if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(renderTexture->klass, "Create", 0)->methodPointer(renderTexture))
	{
		control.ReleaseRenderTexture();
	}

	Il2CppObject* softwareCursorUiCamera;
	il2cpp_field_get_value(control, WindowsGamepadControl__softwareCursorUiCamera, &softwareCursorUiCamera);

	if (!softwareCursorUiCamera)
	{
		return;
	}

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(softwareCursorUiCamera->klass, "set_targetTexture", 1)->methodPointer(softwareCursorUiCamera, renderTexture);

	UnityEngine::Behaviour(softwareCursorUiCamera).enabled(false);
}

static void InitAddress()
{
	WindowsGamepadControl_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, Gallop::WindowsGamepadControl::namespaze, Gallop::WindowsGamepadControl::klassName);
	if (!WindowsGamepadControl_klass)
	{
		// Legacy name
		WindowsGamepadControl_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, Gallop::WindowsGamepadControl::namespaze, Gallop::WindowsGamepadControl::klassNameLegacy);
		WindowsGamepadControl_CreateRenderTextureFromScreen_addr = il2cpp_symbols::get_method_pointer(WindowsGamepadControl_klass, "CreateRenderTextureFromScreen", 0);
	}
	WindowsGamepadControl__softwareCursorUiTexture = il2cpp_class_get_field_from_name(WindowsGamepadControl_klass, "_softwareCursorUiTexture");
	WindowsGamepadControl__softwareCursorUiCamera = il2cpp_class_get_field_from_name(WindowsGamepadControl_klass, "_softwareCursorUiCamera");
	WindowsGamepadControl_UpdateInputControls_addr = il2cpp_symbols::get_method_pointer(WindowsGamepadControl_klass, "UpdateInputControls", 0);
	WindowsGamepadControl_ReleaseRenderTexture_addr = il2cpp_symbols::get_method_pointer(WindowsGamepadControl_klass, "ReleaseRenderTexture", 0);
}

static void HookMethods()
{
	if (config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(WindowsGamepadControl_CreateRenderTextureFromScreen, "Gallop.WindowsGamepadControl::CreateRenderTextureFromScreen at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* WindowsGamepadControl::namespaze = "Gallop";
	const char* WindowsGamepadControl::klassName = "WindowsGamepadControl";
	const char* WindowsGamepadControl::klassNameLegacy = "SteamGamepadControl";

	void WindowsGamepadControl::ReleaseRenderTexture()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(WindowsGamepadControl_ReleaseRenderTexture_addr)(instance);
	}

	void WindowsGamepadControl::UpdateInputControls()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(WindowsGamepadControl_UpdateInputControls_addr)(instance);
	}
}
