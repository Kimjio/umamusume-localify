#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop
{
	class LandscapeUIManager : public UnityEngine::MonoBehaviour
	{
	public:
		enum class CameraType
		{
			None,
			Left,
			Right,
			Center,
		};

		static float WindowScaleRate();

		static CameraType GetTouchPositionCameraType(float touchPositionX);

		static float GetTouchPositionOffsetX(CameraType cameraType);

		using MonoBehaviour::MonoBehaviour;
	};
}
