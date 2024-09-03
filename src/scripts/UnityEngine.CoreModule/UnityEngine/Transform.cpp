#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Transform.hpp"

void* SetParent_addr = nullptr;

void* FindRelativeTransformWithPath_addr = nullptr;

static void InitAddress()
{
	SetParent_addr = il2cpp_resolve_icall("UnityEngine.Transform::SetParent()");
	FindRelativeTransformWithPath_addr = il2cpp_resolve_icall("UnityEngine.Transform::FindRelativeTransformWithPath()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	void Transform::SetParent(Il2CppObject* parent, bool worldPositionStays)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*, bool)>(SetParent_addr)(instance, parent, worldPositionStays);
	}

	Transform Transform::FindRelativeTransformWithPath(Transform transform, Il2CppString* path, bool isActiveOnly)
	{
		auto relative = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(FindRelativeTransformWithPath_addr)(transform, path, isActiveOnly);
		return Transform(relative);
	}
}
