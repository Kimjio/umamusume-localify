#include "fpp.h"
#include "MINT.h"

#include <iostream>
#include <Psapi.h>
#include <vector>
#include <string>

using namespace std;

struct DiskSectionInfo
{
	string name;
	ULONG_PTR rvaStart;
	ULONG_PTR rvaEnd;
};

static vector<DiskSectionInfo> GetSectionRangesFromDiskPath(const wstring& filePath)
{
	vector<DiskSectionInfo> result;
	HANDLE hFile = CreateFileW(filePath.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return result;
	}

	HANDLE hMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapping)
	{
		LPVOID fileBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		if (fileBase)
		{
			const auto dosHeader = static_cast<const IMAGE_DOS_HEADER*>(fileBase);
			const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>(static_cast<const std::byte*>(fileBase) + dosHeader->e_lfanew);
			const auto sections = IMAGE_FIRST_SECTION(ntHeader);

			for (WORD i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
			{
				const auto& section = sections[i];
				char nameBuf[9] = { 0 };
				memcpy(nameBuf, section.Name, 8);

				DiskSectionInfo info;
				info.name = nameBuf;
				info.rvaStart = section.VirtualAddress;
				info.rvaEnd = section.VirtualAddress + section.Misc.VirtualSize;
				result.push_back(info);
			}
			UnmapViewOfFile(fileBase);
		}
		CloseHandle(hMapping);
	}
	CloseHandle(hFile);
	return result;
}

static bool RegionIsMappedView(HANDLE ProcessHandle, PVOID BaseAddress, SIZE_T RegionSize)
{
	MEMORY_BASIC_INFORMATION mbi{};
	SIZE_T returnLength = 0;
	NTSTATUS status = NtQueryVirtualMemory(ProcessHandle, BaseAddress, MemoryBasicInformation, PVOID(&mbi), sizeof(mbi), &returnLength);
	return status == STATUS_SUCCESS &&
		mbi.State == MEM_COMMIT &&
		mbi.Type == MEM_MAPPED;
}

static bool ViewHasProtectedProtection(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	SIZE_T RegionSize,
	DWORD NewProtection
)
{
	PVOID regionBase = BaseAddress;
	SIZE_T regionSize = RegionSize;
	DWORD oldProtection = 0;
	NTSTATUS status = NtProtectVirtualMemory(ProcessHandle, &regionBase, &regionSize, NewProtection, &oldProtection);
	return status == STATUS_INVALID_PAGE_PROTECTION || status == STATUS_SECTION_PROTECTION;
}

static void UnhookNtdll() {
	auto hookedNtdll = GetModuleHandleA("ntdll.dll");
	if (!hookedNtdll)
	{
		return;
	}

	auto file = CreateFileA(R"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	auto mapping = CreateFileMappingA(file, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
	if (!mapping)
	{
		CloseHandle(file);
		return;
	}

	auto cleanNtdll = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
	if (!cleanNtdll)
	{
		CloseHandle(mapping);
		CloseHandle(file);
		return;
	}

	auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hookedNtdll);
	auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD_PTR>(hookedNtdll) + dosHeader->e_lfanew);

	for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		auto sectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(reinterpret_cast<DWORD_PTR>(IMAGE_FIRST_SECTION(ntHeaders)) + (static_cast<DWORD_PTR>(i) * IMAGE_SIZEOF_SECTION_HEADER));

		if (strcmp(reinterpret_cast<const char*>(sectionHeader->Name), ".text") == 0)
		{
			auto hookedTextSection = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(hookedNtdll) + sectionHeader->VirtualAddress);
			auto cleanTextSection = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(cleanNtdll) + sectionHeader->VirtualAddress);
			SIZE_T size = sectionHeader->Misc.VirtualSize;

			DWORD oldProtect;
			if (VirtualProtect(hookedTextSection, size, PAGE_EXECUTE_READWRITE, &oldProtect))
			{
				memcpy(hookedTextSection, cleanTextSection, size);
				VirtualProtect(hookedTextSection, size, oldProtect, &oldProtect);
				break;
			}
		}
	}

	UnmapViewOfFile(cleanNtdll);
	CloseHandle(mapping);
	CloseHandle(file);
	return;
}

static bool RemapViewOfSection(HANDLE ProcessHandle,
	PVOID BaseAddress,
	SIZE_T RegionSize,
	DWORD NewProtection)
{
	PVOID CopyBuffer = VirtualAlloc(nullptr, RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!CopyBuffer)
	{
		return false;
	}

	bool result = false;

	SIZE_T numberOfBytesRead = 0;
	if (ReadProcessMemory(ProcessHandle, BaseAddress, CopyBuffer, RegionSize, &numberOfBytesRead))
	{
		HANDLE hSection = nullptr;
		LARGE_INTEGER sectionMaxSize{};
		sectionMaxSize.QuadPart = RegionSize;

		if (NT_SUCCESS(NtCreateSection(&hSection, SECTION_ALL_ACCESS, nullptr, &sectionMaxSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, nullptr)))
		{
			NTSTATUS status = NtUnmapViewOfSection(ProcessHandle, BaseAddress);

			if (NT_SUCCESS(status))
			{
				PVOID viewBase = BaseAddress;
				LARGE_INTEGER sectionOffset{};
				SIZE_T viewSize = 0;
				if (NT_SUCCESS(NtMapViewOfSection(hSection, ProcessHandle, &viewBase, 0, RegionSize, &sectionOffset, &viewSize, ViewUnmap, 0, NewProtection)))
				{
					SIZE_T numberOfBytesWritten = 0;
					if (WriteProcessMemory(ProcessHandle, viewBase, CopyBuffer, viewSize, &numberOfBytesWritten))
					{
						result = true;
					}
				}
			}
		}
	}

	VirtualFree(CopyBuffer, 0, MEM_RELEASE);
	return result;
}

void fpp::ChangeGameAssemblyProtection(HMODULE module)
{
	UnhookNtdll();

	MODULEINFO info;
	DWORD oldProtection = 0;
	HANDLE hProcess = GetCurrentProcess();
	GetModuleInformation(hProcess, module, &info, sizeof(info));

	wchar_t pathBuffer[MAX_PATH] = { 0 };
	if (GetModuleFileNameExW(hProcess, module, pathBuffer, MAX_PATH) == 0)
	{
		GetMappedFileNameW(hProcess, module, pathBuffer, MAX_PATH);
	}

	wstring dllDiskPath(pathBuffer);
	vector<DiskSectionInfo> diskSections = GetSectionRangesFromDiskPath(dllDiskPath);
	if (diskSections.empty())
	{
		return;
	}

	ULONG_PTR modBase = reinterpret_cast<ULONG_PTR>(module);
	ULONG_PTR modEnd = modBase + info.SizeOfImage;
	LPVOID pageStart = module;

	while (reinterpret_cast<ULONG_PTR>(pageStart) < modEnd)
	{
		MEMORY_BASIC_INFORMATION mbi{};
		if (!VirtualQueryEx(hProcess, pageStart, &mbi, sizeof(mbi)))
		{
			break;
		}

		if (reinterpret_cast<ULONG_PTR>(mbi.BaseAddress) >= modEnd)
		{
			break;
		}

		SIZE_T RegionSize = mbi.RegionSize;

		if (mbi.State == MEM_COMMIT)
		{
			PVOID currentAllocBase = mbi.AllocationBase;

			while (true)
			{
				PVOID nextAddress = reinterpret_cast<PVOID>(reinterpret_cast<ULONG_PTR>(mbi.BaseAddress) + RegionSize);
				if (reinterpret_cast<ULONG_PTR>(nextAddress) >= modEnd)
				{
					break;
				}

				MEMORY_BASIC_INFORMATION nextMbi{};
				if (!VirtualQueryEx(hProcess, nextAddress, &nextMbi, sizeof(nextMbi)))
				{
					break;
				}

				if (nextMbi.State == MEM_COMMIT && nextMbi.AllocationBase == currentAllocBase)
				{
					RegionSize += nextMbi.RegionSize;
				}
				else
				{
					break;
				}
			}

			ULONG_PTR pageStartRVA = reinterpret_cast<ULONG_PTR>(mbi.BaseAddress) - modBase;
			ULONG_PTR pageEndRVA = pageStartRVA + RegionSize;

			const char* sectionName = nullptr;
			for (const auto& sec : diskSections)
			{
				ULONG_PTR overlapStart = max(pageStartRVA, sec.rvaStart);
				ULONG_PTR overlapEnd = min(pageEndRVA, sec.rvaEnd);

				if (overlapStart < overlapEnd)
				{
					sectionName = sec.name.data();
					break;
				}
			}

			if (sectionName)
			{
				if (RegionIsMappedView(hProcess, mbi.BaseAddress, RegionSize))
				{
					if (!RemapViewOfSection(hProcess, mbi.BaseAddress, RegionSize, PAGE_EXECUTE_READWRITE))
					{
						break;
					}
				}
			}
		}

		LPVOID newAddress = reinterpret_cast<LPVOID>(reinterpret_cast<ULONG_PTR>(mbi.BaseAddress) + RegionSize);
		if (newAddress <= pageStart)
		{
			break;
		}
		pageStart = newAddress;
	}
}
