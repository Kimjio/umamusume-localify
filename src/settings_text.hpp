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

		if (id == "settings_title")
		{
			return isKor ? L"Localify 설정" : L"Localify設定";
		}

		if (id == "open_settings")
		{
			return isKor ? L"설정 열기" : L"設定を開く";
		}

		if (id == "setting_disabled")
		{
			return isKor ? L"설정이 활성화 되지 않음" : L"設定が有効になっていません";
		}

		if (id == "applied_after_restart")
		{
			return isKor ? L"<color=#ff911c>변경 사항은 게임 재실행 후 반영됩니다</color>" : L"<color=#ff911c>変更はゲームの再実行後に反映されます</color>";
		}

		if (id == "anti_aliasing")
		{
			return isKor ? L"안티 엘리어싱" : L"アンチエイリアシング";
		}

		if (id == "graphics_quality")
		{
			return isKor ? L"그래픽 품질" : L"グラフィックの品質";
		}

		if (id == "character_system_text_caption")
		{
			return isKor ? L"캐릭터 대사 자막" : L"キャラクターのダイアログテキストを\nキャプションで表示";
		}

		if (id == "character_system_text_caption_position_x")
		{
			return isKor ? L"X좌표" : L"X座標";
		}

		if (id == "character_system_text_caption_position_y")
		{
			return isKor ? L"Y좌표" : L"Y座標";
		}

		if (id == "character_system_text_caption_background_alpha")
		{
			return isKor ? L"배경 투명도" : L"背景透明度";
		}

		if (id == "character_system_text_caption_font_color")
		{
			return isKor ? L"폰트 색상" : L"フォントの色";
		}

		if (id == "character_system_text_caption_outline_size")
		{
			return isKor ? L"외곽선 크기" : L"アウトラインサイズ";
		}

		if (id == "character_system_text_caption_outline_color")
		{
			return isKor ? L"외곽선 색상" : L"アウトラインの色";
		}

		if (id == "show_caption")
		{
			return isKor ? L"자막 표시" : L"字幕表示";
		}

		if (id == "show_notification")
		{
			return isKor ? L"알림 표시" : L"通知表示";
		}

		if (id == "sample_caption")
		{
			return isKor ? L"샘플 자막" : L"サンプル字幕";
		}

		if (id == "champions_live_show_text")
		{
			return isKor ? L"Ms. VICTORIA 챔피언스 미팅 순위 표시" : L"Ms. VICTORIAチャンピオンズミーティングの\n順位表示";
		}

		if (id == "champions_live_resource_id")
		{
			return isKor ? L"챔피언스 미팅 일정" : L"チャンピオンズミーティングスケジュール";
		}

		if (id == "champions_live_year")
		{
			return isKor ? L"챔피언스 미팅 년도" : L"チャンピオンズミーティングの年";
		}

		if (id == "dump_msgpack")
		{
			return isKor ? L"MessagePack 덤프" : L"MessagePackダンプ";
		}

		if (id == "dump_msgpack_request")
		{
			return isKor ? L"MessagePack 요청 덤프" : L"MessagePackリクエストダンプ";
		}

		if (id == "unlock_live_chara")
		{
			return isKor ? L"라이브 시어터 전체 해방" : L"ライブシアター全体解放";
		}

		if (id == "live_slider_always_show")
		{
			return isKor ? L"재생 슬라이더 항상 표시" : L"プレイスライダーを常に表示";
		}

		if (id == "live_playback_loop")
		{
			return isKor ? L"반복 재생" : L"ループプレイ";
		}

		if (id == "unlock_live_chara_info")
		{
			return isKor ? L"미해금/미해방/미공개 곡 및 우마무스메와 드레스를 사용 가능하게 변경합니다." : L"未解禁/未解放/未公開曲及びウマ娘とドレスを使えるように変更します。";
		}

		if (id == "allow_delete_cookie")
		{
			return isKor ? L"WebView Cookie 삭제 허용" : L"WebView Cookieの削除を許可する";
		}

		if (id == "clear_webview_cache")
		{
			return isKor ? L"WebView 캐시 삭제" : L"WebViewキャッシュの削除";
		}

		if (id == "clear_webview_cache_confirm")
		{
			return isKor ? L"WebView 캐시를 삭제하시겠습니까?" : L"WebViewキャッシュを削除しますか？";
		}

		if (id == "deleted")
		{
			return isKor ? L"삭제했습니다" : L"削除しました";
		}

		if (id == "cyspring_update_mode")
		{
			return isKor ? L"CySpring 업데이트 모드" : L"CySpringアップデートモード";
		}

		if (id == "graphics")
		{
			return isKor ? L"그래픽" : L"グラフィック";
		}

		if (id == "ui_animation_scale")
		{
			return isKor ? L"UI 애니메이션 배율" : L"UIアニメーションスケール";
		}

		if (id == "resolution_3d_scale")
		{
			return isKor ? L"3D 렌더링 해상도 배율" : L"3Dレンダリングの解像度スケール";
		}

		if (id == "screen")
		{
			return isKor ? L"화면" : L"画面";
		}

		if (id == "unlock_size")
		{
			return isKor ? L"해상도 고정 해제" : L"解像度固定解除";
		}

		if (id == "use_system_resolution")
		{
			return isKor ? L"시스템 해상도 사용" : L"システムの解像度を使用する";
		}

		if (id == "ui_scale")
		{
			return isKor ? L"UI 스케일" : L"UIスケール";
		}

		if (id == "auto_fullscreen")
		{
			return isKor ? L"자동 전체화면" : L"自動全画面";
		}

		if (id == "freeform_window")
		{
			return isKor ? L"자유 형식 창" : L"自由形式ウィンドウ";
		}

		if (id == "ui_scale_portrait")
		{
			return isKor ? L"UI 스케일 (세로)" : L"UIスケール(縦)";
		}

		if (id == "ui_scale_landscape")
		{
			return isKor ? L"UI 스케일 (가로)" : L"UIスケール(横)";
		}

		if (id == "use_third_party_news")
		{
			return isKor ? L"타사 공지 사용" : L"サードパーティのお知らせの使用";
		}

		if (id == "use_third_party_news_info")
		{
			return isKor ? L"인앱 공지 대신 타사 공지를 사용합니다" : L"アプリ内お知らせの代わりにサードパーティのお知らせを使用します";
		}

		if (id == "experiments")
		{
			return L"Experiments";
		}

		if (id == "initial_disclaimer_1")
		{
			return isKor ? L"본 개선 모드를 사용함으로서 발생하는 이용 제한, 진행 오류 또는\n이외 기타 문제에 대한 책임은 본인에게 있습니다.\n\n계속 진행하려면 <color=#ff911c><i>" : L"本改善モードを使用することによって発生する利用制限、進行エラーまたは\nその他の問題に対する責任は本人にあります。\n\n続行するには、<color=#ff911c><i>";
		}

		if (id == "initial_disclaimer_2")
		{
			return isKor ? L"</i></color> 를 체크해주세요." : L"</i></color>をチェックしてください。";
		}

		if (id == "title")
		{
			return isKor ? L"우마무스메" : L"ウマ娘";
		}

		return L"";
	}
}
