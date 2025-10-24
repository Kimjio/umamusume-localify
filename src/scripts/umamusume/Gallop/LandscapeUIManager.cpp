#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LandscapeUIManager.hpp"

namespace
{
	void* LandscapeUIManager_Initialize_addr = nullptr;
	void* LandscapeUIManager_Initialize_orig = nullptr;

	FieldInfo* LandscapeUIManager__displayOrientationButtonField = nullptr;
}

static void LandscapeUIManager_Initialize_hook(Il2CppObject* self, float leftUiOffsetX, float rightUiOffsetX)
{
	reinterpret_cast<void (*)(Il2CppObject*, float, float)>(LandscapeUIManager_Initialize_orig)(self, leftUiOffsetX, rightUiOffsetX);

	Il2CppObject* _displayOrientationButton;
	il2cpp_field_get_value(self, LandscapeUIManager__displayOrientationButtonField, &_displayOrientationButton);

	UnityEngine::MonoBehaviour(_displayOrientationButton).gameObject().SetActive(true);

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_displayOrientationButton->klass, "SetOnClick", 1)->methodPointer(_displayOrientationButton,
		&CreateUnityAction(self, *[](Il2CppObject* self)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(self->klass, "OnClickDisplayOrientationButton", 0)->methodPointer(self);
			})->delegate);
}

static void InitAddress()
{
	auto LandscapeUIManager = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "LandscapeUIManager");
	LandscapeUIManager_Initialize_addr = il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(LandscapeUIManager, "Initialize", 2);
	LandscapeUIManager__displayOrientationButtonField = il2cpp_class_get_field_from_name(LandscapeUIManager, "_displayOrientationButton");
}

static void HookMethods()
{
	ADD_HOOK(LandscapeUIManager_Initialize, "Gallop.LandscapeUIManager::Initialize at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
