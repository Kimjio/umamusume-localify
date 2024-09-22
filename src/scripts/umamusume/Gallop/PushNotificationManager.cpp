#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PushNotificationManager.hpp"
#include "Cryptographer.hpp"

#include <filesystem>

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"

#include "config/config.hpp"

void* MakeNotificationId_addr = nullptr;
void* createFavIconFilePath_addr = nullptr;
void* legacy_createFavIconFilePath_addr = nullptr;

static void InitAddress()
{
	MakeNotificationId_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PushNotificationManager", "MakeNotificationId", 2);
	createFavIconFilePath_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PushNotificationManager", "createFavIconFilePath", 2);
	legacy_createFavIconFilePath_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PushNotificationManager", "createFavIconFilePath", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	PushNotificationManager PushNotificationManager::Instance()
	{
		auto instanceField = il2cpp_class_get_field_from_name_wrap(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PushNotificationManager"), "_instance");
		Il2CppObject* instance;
		il2cpp_field_static_get_value(instanceField, &instance);
		return PushNotificationManager(instance);
	}

	Il2CppString* PushNotificationManager::CreatePushIconFilePath(LocalPushDefine::LocalPushType localPushType)
	{
		auto id = Instance().MakeNotificationId(localPushType, 0);

		auto iconName = "NotifceIcon"s;
		if (id > 200)
		{
			iconName.append(to_string(id));
		}

		auto fileName = wstring(Cryptographer::MakeMd5(il2cpp_string_new(iconName.data()))->chars);
		fileName.append(L".png");

		auto path = wstring(UnityEngine::Application::persistentDataPath()->chars);
		path.append(L"/lpfi");

		if (!filesystem::exists(path))
		{
			filesystem::create_directories(path);
		}

		return il2cpp_string_new16((path + L"/" + fileName).data());
	}

	int PushNotificationManager::MakeNotificationId(LocalPushDefine::LocalPushType localPushType, int key)
	{
		return reinterpret_cast<int (*)(Il2CppObject*, LocalPushDefine::LocalPushType, int)>(MakeNotificationId_addr)(instance, localPushType, key);
	}

	Il2CppString* PushNotificationManager::createFavIconFilePath(int _unitId, LocalPushDefine::LocalPushType localPushType)
	{
		if (createFavIconFilePath_addr)
		{
			return reinterpret_cast<Il2CppString * (*)(Il2CppObject*, int, LocalPushDefine::LocalPushType)>(createFavIconFilePath_addr)(instance, _unitId, localPushType);
		}

		return reinterpret_cast<Il2CppString * (*)(Il2CppObject*, int)>(legacy_createFavIconFilePath_addr)(instance, _unitId);
	}
}
