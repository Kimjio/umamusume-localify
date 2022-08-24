#include <stdinclude.hpp>

extern bool init_hook();
extern void uninit_hook();
extern void start_console();

bool g_dump_entries = false;
bool g_enable_logger = false;
bool g_enable_console = false;
int g_max_fps = -1;
bool g_unlock_size = false;
float g_ui_scale = 1.0f;
float g_ui_animation_scale = 1.0f;
float g_aspect_ratio = 16.f / 9.f;
bool g_replace_to_builtin_font = true;
bool g_replace_to_custom_font = false;
std::string g_font_assetbundle_path;
std::string g_font_asset_name;
bool g_auto_fullscreen = true;
int g_graphics_quality = -1;
int g_anti_aliasing = -1;
bool g_force_landscape = false;
float g_force_landscape_ui_scale = 0.5;
bool g_ui_loading_show_orientation_guide = true;
std::string g_custom_title_name;
std::unordered_map<std::string, ReplaceAsset> g_replace_assets;

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
		SetConsoleOutputCP(65001);
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

			if (document.HasMember("enableConsole")) {
				g_enable_console = document["enableConsole"].GetBool();
			}
			if (document.HasMember("enableLogger")) {
				g_enable_logger = document["enableLogger"].GetBool();
			}
			if (document.HasMember("dumpStaticEntries")) {
				g_dump_entries = document["dumpStaticEntries"].GetBool();
			}
			if (document.HasMember("maxFps")) {
				g_max_fps = document["maxFps"].GetInt();
			}
			if (document.HasMember("unlockSize")) {
				g_unlock_size = document["unlockSize"].GetBool();
			}
			if (document.HasMember("uiScale")) {
				g_ui_scale = document["uiScale"].GetFloat();
			}
			if (document.HasMember("uiAnimationScale")) {
				g_ui_animation_scale = document["uiAnimationScale"].GetFloat();
			}
			if (document.HasMember("replaceFont")) {
				g_replace_to_builtin_font = document["replaceFont"].GetBool();
			}
			if (!document.HasMember("replaceFont") && document.HasMember("replaceToBuiltinFont")) {
				g_replace_to_builtin_font = document["replaceToBuiltinFont"].GetBool();
			}
			if (document.HasMember("replaceToCustomFont")) {
				g_replace_to_custom_font = document["replaceToCustomFont"].GetBool();
			}
			if (document.HasMember("fontAssetBundlePath")) {
				g_font_assetbundle_path = std::string(document["fontAssetBundlePath"].GetString());
			}
			if (document.HasMember("fontAssetName")) {
				g_font_asset_name = std::string(document["fontAssetName"].GetString());
			}
			if (document.HasMember("autoFullscreen")) {
				g_auto_fullscreen = document["autoFullscreen"].GetBool();
			}
			if (document.HasMember("graphicsQuality")) {
				g_graphics_quality = document["graphicsQuality"].GetInt();
				if (g_graphics_quality < -1) {
					g_graphics_quality = -1;
				}
				if (g_graphics_quality > 4) {
					g_graphics_quality = 3;
				}
			}
			if (document.HasMember("antiAliasing")) {
				g_anti_aliasing = document["antiAliasing"].GetInt();
				std::vector<int> options = { 0, 2, 4, 8, -1 };
				g_anti_aliasing = std::find(options.begin(), options.end(), g_anti_aliasing) - options.begin();
			}

			if (document.HasMember("forceLandscape")) {
				g_force_landscape = document["forceLandscape"].GetBool();
			}
			if (document.HasMember("forceLandscapeUiScale")) {
				g_force_landscape_ui_scale = document["forceLandscapeUiScale"].GetFloat();
				if (g_force_landscape_ui_scale <= 0) {
					g_force_landscape_ui_scale = 1;
				}
			}
			if (document.HasMember("uiLoadingShowOrientationGuide")) {
				g_ui_loading_show_orientation_guide = document["uiLoadingShowOrientationGuide"].GetBool();
			}

			if (document.HasMember("customTitleName")) {
				g_custom_title_name = document["customTitleName"].GetString();
			}

			if (document.HasMember("replaceAssetsPath")) {
				auto replaceAssetsPath = local::u8_wide(document["replaceAssetsPath"].GetString());
				if (PathIsRelativeW(replaceAssetsPath.data()))
				{
					replaceAssetsPath.insert(0, ((std::wstring)std::filesystem::current_path().native()).append(L"/"));
				}
				if (std::filesystem::exists(replaceAssetsPath) && std::filesystem::is_directory(replaceAssetsPath))
				{
					for (auto &file : std::filesystem::directory_iterator(replaceAssetsPath))
					{
						if (file.is_regular_file())
						{
							g_replace_assets.emplace(file.path().filename().string(), ReplaceAsset{ file.path().string(), nullptr });
						}
					}
				}
			}

			// Looks like not working for now
			// g_aspect_ratio = document["customAspectRatio"].GetFloat();
			if (document.HasMember("dicts")) {
				auto& dicts_arr = document["dicts"];
				auto len = dicts_arr.Size();

				for (size_t i = 0; i < len; ++i)
				{
					auto dict = dicts_arr[i].GetString();

					dicts.push_back(dict);
				}
			}
		}

		config_stream.close();
		return dicts;
	}
}

int __stdcall DllMain(HINSTANCE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// the DMM Launcher set start path to system32 wtf????
		std::string module_name;
		module_name.resize(MAX_PATH);
		module_name.resize(GetModuleFileName(nullptr, module_name.data(), MAX_PATH));

		std::filesystem::path module_path(module_name);

		// check name
		if (module_path.filename() != "umamusume.exe")
			return 1;

		std::filesystem::current_path(
			module_path.parent_path()
		);

		auto dicts = read_config();

		if (g_enable_console)
			create_debug_console();

		std::thread init_thread([dicts]() {
			logger::init_logger();
			local::load_textdb(&dicts);
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
