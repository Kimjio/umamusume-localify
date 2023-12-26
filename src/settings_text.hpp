#pragma once

#include <string>

#include "game.hpp"

using namespace std;

namespace LocalifySettings
{
	const char* GetText(string id)
	{
		bool isKor = Game::CurrentGameRegion == Game::Region::KOR;

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

		if (id == "character_system_text_caption")
		{
			return isKor ? "캐릭터 대사 자막" : "キャラクターのダイアログテキストを\nキャプションで表示";
		}

		if (id == "show_caption")
		{
			return isKor ? "자막 표시" : "字幕表示";
		}

		if (id == "sample_caption")
		{
			return isKor ? "샘플 자막" : "サンプル字幕";
		}

		if (id == "champions_live_show_text")
		{
			return isKor ? "Ms. VICTORIA 챔피언스 미팅 순위 표시" : "Ms. VICTORIAチャンピオンズミーティングの\n順位表示";
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

		return "";
	}
}
