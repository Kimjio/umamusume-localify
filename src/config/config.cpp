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
			if (document.HasMember(L"enableConsole"))
			{
				enable_console = document[L"enableConsole"].GetBool();
			}

			if (document.HasMember(L"enableLogger"))
			{
				enable_logger = document[L"enableLogger"].GetBool();
			}

			if (document.HasMember(L"dumpStaticEntries"))
			{
				dump_entries = document[L"dumpStaticEntries"].GetBool();
			}

			if (document.HasMember(L"dumpIl2Cpp"))
			{
				dump_il2cpp = document[L"dumpIl2Cpp"].GetBool();
			}

			if (document.HasMember(L"staticEntriesUseHash"))
			{
				static_entries_use_hash = document[L"staticEntriesUseHash"].GetBool();
			}

			if (document.HasMember(L"staticEntriesUseTextIdName"))
			{
				static_entries_use_text_id_name = document[L"staticEntriesUseTextIdName"].GetBool();
			}

			if (document.HasMember(L"maxFps"))
			{
				max_fps = document[L"maxFps"].GetInt();
			}

			if (document.HasMember(L"unlockSize"))
			{
				unlock_size = document[L"unlockSize"].GetBool();
			}

			if (document.HasMember(L"unlockSizeUseSystemResolution"))
			{
				unlock_size_use_system_resolution = document[L"unlockSizeUseSystemResolution"].GetBool();
			}

			if (document.HasMember(L"uiScale"))
			{
				ui_scale = document[L"uiScale"].GetFloat();
			}

			if (document.HasMember(L"freeFormWindow"))
			{
				freeform_window = document[L"freeFormWindow"].GetBool();
				if (freeform_window)
				{
					unlock_size = true;
				}
			}

			if (document.HasMember(L"freeFormUiScalePortrait"))
			{
				freeform_ui_scale_portrait = document[L"freeFormUiScalePortrait"].GetFloat();
				if (freeform_ui_scale_portrait <= 0)
				{
					freeform_ui_scale_portrait = 0.5f;
				}
			}

			if (document.HasMember(L"freeFormUiScaleLandscape"))
			{
				freeform_ui_scale_landscape = document[L"freeFormUiScaleLandscape"].GetFloat();
				if (freeform_ui_scale_landscape <= 0)
				{
					freeform_ui_scale_landscape = 0.5f;
				}
			}

			if (document.HasMember(L"initialWidth"))
			{
				initial_width = document[L"initialWidth"].GetInt();
				if (initial_width <= 72)
				{
					initial_width = -1;
				}
			}

			if (document.HasMember(L"initialHeight"))
			{
				initial_height = document[L"initialHeight"].GetInt();
				if (initial_height <= 72)
				{
					initial_height = -1;
				}
			}

			if (document.HasMember(L"uiAnimationScale"))
			{
				ui_animation_scale = document[L"uiAnimationScale"].GetFloat();
			}

			if (document.HasMember(L"resolution3dScale"))
			{
				resolution_3d_scale = document[L"resolution3dScale"].GetFloat();
			}

			if (document.HasMember(L"replaceFont"))
			{
				replace_to_builtin_font = document[L"replaceFont"].GetBool();
			}

			if (!document.HasMember(L"replaceFont") && document.HasMember(L"replaceToBuiltinFont"))
			{
				replace_to_builtin_font = document[L"replaceToBuiltinFont"].GetBool();
			}

			if (document.HasMember(L"replaceToCustomFont"))
			{
				replace_to_custom_font = document[L"replaceToCustomFont"].GetBool();
			}

			if (document.HasMember(L"fontAssetBundlePath"))
			{
				font_assetbundle_path = wstring(document[L"fontAssetBundlePath"].GetString());
			}

			if (document.HasMember(L"fontAssetName"))
			{
				font_asset_name = wstring(document[L"fontAssetName"].GetString());
			}

			if (document.HasMember(L"tmproFontAssetName"))
			{
				tmpro_font_asset_name = wstring(document[L"tmproFontAssetName"].GetString());
			}

			if (document.HasMember(L"autoFullscreen"))
			{
				auto_fullscreen = document[L"autoFullscreen"].GetBool();
			}

			if (document.HasMember(L"graphicsQuality"))
			{
				graphics_quality = document[L"graphicsQuality"].GetInt();
				if (graphics_quality < -1)
				{
					graphics_quality = -1;
				}
				if (graphics_quality > 4)
				{
					graphics_quality = 3;
				}
			}

			if (document.HasMember(L"antiAliasing"))
			{
				anti_aliasing = document[L"antiAliasing"].GetInt();
				vector<int> options = { 0, 2, 4, 8, -1 };
				anti_aliasing = options[find(options.begin(), options.end(), anti_aliasing) - options.begin()];
			}

			if (document.HasMember(L"anisotropicFiltering"))
			{
				anisotropic_filtering = document[L"anisotropicFiltering"].GetInt();
				vector<int> options = { 0, 1, 2, -1 };
				anisotropic_filtering = options[find(options.begin(), options.end(), anisotropic_filtering) - options.begin()];
			}

			if (document.HasMember(L"vSyncCount"))
			{
				vsync_count = document[L"vSyncCount"].GetInt();
				vector<int> options = { 0, 1, 2, 3, 4, -1 };
				vsync_count = options[find(options.begin(), options.end(), vsync_count) - options.begin()];
			}

			if (document.HasMember(L"uiLoadingShowOrientationGuide"))
			{
				ui_loading_show_orientation_guide = document[L"uiLoadingShowOrientationGuide"].GetBool();
			}

			if (document.HasMember(L"customTitleName"))
			{
				custom_title_name = document[L"customTitleName"].GetString();
			}

			if (document.HasMember(L"replaceAssetsPaths") && document[L"replaceAssetsPaths"].IsArray())
			{
				auto array = document[L"replaceAssetsPaths"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					wstring value = it->GetString();

					if (PathIsRelativeW(value.data()))
					{
						value.insert(0, (static_cast<wstring>(filesystem::current_path().native())).append(L"/"));
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

			if (document.HasMember(L"replaceAssetBundleFilePath"))
			{
				replace_assetbundle_file_path = document[L"replaceAssetBundleFilePath"].GetString();
			}

			if (document.HasMember(L"replaceAssetBundleFilePaths") &&
				document[L"replaceAssetBundleFilePaths"].IsArray())
			{
				auto array = document[L"replaceAssetBundleFilePaths"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					wstring value = it->GetString();
					replace_assetbundle_file_paths.emplace_back(value);
				}
			}

			if (document.HasMember(L"replaceTextDBPath"))
			{
				replace_text_db_path = document[L"replaceTextDBPath"].GetString();
			}

			if (document.HasMember(L"characterSystemTextCaption"))
			{
				character_system_text_caption = document[L"characterSystemTextCaption"].GetBool();
			}

			if (document.HasMember(L"characterSystemTextCaptionLineCharCount"))
			{
				character_system_text_caption_line_char_count = document[L"characterSystemTextCaptionLineCharCount"].GetInt();
			}

			if (document.HasMember(L"characterSystemTextCaptionFontSize"))
			{
				character_system_text_caption_font_size = document[L"characterSystemTextCaptionFontSize"].GetInt();
			}

			if (document.HasMember(L"characterSystemTextCaptionFontColor"))
			{
				character_system_text_caption_font_color = document[L"characterSystemTextCaptionFontColor"].GetString();
			}

			if (document.HasMember(L"characterSystemTextCaptionOutlineSize"))
			{
				character_system_text_caption_outline_size = document[L"characterSystemTextCaptionOutlineSize"].GetString();
			}

			if (document.HasMember(L"characterSystemTextCaptionOutlineColor"))
			{
				character_system_text_caption_outline_color = document[L"characterSystemTextCaptionOutlineColor"].GetString();
			}

			if (document.HasMember(L"characterSystemTextCaptionBackgroundAlpha"))
			{
				character_system_text_caption_background_alpha = document[L"characterSystemTextCaptionBackgroundAlpha"].GetFloat();
			}

			if (document.HasMember(L"characterSystemTextCaptionPositionX"))
			{
				character_system_text_caption_position_x = document[L"characterSystemTextCaptionPositionX"].GetFloat();
			}

			if (document.HasMember(L"characterSystemTextCaptionPositionY"))
			{
				character_system_text_caption_position_y = document[L"characterSystemTextCaptionPositionY"].GetFloat();
			}

			if (document.HasMember(L"championsLiveShowText"))
			{
				champions_live_show_text = document[L"championsLiveShowText"].GetBool();
			}

			if (document.HasMember(L"championsLiveResourceId"))
			{
				champions_live_resource_id = document[L"championsLiveResourceId"].GetInt();

				if (champions_live_resource_id < 1)
				{
					champions_live_resource_id = 1;
				}
			}

			if (document.HasMember(L"championsLiveYear"))
			{
				champions_live_year = document[L"championsLiveYear"].GetInt();
			}

			if (document.HasMember(L"cySpringUpdateMode"))
			{
				cyspring_update_mode = document[L"cySpringUpdateMode"].GetInt();
				vector<int> options = { 0, 1, 2, 3, -1 };
				cyspring_update_mode = options[find(options.begin(), options.end(), cyspring_update_mode) - options.begin()];
			}
			else if (max_fps > 30)
			{
				cyspring_update_mode = 1;
			}

			if (document.HasMember(L"hideNowLoading"))
			{
				hide_now_loading = document[L"hideNowLoading"].GetBool();
			}

			// Looks like not working for now
			// aspect_ratio = document[L"customAspectRatio"].GetFloat();

			if (document.HasMember(L"textIdDict"))
			{
				text_id_dict = document[L"textIdDict"].GetString();
			}

			if (document.HasMember(L"codeMapPath"))
			{
				auto path = document[L"codeMapPath"].GetString();

				wifstream code_map_stream{ path };

				if (code_map_stream.is_open())
				{
					rapidjson::WIStreamWrapper wrapper{ code_map_stream };
					code_map.ParseStream(wrapper);

					code_map_stream.close();
				}
			}

			if (document.HasMember(L"discordRichPresence"))
			{
				discord_rich_presence = document[L"discordRichPresence"].GetBool();
			}

			if (document.HasMember(L"allowDeleteCookie"))
			{
				allow_delete_cookie = document[L"allowDeleteCookie"].GetBool();
			}

			if (document.HasMember(L"localifySettingsForceKorean"))
			{
				localify_settings_force_korean = document[L"localifySettingsForceKorean"].GetBool();
			}

			if (document.HasMember(L"dumpMsgPack"))
			{
				dump_msgpack = document[L"dumpMsgPack"].GetBool();
			}

			if (document.HasMember(L"dumpMsgPackRequest"))
			{
				dump_msgpack_request = document[L"dumpMsgPackRequest"].GetBool();
			}

			if (document.HasMember(L"unlockLiveChara"))
			{
				unlock_live_chara = document[L"unlockLiveChara"].GetBool();
			}

			if (document.HasMember(L"notificationTp"))
			{
				notification_tp = document[L"notificationTp"].GetBool();
			}

			if (document.HasMember(L"notificationRp"))
			{
				notification_rp = document[L"notificationRp"].GetBool();
			}

			if (document.HasMember(L"msgpackNotifier"))
			{
				msgpack_notifier = document[L"msgpackNotifier"].GetBool();
			}

			if (document.HasMember(L"msgpackNotifierRequest"))
			{
				msgpack_notifier_request = document[L"msgpackNotifierRequest"].GetBool();
			}

			if (document.HasMember(L"msgpackNotifierHost"))
			{
				msgpack_notifier_host = document[L"msgpackNotifierHost"].GetString();
			}

			if (document.HasMember(L"msgpackNotifierConnectionTimeoutMs"))
			{
				msgpack_notifier_connection_timeout_ms = document[L"msgpackNotifierConnectionTimeoutMs"].GetInt();
			}

			if (document.HasMember(L"msgpackNotifierPrintError"))
			{
				msgpack_notifier_print_error = document[L"msgpackNotifierPrintError"].GetBool();
			}

			if (document.HasMember(L"dicts") && document[L"dicts"].IsArray())
			{
				auto array = document[L"dicts"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					auto value = it->GetString();
					dicts.emplace_back(value);
				}
			}

			if (document.HasMember(L"externalDlls") && document[L"externalDlls"].IsArray())
			{
				auto array = document[L"externalDlls"].GetArray();
				for (auto it = array.Begin(); it != array.End(); it++)
				{
					auto value = it->GetString();
					external_dlls_path.emplace_back(value);
				}
			}
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
