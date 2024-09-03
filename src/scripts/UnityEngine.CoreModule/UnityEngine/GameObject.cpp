#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "GameObject.hpp"

void* Internal_CreateGameObject_addr = nullptr;

void* Internal_AddComponentWithType_addr = nullptr;

void* GetComponentByName_addr = nullptr;

void* get_transform_addr = nullptr;

void* SetActive_addr = nullptr;

static void InitAddress()
{
	Internal_CreateGameObject_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_CreateGameObject()");
	Internal_AddComponentWithType_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Internal_AddComponentWithType()");
	GetComponentByName_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentByName()");
	get_transform_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_transform()");
	SetActive_addr = il2cpp_resolve_icall("UnityEngine.GameObject::SetActive()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
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

	Il2CppObject* GameObject::GetComponentByName(Il2CppString* name)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*)>(GetComponentByName_addr)(instance, name);
	}

	Transform GameObject::transform()
	{
		auto transform = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(get_transform_addr)(instance);
		return Transform(transform);
	}

	void GameObject::SetActive(bool value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, bool)>(SetActive_addr)(instance, value);
	}
}
