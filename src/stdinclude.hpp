#pragma once

#define NOMINMAX

#include <Windows.h>
#include <shlobj.h>
#include <Shlwapi.h>

#include <TlHelp32.h>

#include <cinttypes>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <unordered_map>

#include <MinHook.h>

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include "game.hpp"

#include "experiments.h"

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "local/local.hpp"
#include "logger/logger.hpp"

constexpr auto Cryptographer_KEY = "r!I@mt8e5i=";

// Unity App icon
constexpr auto IDI_APP_ICON = 103;

extern HINSTANCE hInstance;

namespace
{
	BOOL IsElevated()
	{
		BOOL fRet = FALSE;
		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		{
			TOKEN_ELEVATION Elevation{};
			DWORD cbSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
			{
				fRet = Elevation.TokenIsElevated;
			}
		}
		if (hToken)
		{
			CloseHandle(hToken);
		}
		return fRet;
	}

	void KillProcessByName(const wchar_t* filename)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(pEntry);
		BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
		while (hRes)
		{
			if (wcscmp(pEntry.szExeFile, filename) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
					(DWORD)pEntry.th32ProcessID);
				if (hProcess != NULL)
				{
					TerminateProcess(hProcess, 9);
					CloseHandle(hProcess);
				}
			}
			hRes = Process32Next(hSnapShot, &pEntry);
		}
		CloseHandle(hSnapShot);
	}
}
