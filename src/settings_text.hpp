#pragma once

#include <string>

#include "config/config.hpp"
#include "game.hpp"

#include "il2cpp/il2cpp-api-types.h"

using namespace std;

namespace LocalifySettings
{
	inline const Il2CppChar* GetText(string id)
	{
		bool isKor = Game::CurrentGameRegion == Game::Region::KOR || config::localify_settings_force_korean;
		bool isEng = Game::CurrentGameRegion == Game::Region::ENG;

		if (id == "settings_title")
		{
			if (isKor)
			{
				return IL2CPP_STRING("Localify 설정");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Localify Settings");
			}
			return IL2CPP_STRING("Localify設定");
		}

		if (id == "open_settings")
		{
			if (isKor)
			{
				return IL2CPP_STRING("설정 열기");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Open settings");
			}
			return IL2CPP_STRING("設定を開く");
		}

		if (id == "setting_disabled")
		{
			if (isKor)
			{
				return IL2CPP_STRING("설정이 활성화 되지 않음");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Setting are not enabled");
			}
			return IL2CPP_STRING("設定が有効になっていません");
		}

		if (id == "applied_after_restart")
		{
			if (isKor)
			{
				return IL2CPP_STRING("<color=#ff911c>변경 사항은 게임 재실행 후 반영됩니다</color>");
			}
			if (isEng)
			{
				return IL2CPP_STRING("<color=#ff911c>Changes will be applied after restart.</color>");
			}
			return IL2CPP_STRING("<color=#ff911c>変更はゲームの再実行後に反映されます</color>");
		}

		if (id == "anti_aliasing")
		{
			if (isKor)
			{
				return IL2CPP_STRING("안티 엘리어싱");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Anti-aliasing");
			}
			return IL2CPP_STRING("アンチエイリアシング");
		}

		if (id == "graphics_quality")
		{
			if (isKor)
			{
				return IL2CPP_STRING("그래픽 품질");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Graphics quality");
			}
			return IL2CPP_STRING("グラフィックの品質");
		}

		if (id == "character_system_text_caption")
		{
			if (isKor)
			{
				return  IL2CPP_STRING("캐릭터 대사 자막");
			}
			if (isEng)
			{
				return  IL2CPP_STRING("Character dialogue caption");
			}
			return IL2CPP_STRING("キャラクターのダイアログテキストを\nキャプションで表示");
		}

		if (id == "character_system_text_caption_line_char_count")
		{
			if (isKor)
			{
				return IL2CPP_STRING("줄당 글자 수");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Characters per line");
			}
			return IL2CPP_STRING("一行あたりの文字数");
		}

		if (id == "character_system_text_caption_font_size")
		{
			if (isKor)
			{
				return IL2CPP_STRING("폰트 크기");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Font size");
			}
			return IL2CPP_STRING("フォントサイズ");
		}

		if (id == "character_system_text_caption_position_x")
		{
			if (isKor)
			{
				return IL2CPP_STRING("X좌표");
			}
			if (isEng)
			{
				return IL2CPP_STRING("X-coordinate");
			}
			return IL2CPP_STRING("X座標");
		}

		if (id == "character_system_text_caption_position_y")
		{
			if (isKor)
			{
				return IL2CPP_STRING("Y좌표");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Y-coordinate");
			}
			return IL2CPP_STRING("Y座標");
		}

		if (id == "character_system_text_caption_background_alpha")
		{
			if (isKor)
			{
				return IL2CPP_STRING("배경 투명도");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Background transparency");
			}
			return IL2CPP_STRING("背景透明度");
		}

		if (id == "character_system_text_caption_font_color")
		{
			if (isKor)
			{
				return IL2CPP_STRING("폰트 색상");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Font color");
			}
			return IL2CPP_STRING("フォントの色");
		}

		if (id == "character_system_text_caption_outline_size")
		{
			if (isKor)
			{
				return IL2CPP_STRING("외곽선 크기");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Outline size");
			}
			return IL2CPP_STRING("アウトラインサイズ");
		}

		if (id == "character_system_text_caption_outline_color")
		{
			if (isKor)
			{
				return IL2CPP_STRING("외곽선 색상");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Outline color");
			}
			return IL2CPP_STRING("アウトラインの色");
		}

		if (id == "show_caption")
		{
			if (isKor)
			{
				return IL2CPP_STRING("자막 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Show caption");
			}
			return IL2CPP_STRING("字幕表示");
		}

		if (id == "show_notification")
		{
			if (isKor)
			{
				return IL2CPP_STRING("알림 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Show notification");
			}
			return IL2CPP_STRING("通知表示");
		}

		if (id == "sample_caption")
		{
			if (isKor)
			{
				return IL2CPP_STRING("샘플 자막");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Sample caption");
			}
			return IL2CPP_STRING("サンプル字幕");
		}

		if (id == "champions_live_show_text")
		{
			if (isKor)
			{
				return IL2CPP_STRING("Ms. VICTORIA 챔피언스 미팅 순위 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Ms. VICTORIA Champions Meeting Standings");
			}
			return IL2CPP_STRING("Ms. VICTORIAチャンピオンズミーティングの\n順位表示");
		}

		if (id == "champions_live_resource_id")
		{
			if (isKor)
			{
				return IL2CPP_STRING("챔피언스 미팅 일정");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Champions Meeting Schedule");
			}
			return IL2CPP_STRING("チャンピオンズミーティングスケジュール");
		}

		if (id == "champions_live_year")
		{
			if (isKor)
			{
				return IL2CPP_STRING("챔피언스 미팅 년도");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Champions Meeting year");
			}
			return IL2CPP_STRING("チャンピオンズミーティングの年");
		}

		if (id == "dump_msgpack")
		{
			if (isKor)
			{
				return IL2CPP_STRING("MessagePack 덤프");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Dump MessagePack");
			}
			return IL2CPP_STRING("MessagePackダンプ");
		}

		if (id == "dump_msgpack_request")
		{
			if (isKor)
			{
				return IL2CPP_STRING("MessagePack 요청 덤프");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Dump MessagePack request");
			}
			return IL2CPP_STRING("MessagePackリクエストダンプ");
		}

		if (id == "unlock_live_chara")
		{
			if (isKor)
			{
				return IL2CPP_STRING("라이브 시어터 전체 해방");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Unlock full concert theater");
			}
			return IL2CPP_STRING("ライブシアター全体解放");
		}

		if (id == "live_slider_always_show")
		{
			if (isKor)
			{
				return IL2CPP_STRING("재생 슬라이더 항상 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Always show playback slider");
			}
			return IL2CPP_STRING("プレイスライダーを常に表示");
		}

		if (id == "live_playback_loop")
		{
			if (isKor)
			{
				return IL2CPP_STRING("반복 재생");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Repeat playback");
			}
			return IL2CPP_STRING("ループプレイ");
		}

		if (id == "unlock_live_chara_info")
		{
			if (isKor)
			{
				return IL2CPP_STRING("미해금/미해방/미공개 곡 및 우마무스메와 드레스를 사용 가능하게 변경합니다.");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Changes the availability of unreleased/unreleased/unpublished songs and Umamusume and dresses.");
			}
			return IL2CPP_STRING("未解禁/未解放/未公開曲及びウマ娘とドレスを使えるように変更します。");
		}

		if (id == "allow_delete_cookie")
		{
			if (isKor)
			{
				return IL2CPP_STRING("WebView Cookie 삭제 허용");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Allow deletion of WebView cookies");
			}
			return IL2CPP_STRING("WebView Cookieの削除を許可する");
		}

		if (id == "clear_webview_cache")
		{
			if (isKor)
			{
				return IL2CPP_STRING("WebView 캐시 삭제");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Clear WebView cache");
			}
			return IL2CPP_STRING("WebViewキャッシュの削除");
		}

		if (id == "clear_webview_cache_confirm")
		{
			if (isKor)
			{
				return IL2CPP_STRING("WebView 캐시를 삭제하시겠습니까?");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Do you want to clear the WebView cache?");
			}
			return IL2CPP_STRING("WebViewキャッシュを削除しますか？");
		}

		if (id == "deleted")
		{
			if (isKor)
			{
				return IL2CPP_STRING("삭제했습니다");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Deleted");
			}
			return IL2CPP_STRING("削除しました");
		}

		if (id == "cyspring_update_mode")
		{
			if (isKor)
			{
				return IL2CPP_STRING("CySpring 업데이트 모드");
			}
			if (isEng)
			{
				return IL2CPP_STRING("CySpring update mode");
			}
			return IL2CPP_STRING("CySpringアップデートモード");
		}

		if (id == "cyspring_disable_native")
		{
			if (isKor)
			{
				return IL2CPP_STRING("CySpring 네이티브 라이브러리 비활성화");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Disable CySpring native library");
			}
			return IL2CPP_STRING("CySpringネイティブライブラリの無効化");
		}

		if (id == "cyspring_disable_native_info")
		{
			if (isKor)
			{
				return IL2CPP_STRING("CySpring 네이티브 라이브러리를 사용하지 않도록 설정합니다. CySpring 관련 오류가 발생할 때 이 옵션을 켜보세요.");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Set not to use the CySpring native library. If you encounter CySpring-related errors, try turning on this option.");
			}
			return IL2CPP_STRING("CySpring関連のエラーが発生したときにこのオプションをオンにしてみてください。");
		}

		if (id == "cyspring_mono_uncap_frame_scale")
		{
			if (isKor)
			{
				return IL2CPP_STRING("CySpring Mono 프레임 스케일 제한 해제");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Remove CySpring Mono frame scale limit");
			}
			return IL2CPP_STRING("CySpring Monoフレームスケール制限の解除");
		}

		if (id == "cyspring_mono_uncap_frame_scale_info")
		{
			if (isKor)
			{
				return IL2CPP_STRING("프레임 스케일을 현재 FPS 설정 기반으로 변경합니다.\nFPS 설정에 따라 물리 효과가 과도하게 작용될 수 있습니다.");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Change the frame scale based on the current FPS settings.\nDepending on the FPS settings, the physical effects may be excessively applied.");
			}
			return IL2CPP_STRING("フレームスケールを現在のFPS設定に基づいて変更します。\nFPS設定によっては物理効果が過度に作用する可能性があります。");
		}

		if (id == "graphics")
		{
			if (isKor)
			{
				return IL2CPP_STRING("그래픽");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Graphics");
			}
			return IL2CPP_STRING("グラフィック");
		}

		if (id == "ui_animation_scale")
		{
			if (isKor)
			{
				return IL2CPP_STRING("UI 애니메이션 배율");
			}
			if (isEng)
			{
				return IL2CPP_STRING("UI animation scale");
			}
			return IL2CPP_STRING("UIアニメーションスケール");
		}

		if (id == "resolution_3d_scale")
		{
			if (isKor)
			{
				return IL2CPP_STRING("3D 렌더링 해상도 배율");
			}
			if (isEng)
			{
				return IL2CPP_STRING("3D rendering resolution scale");
			}
			return IL2CPP_STRING("3Dレンダリングの解像度スケール");
		}

		if (id == "screen")
		{
			if (isKor)
			{
				return IL2CPP_STRING("화면");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Screen");
			}
			return IL2CPP_STRING("画面");
		}

		if (id == "unlock_size")
		{
			if (isKor)
			{
				return IL2CPP_STRING("해상도 고정 해제");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Unlock resolution");
			}
			return IL2CPP_STRING("解像度固定解除");
		}

		if (id == "ui_scale")
		{
			if (isKor)
			{
				return IL2CPP_STRING("UI 스케일");
			}
			if (isEng)
			{
				return IL2CPP_STRING("UI scale");
			}
			return IL2CPP_STRING("UIスケール");
		}

		if (id == "freeform_window")
		{
			if (isKor)
			{
				return IL2CPP_STRING("자유 형식 창");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Free-form window");
			}
			return IL2CPP_STRING("自由形式ウィンドウ");
		}

		if (id == "ui_scale_portrait")
		{
			if (isKor)
			{
				return IL2CPP_STRING("UI 스케일 (세로)");
			}
			if (isEng)
			{
				return IL2CPP_STRING("UI scale (Vertical)");
			}
			return IL2CPP_STRING("UIスケール(縦)");
		}

		if (id == "ui_scale_landscape")
		{
			if (isKor)
			{
				return IL2CPP_STRING("UI 스케일 (가로)");
			}
			if (isEng)
			{
				return IL2CPP_STRING("UI scale (Horizontal)");
			}
			return IL2CPP_STRING("UIスケール(横)");
		}

		if (id == "use_third_party_news")
		{
			if (isKor)
			{
				return IL2CPP_STRING("타사 공지 사용");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Use of Third-Party notices");
			}
			return IL2CPP_STRING("サードパーティのお知らせの使用");
		}

		if (id == "use_third_party_news_info")
		{
			if (isKor)
			{
				return IL2CPP_STRING("인앱 공지 대신 타사 공지를 사용합니다");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Use third-party notices instead of in-app notices");
			}
			return IL2CPP_STRING("アプリ内お知らせの代わりにサードパーティのお知らせを使用します");
		}

		if (id == "taskbar")
		{
			if (isKor)
			{
				return IL2CPP_STRING("작업 표시줄");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Taskbar");
			}
			return IL2CPP_STRING("タスクバー");
		}

		if (id == "taskbar_show_progress_on_download")
		{
			if (isKor)
			{
				return IL2CPP_STRING("다운로드 진행률 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Show download progress");
			}
			return IL2CPP_STRING("ダウンロードの進行状況を表示");
		}

		if (id == "taskbar_show_progress_on_connecting")
		{
			if (isKor)
			{
				return IL2CPP_STRING("Connecting 진행률 표시");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Show Connecting progress");
			}
			return IL2CPP_STRING("Connectingの進捗状況を表示");
		}

		if (id == "persistent_data_path")
		{
			if (isKor)
			{
				return IL2CPP_STRING("로컬 데이터 경로");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Local data path");
			}
			return IL2CPP_STRING("ローカルデータパス");
		}

		if (id == "experiments")
		{
			return IL2CPP_STRING("Experiments");
		}

		if (id == "initial_disclaimer_1")
		{
			if (isKor)
			{
				return IL2CPP_STRING("본 개선 모드를 사용함으로서 발생하는 이용 제한, 진행 오류 또는\n이외 기타 문제에 대한 책임은 본인에게 있습니다.\n\n계속 진행하려면 <color=#ff911c><i>");
			}
			if (isEng)
			{
				return IL2CPP_STRING("You are responsible for any limitations, errors, or other issues that may arise from using this improvement mod.\n\nPlease check <color=#ff911c><i>");
			}
			return IL2CPP_STRING("本改善モードを使用することによって発生する利用制限、進行エラーまたは\nその他の問題に対する責任は本人にあります。\n\n続行するには、<color=#ff911c><i>");
		}

		if (id == "initial_disclaimer_2")
		{
			if (isKor)
			{
				return IL2CPP_STRING("</i></color> 를 체크해주세요.");
			}
			if (isEng)
			{
				return IL2CPP_STRING("</i></color> to continue.");
			}
			return IL2CPP_STRING("</i></color>をチェックしてください。");
		}

		if (id == "title")
		{
			if (isKor)
			{
				return IL2CPP_STRING("우마무스메");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Umamusume");
			}
			return IL2CPP_STRING("ウマ娘");
		}

		if (id == "unknown")
		{
			if (isKor)
			{
				return IL2CPP_STRING("알수없음");
			}
			if (isEng)
			{
				return IL2CPP_STRING("Unknown");
			}
			return IL2CPP_STRING("不明");
		}

		return IL2CPP_STRING("");
	}
}
