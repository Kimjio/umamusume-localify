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
		static Il2CppArraySize_t<Il2CppObject*>* FindObjectsByType(Il2CppObject* type, FindObjectsInactive findObjectsInactive, FindObjectsSortMode sortMode);
		static Il2CppArraySize_t<Il2CppObject*>* FindObjectsOfType(Il2CppObject* type, bool includeInactive);
		static Il2CppObject* Internal_CloneSingleWithParent(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays);
		static Il2CppObject* Internal_CloneSingle(Il2CppObject* data);
		static void DontDestroyOnLoad(Il2CppObject* obj);
		static Il2CppString* Name(Il2CppObject* obj);
		static void Name(Il2CppObject* obj, Il2CppString* name);
		static bool IsNativeObjectAlive(Il2CppObject* obj);

		Il2CppObject* NativeObject() const noexcept
		{
			return instance;
		}

		explicit operator bool() const noexcept
		{
			return instance != nullptr;
		}

		operator Il2CppObject* () const noexcept
		{
			return instance;
		}

		operator Il2CppClass* () const noexcept
		{
			return instance->klass;
		}

		Object(Il2CppObject* instance)
		{
			this->instance = instance;
		}

	protected:
		Il2CppObject* instance;
	};
}