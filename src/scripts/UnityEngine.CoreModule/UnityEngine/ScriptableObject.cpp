#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "ScriptableObject.hpp"

namespace
{
	void* CreateScriptableObject_addr = nullptr;

	void* CreateScriptableObject_orig = nullptr;

	void* CreateScriptableObjectInstanceFromType_addr = nullptr;

	void* CreateScriptableObjectInstanceFromType_orig = nullptr;
}

static void CreateScriptableObject_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(CreateScriptableObject_hook)*>(CreateScriptableObject_orig)(self);

	cout << "CreateScriptableObject " << self->klass->name << endl;

	if (self->klass->name == "UniversalRenderPipelineAsset"s)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(self->klass, "set_msaaSampleCount", 1)->methodPointer(self, 8);
	}
}

static Il2CppObject* CreateScriptableObjectInstanceFromType_hook(Il2CppObject* runtimeType, bool applyDefaultsAndReset)
{
	auto object = reinterpret_cast<decltype(CreateScriptableObjectInstanceFromType_hook)*>(CreateScriptableObjectInstanceFromType_orig)(runtimeType, applyDefaultsAndReset);

	cout << "CreateScriptableObjectInstanceFromType " << object->klass->name << endl;

	return object;
}

static void InitAddress()
{
	CreateScriptableObject_addr = il2cpp_resolve_icall("UnityEngine.ScriptableObject::CreateScriptableObject()");
	CreateScriptableObjectInstanceFromType_addr = il2cpp_resolve_icall("UnityEngine.ScriptableObject::CreateScriptableObjectInstanceFromType()");
}

static void HookMethods()
{
	// ADD_HOOK(CreateScriptableObject, "UnityEngine.ScriptableObject::CreateScriptableObject at %p\n");
	// ADD_HOOK(CreateScriptableObjectInstanceFromType, "UnityEngine.ScriptableObject::CreateScriptableObjectInstanceFromType at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	void ScriptableObject::CreateScriptableObject(ScriptableObject self)
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(CreateScriptableObject_addr)(self);
	}

	ScriptableObject ScriptableObject::CreateScriptableObjectInstanceFromType(Il2CppObject* runtimeType, bool applyDefaultsAndReset)
	{
		return ScriptableObject{ reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, bool)>(CreateScriptableObjectInstanceFromType_addr)(runtimeType, applyDefaultsAndReset) };
	}
}
