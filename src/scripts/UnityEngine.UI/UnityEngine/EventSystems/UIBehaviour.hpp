#pragma once
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace UnityEngine::EventSystems
{
	class UIBehaviour abstract : public MonoBehaviour
	{
	public:
		virtual bool IsActive()
		{
			return isActiveAndEnabled();
		}

		bool IsDestroyed() const
		{
			return !instance;
		}

		using MonoBehaviour::MonoBehaviour;

	protected:
		virtual void Awake() {}
		virtual void OnEnable() {}
		virtual void Start() {}
		virtual void OnDisable() {}
		virtual void OnDestroy() {}
		virtual void OnRectTransformDimensionsChange() {}
		virtual void OnBeforeTransformParentChanged() {}
		virtual void OnTransformParentChanged() {}
		virtual void OnDidApplyAnimationProperties() {}
		virtual void OnCanvasGroupChanged() {}
		virtual void OnCanvasHierarchyChanged() {}
	};
}
