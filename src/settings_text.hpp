#pragma once

#include <string>

#include "config/config.hpp"
#include "game.hpp"

using namespace std;

namespace LocalifySettings
{
	inline const wchar_t* GetText(string id)
	{
		bool isKor = Game::CurrentGameRegion == Game::Region::KOR || config::localify_settings_force_korean;
		bool isEng = Game::CurrentGameRegion == Game::Region::ENG;

		if (id == "settings_title")
		{
			if (isKor)
			{
				return L"Localify 설정";
			}
			if (isEng)
			{
				return L"Localify Settings";
			}
			return L"Localify設定";
		}

		if (id == "open_settings")
		{
			if (isKor)
			{
				return L"설정 열기";
			}
			if (isEng)
			{
				return L"Open settings";
			}
			return L"設定を開く";
		}

		if (id == "setting_disabled")
		{
			if (isKor)
			{
				return L"설정이 활성화 되지 않음";
			}
			if (isEng)
			{
				return L"Setting are not enabled";
			}
			return L"設定が有効になっていません";
		}

		if (id == "applied_after_restart")
		{
			if (isKor)
			{
				return L"<color=#ff911c>변경 사항은 게임 재실행 후 반영됩니다</color>";
			}
			if (isEng)
			{
				return L"<color=#ff911c>Changes will be applied after restart.</color>";
			}
			return L"<color=#ff911c>変更はゲームの再実行後に反映されます</color>";
		}

		if (id == "anti_aliasing")
		{
			if (isKor)
			{
				return L"안티 엘리어싱";
			}
			if (isEng)
			{
				return L"Anti-aliasing";
			}
			return L"アンチエイリアシング";
		}

		if (id == "graphics_quality")
		{
			if (isKor)
			{
				return L"그래픽 품질";
			}
			if (isEng)
			{
				return L"Graphics quality";
			}
			return L"グラフィックの品質";
		}

		if (id == "character_system_text_caption")
		{
			if (isKor)
			{
				return  L"캐릭터 대사 자막";
			}
			if (isEng)
			{
				return  L"Character dialogue caption";
			}
			return L"キャラクターのダイアログテキストを\nキャプションで表示";
		}

		if (id == "character_system_text_caption_line_char_count")
		{
			if (isKor)
			{
				return L"줄당 글자 수";
			}
			if (isEng)
			{
				return L"Characters per line";
			}
			return L"一行あたりの文字数";
		}

		if (id == "character_system_text_caption_font_size")
		{
			if (isKor)
			{
				return L"폰트 크기";
			}
			if (isEng)
			{
				return L"Font size";
			}
			return L"フォントサイズ";
		}

		if (id == "character_system_text_caption_position_x")
		{
			if (isKor)
			{
				return L"X좌표";
			}
			if (isEng)
			{
				return L"X-coordinate";
			}
			return L"X座標";
		}

		if (id == "character_system_text_caption_position_y")
		{
			if (isKor)
			{
				return L"Y좌표";
			}
			if (isEng)
			{
				return L"Y-coordinate";
			}
			return L"Y座標";
		}

		if (id == "character_system_text_caption_background_alpha")
		{
			if (isKor)
			{
				return L"배경 투명도";
			}
			if (isEng)
			{
				return L"Background transparency";
			}
			return L"背景透明度";
		}

		if (id == "character_system_text_caption_font_color")
		{
			if (isKor)
			{
				return L"폰트 색상";
			}
			if (isEng)
			{
				return L"Font color";
			}
			return L"フォントの色";
		}

		if (id == "character_system_text_caption_outline_size")
		{
			if (isKor)
			{
				return L"외곽선 크기";
			}
			if (isEng)
			{
				return L"Outline size";
			}
			return L"アウトラインサイズ";
		}

		if (id == "character_system_text_caption_outline_color")
		{
			if (isKor)
			{
				return L"외곽선 색상";
			}
			if (isEng)
			{
				return L"Outline color";
			}
			return L"アウトラインの色";
		}

		if (id == "show_caption")
		{
			if (isKor)
			{
				return L"자막 표시";
			}
			if (isEng)
			{
				return L"Show caption";
			}
			return L"字幕表示";
		}

		if (id == "show_notification")
		{
			if (isKor)
			{
				return L"알림 표시";
			}
			if (isEng)
			{
				return L"Show notification";
			}
			return L"通知表示";
		}

		if (id == "sample_caption")
		{
			if (isKor)
			{
				return L"샘플 자막";
			}
			if (isEng)
			{
				return L"Sample caption";
			}
			return L"サンプル字幕";
		}

		if (id == "champions_live_show_text")
		{
			if (isKor)
			{
				return L"Ms. VICTORIA 챔피언스 미팅 순위 표시";
			}
			if (isEng)
			{
				return L"Ms. VICTORIA Champions Meeting Standings";
			}
			return L"Ms. VICTORIAチャンピオンズミーティングの\n順位表示";
		}

		if (id == "champions_live_resource_id")
		{
			if (isKor)
			{
				return L"챔피언스 미팅 일정";
			}
			if (isEng)
			{
				return L"Champions Meeting Schedule";
			}
			return L"チャンピオンズミーティングスケジュール";
		}

		if (id == "champions_live_year")
		{
			if (isKor)
			{
				return L"챔피언스 미팅 년도";
			}
			if (isEng)
			{
				return L"Champions Meeting year";
			}
			return L"チャンピオンズミーティングの年";
		}

		if (id == "dump_msgpack")
		{
			if (isKor)
			{
				return L"MessagePack 덤프";
			}
			if (isEng)
			{
				return L"Dump MessagePack";
			}
			return L"MessagePackダンプ";
		}

		if (id == "dump_msgpack_request")
		{
			if (isKor)
			{
				return L"MessagePack 요청 덤프";
			}
			if (isEng)
			{
				return L"Dump MessagePack request";
			}
			return L"MessagePackリクエストダンプ";
		}

		if (id == "unlock_live_chara")
		{
			if (isKor)
			{
				return L"라이브 시어터 전체 해방";
			}
			if (isEng)
			{
				return L"Unlock full concert theater";
			}
			return L"ライブシアター全体解放";
		}

		if (id == "live_slider_always_show")
		{
			if (isKor)
			{
				return L"재생 슬라이더 항상 표시";
			}
			if (isEng)
			{
				return L"Always show playback slider";
			}
			return L"プレイスライダーを常に表示";
		}

		if (id == "live_playback_loop")
		{
			if (isKor)
			{
				return L"반복 재생";
			}
			if (isEng)
			{
				return L"Repeat playback";
			}
			return L"ループプレイ";
		}

		if (id == "unlock_live_chara_info")
		{
			if (isKor)
			{
				return L"미해금/미해방/미공개 곡 및 우마무스메와 드레스를 사용 가능하게 변경합니다.";
			}
			if (isEng)
			{
				return L"Changes the availability of unreleased/unreleased/unpublished songs and Umamusume and dresses.";
			}
			return L"未解禁/未解放/未公開曲及びウマ娘とドレスを使えるように変更します。";
		}

		if (id == "allow_delete_cookie")
		{
			if (isKor)
			{
				return L"WebView Cookie 삭제 허용";
			}
			if (isEng)
			{
				return L"Allow deletion of WebView cookies";
			}
			return L"WebView Cookieの削除を許可する";
		}

		if (id == "clear_webview_cache")
		{
			if (isKor)
			{
				return L"WebView 캐시 삭제";
			}
			if (isEng)
			{
				return L"Clear WebView cache";
			}
			return L"WebViewキャッシュの削除";
		}

		if (id == "clear_webview_cache_confirm")
		{
			if (isKor)
			{
				return L"WebView 캐시를 삭제하시겠습니까?";
			}
			if (isEng)
			{
				return L"Do you want to clear the WebView cache?";
			}
			return L"WebViewキャッシュを削除しますか？";
		}

		if (id == "deleted")
		{
			if (isKor)
			{
				return L"삭제했습니다";
			}
			if (isEng)
			{
				return L"Deleted";
			}
			return L"削除しました";
		}

		if (id == "cyspring_update_mode")
		{
			if (isKor)
			{
				return L"CySpring 업데이트 모드";
			}
			if (isEng)
			{
				return L"CySpring update mode";
			}
			return L"CySpringアップデートモード";
		}

		if (id == "graphics")
		{
			if (isKor)
			{
				return L"그래픽";
			}
			if (isEng)
			{
				return L"Graphics";
			}
			return L"グラフィック";
		}

		if (id == "ui_animation_scale")
		{
			if (isKor)
			{
				return L"UI 애니메이션 배율";
			}
			if (isEng)
			{
				return L"UI animation scale";
			}
			return L"UIアニメーションスケール";
		}

		if (id == "resolution_3d_scale")
		{
			if (isKor)
			{
				return L"3D 렌더링 해상도 배율";
			}
			if (isEng)
			{
				return L"3D rendering resolution scale";
			}
			return L"3Dレンダリングの解像度スケール";
		}

		if (id == "screen")
		{
			if (isKor)
			{
				return L"화면";
			}
			if (isEng)
			{
				return L"Screen";
			}
			return L"画面";
		}

		if (id == "unlock_size")
		{
			if (isKor)
			{
				return L"해상도 고정 해제";
			}
			if (isEng)
			{
				return L"Unlock resolution";
			}
			return L"解像度固定解除";
		}

		if (id == "use_system_resolution")
		{
			if (isKor)
			{
				return L"시스템 해상도 사용";
			}
			if (isEng)
			{
				return L"Use system resolution";
			}
			return L"システムの解像度を使用する";
		}

		if (id == "ui_scale")
		{
			if (isKor)
			{
				return L"UI 스케일";
			}
			if (isEng)
			{
				return L"UI scale";
			}
			return L"UIスケール";
		}

		if (id == "auto_fullscreen")
		{
			if (isKor)
			{
				return L"자동 전체화면";
			}
			if (isEng)
			{
				return L"Auto fullscreen";
			}
			return L"自動全画面";
		}

		if (id == "freeform_window")
		{
			if (isKor)
			{
				return L"자유 형식 창";
			}
			if (isEng)
			{
				return L"Free-form window";
			}
			return L"自由形式ウィンドウ";
		}

		if (id == "ui_scale_portrait")
		{
			if (isKor)
			{
				return L"UI 스케일 (세로)";
			}
			if (isEng)
			{
				return L"UI scale (Vertical)";
			}
			return L"UIスケール(縦)";
		}

		if (id == "ui_scale_landscape")
		{
			if (isKor)
			{
				return L"UI 스케일 (가로)";
			}
			if (isEng)
			{
				return L"UI scale (Horizontal)";
			}
			return L"UIスケール(横)";
		}

		if (id == "use_third_party_news")
		{
			if (isKor)
			{
				return L"타사 공지 사용";
			}
			if (isEng)
			{
				return L"Use of Third-Party notices";
			}
			return L"サードパーティのお知らせの使用";
		}

		if (id == "use_third_party_news_info")
		{
			if (isKor)
			{
				return L"인앱 공지 대신 타사 공지를 사용합니다";
			}
			if (isEng)
			{
				return L"Use third-party notices instead of in-app notices";
			}
			return L"アプリ内お知らせの代わりにサードパーティのお知らせを使用します";
		}

		if (id == "taskbar")
		{
			if (isKor)
			{
				return L"작업 표시줄";
			}
			if (isEng)
			{
				return L"Taskbar";
			}
			return L"タスクバー";
		}

		if (id == "taskbar_show_progress_on_download")
		{
			if (isKor)
			{
				return L"다운로드 진행률 표시";
			}
			if (isEng)
			{
				return L"Show download progress";
			}
			return L"ダウンロードの進行状況を表示";
		}

		if (id == "taskbar_show_progress_on_connecting")
		{
			if (isKor)
			{
				return L"Connecting 진행률 표시";
			}
			if (isEng)
			{
				return L"Show Connecting progress";
			}
			return L"Connectingの進捗状況を表示";
		}

		if (id == "persistent_data_path")
		{
			if (isKor)
			{
				return L"로컬 데이터 경로";
			}
			if (isEng)
			{
				return L"Local data path";
			}
			return L"ローカルデータパス";
		}

		if (id == "experiments")
		{
			return L"Experiments";
		}

		if (id == "initial_disclaimer_1")
		{
			if (isKor)
			{
				return L"본 개선 모드를 사용함으로서 발생하는 이용 제한, 진행 오류 또는\n이외 기타 문제에 대한 책임은 본인에게 있습니다.\n\n계속 진행하려면 <color=#ff911c><i>";
			}
			if (isEng)
			{
				return L"You are responsible for any limitations, errors, or other issues that may arise from using this improvement mod.\n\nPlease check <color=#ff911c><i>";
			}
			return L"本改善モードを使用することによって発生する利用制限、進行エラーまたは\nその他の問題に対する責任は本人にあります。\n\n続行するには、<color=#ff911c><i>";
		}

		if (id == "initial_disclaimer_2")
		{
			if (isKor)
			{
				return L"</i></color> 를 체크해주세요.";
			}
			if (isEng)
			{
				return L"</i></color> to continue.";
			}
			return L"</i></color>をチェックしてください。";
		}

		if (id == "title")
		{
			if (isKor)
			{
				return L"우마무스메";
			}
			if (isEng)
			{
				return L"Umamusume";
			}
			return L"ウマ娘";
		}

		if (id == "unknown")
		{
			if (isKor)
			{
				return L"알수없음";
			}
			if (isEng)
			{
				return L"Unknown";
			}
			return L"不明";
		}

		return L"";
	}
}
