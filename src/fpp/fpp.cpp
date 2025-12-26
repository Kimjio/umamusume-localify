#include "fpp.h"
#include "MINT.h"

#include <iostream>
#include <Psapi.h>
#include <vector>
#include <string>

using namespace std;

static bool RegionIsMappedView(HANDLE ProcessHandle, PVOID BaseAddress, SIZE_T RegionSize)
{
	MEMORY_BASIC_INFORMATION mbi{};
	SIZE_T returnLength = 0;
	NTSTATUS status = NtQueryVirtualMemory(ProcessHandle, BaseAddress, MemoryBasicInformation, PVOID(&mbi), sizeof(mbi), &returnLength);
	return status == STATUS_SUCCESS &&
		mbi.State == MEM_COMMIT &&
		mbi.Type == MEM_MAPPED;
}

static bool ViewHasProtectedProtection(HANDLE ProcessHandle,
	PVOID BaseAddress,
	SIZE_T RegionSize,
	DWORD NewProtection)
{
	PVOID regionBase = BaseAddress;
	SIZE_T regionSize = RegionSize;
	DWORD oldProtection = 0;
	NTSTATUS status = NtProtectVirtualMemory(ProcessHandle, &regionBase, &regionSize, NewProtection, &oldProtection);
	return status == STATUS_INVALID_PAGE_PROTECTION ||
		status == STATUS_SECTION_PROTECTION;
}

static DWORD GetSyscallNumber(const char* functionName)
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	if (!hNtdll)
	{
		return 0;
	}

	FARPROC funcAddr = GetProcAddress(hNtdll, functionName);
	if (!funcAddr)
	{
		return 0;
	}

	BYTE* pByte = reinterpret_cast<BYTE*>(funcAddr);
	if (pByte[0] == 0x4C && pByte[1] == 0x8B && pByte[2] == 0xD1 && pByte[3] == 0xB8)
	{
		return *reinterpret_cast<DWORD*>(pByte + 4);
	}

	return 0;
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
			static DWORD ssnNtUnmap = 0;
			NTSTATUS status;
			if (ssnNtUnmap == 0)
			{
				ssnNtUnmap = GetSyscallNumber("NtUnmapViewOfSectionEx");
			}

			if (ssnNtUnmap == 0)
			{
				// ntdll corrupted
				return false;
			}

			status = NtUnmapViewOfSectionEx(ProcessHandle, BaseAddress, 0);

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
	MODULEINFO info;
	DWORD oldProtection = 0;
	HANDLE hProcess = GetCurrentProcess();
	GetModuleInformation(hProcess, module, &info, sizeof(info));
	if (!VirtualProtectEx(hProcess, PVOID(module), info.SizeOfImage, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		const auto header = static_cast<const IMAGE_DOS_HEADER*>(info.lpBaseOfDll);
		const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>(
			static_cast<const std::byte*>(info.lpBaseOfDll) + header->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(ntHeader);

		vector<pair<SIZE_T, const char*>> range;

		for (std::size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
		{
			const auto& section = sections[i];

			SIZE_T start = 0;

			if (!range.empty())
			{
				start = range.back().first;
			}

			range.emplace_back(make_pair(start + section.SizeOfRawData, reinterpret_cast<const char*>(section.Name)));

			if (range.back().second == "il2cpp"s)
			{
				break;
			}
		}

		LPVOID pageStart = module;
		SIZE_T numBytes = 0;
		SIZE_T totalRegionSize = 0;
		do
		{
			MEMORY_BASIC_INFORMATION mbi{};
			numBytes = VirtualQueryEx(hProcess, reinterpret_cast<LPVOID>(pageStart), &mbi, sizeof(mbi));

			totalRegionSize += mbi.RegionSize;

			const char* name = nullptr;

			for (auto& pair : range)
			{
				auto size = totalRegionSize;

				if (size <= pair.first)
				{
					name = pair.second;
					break;
				}
			}

			if (!name)
			{
				break;
			}

			if (mbi.State != MEM_FREE)
			{
				if (RegionIsMappedView(hProcess, reinterpret_cast<PVOID>(pageStart), mbi.RegionSize))
				{
					if (ViewHasProtectedProtection(hProcess, reinterpret_cast<PVOID>(pageStart), mbi.RegionSize, PAGE_EXECUTE_READWRITE))
					{
						if (!RemapViewOfSection(hProcess, reinterpret_cast<PVOID>(pageStart), mbi.RegionSize, PAGE_EXECUTE_READWRITE))
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			LPVOID newAddress = reinterpret_cast<LPCH>(mbi.BaseAddress) + mbi.RegionSize;

			if (newAddress <= pageStart)
			{
				break;
			}

			pageStart = newAddress;
		} while (numBytes);
	}
}
