#include <stdinclude.hpp>

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

namespace
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
			auto original_dll = LoadLibraryW(L"version.win.dll");

			GetFileVersionInfoA_Original = GetProcAddress(original_dll, "GetFileVersionInfoA");
			GetFileVersionInfoByHandle_Original = GetProcAddress(original_dll, "GetFileVersionInfoByHandle");
			GetFileVersionInfoExA_Original = GetProcAddress(original_dll, "GetFileVersionInfoExA");
			GetFileVersionInfoExW_Original = GetProcAddress(original_dll, "GetFileVersionInfoExW");
			GetFileVersionInfoSizeA_Original = GetProcAddress(original_dll, "GetFileVersionInfoSizeA");
			GetFileVersionInfoSizeExA_Original = GetProcAddress(original_dll, "GetFileVersionInfoSizeExA");
			GetFileVersionInfoSizeExW_Original = GetProcAddress(original_dll, "GetFileVersionInfoSizeExW");
			GetFileVersionInfoSizeW_Original = GetProcAddress(original_dll, "GetFileVersionInfoSizeW");
			GetFileVersionInfoW_Original = GetProcAddress(original_dll, "GetFileVersionInfoW");
			VerFindFileA_Original = GetProcAddress(original_dll, "VerFindFileA");
			VerFindFileW_Original = GetProcAddress(original_dll, "VerFindFileW");
			VerInstallFileA_Original = GetProcAddress(original_dll, "VerInstallFileA");
			VerInstallFileW_Original = GetProcAddress(original_dll, "VerInstallFileW");
			VerLanguageNameA_Original = GetProcAddress(original_dll, "VerLanguageNameA");
			VerLanguageNameW_Original = GetProcAddress(original_dll, "VerLanguageNameW");
			VerQueryValueA_Original = GetProcAddress(original_dll, "VerQueryValueA");
			VerQueryValueW_Original = GetProcAddress(original_dll, "VerQueryValueW");
		};
	};

	version_init init{};
}
