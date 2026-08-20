#pragma once

#ifdef _MSC_VER
#define NOMINMAX

#include <Windows.h>
#include <shlobj.h>
#include <Shlwapi.h>

#include <TlHelp32.h>

#include <MinHook.h>
#else
#if defined(__ARM_ARCH_7A__)
#define ABI "armeabi-v7a"
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif

#include <bits/get_device_api_level_inlines.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <dobby.h>
#include <jni.h>
#include <pthread.h>

#include <array>

#include "fnv1a_hash.hpp"

#include "log.h"
#endif

#include <cinttypes>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <unordered_map>

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
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

namespace
{
#ifdef _MSC_VER
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
#else
    int GetAndroidApiLevel() {
        return android_get_device_api_level();
    }

    string GetNativeBridgeLibrary() {
        auto nativeBridge = array<char, PROP_VALUE_MAX>();
        __system_property_get("ro.dalvik.vm.native.bridge", nativeBridge.data());
        return {nativeBridge.data()};
    }

    bool IsABIRequiredNativeBridge() {
        // x86 is Play Games build
        return ABI == "x86_64"s;
    }

    bool IsRunningOnNativeBridge() {
        auto systemAbi = array<char, PROP_VALUE_MAX>();
        __system_property_get("ro.product.cpu.abi", systemAbi.data());
        auto isaArm = array<char, PROP_VALUE_MAX>();
        __system_property_get("ro.dalvik.vm.isa.arm", isaArm.data());
        return ((systemAbi.data() == "x86"s || systemAbi.data() == "x86_64"s) || isaArm.data() == "x86"s) &&
               (ABI == "armeabi-v7a"s || ABI == "arm64-v8a"s);
    }
#endif
}
