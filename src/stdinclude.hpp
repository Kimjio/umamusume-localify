#pragma once

#include <Windows.h>
#include <shlobj.h>
#include <Shlwapi.h>

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

#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include "il2cpp/il2cpp_symbols.hpp"
#include "local/local.hpp"
#include "logger/logger.hpp"

struct ReplaceAsset {
	std::string path;
	Il2CppObject* asset;
};

extern bool g_dump_entries;
extern bool g_enable_logger;
extern bool g_enable_console;
extern bool g_static_entries_use_hash;
extern int g_max_fps;
extern bool g_unlock_size;
extern float g_ui_scale;
extern float g_ui_animation_scale;
extern float g_aspect_ratio;
extern bool g_replace_to_builtin_font;
extern bool g_replace_to_custom_font;
extern std::string g_font_assetbundle_path;
extern std::string g_font_asset_name;
extern bool g_auto_fullscreen;
extern int g_graphics_quality;
extern int g_anti_aliasing;
extern bool g_force_landscape;
extern float g_force_landscape_ui_scale;
extern bool g_ui_loading_show_orientation_guide;
extern std::string g_custom_title_name;
extern std::unordered_map<std::string, ReplaceAsset> g_replace_assets;

namespace {
	// copy-pasted from https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
	void replaceAll(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
}