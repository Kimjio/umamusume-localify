#include <stdinclude.hpp>
#include <Shlwapi.h>
#include <WinTrust.h>
#include <mscat.h>

#include <algorithm>
#include <vector>

#include <filesystem>

#include "MINT.h"

using namespace std;

namespace
{
	bool mh_inited = false;
}

void* NtCreateFile_orig = nullptr;
NTSTATUS
NTAPI
NtCreateFile_hook(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER AllocationSize,
	_In_ ULONG FileAttributes,
	_In_ ULONG ShareAccess,
	_In_ ULONG CreateDisposition,
	_In_ ULONG CreateOptions,
	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
	_In_ ULONG EaLength
)
{
	if (wstring(ObjectAttributes->ObjectName->Buffer).find(L"VERSION.dll") != wstring::npos)
	{
		UNICODE_STRING newName = RTL_CONSTANT_STRING(L"\\??\\C:\\Windows\\System32\\VERSION.dll");
		ObjectAttributes->ObjectName = &newName;
	}
	return reinterpret_cast<decltype(NtCreateFile)*>(NtCreateFile_orig)(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

void* NtQueryAttributesFile_orig = nullptr;
NTSTATUS
NTAPI
NtQueryAttributesFile_hook(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PFILE_BASIC_INFORMATION FileInformation
)
{
	if (wstring(reinterpret_cast<wchar_t*>(ObjectAttributes->ObjectName->Buffer)).find(L".Local") != wstring::npos ||
		wstring(reinterpret_cast<wchar_t*>(ObjectAttributes->ObjectName->Buffer)).find(L".local") != wstring::npos)
	{
		// Block DotLocal files to load system DLLs
		return STATUS_NOT_FOUND;
	}

	return reinterpret_cast<decltype(NtQueryAttributesFile)*>(NtQueryAttributesFile_orig)(ObjectAttributes, FileInformation);
}

void* PathFileExistsW_orig = nullptr;
BOOL PathFileExistsW_hook(_In_ LPCWSTR pszPath)
{
	if (wstring(pszPath).find(L".local") != wstring::npos)
	{
		// Bypass DotLocal directory
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}

	return reinterpret_cast<decltype(PathFileExistsW)*>(PathFileExistsW_orig)(pszPath);
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

	if (filesystem::current_path().string() + "\\umamusume_Data\\\\Plugins\\\\x86_64\\\\\\*" == lpFileName)
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

void* GetModuleHandleW_orig = nullptr;
HMODULE
WINAPI
GetModuleHandleW_hook(
	_In_opt_ LPCWSTR lpModuleName
)
{
	if (lpModuleName)
	{
		if (lpModuleName == L"version.dll"s)
		{
			return LoadLibraryW(L"C:\\Windows\\System32\\VERSION.dll");
		}
	}
	return reinterpret_cast<decltype(GetModuleHandleW)*>(GetModuleHandleW_orig)(lpModuleName);
}

bool init_hook()
{
	if (mh_inited)
		return false;

	if (MH_Initialize() != MH_OK)
		return false;

	mh_inited = true;

	filesystem::path path = "umamusume_Data\\Plugins\\x86_64";
	for (const auto& entry : filesystem::directory_iterator(path))
	{
		expectedDlls.emplace_back(entry.path().filename().string());
	}

	MH_CreateHook(PathFileExistsW, PathFileExistsW_hook, &PathFileExistsW_orig);
	MH_EnableHook(PathFileExistsW);

	MH_CreateHook(NtQueryAttributesFile, NtQueryAttributesFile_hook, &NtQueryAttributesFile_orig);
	MH_EnableHook(NtQueryAttributesFile);

	MH_CreateHook(FindFirstFileA, FindFirstFileA_hook, &FindFirstFileA_orig);
	MH_EnableHook(FindFirstFileA);

	MH_CreateHook(FindNextFileA, FindNextFileA_hook, &FindNextFileA_orig);
	MH_EnableHook(FindNextFileA);

    MH_CreateHook(GetModuleHandleW, GetModuleHandleW_hook, &GetModuleHandleW_orig);
	MH_EnableHook(GetModuleHandleW);

	MH_CreateHook(NtCreateFile, NtCreateFile_hook, &NtCreateFile_orig);
	MH_EnableHook(NtCreateFile);

	hook::proxy init{};

	return true;
}

void uninit_hook()
{
	if (!mh_inited)
		return;

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
