#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/ScreenOrientation.hpp"
#include "scripts/umamusume/Gallop/LocalPushDefine.hpp"

namespace Gallop
{
	class PushNotificationManager
	{
	public:
		static Il2CppString* createFavIconFilePath(int _unitId, LocalPushDefine::LocalPushType localPushType);
	};
}
