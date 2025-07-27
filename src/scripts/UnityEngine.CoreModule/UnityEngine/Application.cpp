#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Application.hpp"

#include "game.hpp"
#include "config/config.hpp"

#include <filesystem>

void* Quit_addr = nullptr;
void* Quit_orig = nullptr;

void* OpenURL_addr = nullptr;

void* get_unityVersion_addr = nullptr;

void* get_companyName_addr = nullptr;

void* get_productName_addr = nullptr;

void* get_persistentDataPath_addr = nullptr;

void* set_targetFrameRate_addr = nullptr;
void* set_targetFrameRate_orig = nullptr;

void* get_targetFrameRate_addr = nullptr;

void* get_internetReachability_addr = nullptr;

static void Quit_hook(int exitCode)
{
	UnityEngine::Application::Exit(exitCode);
}

static void set_targetFrameRate_hook(int value)
{
	reinterpret_cast<decltype(set_targetFrameRate_hook)*>(set_targetFrameRate_orig)(config::runtime::useDefaultFPS ? value : config::max_fps);
}

static void InitAddress()
{
	Quit_addr = il2cpp_resolve_icall("UnityEngine.Application::Quit(System.Int32)");
	OpenURL_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "Application", "OpenURL", 1);
	get_unityVersion_addr = il2cpp_resolve_icall("UnityEngine.Application::get_unityVersion()");
	get_companyName_addr = il2cpp_resolve_icall("UnityEngine.Application::get_companyName()");
	get_productName_addr = il2cpp_resolve_icall("UnityEngine.Application::get_productName()");
	get_persistentDataPath_addr = il2cpp_resolve_icall("UnityEngine.Application::get_persistentDataPath()");
	set_targetFrameRate_addr = il2cpp_resolve_icall("UnityEngine.Application::set_targetFrameRate(System.Int32)");
	get_targetFrameRate_addr = il2cpp_resolve_icall("UnityEngine.Application::get_targetFrameRate()");
	get_internetReachability_addr = il2cpp_resolve_icall("UnityEngine.Application::get_internetReachability()");
}

static void HookMethods()
{
	ADD_HOOK(Quit, "UnityEngine.Application::Quit at %p\n");

	if (config::max_fps > -1)
	{
		ADD_HOOK(set_targetFrameRate, "UnityEngine.Application.set_targetFrameRate at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	void Application::Quit(int exitCode)
	{
		reinterpret_cast<decltype(Quit)*>(Quit_addr)(exitCode);
	}

	void Application::OpenURL(Il2CppString* url)
	{
		reinterpret_cast<decltype(OpenURL)*>(OpenURL_addr)(url);
	}

	Il2CppString* Application::unityVersion()
	{
		return reinterpret_cast<Il2CppString * (*)()>(get_unityVersion_addr)();
	}

	Il2CppString* Application::companyName()
	{
		return reinterpret_cast<Il2CppString * (*)()>(get_companyName_addr)();
	}

	Il2CppString* Application::productName()
	{
		return reinterpret_cast<Il2CppString * (*)()>(get_productName_addr)();
	}

	Il2CppString* Application::persistentDataPath()
	{
		return reinterpret_cast<Il2CppString * (*)()>(get_persistentDataPath_addr)();
	}

	void Application::targetFrameRate(int value)
	{
		reinterpret_cast<void (*)(int)>(set_targetFrameRate_addr)(value);
	}

	int Application::targetFrameRate()
	{
		return reinterpret_cast<int (*)()>(get_targetFrameRate_addr)();
	}

	NetworkReachability Application::internetReachability()
	{
		return reinterpret_cast<NetworkReachability(*)()>(get_internetReachability_addr)();
	}

	void Application::Exit(int exitCode)
	{
#ifdef _MSC_VER
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			auto module_path = filesystem::current_path();
			auto uncheater_path_new = module_path.string().append("\\umamusume_Data\\StreamingAssets\\_Uncheater"s).data();
			auto uncheater_path = module_path.string().append("\\umamusume_Data\\StreamingAssets\\Uncheater"s).data();
			if (filesystem::exists(uncheater_path_new))
			{
				try
				{
					filesystem::rename(uncheater_path_new, uncheater_path);
				}
				catch (exception& e)
				{
					cout << "Uncheater rename error: " << e.what() << endl;
				}
			}
		}

		TerminateProcess(GetCurrentProcess(), exitCode);
#else
		reinterpret_cast<decltype(Quit)*>(Quit_orig)(exitCode);
#endif
	}
}
