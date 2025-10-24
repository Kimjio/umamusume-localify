#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Component.hpp"
#include "Vector3.hpp"

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

		Transform GetParent();

		Transform Find(Il2CppString* name)
		{
			return FindRelativeTransformWithPath(instance, name, false);
		}

		Vector3 InverseTransformPoint(Vector3 position);

		Vector3 position();

		void position(Vector3 value);

		Vector3 localScale();

		void localScale(Vector3 value);

		int childCount();

		Transform GetChild(int index);

		using Component::Component;

	private:
		Transform FindRelativeTransformWithPath(Transform transform, Il2CppString* path, bool isActiveOnly);
	};
}
