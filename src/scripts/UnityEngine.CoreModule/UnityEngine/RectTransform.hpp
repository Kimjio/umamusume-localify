#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Transform.hpp"
#include "Vector2.hpp"
#include "Rect.hpp"

namespace UnityEngine
{
	class RectTransform : public Transform
	{
	public:
		void anchoredPosition(Vector2 value);
		Vector2 anchoredPosition();
		void anchorMax(Vector2 value);
		Vector2 anchorMax();
		void anchorMin(Vector2 value);
		Vector2 anchorMin();
		void pivot(Vector2 value);
		Vector2 pivot();
		void sizeDelta(Vector2 value);
		Vector2 sizeDelta();
		Rect rect();

		using Transform::Transform;
	};
}
