#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "scripts/umamusume/Gallop/LocalPushDefine.hpp"

using namespace UnityEngine;

namespace Gallop
{
	class PushNotificationManager : public MonoBehaviour
	{
	public:
		static PushNotificationManager Instance();

		static Il2CppString* CreatePushIconFilePath(LocalPushDefine::LocalPushType localPushType);

		Il2CppString* createFavIconFilePath(int _unitId, LocalPushDefine::LocalPushType localPushType);

		int MakeNotificationId(LocalPushDefine::LocalPushType localPushType, int key);

		using MonoBehaviour::MonoBehaviour;
	};
}
