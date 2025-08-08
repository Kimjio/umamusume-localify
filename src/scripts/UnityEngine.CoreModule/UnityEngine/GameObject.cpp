#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "GameObject.hpp"

void* Internal_CreateGameObject_addr = nullptr;

void* Internal_CreateGameObject_orig = nullptr;

void* Internal_AddComponentWithType_addr = nullptr;

void* Internal_AddComponentWithType_orig = nullptr;

void* TryGetComponentFastPath_addr = nullptr;

void* TryGetComponentFastPath_orig = nullptr;

void* GetComponent_addr = nullptr;

void* GameObject_get_transform_addr = nullptr;

void* get_tag_addr = nullptr;

void* set_tag_addr = nullptr;

void* SetActive_addr = nullptr;

void* Find_addr = nullptr;

static void Internal_CreateGameObject_hook(Il2CppObject* _this, Il2CppString* name)
{
	reinterpret_cast<decltype(Internal_CreateGameObject_hook)*>(Internal_CreateGameObject_orig)(_this, name);

	if (name)
	{
		wcout << L"Internal_CreateGameObject " << name->chars << endl;
	}
}

static Il2CppObject* Internal_AddComponentWithType_hook(Il2CppObject* _this, Il2CppObject* runtimeType)
{
	auto component = reinterpret_cast<decltype(Internal_AddComponentWithType_hook)*>(Internal_AddComponentWithType_orig)(_this, runtimeType);

	if (string(component->klass->namespaze).find("Gallop") != string::npos &&
		string(component->klass->name).find("CameraData") != string::npos)
	{
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(component->klass, "set_RenderingAntiAliasing", 1)->methodPointer(component, 8);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(component->klass, "set_IsCreateAntialiasTexture", 1)->methodPointer(component, true);
	}

	return component;
}

struct CastHelper
{
	Il2CppObject* obj;
	uintptr_t oneFurtherThanResultValue;
};

static void TryGetComponentFastPath_hook(Il2CppObject* self, Il2CppObject* runtimeType, uintptr_t oneFurtherThanResultValue)
{
	reinterpret_cast<decltype(TryGetComponentFastPath_hook)*>(TryGetComponentFastPath_orig)(self, runtimeType, oneFurtherThanResultValue);
	
	auto helper = reinterpret_cast<CastHelper*>(oneFurtherThanResultValue - sizeof(Il2CppObject*));

	if (helper->obj)
	{
		if (helper->obj->klass->name == "CameraData"s)
		{
			auto data = helper->obj;
			if (!il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(data->klass, "get_IsUIRendering", 0)->methodPointer(data))
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(data->klass, "set_RenderingAntiAliasing", 1)->methodPointer(data, config::anti_aliasing);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(data->klass, "set_IsCreateAntialiasTexture", 1)->methodPointer(data, true);
			}
		}
	}
}

static void InitAddress()
{
	Internal_CreateGameObject_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_CreateGameObject()");
	Internal_AddComponentWithType_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_AddComponentWithType()");
	TryGetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::TryGetComponentFastPath()");
	GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent()");
	GameObject_get_transform_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_transform()");
	get_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_tag()");
	set_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::set_tag()");
	SetActive_addr = il2cpp_resolve_icall("UnityEngine.GameObject::SetActive()");
	Find_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Find()");
}

static void HookMethods()
{
	// ADD_HOOK(Internal_CreateGameObject, "UnityEngine.GameObject::Internal_CreateGameObject at %p\n");
	// ADD_HOOK(Internal_AddComponentWithType, "UnityEngine.GameObject::Internal_AddComponentWithType at %p\n");
	// ADD_HOOK(TryGetComponentFastPath, "UnityEngine.GameObject::TryGetComponentFastPath at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	GameObject::GameObject(Il2CppString* name) : Object::Object(nullptr)
	{
		auto gameObject = il2cpp_object_new(il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine", "GameObject"));
		Internal_CreateGameObject(gameObject, name);
		this->instance = gameObject;
	}

	GameObject GameObject::Find(Il2CppString* name)
	{
		auto gameObject = reinterpret_cast<Il2CppObject * (*)(Il2CppString*)>(Find_addr)(name);
		return GameObject(gameObject);
	}

	void GameObject::Internal_CreateGameObject(Il2CppObject* self, Il2CppString* name)
	{
		reinterpret_cast<decltype(Internal_CreateGameObject)*>(Internal_CreateGameObject_addr)(self, name);
	}

	Il2CppObject* GameObject::Internal_AddComponentWithType(Il2CppReflectionType* runtimeType)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(Internal_AddComponentWithType_addr)(instance, runtimeType);
	}

	Il2CppObject* GameObject::GetComponent(Il2CppReflectionType* runtimeType)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(GetComponent_addr)(instance, runtimeType);
	}

	Transform GameObject::transform()
	{
		auto transform = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(GameObject_get_transform_addr)(instance);
		return Transform(transform);
	}

	Il2CppString* GameObject::tag()
	{
		return reinterpret_cast<Il2CppString * (*)(Il2CppObject*)>(get_tag_addr)(instance);
	}

	void GameObject::tag(Il2CppString* value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(set_tag_addr)(instance, value);
	}

	void GameObject::SetActive(bool value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, bool)>(SetActive_addr)(instance, value);
	}
}
