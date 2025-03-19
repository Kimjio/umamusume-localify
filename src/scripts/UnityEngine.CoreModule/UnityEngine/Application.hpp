#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "NetworkReachability.hpp"

namespace UnityEngine
{
	class Application
	{
	public:
		static void Quit(int exitCode);
		static void OpenURL(Il2CppString* url);
		static Il2CppString* unityVersion();
		static Il2CppString* companyName();
		static Il2CppString* productName();
		static Il2CppString* persistentDataPath();
		static void targetFrameRate(int value);
		static int targetFrameRate();
		static NetworkReachability internetReachability();

		static void Exit(int exitCode);
	};
}