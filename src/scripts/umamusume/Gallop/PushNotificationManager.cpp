#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PushNotificationManager.hpp"

#include "config/config.hpp"

void* createFavIconFilePath_addr = nullptr;
void* legacy_createFavIconFilePath_addr = nullptr;

static void InitAddress()
{
	createFavIconFilePath_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PushNotificationManager", "createFavIconFilePath", 2);
	legacy_createFavIconFilePath_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PushNotificationManager", "createFavIconFilePath", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	Il2CppString* PushNotificationManager::createFavIconFilePath(int _unitId, LocalPushDefine::LocalPushType localPushType)
	{
		if (createFavIconFilePath_addr)
		{
			return reinterpret_cast<decltype(createFavIconFilePath)*>(createFavIconFilePath_addr)(_unitId, localPushType);
		}

		return reinterpret_cast<Il2CppString* (*)(int)>(legacy_createFavIconFilePath_addr)(_unitId);
	}
}
