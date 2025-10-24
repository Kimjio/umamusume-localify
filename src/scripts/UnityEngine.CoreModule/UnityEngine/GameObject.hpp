#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Object.hpp"

namespace UnityEngine
{
	class Transform;
	class GameObject : public Object
	{
	public:
		GameObject() : GameObject(nullptr)
		{}

		GameObject(Il2CppString* name);

		GameObject gameObject()
		{
			return *this;
		}

		static GameObject Find(Il2CppString* name);

		Il2CppObject* GetComponent(Il2CppReflectionType* runtimeType);

		Il2CppObject* GetComponentInChildren(Il2CppReflectionType* runtimeType, bool includeInactive);

		Il2CppObject* AddComponent(Il2CppReflectionType* runtimeType)
		{
			return Internal_AddComponentWithType(runtimeType);
		}

		Il2CppArraySize_t<Il2CppObject*>* GetComponents(Il2CppReflectionType* runtimeType)
		{
			return GetComponentsInternal(runtimeType, false, false, true, false, nullptr);
		}

		Il2CppArraySize_t<Il2CppObject*>* GetComponentsInChildren(Il2CppReflectionType* runtimeType, bool includeInactive)
		{
			return GetComponentsInternal(runtimeType, true, true, includeInactive, false, nullptr);
		}

		Transform transform();

		Il2CppString* tag();

		void tag(Il2CppString* value);

		void SetActive(bool value);

		using Object::Object;

	private:
		static void Internal_CreateGameObject(Il2CppObject* self, Il2CppString* name);

		Il2CppArraySize_t<Il2CppObject*>* GetComponentsInternal(Il2CppReflectionType* runtimeType, bool useSearchTypeAsArrayReturnType, bool recursive, bool includeInactive, bool reverse, Il2CppObject** resultList);
		
		Il2CppObject* Internal_AddComponentWithType(Il2CppReflectionType* runtimeType);
	};
}

#include "Transform.hpp"
