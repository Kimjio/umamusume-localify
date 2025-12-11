#pragma once

#include <string>

#include "config/config.hpp"
#include "game.hpp"

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
				return u"Localify 설정";
			}
			if (isEng)
			{
				return u"Localify Settings";
			}
			return u"Localify設定";
		}

		if (id == "open_settings")
		{
			if (isKor)
			{
				return u"설정 열기";
			}
			if (isEng)
			{
				return u"Open settings";
			}
			return u"設定を開く";
		}

		if (id == "setting_disabled")
		{
			if (isKor)
			{
				return u"설정이 활성화 되지 않음";
			}
			if (isEng)
			{
				return u"Setting are not enabled";
			}
			return u"設定が有効になっていません";
		}

		if (id == "applied_after_restart")
		{
			if (isKor)
			{
				return u"<color=#ff911c>변경 사항은 게임 재실행 후 반영됩니다</color>";
			}
			if (isEng)
			{
				return u"<color=#ff911c>Changes will be applied after restart.</color>";
			}
			return u"<color=#ff911c>変更はゲームの再実行後に反映されます</color>";
		}

		if (id == "anti_aliasing")
		{
			if (isKor)
			{
				return u"안티 엘리어싱";
			}
			if (isEng)
			{
				return u"Anti-aliasing";
			}
			return u"アンチエイリアシング";
		}

		if (id == "graphics_quality")
		{
			if (isKor)
			{
				return u"그래픽 품질";
			}
			if (isEng)
			{
				return u"Graphics quality";
			}
			return u"グラフィックの品質";
		}

		if (id == "character_system_text_caption")
		{
			if (isKor)
			{
				return  u"캐릭터 대사 자막";
			}
			if (isEng)
			{
				return  u"Character dialogue caption";
			}
			return u"キャラクターのダイアログテキストを\nキャプションで表示";
		}

		if (id == "character_system_text_caption_line_char_count")
		{
			if (isKor)
			{
				return u"줄당 글자 수";
			}
			if (isEng)
			{
				return u"Characters per line";
			}
			return u"一行あたりの文字数";
		}

		if (id == "character_system_text_caption_font_size")
		{
			if (isKor)
			{
				return u"폰트 크기";
			}
			if (isEng)
			{
				return u"Font size";
			}
			return u"フォントサイズ";
		}

		if (id == "character_system_text_caption_position_x")
		{
			if (isKor)
			{
				return u"X좌표";
			}
			if (isEng)
			{
				return u"X-coordinate";
			}
			return u"X座標";
		}

		if (id == "character_system_text_caption_position_y")
		{
			if (isKor)
			{
				return u"Y좌표";
			}
			if (isEng)
			{
				return u"Y-coordinate";
			}
			return u"Y座標";
		}

		if (id == "character_system_text_caption_background_alpha")
		{
			if (isKor)
			{
				return u"배경 투명도";
			}
			if (isEng)
			{
				return u"Background transparency";
			}
			return u"背景透明度";
		}

		if (id == "character_system_text_caption_font_color")
		{
			if (isKor)
			{
				return u"폰트 색상";
			}
			if (isEng)
			{
				return u"Font color";
			}
			return u"フォントの色";
		}

		if (id == "character_system_text_caption_outline_size")
		{
			if (isKor)
			{
				return u"외곽선 크기";
			}
			if (isEng)
			{
				return u"Outline size";
			}
			return u"アウトラインサイズ";
		}

		if (id == "character_system_text_caption_outline_color")
		{
			if (isKor)
			{
				return u"외곽선 색상";
			}
			if (isEng)
			{
				return u"Outline color";
			}
			return u"アウトラインの色";
		}

		if (id == "show_caption")
		{
			if (isKor)
			{
				return u"자막 표시";
			}
			if (isEng)
			{
				return u"Show caption";
			}
			return u"字幕表示";
		}

		if (id == "show_notification")
		{
			if (isKor)
			{
				return u"알림 표시";
			}
			if (isEng)
			{
				return u"Show notification";
			}
			return u"通知表示";
		}

		if (id == "sample_caption")
		{
			if (isKor)
			{
				return u"샘플 자막";
			}
			if (isEng)
			{
				return u"Sample caption";
			}
			return u"サンプル字幕";
		}

		if (id == "champions_live_show_text")
		{
			if (isKor)
			{
				return u"Ms. VICTORIA 챔피언스 미팅 순위 표시";
			}
			if (isEng)
			{
				return u"Ms. VICTORIA Champions Meeting Standings";
			}
			return u"Ms. VICTORIAチャンピオンズミーティングの\n順位表示";
		}

		if (id == "champions_live_resource_id")
		{
			if (isKor)
			{
				return u"챔피언스 미팅 일정";
			}
			if (isEng)
			{
				return u"Champions Meeting Schedule";
			}
			return u"チャンピオンズミーティングスケジュール";
		}

		if (id == "champions_live_year")
		{
			if (isKor)
			{
				return u"챔피언스 미팅 년도";
			}
			if (isEng)
			{
				return u"Champions Meeting year";
			}
			return u"チャンピオンズミーティングの年";
		}

		if (id == "dump_msgpack")
		{
			if (isKor)
			{
				return u"MessagePack 덤프";
			}
			if (isEng)
			{
				return u"Dump MessagePack";
			}
			return u"MessagePackダンプ";
		}

		if (id == "dump_msgpack_request")
		{
			if (isKor)
			{
				return u"MessagePack 요청 덤프";
			}
			if (isEng)
			{
				return u"Dump MessagePack request";
			}
			return u"MessagePackリクエストダンプ";
		}

		if (id == "unlock_live_chara")
		{
			if (isKor)
			{
				return u"라이브 시어터 전체 해방";
			}
			if (isEng)
			{
				return u"Unlock full concert theater";
			}
			return u"ライブシアター全体解放";
		}

		if (id == "live_slider_always_show")
		{
			if (isKor)
			{
				return u"재생 슬라이더 항상 표시";
			}
			if (isEng)
			{
				return u"Always show playback slider";
			}
			return u"プレイスライダーを常に表示";
		}

		if (id == "live_playback_loop")
		{
			if (isKor)
			{
				return u"반복 재생";
			}
			if (isEng)
			{
				return u"Repeat playback";
			}
			return u"ループプレイ";
		}

		if (id == "unlock_live_chara_info")
		{
			if (isKor)
			{
				return u"미해금/미해방/미공개 곡 및 우마무스메와 드레스를 사용 가능하게 변경합니다.";
			}
			if (isEng)
			{
				return u"Changes the availability of unreleased/unreleased/unpublished songs and Umamusume and dresses.";
			}
			return u"未解禁/未解放/未公開曲及びウマ娘とドレスを使えるように変更します。";
		}

		if (id == "allow_delete_cookie")
		{
			if (isKor)
			{
				return u"WebView Cookie 삭제 허용";
			}
			if (isEng)
			{
				return u"Allow deletion of WebView cookies";
			}
			return u"WebView Cookieの削除を許可する";
		}

		if (id == "clear_webview_cache")
		{
			if (isKor)
			{
				return u"WebView 캐시 삭제";
			}
			if (isEng)
			{
				return u"Clear WebView cache";
			}
			return u"WebViewキャッシュの削除";
		}

		if (id == "clear_webview_cache_confirm")
		{
			if (isKor)
			{
				return u"WebView 캐시를 삭제하시겠습니까?";
			}
			if (isEng)
			{
				return u"Do you want to clear the WebView cache?";
			}
			return u"WebViewキャッシュを削除しますか？";
		}

		if (id == "deleted")
		{
			if (isKor)
			{
				return u"삭제했습니다";
			}
			if (isEng)
			{
				return u"Deleted";
			}
			return u"削除しました";
		}

		if (id == "cyspring_update_mode")
		{
			if (isKor)
			{
				return u"CySpring 업데이트 모드";
			}
			if (isEng)
			{
				return u"CySpring update mode";
			}
			return u"CySpringアップデートモード";
		}

		if (id == "graphics")
		{
			if (isKor)
			{
				return u"그래픽";
			}
			if (isEng)
			{
				return u"Graphics";
			}
			return u"グラフィック";
		}

		if (id == "ui_animation_scale")
		{
			if (isKor)
			{
				return u"UI 애니메이션 배율";
			}
			if (isEng)
			{
				return u"UI animation scale";
			}
			return u"UIアニメーションスケール";
		}

		if (id == "resolution_3d_scale")
		{
			if (isKor)
			{
				return u"3D 렌더링 해상도 배율";
			}
			if (isEng)
			{
				return u"3D rendering resolution scale";
			}
			return u"3Dレンダリングの解像度スケール";
		}

		if (id == "screen")
		{
			if (isKor)
			{
				return u"화면";
			}
			if (isEng)
			{
				return u"Screen";
			}
			return u"画面";
		}

		if (id == "unlock_size")
		{
			if (isKor)
			{
				return u"해상도 고정 해제";
			}
			if (isEng)
			{
				return u"Unlock resolution";
			}
			return u"解像度固定解除";
		}

		if (id == "use_system_resolution")
		{
			if (isKor)
			{
				return u"시스템 해상도 사용";
			}
			if (isEng)
			{
				return u"Use system resolution";
			}
			return u"システムの解像度を使用する";
		}

		if (id == "ui_scale")
		{
			if (isKor)
			{
				return u"UI 스케일";
			}
			if (isEng)
			{
				return u"UI scale";
			}
			return u"UIスケール";
		}

		if (id == "auto_fullscreen")
		{
			if (isKor)
			{
				return u"자동 전체화면";
			}
			if (isEng)
			{
				return u"Auto fullscreen";
			}
			return u"自動全画面";
		}

		if (id == "freeform_window")
		{
			if (isKor)
			{
				return u"자유 형식 창";
			}
			if (isEng)
			{
				return u"Free-form window";
			}
			return u"自由形式ウィンドウ";
		}

		if (id == "ui_scale_portrait")
		{
			if (isKor)
			{
				return u"UI 스케일 (세로)";
			}
			if (isEng)
			{
				return u"UI scale (Vertical)";
			}
			return u"UIスケール(縦)";
		}

		if (id == "ui_scale_landscape")
		{
			if (isKor)
			{
				return u"UI 스케일 (가로)";
			}
			if (isEng)
			{
				return u"UI scale (Horizontal)";
			}
			return u"UIスケール(横)";
		}

		if (id == "use_third_party_news")
		{
			if (isKor)
			{
				return u"타사 공지 사용";
			}
			if (isEng)
			{
				return u"Use of Third-Party notices";
			}
			return u"サードパーティのお知らせの使用";
		}

		if (id == "use_third_party_news_info")
		{
			if (isKor)
			{
				return u"인앱 공지 대신 타사 공지를 사용합니다";
			}
			if (isEng)
			{
				return u"Use third-party notices instead of in-app notices";
			}
			return u"アプリ内お知らせの代わりにサードパーティのお知らせを使用します";
		}

		if (id == "taskbar")
		{
			if (isKor)
			{
				return u"작업 표시줄";
			}
			if (isEng)
			{
				return u"Taskbar";
			}
			return u"タスクバー";
		}

		if (id == "taskbar_show_progress_on_download")
		{
			if (isKor)
			{
				return u"다운로드 진행률 표시";
			}
			if (isEng)
			{
				return u"Show download progress";
			}
			return u"ダウンロードの進行状況を表示";
		}

		if (id == "taskbar_show_progress_on_connecting")
		{
			if (isKor)
			{
				return u"Connecting 진행률 표시";
			}
			if (isEng)
			{
				return u"Show Connecting progress";
			}
			return u"Connectingの進捗状況を表示";
		}

		if (id == "persistent_data_path")
		{
			if (isKor)
			{
				return u"로컬 데이터 경로";
			}
			if (isEng)
			{
				return u"Local data path";
			}
			return u"ローカルデータパス";
		}

		if (id == "experiments")
		{
			return u"Experiments";
		}

		if (id == "initial_disclaimer_1")
		{
			if (isKor)
			{
				return u"본 개선 모드를 사용함으로서 발생하는 이용 제한, 진행 오류 또는\n이외 기타 문제에 대한 책임은 본인에게 있습니다.\n\n계속 진행하려면 <color=#ff911c><i>";
			}
			if (isEng)
			{
				return u"You are responsible for any limitations, errors, or other issues that may arise from using this improvement mod.\n\nPlease check <color=#ff911c><i>";
			}
			return u"本改善モードを使用することによって発生する利用制限、進行エラーまたは\nその他の問題に対する責任は本人にあります。\n\n続行するには、<color=#ff911c><i>";
		}

		if (id == "initial_disclaimer_2")
		{
			if (isKor)
			{
				return u"</i></color> 를 체크해주세요.";
			}
			if (isEng)
			{
				return u"</i></color> to continue.";
			}
			return u"</i></color>をチェックしてください。";
		}

		if (id == "title")
		{
			if (isKor)
			{
				return u"우마무스메";
			}
			if (isEng)
			{
				return u"Umamusume";
			}
			return u"ウマ娘";
		}

		if (id == "unknown")
		{
			if (isKor)
			{
				return u"알수없음";
			}
			if (isEng)
			{
				return u"Unknown";
			}
			return u"不明";
		}

		return u"";
	}
}
