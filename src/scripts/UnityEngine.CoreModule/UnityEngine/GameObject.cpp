#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "GameObject.hpp"

void* Internal_CreateGameObject_addr = nullptr;

void* Internal_CreateGameObject_orig = nullptr;

void* Internal_AddComponentWithType_addr = nullptr;

void* Internal_AddComponentWithType_orig = nullptr;

void* GetComponent_addr = nullptr;

void* GetComponentByName_addr = nullptr;

void* get_transform_addr = nullptr;

void* get_tag_addr = nullptr;

void* set_tag_addr = nullptr;

void* SetActive_addr = nullptr;

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

	return component;
}

static void InitAddress()
{
	Internal_CreateGameObject_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_CreateGameObject()");
	Internal_AddComponentWithType_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_AddComponentWithType()");
	GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent()");
	GetComponentByName_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentByName()");
	get_transform_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_transform()");
	get_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_tag()");
	set_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::set_tag()");
	SetActive_addr = il2cpp_resolve_icall("UnityEngine.GameObject::SetActive()");
}

static void HookMethods()
{
	// ADD_HOOK(Internal_CreateGameObject, "UnityEngine.GameObject::Internal_CreateGameObject at %p\n");
	// ADD_HOOK(Internal_AddComponentWithType, "UnityEngine.GameObject::Internal_AddComponentWithType at %p\n");
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

	void GameObject::Internal_CreateGameObject(Il2CppObject* self, Il2CppString* name)
	{
		reinterpret_cast<decltype(Internal_CreateGameObject)*>(Internal_CreateGameObject_addr)(self, name);
	}

	Il2CppObject* GameObject::Internal_AddComponentWithType(Il2CppObject* runtimeType)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(Internal_AddComponentWithType_addr)(instance, runtimeType);
	}

	Il2CppObject* GameObject::GetComponent(Il2CppObject* runtimeType)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(GetComponent_addr)(instance, runtimeType);
	}

	Il2CppObject* GameObject::GetComponentByName(Il2CppString* name)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*)>(GetComponentByName_addr)(instance, name);
	}

	Transform GameObject::transform()
	{
		auto transform = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_transform_addr)(instance);
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
