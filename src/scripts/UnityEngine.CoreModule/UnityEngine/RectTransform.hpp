#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "Transform.hpp"
#include "Vector2.hpp"

namespace UnityEngine
{
	class RectTransform : public Transform
	{
	public:
		void anchoredPosition(Vector2 value);
		void anchorMax(Vector2 value);
		void anchorMin(Vector2 value);
		void pivot(Vector2 value);
		void sizeDelta(Vector2 value);

		using Transform::Transform;
	};
}
