#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

namespace UnityEngine
{
	enum FindObjectsInactive
	{
		Exclude,
		Include
	};

	enum FindObjectsSortMode
	{
		None,
		InstanceID
	};

	class Object
	{
	public:
		static Il2CppArraySize_t<Il2CppObject*>* FindObjectsByType(Il2CppReflectionType* type, FindObjectsInactive findObjectsInactive, FindObjectsSortMode sortMode);
		static Il2CppArraySize_t<Il2CppObject*>* FindObjectsOfType(Il2CppReflectionType* type, bool includeInactive);
		static Il2CppObject* Internal_CloneSingleWithParent(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays);
		static Il2CppObject* Internal_CloneSingle(Il2CppObject* data);
		static void DontDestroyOnLoad(Il2CppObject* obj);
		static Il2CppString* Name(Il2CppObject* obj);
		static void Name(Il2CppObject* obj, Il2CppString* name);
		static void Destroy(Il2CppObject* obj, float t);
		static void Destroy(Il2CppObject* obj)
		{
			Destroy(obj, 0.0f);
		}
		static void DestroyImmediate(Il2CppObject* obj, bool allowDestroyingAssets);
		static void DestroyImmediate(Il2CppObject* obj)
		{
			DestroyImmediate(obj, false);
		}
		static bool IsNativeObjectAlive(Il2CppObject* obj);

		Il2CppObject* NativeObject() const noexcept
		{
			return instance;
		}

		explicit operator bool() const noexcept
		{
			return instance != nullptr && IsNativeObjectAlive(instance);
		}

		operator Il2CppObject* () const noexcept
		{
			return instance;
		}

		operator Il2CppClass* () const noexcept
		{
			return instance->klass;
		}

		operator void* () const noexcept
		{
			return instance;
		}

		Object(Il2CppObject* instance)
		{
			this->instance = instance;
		}

	protected:
		Il2CppObject* instance;
	};
}