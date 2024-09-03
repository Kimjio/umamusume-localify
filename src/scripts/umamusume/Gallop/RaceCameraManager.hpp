#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "MonoSingleton.hpp"
#include "LowResolutionCameraUtil.hpp"

namespace Gallop
{
	class RaceCameraManager : public MonoSingleton<RaceCameraManager>
	{
	public:
		void SetupOrientation(LowResolutionCameraUtil::DrawDirection drawDirection);
		
		Il2CppObject* ChangeOrientationCoroutine(LowResolutionCameraUtil::DrawDirection direction, bool isFade,
			Il2CppDelegate_t<void (*)(LowResolutionCameraUtil::DrawDirection, bool)>* onChangeResolution);

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
