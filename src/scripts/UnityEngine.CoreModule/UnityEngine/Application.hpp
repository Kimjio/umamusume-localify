#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

namespace UnityEngine
{
	class Application
	{
	public:
		static void Quit(int exitCode);
		static void OpenURL(Il2CppString* url);
		static Il2CppString* unityVersion();
		static Il2CppString* companyName();
		static Il2CppString* persistentDataPath();
		static void targetFrameRate(int value);

		static void Exit(int exitCode);
	};
}