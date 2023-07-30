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

#include "game.hpp"

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "local/local.hpp"
#include "logger/logger.hpp"

struct ReplaceAsset {
	std::string path;
	Il2CppObject* asset;
};

extern bool g_dump_entries;
extern bool g_dump_il2cpp;
extern bool g_enable_logger;
extern bool g_enable_console;
extern bool g_static_entries_use_hash;
extern bool g_static_entries_use_text_id_name;
extern int g_max_fps;
extern bool g_unlock_size;
extern bool g_unlock_size_use_system_resolution;
extern float g_ui_scale;
extern bool g_freeform_window;
extern float g_freeform_ui_scale_portrait;
extern float g_freeform_ui_scale_landscape;
extern int g_initial_width;
extern int g_initial_height;
extern float g_ui_animation_scale;
extern float g_aspect_ratio;
extern float g_resolution_3d_scale;
extern bool g_replace_to_builtin_font;
extern bool g_replace_to_custom_font;
extern std::string g_font_assetbundle_path;
extern std::string g_font_asset_name;
extern std::string g_tmpro_font_asset_name;
extern bool g_auto_fullscreen;
extern int g_graphics_quality;
extern int g_anti_aliasing;
extern bool g_ui_loading_show_orientation_guide;
extern std::string g_custom_title_name;
extern std::unordered_map<std::string, ReplaceAsset> g_replace_assets;
extern std::string g_replace_assetbundle_file_path;
extern std::vector<std::string> g_replace_assetbundle_file_paths;
extern std::string g_replace_text_db_path;
extern bool g_character_system_text_caption;
extern int g_character_system_text_caption_font_size;
extern string g_character_system_text_caption_font_color;
extern string g_character_system_text_caption_outline_size;
extern string g_character_system_text_caption_outline_color;
extern float g_character_system_text_caption_background_alpha;
extern float g_character_system_text_caption_position_x;
extern float g_character_system_text_caption_position_y;
/*
 * ModeNormal 0
 * Mode60FPS 1
 * SkipFrame 2
 * SkipFramePostAlways 3
 */
extern int g_cyspring_update_mode;
extern bool g_hide_now_loading;
extern bool g_discord_rich_presence;

extern rapidjson::Document code_map;

extern bool has_json_parse_error;
extern std::string json_parse_error_msg;

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