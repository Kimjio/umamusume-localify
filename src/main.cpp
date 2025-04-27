#include <stdinclude.hpp>

#include "config/config.hpp"

extern void init_hook();
extern void uninit_hook();

namespace
{
	BOOL WINAPI AttachOrCreateConsole()
	{
		if (AttachConsole(ATTACH_PARENT_PROCESS))
		{
			return TRUE;
		}

		return AllocConsole();
	}

	void create_debug_console()
	{
		AttachOrCreateConsole();

		// open stdout stream
		auto _ = freopen("CONOUT$", "w+t", stdout);
		_ = freopen("CONOUT$", "w", stderr);
		_ = freopen("CONIN$", "r", stdin);

		SetConsoleTitleW(L"Umamusume - Debug Console");

		// set this to avoid turn japanese texts into question mark
		SetConsoleOutputCP(CP_UTF8);
		locale::global(locale(".UTF-8"));

		wcout << L"\u30a6\u30de\u5a18 Localify Patch Loaded! - By GEEKiDoS/Kimjio" << endl;
	}
}

void DoStopSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	SERVICE_STATUS_PROCESS ssp;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000; // 30-second time-out
	DWORD dwWaitTime;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		cout << "OpenSCManager failed (" << GetLastError() << ")" << endl;
		return;
	}

	// Get a handle to the service.

	schService = OpenServiceW(
		schSCManager,         // SCM database 
		L"ucldr_Umamusume_KR",            // name of service 
		SERVICE_STOP |
		SERVICE_QUERY_STATUS |
		SERVICE_ENUMERATE_DEPENDENTS);

	if (schService == NULL)
	{
		cout << "OpenService failed (" << GetLastError() << ")" << endl;
		CloseServiceHandle(schSCManager);
		return;
	}

	// Make sure the service is not already stopped.

	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		cout << "QueryServiceStatusEx failed (" << GetLastError() << ")" << endl;;
		goto stop_cleanup;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		cout << "Service is already stopped." << endl;
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		cout << "Service stop pending..." << endl;;

		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			cout << "QueryServiceStatusEx failed (" << GetLastError() << ")" << endl;
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			cout << "Service stopped successfully." << endl;
			goto stop_cleanup;
		}

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			cout << "Service stop timed out." << endl;
			goto stop_cleanup;
		}
	}

	// Send a stop code to the service.

	if (!ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		cout << "ControlService failed (" << GetLastError() << ")" << endl;
		goto stop_cleanup;
	}

	// Wait for the service to stop.

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssp.dwWaitHint);
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			cout << "QueryServiceStatusEx failed (" << GetLastError() << ")" << endl;
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			cout << "Wait timed out" << endl;
			goto stop_cleanup;
		}
	}
	cout << "Service stopped successfully" << endl;

stop_cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// the DMM Launcher set start path to system32 wtf????
		wstring module_name;
		module_name.resize(MAX_PATH);
		module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

		filesystem::path module_path(module_name);

		// check name
		if (module_path.filename() != "umamusume.exe")
		{
			return TRUE;
		}

		filesystem::current_path(module_path.parent_path());

		config::read_config_init();

		if (config::enable_console)
		{
			create_debug_console();
		}

		wifstream appinfo(module_path.parent_path().append(module_path.filename().replace_extension().string().append("_Data\\app.info"s).data()));
		appinfo.imbue(locale(".UTF-8"));

		wstring company;
		getline(appinfo, company);

		wstring product;
		getline(appinfo, product);

		appinfo.close();

		if (company == L"Cygames_KakaoGames")
		{
			Game::CurrentGameRegion = Game::Region::KOR;

			if (IsElevated())
			{
				DoStopSvc();
			}

			auto uncheater_path = filesystem::path(module_path.parent_path().append(module_path.filename().replace_extension().string().append("_Data\\StreamingAssets\\Uncheater"s).data()));
			auto uncheater_path_new = filesystem::path(module_path.parent_path().append(module_path.filename().replace_extension().string().append("_Data\\StreamingAssets\\_Uncheater"s).data()));

			if (filesystem::exists(uncheater_path_new))
			{
				try
				{
					filesystem::remove_all(uncheater_path_new);
				}
				catch (exception& e)
				{
					cout << "_Uncheater remove error: " << e.what() << endl;
				}
			}

			if (filesystem::exists(uncheater_path))
			{
				try
				{
					filesystem::rename(uncheater_path, uncheater_path_new);
				}
				catch (exception& e)
				{
					cout << "Uncheater rename error: " << e.what() << endl;

					auto xnina_path = filesystem::path(module_path.parent_path().append(module_path.filename().replace_extension().string().append("_Data\\StreamingAssets\\Uncheater\\xnina_x64.xem"s).data()));
					if (filesystem::exists(xnina_path))
					{
						try
						{
							filesystem::remove(xnina_path);
						}
						catch (exception& e)
						{
							cout << "xnina_x64.xem remove error: " << e.what() << endl;
						}
					}
				}
			}

		}
		else
		{
			Game::CurrentGameRegion = Game::Region::JPN;
		}

		if (Game::CurrentGameRegion != Game::Region::JPN &&
			Game::CurrentGameRegion != Game::Region::ENG)
		{
			wstringstream subKeyStream;

			subKeyStream << L"Software";
			subKeyStream << L"\\" << company;
			subKeyStream << L"\\" << product;

			DWORD data{};
			DWORD dataSize = sizeof(data);
			RegGetValueW(HKEY_CURRENT_USER, subKeyStream.str().data(), L"AgreeOwnYourRisk", RRF_RT_REG_DWORD, nullptr, &data, &dataSize);

			config::runtime::allowStart = data > 0;
		}

		init_hook();

		thread init_thread([]() {
			logger::init_logger();
			local::load_textdb(&config::dicts);

			if (!config::text_id_dict.empty())
			{
				local::load_textId_textdb(config::text_id_dict);
			}
			});
		init_thread.detach();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		uninit_hook();
		logger::close_logger();
	}

	return TRUE;
}
