#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

#include <cstdint>

namespace Gallop
{
    class SceneManager : public MonoSingleton<SceneManager>
    {
    public:
        uint64_t GetCurrentSceneId();

        Il2CppString* GetCurrentSceneIdName();

        Il2CppObject* GetCurrentViewController();

        Il2CppObject* GetCurrentSceneController();

        void BootView();

        using MonoSingleton::MonoSingleton;

        static const char* namespaze;
        static const char* klassName;
    };
}
