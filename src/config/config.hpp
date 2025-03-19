#pragma once

#include <Shlwapi.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>

#include "il2cpp/il2cpp_symbols.hpp"

#define _EXPERIMENTS

typedef rapidjson::GenericDocument<rapidjson::UTF16<>> WDocument;
typedef rapidjson::GenericValue<rapidjson::UTF16<>> WValue;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<>> WStringBuffer;

using namespace std;

struct ReplaceAsset {
	wstring path;
	Il2CppObject* asset;
};

namespace config
{
	extern bool dump_entries;
	extern bool dump_il2cpp;
	extern bool static_entries_use_hash;
	extern bool static_entries_use_text_id_name;
	extern bool enable_logger;
	extern bool enable_console;
	extern int max_fps;
	extern bool unlock_size;
	extern bool unlock_size_use_system_resolution;
	extern float ui_scale;
	extern bool freeform_window;
	extern float freeform_ui_scale_portrait;
	extern float freeform_ui_scale_landscape;
	extern int initial_width;
	extern int initial_height;
	extern float ui_animation_scale;
	extern float aspect_ratio;
	extern float resolution_3d_scale;
	extern bool replace_to_builtin_font;
	extern bool replace_to_custom_font;
	extern wstring font_assetbundle_path;
	extern wstring font_asset_name;
	extern wstring tmpro_font_asset_name;
	extern bool auto_fullscreen;
	extern int graphics_quality;
	extern int anti_aliasing;
	extern int anisotropic_filtering;
	extern int vsync_count;
	extern bool ui_loading_show_orientation_guide;
	extern wstring custom_title_name;
	extern unordered_map<wstring, ReplaceAsset> replace_assets;
	extern wstring replace_assetbundle_file_path;
	extern vector<wstring> replace_assetbundle_file_paths;
	extern wstring replace_text_db_path;
	extern bool character_system_text_caption;
	extern int character_system_text_caption_line_char_count;
	extern int character_system_text_caption_font_size;
	extern wstring character_system_text_caption_font_color;
	extern wstring character_system_text_caption_outline_size;
	extern wstring character_system_text_caption_outline_color;
	extern float character_system_text_caption_background_alpha;
	extern float character_system_text_caption_position_x;
	extern float character_system_text_caption_position_y;
	extern bool champions_live_show_text;
	extern int champions_live_resource_id;
	extern int champions_live_year;
	extern bool live_slider_always_show;
	extern bool live_playback_loop;
	/*
	 * ModeNormal 0
	 * Mode60FPS 1
	 * SkipFrame 2
	 * SkipFramePostAlways 3
	 */
	extern int cyspring_update_mode;
	extern bool hide_now_loading;
	extern bool discord_rich_presence;
	extern bool allow_delete_cookie;
	extern bool localify_settings_force_korean;
	extern bool dump_msgpack;
	extern bool dump_msgpack_request;
	extern bool unlock_live_chara;
	extern bool notification_tp;
	extern bool notification_rp;
	extern bool notification_jobs;
	extern bool msgpack_notifier;
	extern bool msgpack_notifier_request;
	extern wstring msgpack_notifier_host;
	extern int msgpack_notifier_connection_timeout_ms;
	extern bool msgpack_notifier_print_error;
	extern bool use_third_party_news;

	extern wstring text_id_dict;

	extern rapidjson::Document code_map;
	extern rapidjson::Document fn_map;

	extern bool has_json_parse_error;
	extern wstring json_parse_error_msg;

	extern vector<wstring> external_dlls_path;
	extern vector<wstring> dicts;

	extern WDocument config_document;
	extern WDocument backup_document;

	namespace runtime
	{
		extern bool useDefaultFPS;
		extern float ratioVertical;
		extern float ratioHorizontal;
		extern bool allowStart;
		extern Il2CppObject* fontAssets;
		extern vector<Il2CppObject*> replaceAssets;
		extern vector<wstring> replaceAssetNames;
	}

	void read_config_init();
	bool read_config();
	void write_config();
	void rollback_config();
}
