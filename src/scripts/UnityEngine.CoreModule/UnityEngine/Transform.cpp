#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Transform.hpp"

namespace
{
	void* Transform_SetParent_addr = nullptr;

	void* Transform_GetParent_addr = nullptr;

	void* Transform_FindRelativeTransformWithPath_addr = nullptr;

	void* Transform_InverseTransformPoint_Injected_addr = nullptr;

	void* Transform_get_position_Injected_addr = nullptr;

	void* Transform_set_position_Injected_addr = nullptr;

	void* Transform_get_localScale_Injected_addr = nullptr;

	void* Transform_set_localScale_Injected_addr = nullptr;

	void* Transform_get_childCount_addr = nullptr;

	void* Transform_GetChild_addr = nullptr;
}

static void InitAddress()
{
	Transform_SetParent_addr = il2cpp_resolve_icall("UnityEngine.Transform::SetParent()");
	Transform_GetParent_addr = il2cpp_resolve_icall("UnityEngine.Transform::GetParent()");
	Transform_FindRelativeTransformWithPath_addr = il2cpp_resolve_icall("UnityEngine.Transform::FindRelativeTransformWithPath()");
	Transform_InverseTransformPoint_Injected_addr = il2cpp_resolve_icall("UnityEngine.Transform::InverseTransformPoint_Injected()");
	Transform_get_position_Injected_addr = il2cpp_resolve_icall("UnityEngine.Transform::get_position_Injected()");
	Transform_set_position_Injected_addr = il2cpp_resolve_icall("UnityEngine.Transform::set_position_Injected()");
	Transform_get_localScale_Injected_addr = il2cpp_resolve_icall("UnityEngine.Transform::get_localScale_Injected()");
	Transform_set_localScale_Injected_addr = il2cpp_resolve_icall("UnityEngine.Transform::set_localScale_Injected()");
	Transform_get_childCount_addr = il2cpp_resolve_icall("UnityEngine.Transform::get_childCount()");
	Transform_GetChild_addr = il2cpp_resolve_icall("UnityEngine.Transform::GetChild()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	void Transform::SetParent(Il2CppObject* parent, bool worldPositionStays)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*, bool)>(Transform_SetParent_addr)(instance, parent, worldPositionStays);
	}

	Transform Transform::GetParent()
	{
		auto obj = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(Transform_GetParent_addr)(instance);
		return Transform(obj);
	}

	Transform Transform::FindRelativeTransformWithPath(Transform transform, Il2CppString* path, bool isActiveOnly)
	{
		auto relative = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(Transform_FindRelativeTransformWithPath_addr)(transform, path, isActiveOnly);
		return Transform(relative);
	}

	Vector3 Transform::InverseTransformPoint(Vector3 position)
	{
		Vector3 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector3&, Vector3*)>(Transform_InverseTransformPoint_Injected_addr)(instance, position, &vector);
		return vector;
	}

	Vector3 Transform::position()
	{
		Vector3 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector3*)>(Transform_get_position_Injected_addr)(instance, &vector);
		return vector;

	}

	void Transform::position(Vector3 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector3&)>(Transform_set_position_Injected_addr)(instance, value);
	}

	Vector3 Transform::localScale()
	{
		Vector3 vector;
		reinterpret_cast<void (*)(Il2CppObject*, Vector3*)>(Transform_get_localScale_Injected_addr)(instance, &vector);
		return vector;
	}

	void Transform::localScale(Vector3 value)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Vector3&)>(Transform_set_localScale_Injected_addr)(instance, value);
	}

	int Transform::childCount()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(Transform_get_childCount_addr)(instance);
	}

	Transform Transform::GetChild(int index)
	{
		auto obj = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, int)>(Transform_GetChild_addr)(instance, index);
		return Transform(obj);
	}
}
