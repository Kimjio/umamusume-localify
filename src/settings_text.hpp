#pragma once

#include <string>

#include "game.hpp"

using namespace std;

extern bool g_localify_settings_force_korean;

namespace LocalifySettings
{
	const char* GetText(string id)
	{
		bool isKor = Game::CurrentGameRegion == Game::Region::KOR || g_localify_settings_force_korean;

		if (id == "title")
		{
			return isKor ? "Localify 설정" : "Localify設定";
		}

		if (id == "open_settings")
		{
			return isKor ? "설정 열기" : "設定を開く";
		}

		if (id == "setting_disabled")
		{
			return isKor ? "설정이 활성화 되지 않음" : "設定が有効になっていません";
		}

		if (id == "applied_after_restart")
		{
			return isKor ? "<color=#ff911c>변경 사항은 게임 재실행 후 반영됩니다</color>" : "<color=#ff911c>変更はゲームの再実行後に反映されます</color>";
		}

		if (id == "anti_aliasing")
		{
			return isKor ? "안티 엘리어싱" : "アンチエイリアシング";
		}

		if (id == "graphics_quality")
		{
			return isKor ? "그래픽 품질" : "グラフィックの品質";
		}

		if (id == "character_system_text_caption")
		{
			return isKor ? "캐릭터 대사 자막" : "キャラクターのダイアログテキストを\nキャプションで表示";
		}

		if (id == "character_system_text_caption_position_x")
		{
			return isKor ? "X좌표" : "X座標";
		}

		if (id == "character_system_text_caption_position_y")
		{
			return isKor ? "Y좌표" : "Y座標";
		}

		if (id == "character_system_text_caption_background_alpha")
		{
			return isKor ? "배경 투명도" : "背景透明度";
		}

		if (id == "character_system_text_caption_font_color")
		{
			return isKor ? "폰트 색상" : "";
		}

		if (id == "character_system_text_caption_outline_size")
		{
			return isKor ? "외곽선 크기" : "";
		}

		if (id == "character_system_text_caption_outline_color")
		{
			return isKor ? "외곽선 색상" : "";
		}

		if (id == "show_caption")
		{
			return isKor ? "자막 표시" : "字幕表示";
		}

		if (id == "show_notification")
		{
			return isKor ? "알림 표시" : "通知表示";
		}

		if (id == "sample_caption")
		{
			return isKor ? "샘플 자막" : "サンプル字幕";
		}

		if (id == "champions_live_show_text")
		{
			return isKor ? "Ms. VICTORIA 챔피언스 미팅 순위 표시" : "Ms. VICTORIAチャンピオンズミーティングの\n順位表示";
		}

		if (id == "champions_live_resource_id")
		{
			return isKor ? "챔피언스 미팅 일정" : "チャンピオンズミーティングスケジュール";
		}

		if (id == "champions_live_year")
		{
			return isKor ? "챔피언스 미팅 년도" : "チャンピオンズミーティングの年";
		}

		if (id == "dump_msgpack")
		{
			return isKor ? "MessagePack 덤프" : "MessagePackダンプ";
		}

		if (id == "dump_msgpack_request")
		{
			return isKor ? "MessagePack 요청 덤프" : "MessagePackリクエストダンプ";
		}

		if (id == "unlock_live_chara")
		{
			return isKor ? "라이브 시어터 전체 해방" : "ライブシアター全体解放";
		}

		if (id == "unlock_live_chara_info")
		{
			return isKor ? "미해금/미해방/미공개 곡 및 우마무스메와 드레스를 사용 가능하게 변경합니다." : "未解禁/未解放/未公開曲及びウマ娘とドレスを使えるように変更します。";
		}

		if (id == "allow_delete_cookie")
		{
			return isKor ? "WebView Cookie 삭제 허용" : "WebView Cookieの削除を許可する";
		}

		if (id == "clear_webview_cache")
		{
			return isKor ? "WebView 캐시 삭제" : "WebViewキャッシュの削除";
		}

		if (id == "clear_webview_cache_confirm")
		{
			return isKor ? "WebView 캐시를 삭제하시겠습니까?" : "WebViewキャッシュを削除しますか？";
		}

		if (id == "deleted")
		{
			return isKor ? "삭제했습니다" : "削除しました";
		}

		if (id == "cyspring_update_mode")
		{
			return isKor ? "CySpring 업데이트 모드" : "CySpringアップデートモード";
		}

		if (id == "graphics")
		{
			return isKor ? "그래픽" : "グラフィック";
		}

		if (id == "ui_animation_scale")
		{
			return isKor ? "UI 애니메이션 배율" : "UIアニメーションスケール";
		}

		if (id == "resolution_3d_scale")
		{
			return isKor ? "3D 렌더링 해상도 배율" : "3Dレンダリングの解像度スケール";
		}

		if (id == "screen")
		{
			return isKor ? "화면" : "画面";
		}

		if (id == "unlock_size")
		{
			return isKor ? "해상도 고정 해제" : "解像度固定解除";
		}

		if (id == "use_system_resolution")
		{
			return isKor ? "시스템 해상도 사용" : "システムの解像度を使用する";
		}

		if (id == "ui_scale")
		{
			return isKor ? "UI 스케일" : "UIスケール";
		}

		if (id == "auto_fullscreen")
		{
			return isKor ? "자동 전체화면" : "自動全画面";
		}

		if (id == "freeform_window")
		{
			return isKor ? "자유 형식 창" : "自由形式ウィンドウ";
		}

		if (id == "ui_scale_portrait")
		{
			return isKor ? "UI 스케일 (세로)" : "UIスケール(縦)";
		}

		if (id == "ui_scale_landscape")
		{
			return isKor ? "UI 스케일 (가로)" : "UIスケール(横)";
		}

		if (id == "experiments")
		{
			return "Experiments";
		}

		if (id == "initial_disclaimer_1")
		{
			return isKor ? "본 개선 모드를 사용함으로서 발생하는 이용 제한, 진행 오류 또는\n이외 기타 문제에 대한 책임은 본인에게 있습니다.\n\n계속 진행하려면 <color=#ff911c><i>" : "本改善モードを使用することによって発生する利用制限、進行エラーまたは\nその他の問題に対する責任は本人にあります。\n\n続行するには、<color=#ff911c><i>";
		}

		if (id == "initial_disclaimer_2")
		{
			return isKor ? "</i></color> 를 체크해주세요." : "</i></color>をチェックしてください。";
		}

		return "";
	}
}
