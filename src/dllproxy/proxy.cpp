#include <stdinclude.hpp>

#include "proxy.hpp"

extern "C"
{
	void* GetFileVersionInfoA_Original = nullptr;
	void* GetFileVersionInfoByHandle_Original = nullptr;
	void* GetFileVersionInfoExA_Original = nullptr;
	void* GetFileVersionInfoExW_Original = nullptr;
	void* GetFileVersionInfoSizeA_Original = nullptr;
	void* GetFileVersionInfoSizeExA_Original = nullptr;
	void* GetFileVersionInfoSizeExW_Original = nullptr;
	void* GetFileVersionInfoSizeW_Original = nullptr;
	void* GetFileVersionInfoW_Original = nullptr;
	void* VerFindFileA_Original = nullptr;
	void* VerFindFileW_Original = nullptr;
	void* VerInstallFileA_Original = nullptr;
	void* VerInstallFileW_Original = nullptr;
	void* VerLanguageNameA_Original = nullptr;
	void* VerLanguageNameW_Original = nullptr;
	void* VerQueryValueA_Original = nullptr;
	void* VerQueryValueW_Original = nullptr;
}

using namespace std;

namespace proxy
{
	class version_init
	{
	public:
		version_init()
		{
			wstring dll_path;
			dll_path.resize(MAX_PATH);
			dll_path.resize(GetSystemDirectoryW(dll_path.data(), MAX_PATH));

			dll_path += LR"(\version.dll)"s;

			try
			{
				filesystem::copy_file(dll_path, L"version.win.dll", filesystem::copy_options::update_existing | filesystem::copy_options::create_symlinks);
			}
			catch (exception&)
			{
			}

			// Loading version.dll from another path is prohibited.
			//auto original_dll = LoadLibraryW(dll_path.data());
			version = LoadLibraryW(L"version.win.dll");

			GetFileVersionInfoA_Original = GetProcAddress(version, "GetFileVersionInfoA");
			GetFileVersionInfoByHandle_Original = GetProcAddress(version, "GetFileVersionInfoByHandle");
			GetFileVersionInfoExA_Original = GetProcAddress(version, "GetFileVersionInfoExA");
			GetFileVersionInfoExW_Original = GetProcAddress(version, "GetFileVersionInfoExW");
			GetFileVersionInfoSizeA_Original = GetProcAddress(version, "GetFileVersionInfoSizeA");
			GetFileVersionInfoSizeExA_Original = GetProcAddress(version, "GetFileVersionInfoSizeExA");
			GetFileVersionInfoSizeExW_Original = GetProcAddress(version, "GetFileVersionInfoSizeExW");
			GetFileVersionInfoSizeW_Original = GetProcAddress(version, "GetFileVersionInfoSizeW");
			GetFileVersionInfoW_Original = GetProcAddress(version, "GetFileVersionInfoW");
			VerFindFileA_Original = GetProcAddress(version, "VerFindFileA");
			VerFindFileW_Original = GetProcAddress(version, "VerFindFileW");
			VerInstallFileA_Original = GetProcAddress(version, "VerInstallFileA");
			VerInstallFileW_Original = GetProcAddress(version, "VerInstallFileW");
			VerLanguageNameA_Original = GetProcAddress(version, "VerLanguageNameA");
			VerLanguageNameW_Original = GetProcAddress(version, "VerLanguageNameW");
			VerQueryValueA_Original = GetProcAddress(version, "VerQueryValueA");
			VerQueryValueW_Original = GetProcAddress(version, "VerQueryValueW");
		};
	};

	version_init init{};
}
