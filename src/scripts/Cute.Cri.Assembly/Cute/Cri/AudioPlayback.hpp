#pragma once

#include "il2cpp/il2cpp-object-internals.h"

#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomExPlayback.hpp"

namespace Cute
{
	namespace Cri
	{
		struct AudioPlayback
		{
		public:
			CriWare::CriAtomExPlayback criAtomExPlayback;
			bool isError;
			int soundGroup;
			bool is3dSound;
			int atomSourceListIndex;
			Il2CppString* cueSheetName;
			Il2CppString* cueName;
			int cueId;
		};
	};
}
