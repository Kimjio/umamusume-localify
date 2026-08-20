#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "SceneManager.hpp"

namespace
{
    Il2CppClass* SceneDefine_SceneId_klass = nullptr;

    Il2CppMethodPointer SceneManager_GetCurrentSceneId_addr = nullptr;

    Il2CppMethodPointer SceneManager_GetCurrentViewController_addr = nullptr;

    Il2CppMethodPointer SceneManager_GetCurrentSceneController_addr = nullptr;

    Il2CppMethodPointer SceneManager_BootView_addr = nullptr;
}

static void InitAddress()
{
    auto SceneManager_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SceneManager");
    SceneManager_GetCurrentSceneId_addr = il2cpp_symbols::get_method_pointer(SceneManager_klass, "GetCurrentSceneId", 0);
    SceneManager_GetCurrentViewController_addr = il2cpp_symbols::find_method(SceneManager_klass,
        [](const MethodInfo* info)
        {
            if (Game::CurrentUnityVersion == Game::UnityVersion::Unity20)
            {
                auto info2020 = reinterpret_cast<const MethodInfo2020*>(info);
                return info2020->name == "GetCurrentViewController"s && !info2020->is_generic;
            }

            return info->name == "GetCurrentViewController"s && !info->is_generic;
        }
    );
    SceneManager_GetCurrentSceneController_addr = il2cpp_symbols::find_method(SceneManager_klass,
        [](const MethodInfo* info)
        {
            if (Game::CurrentUnityVersion == Game::UnityVersion::Unity20)
            {
                auto info2020 = reinterpret_cast<const MethodInfo2020*>(info);
                return info2020->name == "GetCurrentSceneController"s && !info2020->is_generic;
            }

            return info->name == "GetCurrentSceneController"s && !info->is_generic;
        }
    );
	SceneManager_BootView_addr = il2cpp_symbols::get_method_pointer(SceneManager_klass, "BootView", 0);
    SceneDefine_SceneId_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SceneDefine/SceneId");
}

STATIC
{
    il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
    const char* SceneManager::namespaze = "Gallop";
    const char* SceneManager::klassName = "SceneManager";

    int SceneManager::GetCurrentSceneId()
    {
        return reinterpret_cast<int (*)(Il2CppObject*)>(SceneManager_GetCurrentSceneId_addr)(instance);
    }

    Il2CppString* SceneManager::GetCurrentSceneIdName()
    {
        return GetEnumName(GetRuntimeType(SceneDefine_SceneId_klass), GetCurrentSceneId());
    }

    Il2CppObject* SceneManager::GetCurrentViewController()
    {
        return reinterpret_cast<Il2CppObject* (*)(Il2CppObject*)>(SceneManager_GetCurrentViewController_addr)(instance);
    }

    Il2CppObject* SceneManager::GetCurrentSceneController()
    {
        return reinterpret_cast<Il2CppObject* (*)(Il2CppObject*)>(SceneManager_GetCurrentSceneController_addr)(instance);
    }

	void SceneManager::BootView()
	{
		reinterpret_cast<void (*)(Il2CppObject*)>(SceneManager_BootView_addr)(instance);
	}
}
