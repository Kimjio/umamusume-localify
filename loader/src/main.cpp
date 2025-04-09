#include <stdinclude.hpp>

extern bool init_hook();
extern void uninit_hook();

int __stdcall DllMain(HINSTANCE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// the DMM Launcher set start path to system32 wtf????
		wstring module_name;
		module_name.resize(MAX_PATH);
		module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

		filesystem::path module_path(module_name);

		// check name
		if (module_path.filename() != L"umamusume.exe")
			return 1;

		std::filesystem::current_path(
			module_path.parent_path()
		);

		init_hook();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		uninit_hook();
	}

	return 1;
}
