#include "config.hpp"

#include "local/local.hpp"

namespace config
{
	bool dump_entries = false;
	bool dump_il2cpp = false;
	bool static_entries_use_hash = false;
	bool static_entries_use_text_id_name = false;
	bool enable_logger = false;
	bool enable_console = false;
	int max_fps = -1;
	bool unlock_size = false;
	bool unlock_size_use_system_resolution = false;
	float ui_scale = 1.0f;
	bool freeform_window = false;
	float freeform_ui_scale_portrait = 0.5f;
	float freeform_ui_scale_landscape = 0.5f;
	int initial_width = -1;
	int initial_height = -1;
	float ui_animation_scale = 1.0f;
	float aspect_ratio = 16.f / 9.f;
	float resolution_3d_scale = 1.0f;
	bool replace_to_builtin_font = false;
	bool replace_to_custom_font = false;
	wstring font_assetbundle_path;
	wstring font_asset_name;
	wstring tmpro_font_asset_name;
	bool auto_fullscreen = true;
	int graphics_quality = -1;
	int anti_aliasing = -1;
	int anisotropic_filtering = -1;
	int vsync_count = -1;
	bool ui_loading_show_orientation_guide = true;
	wstring custom_title_name;
	unordered_map<wstring, ReplaceAsset> replace_assets;
	wstring replace_assetbundle_file_path;
	vector<wstring> replace_assetbundle_file_paths;
	wstring replace_text_db_path;
	bool character_system_text_caption = false;
	int character_system_text_caption_line_char_count = 26;
	int character_system_text_caption_font_size = 50;
	wstring character_system_text_caption_font_color = L"White";
	wstring character_system_text_caption_outline_size = L"L";
	wstring character_system_text_caption_outline_color = L"Brown";
	float character_system_text_caption_background_alpha = 0;
	float character_system_text_caption_position_x = 0;
	float character_system_text_caption_position_y = -3;
	bool champions_live_show_text = false;
	int champions_live_resource_id = 1;
	int champions_live_year = 2023;
	/*
	 * ModeNormal 0
	 * Mode60FPS 1
	 * SkipFrame 2
	 * SkipFramePostAlways 3
	 */
	int cyspring_update_mode = -1;
	bool hide_now_loading = false;
	bool discord_rich_presence = false;
	bool allow_delete_cookie = false;
	bool localify_settings_force_korean = false;
	bool dump_msgpack = false;
	bool dump_msgpack_request = false;
	bool unlock_live_chara = false;
	bool notification_tp = false;
	bool notification_rp = false;
	bool msgpack_notifier = false;
	bool msgpack_notifier_request = false;
	wstring msgpack_notifier_host = L"http://localhost:4693";
	int msgpack_notifier_connection_timeout_ms = 1000;
	bool msgpack_notifier_print_error = false;

	wstring text_id_dict;

	WDocument code_map;

	bool has_json_parse_error = false;
	wstring json_parse_error_msg;

	vector<wstring> external_dlls_path;
	vector<wstring> dicts;

	WDocument config_document;
	WDocument backup_document;

	void read_config_init()
	{
		wifstream config_stream{ "config.json" };
		config_stream.imbue(locale(".UTF-8"));

		if (!config_stream.is_open())
		{
			return;
		}

		rapidjson::WIStreamWrapper wrapper{ config_stream };
		WDocument document;
		document.ParseStream(wrapper);

		config_stream.close();

		if (!document.HasParseError())
		{
#define GetValue(_name_, _type_, _value_, ...)\
if (document.HasMember(L##_name_) && document[L##_name_].Is##_type_())\
{\
	_value_ = document[L##_name_].Get##_type_();\
	__VA_ARGS__\
}

			GetValue("enableConsole", Bool, enable_console);

			GetValue("enableLogger", Bool, enable_logger);

			GetValue("dumpStaticEntries", Bool, dump_entries);

			GetValue("dumpIl2Cpp", Bool, dump_il2cpp);

			GetValue("staticEntriesUseHash", Bool, static_entries_use_hash);

			GetValue("staticEntriesUseTextIdName", Bool, static_entries_use_text_id_name);

			GetValue("maxFps", Int, max_fps);

			GetValue("unlockSize", Bool, unlock_size);

			GetValue("unlockSizeUseSystemResolution", Bool, unlock_size_use_system_resolution);

			GetValue("uiScale", Float, ui_scale);

			GetValue("freeFormWindow", Bool, freeform_window,
				if (freeform_window)
				{
					unlock_size = true;
				}
				);

			GetValue("freeFormUiScalePortrait", Float, freeform_ui_scale_portrait,
				if (freeform_ui_scale_portrait <= 0)
				{
					freeform_ui_scale_portrait = 0.5f;
				}
				);

			GetValue("freeFormUiScaleLandscape", Float, freeform_ui_scale_landscape,
				if (freeform_ui_scale_landscape <= 0)
				{
					freeform_ui_scale_landscape = 0.5f;
				}
				);

			GetValue("initialWidth", Int, initial_width,
				if (initial_width <= 72)
				{
					initial_width = -1;
				}
				);

			GetValue("initialHeight", Int, initial_height,
				if (initial_height <= 72)
				{
					initial_height = -1;
				}
				);

			GetValue("uiAnimationScale", Float, resolution_3d_scale);

			GetValue("resolution3dScale", Float, resolution_3d_scale);

			GetValue("replaceToBuiltinFont", Bool, replace_to_builtin_font);

			GetValue("replaceToCustomFont", Bool, replace_to_custom_font);

			GetValue("fontAssetBundlePath", String, font_assetbundle_path);

			GetValue("fontAssetName", String, font_asset_name);

			GetValue("tmproFontAssetName", String, tmpro_font_asset_name);

			GetValue("autoFullscreen", Bool, auto_fullscreen);

			GetValue("graphicsQuality", Int, graphics_quality,
				if (graphics_quality < -1)
				{
					graphics_quality = -1;
				}
				if (graphics_quality > 4)
				{
					graphics_quality = 3;
				}
				);

			GetValue("antiAliasing", Int, anti_aliasing,
				vector<int> options = { 0, 2, 4, 8, -1 };
				anti_aliasing = options[find(options.begin(), options.end(), anti_aliasing) - options.begin()];
				);

			GetValue("anisotropicFiltering", Int, anisotropic_filtering,
				vector<int> options = { 0, 1, 2, -1 };
				anisotropic_filtering = options[find(options.begin(), options.end(), anisotropic_filtering) - options.begin()];
				);

			GetValue("vSyncCount", Int, vsync_count,
				vector<int> options = { 0, 1, 2, 3, 4, -1 };
				vsync_count = options[find(options.begin(), options.end(), vsync_count) - options.begin()];
				);

			GetValue("uiLoadingShowOrientationGuide", Bool, ui_loading_show_orientation_guide);

			GetValue("customTitleName", String, custom_title_name);

			GetValue("replaceAssetsPaths", Array, auto array,
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					if (it->IsString())
					{
						wstring value = it->GetString();

						if (PathIsRelativeW(value.data()))
						{
							value.insert(0, filesystem::current_path().wstring().append(L"/"));
						}
						if (filesystem::exists(value) && filesystem::is_directory(value))
						{
							for (auto& file : filesystem::directory_iterator(value))
							{
								if (file.is_regular_file())
								{
									replace_assets.emplace(file.path().filename().wstring(), ReplaceAsset{ file.path().wstring(), nullptr });
								}
							}
						}
					}
				}
				);

			GetValue("replaceAssetBundleFilePath", String, replace_assetbundle_file_path);

			GetValue("replaceAssetBundleFilePaths", Array, auto array,
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					if (it->IsString())
					{
						wstring value = it->GetString();
						replace_assetbundle_file_paths.emplace_back(value);
					}
				}
				);

			GetValue("replaceTextDBPath", String, replace_text_db_path);

			GetValue("characterSystemTextCaption", Bool, character_system_text_caption);

			GetValue("characterSystemTextCaptionLineCharCount", Int, character_system_text_caption_line_char_count);

			GetValue("characterSystemTextCaptionFontSize", Int, character_system_text_caption_font_size);

			GetValue("characterSystemTextCaptionFontColor", String, character_system_text_caption_font_color);

			GetValue("characterSystemTextCaptionOutlineSize", String, character_system_text_caption_outline_size);

			GetValue("characterSystemTextCaptionOutlineColor", String, character_system_text_caption_outline_color);

			GetValue("characterSystemTextCaptionBackgroundAlpha", Float, character_system_text_caption_background_alpha);

			GetValue("characterSystemTextCaptionPositionX", Float, character_system_text_caption_position_x);

			GetValue("characterSystemTextCaptionPositionY", Float, character_system_text_caption_position_y);

			GetValue("championsLiveShowText", Bool, champions_live_show_text);

			GetValue("championsLiveResourceId", Int, champions_live_resource_id,
				if (champions_live_resource_id < 1)
				{
					champions_live_resource_id = 1;
				}
				);

			GetValue("championsLiveYear", Int, champions_live_year);

			GetValue("cySpringUpdateMode", Int, cyspring_update_mode,
				vector<int> options = { 0, 1, 2, 3, -1 };
				cyspring_update_mode = options[find(options.begin(), options.end(), cyspring_update_mode) - options.begin()];
				)
			else if (max_fps > 30)
			{
				cyspring_update_mode = 1;
			}

			GetValue("hideNowLoading", Bool, hide_now_loading);

			// Looks like not working for now
			// aspect_ratio = document[L"customAspectRatio"].GetFloat();

			GetValue("textIdDict", String, text_id_dict);

			GetValue("codeMapPath", String, auto path,
				wifstream code_map_stream{ path };
				code_map_stream.imbue(locale(".UTF-8"));

				if (code_map_stream.is_open())
				{
					rapidjson::WIStreamWrapper wrapper{ code_map_stream };
					code_map.ParseStream(wrapper);

					code_map_stream.close();
				}
				);

			GetValue("discordRichPresence", Bool, discord_rich_presence);

			GetValue("allowDeleteCookie", Bool, allow_delete_cookie);

			GetValue("localifySettingsForceKorean", Bool, localify_settings_force_korean);

			GetValue("dumpMsgPack", Bool, dump_msgpack);

			GetValue("dumpMsgPackRequest", Bool, dump_msgpack_request);

#ifdef _DEBUG
			GetValue("unlockLiveChara", Bool, unlock_live_chara);
#endif

			GetValue("notificationTp", Bool, notification_tp);

			GetValue("notificationRp", Bool, notification_rp);

			GetValue("msgpackNotifier", Bool, msgpack_notifier);

			GetValue("msgpackNotifierRequest", Bool, msgpack_notifier_request);

			GetValue("msgpackNotifierHost", String, msgpack_notifier_host);

			GetValue("msgpackNotifierConnectionTimeoutMs", Int, msgpack_notifier_connection_timeout_ms);

			GetValue("msgpackNotifierPrintError", Bool, msgpack_notifier_print_error);

			GetValue("dicts", Array, auto array,
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					if (it->IsString())
					{
						auto value = it->GetString();
						dicts.emplace_back(value);
					}
				}
				);

			GetValue("externalDlls", Array, auto array,
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					if (it->IsString())
					{
						auto value = it->GetString();
						external_dlls_path.emplace_back(value);
					}
				}
				);
		}
		else
		{
			has_json_parse_error = true;
			wstringstream str_stream;
			str_stream << "JSON parse error: " << GetParseError_En(document.GetParseError()) << " (" << document.GetErrorOffset() << ")";
			json_parse_error_msg = str_stream.str();
		}
	}

	bool read_config()
	{
		wifstream config_stream{ "config.json" };
		config_stream.imbue(locale(".UTF-8"));

		if (!config_stream.is_open())
		{
			return false;
		}

		rapidjson::WIStreamWrapper wrapper{ config_stream };

		config_document.ParseStream(wrapper);
		config_stream.close();

		backup_document.CopyFrom(config_document, backup_document.GetAllocator(), true);

		return !config_document.HasParseError();
	}

	void write_config()
	{
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF16<>, rapidjson::UTF8<>> writer(buffer);
		config_document.Accept(writer);

		ofstream config_stream{ "config.json" };
		config_stream << buffer.GetString() << endl;
		config_stream.close();
	}

	void rollback_config()
	{
		config_document.CopyFrom(backup_document, config_document.GetAllocator(), true);
	}

}
