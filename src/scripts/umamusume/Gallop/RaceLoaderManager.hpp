#pragma once

#include "il2cpp/il2cpp-object-internals.h"

namespace Gallop
{
	class RaceLoaderManager
	{
	public:
		struct CourceContext
		{
		public:
			int courseId;
			int timeEnum;
			int seasonEnum;
			int turfGoalGate;
			int turfGoalFlower;
			int dirtGoalGate;
			int dirtGoalFlower;
			int skydomeCourseId;
			int skydomeSeasonEnum;
			int skydomeWeatherEnum;
			int skydomeTimeEnum;
			int audienceEnum;
			int audienceWeatherEnum;
			int audienceSeasonEnum;
			int treeWeaterEnum;
			int treeTimeEnum;
			int RotationCategoryEnum;
			int lightProbeId;
			Il2CppArraySize* materialTeturePairs;
			Il2CppArraySize* materialSubTexturePairs;
			bool halfStartGate;
			int CourseStartGateBaseId;
		};
	};
}
