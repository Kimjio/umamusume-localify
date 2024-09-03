#pragma once

#include "il2cpp/il2cpp_symbols.hpp"

namespace CriWare
{
	class CriAtomEx
	{
	public:
		enum Randomize3dCalcType
		{
			None = -1,
			Rectangle,
			Cuboid,
			Circle,
			Cylinder,
			Sphere,
			List = 6
		};

		struct Randomize3dConfig
		{
		public:
			const int NumOfCalcParams = 3;
			bool followsOriginalSource;
			CriWare::CriAtomEx::Randomize3dCalcType calculationType;
			Il2CppArraySize_t<float>* calculationParameters;
		};

		struct CuePos3dInfo
		{
		public:
			float coneInsideAngle;
			float coneOutsideAngle;
			float minAttenuationDistance;
			float maxAttenuationDistance;
			float sourceRadius;
			float interiorDistance;
			float dopplerFactor;
			CriWare::CriAtomEx::Randomize3dConfig randomPos;
			unsigned short distanceAisacControl;
			unsigned short listenerBaseAngleAisacControl;
			unsigned short sourceBaseAngleAisacControl;
			unsigned short listenerBaseElevationAisacControl;
			unsigned short sourceBaseElevationAisacControl;
			unsigned short reserved[1];
		};

		struct GameVariableInfo
		{
		public:
			char* name;
			unsigned int id;
			float gameValue;
		};

		struct CueInfo
		{
		public:
			int id;
			int type;
			char* name;
			char* userData;
			size_t length;
			unsigned short categories[16];
			short numLimits;
			unsigned short numBlocks;
			unsigned short numTracks;
			unsigned short numRelatedWaveForms;
			char priority;
			char headerVisibility;
			char ignore_player_parameter;
			char probability;
			int panType;
			CriWare::CriAtomEx::CuePos3dInfo pos3dInfo;
			CriWare::CriAtomEx::GameVariableInfo gameVariableInfo;
		};
	};
}
