#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "GameObject.hpp"

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

namespace
{
	void* Internal_CreateGameObject_addr = nullptr;

	void* Internal_AddComponentWithType_addr = nullptr;

	void* Internal_AddComponentWithType_orig = nullptr;

	void* GetComponentFastPath_addr = nullptr;

	void* GetComponentFastPath_orig = nullptr;

	void* TryGetComponentFastPath_addr = nullptr;

	void* TryGetComponentFastPath_orig = nullptr;

	void* GetComponent_addr = nullptr;

	void* GetComponentInChildren_addr = nullptr;

	void* GetComponentsInternal_addr = nullptr;

	void* GameObject_get_transform_addr = nullptr;

	void* get_tag_addr = nullptr;

	void* set_tag_addr = nullptr;

	void* SetActive_addr = nullptr;
	void* SetActive_orig = nullptr;

	void* Find_addr = nullptr;

	Il2CppClass* FlashActionPlayerClass;
}

struct CastHelper
{
	Il2CppObject* obj;
	uintptr_t oneFurtherThanResultValue;
};

static void GetComponentFastPath_hook(Il2CppObject* self, Il2CppObject* runtimeType, uintptr_t oneFurtherThanResultValue)
{
	reinterpret_cast<decltype(GetComponentFastPath_hook)*>(GetComponentFastPath_orig)(self, runtimeType, oneFurtherThanResultValue);

	auto helper = reinterpret_cast<CastHelper*>(oneFurtherThanResultValue - sizeof(Il2CppObject*));

	if (helper->obj)
	{
		if (helper->obj->klass == FlashActionPlayerClass)
		{
			auto _flashPrefabPathField = il2cpp_class_get_field_from_name(helper->obj->klass, "_flashPrefabPath");
			Il2CppString* _flashPrefabPath;
			il2cpp_field_get_value(helper->obj, _flashPrefabPathField, &_flashPrefabPath);

			if (_flashPrefabPath)
			{
				u16stringstream pathStream(_flashPrefabPath->chars);
				u16string segment;
				vector<u16string> splited;
				while (getline(pathStream, segment, u'/'))
				{
					splited.emplace_back(segment);
				}

				auto& fileName = splited.back();
				if (find_if(config::runtime::replaceAssetNames.begin(), config::runtime::replaceAssetNames.end(), [fileName](const u16string& item)
					{
						return item.find(fileName) != wstring::npos;
					}) != config::runtime::replaceAssetNames.end())
				{
					auto result = GetReplacementAssets(il2cpp_string_new16(fileName.data()), GetRuntimeType(self->klass));

					if (result)
					{
						auto _flashPrefabField = il2cpp_class_get_field_from_name(helper->obj->klass, "_flashPrefab");
						il2cpp_field_set_value(helper->obj, _flashPrefabField, result);
					}
				}
			}
		}
	}
}

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
	GetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentFastPath()");
	TryGetComponentFastPath_addr = il2cpp_resolve_icall("UnityEngine.GameObject::TryGetComponentFastPath()");
	GetComponent_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponent()");
	GetComponentInChildren_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentInChildren()");
	GetComponentsInternal_addr = il2cpp_resolve_icall("UnityEngine.GameObject::GetComponentsInternal()");
	GameObject_get_transform_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_transform()");
	get_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::get_tag()");
	set_tag_addr = il2cpp_resolve_icall("UnityEngine.GameObject::set_tag()");
	SetActive_addr = il2cpp_resolve_icall("UnityEngine.GameObject::SetActive()");
	Find_addr = il2cpp_resolve_icall("UnityEngine.GameObject::Find()");

	FlashActionPlayerClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "FlashActionPlayer");
}

static void HookMethods()
{
	ADD_HOOK(GetComponentFastPath, "UnityEngine.GameObject::GetComponentFastPath at %p\n");
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

	Il2CppObject* GameObject::GetComponentInChildren(Il2CppReflectionType* runtimeType, bool includeInactive)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*, bool)>(GetComponentInChildren_addr)(instance, runtimeType, includeInactive);
	}

	Il2CppArraySize_t<Il2CppObject*>* GameObject::GetComponentsInternal(Il2CppReflectionType* runtimeType, bool useSearchTypeAsArrayReturnType, bool recursive, bool includeInactive, bool reverse, Il2CppObject** resultList)
	{
		return reinterpret_cast<Il2CppArraySize_t<Il2CppObject*>*(*)(Il2CppObject*, Il2CppReflectionType*, bool, bool, bool, bool, Il2CppObject**)>(GetComponentsInternal_addr)(instance, runtimeType, useSearchTypeAsArrayReturnType, recursive, includeInactive, reverse, resultList);
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
