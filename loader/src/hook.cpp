#include <stdinclude.hpp>
#include <Shlwapi.h>

#include <algorithm>
#include <vector>

#include "MINT.h"

using namespace std;

namespace
{
	string module_name;
}

void* NtQueryAttributesFile_orig = nullptr;
NTSTATUS
NTAPI
NtQueryAttributesFile_hook(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PFILE_BASIC_INFORMATION FileInformation
)
{
	if (ObjectAttributes)
	{
		if (wstring(reinterpret_cast<wchar_t*>(ObjectAttributes->ObjectName->Buffer)).find(L".Local") != wstring::npos ||
			wstring(reinterpret_cast<wchar_t*>(ObjectAttributes->ObjectName->Buffer)).find(L".local") != wstring::npos)
		{
			// Block DotLocal files to load system DLLs
			return STATUS_NOT_FOUND;
		}
	}

	return reinterpret_cast<decltype(NtQueryAttributesFile)*>(NtQueryAttributesFile_orig)(ObjectAttributes, FileInformation);
}

void* PathFileExistsW_orig = nullptr;
BOOL PathFileExistsW_hook(_In_ LPCWSTR pszPath)
{
	if (wstring(pszPath).find(L".local") != wstring::npos ||
		wstring(pszPath).find(L".Local") != wstring::npos)
	{
		// Bypass DotLocal directory
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}

	return reinterpret_cast<decltype(PathFileExistsW)*>(PathFileExistsW_orig)(pszPath);
}

void* GetFileAttributesW_orig = nullptr;
DWORD
WINAPI
GetFileAttributesW_hook(
	_In_ LPCWSTR lpFileName
)
{
	if (wstring(lpFileName).find(L".local") != wstring::npos ||
		wstring(lpFileName).find(L".Local") != wstring::npos)
	{
		// Bypass DotLocal directory
		SetLastError(ERROR_FILE_NOT_FOUND);
		return INVALID_FILE_ATTRIBUTES;
	}

	return reinterpret_cast<decltype(GetFileAttributesW)*>(GetFileAttributesW_orig)(lpFileName);
}


HANDLE currentRootFindHandle;
HANDLE currentPluginsFindHandle;

void* FindFirstFileA_orig = nullptr;
HANDLE
WINAPI
FindFirstFileA_hook(
	_In_ LPCSTR lpFileName,
	_Out_ LPWIN32_FIND_DATAA lpFindFileData
)
{
	auto result = reinterpret_cast<decltype(FindFirstFileA)*>(FindFirstFileA_orig)(lpFileName, lpFindFileData);

	if (filesystem::current_path().string() + "\\\\*" == lpFileName)
	{
		currentRootFindHandle = result;
	}

	if (filesystem::current_path().string() + "\\" + module_name + "_Data\\\\Plugins\\\\x86_64\\\\\\ * " == lpFileName)
	{
		currentPluginsFindHandle = result;
	}

	return result;
}

vector<string> expectedDlls = {
	"baselib.dll",
	"GameAssembly.dll",
	"UnityPlayer.dll"
};

void* FindNextFileA_orig = nullptr;
BOOL
WINAPI
FindNextFileA_hook(
	_In_ HANDLE hFindFile,
	_Out_ LPWIN32_FIND_DATAA lpFindFileData
)
{
	auto result = reinterpret_cast<decltype(FindNextFileA)*>(FindNextFileA_orig)(hFindFile, lpFindFileData);

	if (hFindFile == currentRootFindHandle ||
		hFindFile == currentPluginsFindHandle)
	{
		if (string(lpFindFileData->cFileName).find(".dll") != string::npos &&
			find(expectedDlls.begin(), expectedDlls.end(), lpFindFileData->cFileName) == expectedDlls.end())
		{
			// Unexpected dll
			*lpFindFileData = WIN32_FIND_DATAA{};
		}
	}

	return result;
}

void* LoadLibraryExW_orig = nullptr;
HMODULE
WINAPI
LoadLibraryExW_hook(
	_In_ LPCWSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
)
{
	if (lpLibFileName && wstring(lpLibFileName).find(L"UnityPlayer.dll") != wstring::npos)
	{
		auto version = reinterpret_cast<decltype(LoadLibraryExW)*>(LoadLibraryExW_orig)(L"localify.dll", hFile, dwFlags);
		wstring module_name;
		module_name.resize(MAX_PATH);
		module_name.resize(GetModuleFileNameW(version, module_name.data(), MAX_PATH));
	}
	return reinterpret_cast<decltype(LoadLibraryExW)*>(LoadLibraryExW_orig)(lpLibFileName, hFile, dwFlags);
}

void* CreateProcessW_orig = nullptr;
BOOL
WINAPI
CreateProcessW_hook(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
	return reinterpret_cast<decltype(CreateProcessW)*>(CreateProcessW_orig)(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);
}

void* CheckRemoteDebuggerPresent_orig = nullptr;
BOOL
WINAPI
CheckRemoteDebuggerPresent_hook(
	_In_ HANDLE hProcess,
	_Out_ PBOOL pbDebuggerPresent
)
{
	*pbDebuggerPresent = FALSE;
	return TRUE;
}

void* IsDebuggerPresent_orig = nullptr;
BOOL
WINAPI
IsDebuggerPresent_hook(
	VOID
)
{
	return FALSE;
}

void init_hook(filesystem::path module_path)
{
	if (MH_Initialize() != MH_OK) return;

	auto PathFileExistsW_addr = GetProcAddress(GetModuleHandleW(L"KernelBase.dll"), "PathFileExistsW");
	MH_CreateHook(PathFileExistsW_addr, PathFileExistsW_hook, &PathFileExistsW_orig);
	MH_EnableHook(PathFileExistsW_addr);

	auto GetFileAttributesW_addr = GetProcAddress(GetModuleHandleW(L"KernelBase.dll"), "GetFileAttributesW");
	MH_CreateHook(GetFileAttributesW_addr, GetFileAttributesW_hook, &GetFileAttributesW_orig);
	MH_EnableHook(GetFileAttributesW_addr);

	auto NtQueryAttributesFile_addr = GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryAttributesFile");
	MH_CreateHook(NtQueryAttributesFile_addr, NtQueryAttributesFile_hook, &NtQueryAttributesFile_orig);
	MH_EnableHook(NtQueryAttributesFile_addr);

	auto LoadLibraryExW_addr = GetProcAddress(GetModuleHandleW(L"KernelBase.dll"), "LoadLibraryExW");
	MH_CreateHook(LoadLibraryExW_addr, LoadLibraryExW_hook, &LoadLibraryExW_orig);
	MH_EnableHook(LoadLibraryExW_addr);

	auto CheckRemoteDebuggerPresent_addr = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CheckRemoteDebuggerPresent");
	MH_CreateHook(CheckRemoteDebuggerPresent_addr, CheckRemoteDebuggerPresent_hook, &CheckRemoteDebuggerPresent_orig);
	MH_EnableHook(CheckRemoteDebuggerPresent_addr);

	auto IsDebuggerPresent_addr = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsDebuggerPresent");
	MH_CreateHook(IsDebuggerPresent_addr, IsDebuggerPresent_hook, &IsDebuggerPresent_orig);
	MH_EnableHook(IsDebuggerPresent_addr);

	hook::proxy init{};

	module_name = module_path.filename().replace_extension().string();
	filesystem::path path = module_name.append("_Data\\Plugins\\x86_64");

	for (const auto& entry : filesystem::directory_iterator(path))
	{
		expectedDlls.emplace_back(entry.path().filename().string());
	}

	MH_CreateHook(FindFirstFileA, FindFirstFileA_hook, &FindFirstFileA_orig);
	MH_EnableHook(FindFirstFileA);

	MH_CreateHook(FindNextFileA, FindNextFileA_hook, &FindNextFileA_orig);
	MH_EnableHook(FindNextFileA);
}

void uninit_hook()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
