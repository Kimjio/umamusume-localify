#pragma once
#include "il2cpp/il2cpp-object-internals.h"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace UnityEngine::EventSystems
{
	struct RaycastResult
	{
		Il2CppObject* m_GameObject;
		Il2CppObject* module;
		float distance;
		float index;
		int depth;
		int sortingGroupID;
		int sortingGroupOrder;
		int sortingLayer;
		int sortingOrder;
		Vector3 worldPosition;
		Vector3 worldNormal;
		Vector2 screenPosition;
		int displayIndex;
	};
}
