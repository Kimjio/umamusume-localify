#pragma once
#include "MonoSingleton.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector3.hpp"

namespace Gallop
{
	class FrameRateController : public MonoSingleton<FrameRateController>
	{
	public:
		enum class FrameRateOverrideLayer
		{
			SystemValue,
			SceneOverride,
			ViewOverride,
			ViewOverrideSub,
			DialogOpenAnimation,
			DialogCloseAnimation,
			LiveInitialize,
			StoryMovieInitialize,
			ChangeViewProcess,
			TitleDownloading,
		};

		int GetLayerFrameRate(FrameRateOverrideLayer layer);
		void ReflectionFrameRate();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
