#include <stdinclude.hpp>
#include <rapidjson/error/en.h>

extern bool init_hook_base();
extern bool init_hook();
extern void uninit_hook();
extern void start_console();

bool g_dump_entries = false;
bool g_dump_il2cpp = false;
bool g_static_entries_use_hash = false;
bool g_static_entries_use_text_id_name = false;
bool g_enable_logger = false;
bool g_enable_console = false;
int g_max_fps = -1;
bool g_unlock_size = false;
bool g_unlock_size_use_system_resolution = false;
float g_ui_scale = 1.0f;
bool g_freeform_window = false;
float g_freeform_ui_scale_portrait = 0.5f;
float g_freeform_ui_scale_landscape = 0.5f;
int g_initial_width = -1;
int g_initial_height = -1;
float g_ui_animation_scale = 1.0f;
float g_aspect_ratio = 16.f / 9.f;
float g_resolution_3d_scale = 1.0f;
bool g_replace_to_builtin_font = false;
bool g_replace_to_custom_font = false;
std::string g_font_assetbundle_path;
std::string g_font_asset_name;
std::string g_tmpro_font_asset_name;
bool g_auto_fullscreen = true;
int g_graphics_quality = -1;
int g_anti_aliasing = -1;
int g_anisotropic_filtering = -1;
int g_vsync_count = -1;
bool g_ui_loading_show_orientation_guide = true;
std::string g_custom_title_name;
std::unordered_map<std::string, ReplaceAsset> g_replace_assets;
std::string g_replace_assetbundle_file_path;
std::vector<std::string> g_replace_assetbundle_file_paths;
std::string g_replace_text_db_path;
bool g_character_system_text_caption = false;
int g_character_system_text_caption_line_char_count = 26;
int g_character_system_text_caption_font_size = 50;
string g_character_system_text_caption_font_color = "White";
string g_character_system_text_caption_outline_size = "L";
string g_character_system_text_caption_outline_color = "Brown";
float g_character_system_text_caption_background_alpha = 0;
float g_character_system_text_caption_position_x = 0;
float g_character_system_text_caption_position_y = -3;
bool g_champions_live_show_text = false;
int g_champions_live_resource_id = 1;
int g_champions_live_year = 2023;
int g_cyspring_update_mode = -1;
bool g_hide_now_loading = false;
bool g_discord_rich_presence = false;
std::string text_id_dict;

rapidjson::Document code_map;

bool has_json_parse_error = false;
std::string json_parse_error_msg;

namespace
{
	void create_debug_console()
	{
		AllocConsole();

		// open stdout stream
		auto _ = freopen("CONOUT$", "w+t", stdout);
		_ = freopen("CONOUT$", "w", stderr);
		_ = freopen("CONIN$", "r", stdin);

		SetConsoleTitle("Umamusume - Debug Console");

		// set this to avoid turn japanese texts into question mark
		SetConsoleOutputCP(CP_UTF8);
		std::locale::global(std::locale(""));

		wprintf(L"\u30a6\u30de\u5a18 Localify Patch Loaded! - By GEEKiDoS\n");
	}

	std::vector<std::string> read_config()
	{
		std::ifstream config_stream{ "config.json" };
		std::vector<std::string> dicts{};

		if (!config_stream.is_open())
			return dicts;

		rapidjson::IStreamWrapper wrapper{ config_stream };
		rapidjson::Document document;

		document.ParseStream(wrapper);

		if (!document.HasParseError())
		{
			if (document.HasMember("enableConsole"))
			{
				g_enable_console = document["enableConsole"].GetBool();
			}

			if (document.HasMember("enableLogger"))
			{
				g_enable_logger = document["enableLogger"].GetBool();
			}

			if (document.HasMember("dumpStaticEntries"))
			{
				g_dump_entries = document["dumpStaticEntries"].GetBool();
			}

			if (document.HasMember("dumpIl2Cpp"))
			{
				g_dump_il2cpp = document["dumpIl2Cpp"].GetBool();
			}

			if (document.HasMember("staticEntriesUseHash"))
			{
				g_static_entries_use_hash = document["staticEntriesUseHash"].GetBool();
			}

			if (document.HasMember("staticEntriesUseTextIdName"))
			{
				g_static_entries_use_text_id_name = document["staticEntriesUseTextIdName"].GetBool();
			}

			if (document.HasMember("maxFps"))
			{
				g_max_fps = document["maxFps"].GetInt();
			}

			if (document.HasMember("unlockSize"))
			{
				g_unlock_size = document["unlockSize"].GetBool();
			}

			if (document.HasMember("unlockSizeUseSystemResolution"))
			{
				g_unlock_size_use_system_resolution = document["unlockSizeUseSystemResolution"].GetBool();
			}

			if (document.HasMember("uiScale"))
			{
				g_ui_scale = document["uiScale"].GetFloat();
			}

			if (document.HasMember("freeFormWindow"))
			{
				g_freeform_window = document["freeFormWindow"].GetBool();
				if (g_freeform_window)
				{
					g_unlock_size = true;
				}
			}

			if (document.HasMember("freeFormUiScalePortrait"))
			{
				g_freeform_ui_scale_portrait = document["freeFormUiScalePortrait"].GetFloat();
				if (g_freeform_ui_scale_portrait <= 0)
				{
					g_freeform_ui_scale_portrait = 0.5f;
				}
			}

			if (document.HasMember("freeFormUiScaleLandscape"))
			{
				g_freeform_ui_scale_landscape = document["freeFormUiScaleLandscape"].GetFloat();
				if (g_freeform_ui_scale_landscape <= 0)
				{
					g_freeform_ui_scale_landscape = 0.5f;
				}
			}

			if (document.HasMember("initialWidth"))
			{
				g_initial_width = document["initialWidth"].GetInt();
				if (g_initial_width <= 72)
				{
					g_initial_width = -1;
				}
			}

			if (document.HasMember("initialHeight"))
			{
				g_initial_height = document["initialHeight"].GetInt();
				if (g_initial_height <= 72)
				{
					g_initial_height = -1;
				}
			}

			if (document.HasMember("uiAnimationScale"))
			{
				g_ui_animation_scale = document["uiAnimationScale"].GetFloat();
			}

			if (document.HasMember("resolution3dScale"))
			{
				g_resolution_3d_scale = document["resolution3dScale"].GetFloat();
			}

			if (document.HasMember("replaceFont"))
			{
				g_replace_to_builtin_font = document["replaceFont"].GetBool();
			}

			if (!document.HasMember("replaceFont") && document.HasMember("replaceToBuiltinFont"))
			{
				g_replace_to_builtin_font = document["replaceToBuiltinFont"].GetBool();
			}

			if (document.HasMember("replaceToCustomFont"))
			{
				g_replace_to_custom_font = document["replaceToCustomFont"].GetBool();
			}

			if (document.HasMember("fontAssetBundlePath"))
			{
				g_font_assetbundle_path = std::string(document["fontAssetBundlePath"].GetString());
			}

			if (document.HasMember("fontAssetName"))
			{
				g_font_asset_name = std::string(document["fontAssetName"].GetString());
			}

			if (document.HasMember("tmproFontAssetName"))
			{
				g_tmpro_font_asset_name = std::string(document["tmproFontAssetName"].GetString());
			}

			if (document.HasMember("autoFullscreen"))
			{
				g_auto_fullscreen = document["autoFullscreen"].GetBool();
			}

			if (document.HasMember("graphicsQuality"))
			{
				g_graphics_quality = document["graphicsQuality"].GetInt();
				if (g_graphics_quality < -1)
				{
					g_graphics_quality = -1;
				}
				if (g_graphics_quality > 4)
				{
					g_graphics_quality = 3;
				}
			}

			if (document.HasMember("antiAliasing"))
			{
				g_anti_aliasing = document["antiAliasing"].GetInt();
				std::vector<int> options = { 0, 2, 4, 8, -1 };
				g_anti_aliasing = std::find(options.begin(), options.end(), g_anti_aliasing) - options.begin();
			}

			if (document.HasMember("anisotropicFiltering"))
			{
				g_anisotropic_filtering = document["anisotropicFiltering"].GetInt();
				std::vector<int> options = { 0, 1, 2, -1 };
				g_anisotropic_filtering = std::find(options.begin(), options.end(), g_anisotropic_filtering) - options.begin();
			}

			if (document.HasMember("vSyncCount"))
			{
				g_vsync_count = document["vSyncCount"].GetInt();
				std::vector<int> options = { 0, 1, 2, 3, 4, -1 };
				g_vsync_count = std::find(options.begin(), options.end(), g_vsync_count) - options.begin();
			}

			if (document.HasMember("uiLoadingShowOrientationGuide"))
			{
				g_ui_loading_show_orientation_guide = document["uiLoadingShowOrientationGuide"].GetBool();
			}

			if (document.HasMember("customTitleName"))
			{
				g_custom_title_name = document["customTitleName"].GetString();
			}

			if (document.HasMember("replaceAssetsPaths"))
			{
				auto array = document["replaceAssetsPaths"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					auto value = local::u8_wide(it->GetString());

					if (PathIsRelativeW(value.data()))
					{
						value.insert(0, ((std::wstring)std::filesystem::current_path().native()).append(L"/"));
					}
					if (std::filesystem::exists(value) && std::filesystem::is_directory(value))
					{
						for (auto& file : std::filesystem::directory_iterator(value))
						{
							if (file.is_regular_file())
							{
								g_replace_assets.emplace(local::wide_u8(local::acp_wide(file.path().filename().string())), ReplaceAsset{ local::wide_u8(local::acp_wide(file.path().string())), nullptr });
							}
						}
					}
				}
			}

			if (document.HasMember("replaceAssetBundleFilePath"))
			{
				g_replace_assetbundle_file_path = document["replaceAssetBundleFilePath"].GetString();
			}

			if (document.HasMember("replaceAssetBundleFilePaths") &&
				document["replaceAssetBundleFilePaths"].IsArray())
			{
				auto array = document["replaceAssetBundleFilePaths"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					auto value = it->GetString();
					g_replace_assetbundle_file_paths.emplace_back(value);
				}
			}

			if (document.HasMember("replaceTextDBPath"))
			{
				g_replace_text_db_path = document["replaceTextDBPath"].GetString();
			}

			if (document.HasMember("characterSystemTextCaption"))
			{
				g_character_system_text_caption = document["characterSystemTextCaption"].GetBool();
			}

			if (document.HasMember("characterSystemTextCaptionLineCharCount"))
			{
				g_character_system_text_caption_line_char_count = document["characterSystemTextCaptionLineCharCount"].GetInt();
			}

			if (document.HasMember("characterSystemTextCaptionFontSize"))
			{
				g_character_system_text_caption_font_size = document["characterSystemTextCaptionFontSize"].GetInt();
			}

			if (document.HasMember("characterSystemTextCaptionFontColor"))
			{
				g_character_system_text_caption_font_color = document["characterSystemTextCaptionFontColor"].GetString();
			}

			if (document.HasMember("characterSystemTextCaptionOutlineSize"))
			{
				g_character_system_text_caption_outline_size = document["characterSystemTextCaptionOutlineSize"].GetString();
			}

			if (document.HasMember("characterSystemTextCaptionOutlineColor"))
			{
				g_character_system_text_caption_outline_color = document["characterSystemTextCaptionOutlineColor"].GetString();
			}

			if (document.HasMember("characterSystemTextCaptionBackgroundAlpha"))
			{
				g_character_system_text_caption_background_alpha = document["characterSystemTextCaptionBackgroundAlpha"].GetFloat();
			}

			if (document.HasMember("characterSystemTextCaptionPositionX"))
			{
				g_character_system_text_caption_position_x = document["characterSystemTextCaptionPositionX"].GetFloat();
			}

			if (document.HasMember("characterSystemTextCaptionPositionY"))
			{
				g_character_system_text_caption_position_y = document["characterSystemTextCaptionPositionY"].GetFloat();
			}

			if (document.HasMember("championsLiveShowText"))
			{
				g_champions_live_show_text = document["championsLiveShowText"].GetBool();
			}

			if (document.HasMember("championsLiveResourceId"))
			{
				g_champions_live_resource_id = document["championsLiveResourceId"].GetInt();

				if (g_champions_live_resource_id < 1)
				{
					g_champions_live_resource_id = 1;
				}
			}

			if (document.HasMember("championsLiveYear"))
			{
				g_champions_live_year = document["championsLiveYear"].GetInt();
			}

			if (document.HasMember("cySpringUpdateMode"))
			{
				g_cyspring_update_mode = document["cySpringUpdateMode"].GetInt();
				std::vector<int> options = { 0, 1, 2, 3, -1 };
				g_cyspring_update_mode = std::find(options.begin(), options.end(), g_cyspring_update_mode) - options.begin();
			}
			else if (g_max_fps > 30)
			{
				g_cyspring_update_mode = 1;
			}

			if (document.HasMember("hideNowLoading"))
			{
				g_hide_now_loading = document["hideNowLoading"].GetBool();
			}

			// Looks like not working for now
			// g_aspect_ratio = document["customAspectRatio"].GetFloat();

			if (document.HasMember("textIdDict"))
			{
				text_id_dict = document["textIdDict"].GetString();
			}

			if (document.HasMember("codeMapPath"))
			{
				auto path = document["codeMapPath"].GetString();

				std::ifstream code_map_stream{ path };

				if (code_map_stream.is_open())
				{
					rapidjson::IStreamWrapper wrapper{ code_map_stream };
					code_map.ParseStream(wrapper);

					code_map_stream.close();
				}
			}

			if (document.HasMember("discordRichPresence"))
			{
				g_discord_rich_presence = document["discordRichPresence"].GetBool();
			}

			if (document.HasMember("dicts"))
			{
				auto& dicts_arr = document["dicts"];
				auto len = dicts_arr.Size();

				for (size_t i = 0; i < len; ++i)
				{
					auto dict = dicts_arr[i].GetString();

					dicts.emplace_back(dict);
				}
			}
		}
		else
		{
			has_json_parse_error = true;
			std::stringstream str_stream;
			str_stream << "JSON parse error: " << GetParseError_En(document.GetParseError()) << " (" << document.GetErrorOffset() << ")";
			json_parse_error_msg = str_stream.str();
		}

		config_stream.close();
		return dicts;
	}
}

extern "C" __declspec(dllexport) int __stdcall UnityMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	auto player = LoadLibraryA("UnityPlayer.orig.dll");
	return reinterpret_cast<decltype(UnityMain)*>(GetProcAddress(player, "UnityMain"))(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
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

	schService = OpenService(
		schSCManager,         // SCM database 
		"ucldr_Umamusume_KR",            // name of service 
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


int __stdcall DllMain(HINSTANCE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// the DMM Launcher set start path to system32 wtf????
		string module_name;
		module_name.resize(MAX_PATH);
		module_name.resize(GetModuleFileName(nullptr, module_name.data(), MAX_PATH));

		filesystem::path module_path(local::wide_u8(local::acp_wide(module_name)));

		// check name
		if (module_path.filename() != "umamusume.exe")
			return 1;

		std::filesystem::current_path(
			module_path.parent_path()
		);

		auto dicts = read_config();

		if (g_enable_console)
			create_debug_console();

		if (filesystem::exists(module_path.parent_path().append(module_path.filename().replace_extension().string().append("_Data\\Plugins\\x86_64\\KakaoGameWin.dll"s).data())))
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
			Game::CurrentGameRegion = Game::Region::JAP;
		}

		init_hook_base();

		std::thread init_thread([dicts]() {
			logger::init_logger();
			local::load_textdb(&dicts);
			if (!text_id_dict.empty())
			{
				local::load_textId_textdb(text_id_dict);
			}
			init_hook();

			if (g_enable_console)
				start_console();
			});
		init_thread.detach();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		uninit_hook();
		logger::close_logger();
	}

	return 1;
}
