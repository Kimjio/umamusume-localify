#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Component.hpp"

namespace UnityEngine
{
	class Transform : public Component
	{
	public:
		void SetParent(Transform parent)
		{
			SetParent(parent, true);
		}

		void SetParent(Transform parent, bool worldPositionStays)
		{
			SetParent(parent.NativeObject(), worldPositionStays);
		}

		void SetParent(Il2CppObject* parent, bool worldPositionStays);

		Transform Find(Il2CppString* name)
		{
			return FindRelativeTransformWithPath(instance, name, false);
		}

		using Component::Component;

	private:
		Transform FindRelativeTransformWithPath(Transform transform, Il2CppString* path, bool isActiveOnly);
	};
}
