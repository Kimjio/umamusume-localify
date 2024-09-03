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

		Il2CppObject* GetComponentByName(Il2CppString* name);

		Il2CppObject* AddComponent(Il2CppObject* runtimeType)
		{
			return Internal_AddComponentWithType(runtimeType);
		}

		Transform transform();

		void SetActive(bool value);

		using Object::Object;

	private:
		static void Internal_CreateGameObject(Il2CppObject* self, Il2CppString* name);

	    Il2CppObject* Internal_AddComponentWithType(Il2CppObject* runtimeType);
	};
}

#include "Transform.hpp"
