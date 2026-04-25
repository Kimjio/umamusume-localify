#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.UI/UnityEngine/EventSystems/RaycastResult.hpp"

namespace UnityEngine::EventSystems
{
	class GallopStandaloneInputModule : public MonoBehaviour
	{
	public:
		Il2CppObject* GetMousePointerEventData(int id);
		void ProcessMousePress(Il2CppObject* data);
		void ProcessMove(Il2CppObject* pointerEvent);
		void ProcessDrag(Il2CppObject* pointerEvent);

		Il2CppObject* pointerEventData();
		UnityEngine::Vector2 m_MousePosition();
		UnityEngine::Vector2 m_LastMousePosition();
		UnityEngine::EventSystems::RaycastResult CurrentRaycast();

		using MonoBehaviour::MonoBehaviour;
	};
}
