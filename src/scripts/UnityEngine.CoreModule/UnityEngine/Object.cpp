#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Object.hpp"

namespace
{
void* FindObjectsByType_addr = nullptr;

void* FindObjectsOfType_addr = nullptr;

void* Internal_CloneSingleWithParent_addr = nullptr;
void* Internal_CloneSingleWithParent_orig = nullptr;

void* Internal_CloneSingle_addr = nullptr;
void* Internal_CloneSingle_orig = nullptr;

void* DontDestroyOnLoad_addr = nullptr;

void* GetName_addr = nullptr;

void* SetName_addr = nullptr;

void* IsNativeObjectAlive_addr = nullptr;
}

static void InitAddress()
{
	FindObjectsByType_addr = il2cpp_resolve_icall("UnityEngine.Object::FindObjectsByType()");
	FindObjectsOfType_addr = il2cpp_resolve_icall("UnityEngine.Object::FindObjectsOfType()");
	Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");
	Internal_CloneSingle_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingle()");
	DontDestroyOnLoad_addr = il2cpp_resolve_icall("UnityEngine.Object::DontDestroyOnLoad()");
	GetName_addr = il2cpp_resolve_icall("UnityEngine.Object::GetName()");
	SetName_addr = il2cpp_resolve_icall("UnityEngine.Object::SetName()");
	IsNativeObjectAlive_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "Object", "IsNativeObjectAlive", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Il2CppArraySize_t<Il2CppObject*>* Object::FindObjectsByType(Il2CppReflectionType* type, FindObjectsInactive findObjectsInactive, FindObjectsSortMode sortMode)
	{
		if (!FindObjectsByType_addr)
		{
			return FindObjectsOfType(type, findObjectsInactive == FindObjectsInactive::Include);
		}

		return reinterpret_cast<decltype(FindObjectsByType)*>(FindObjectsByType_addr)(type, findObjectsInactive, sortMode);
	}

	Il2CppArraySize_t<Il2CppObject*>* Object::FindObjectsOfType(Il2CppReflectionType* type, bool includeInactive)
	{
		return reinterpret_cast<decltype(FindObjectsOfType)*>(FindObjectsOfType_addr)(type, includeInactive);
	}

	Il2CppObject* Object::Internal_CloneSingleWithParent(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		return reinterpret_cast<decltype(Internal_CloneSingleWithParent)*>(Internal_CloneSingleWithParent_addr)(data, parent, worldPositionStays);
	}

	Il2CppObject* Object::Internal_CloneSingle(Il2CppObject* data)
	{
		return reinterpret_cast<decltype(Internal_CloneSingle)*>(Internal_CloneSingle_addr)(data);
	}

	void Object::DontDestroyOnLoad(Il2CppObject* obj)
	{
		reinterpret_cast<decltype(DontDestroyOnLoad)*>(DontDestroyOnLoad_addr)(obj);
	}

	Il2CppString* Object::Name(Il2CppObject* obj)
	{
		return reinterpret_cast<Il2CppString* (*)(Il2CppObject*)>(GetName_addr)(obj);
	}

	void Object::Name(Il2CppObject* obj, Il2CppString* name)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(SetName_addr)(obj, name);
	}

	bool Object::IsNativeObjectAlive(Il2CppObject* obj)
	{
		return reinterpret_cast<decltype(IsNativeObjectAlive)*>(IsNativeObjectAlive_addr)(obj);
	}
}