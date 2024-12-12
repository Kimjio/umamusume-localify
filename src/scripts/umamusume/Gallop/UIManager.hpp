#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ScreenOrientation.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"

using namespace UnityEngine;

namespace Gallop
{
	class UIManager : public MonoSingleton<UIManager>
	{
	public:
		Il2CppObject* _noticeCanvas();

		Il2CppObject* _bgCamera();

		Il2CppObject* _uiCamera();

		Il2CppObject* _bgManager();

		Il2CppObject* _uiToFrameBufferRenderCameraData();

		Il2CppObject* _uiCommandBuffer();

		Il2CppObject* _blitToFrameMaterial();

		Il2CppObject* _noImageEffectUICamera();

		Il2CppObject* _noImageEffectCanvas();

		Il2CppObject* _uiToFrameBufferBlitCamera();

		Il2CppObject* _mainCanvas();

		Il2CppObject* UITexture();

		Il2CppObject* CommonHeaderTitle();

		void UITexture(Il2CppObject* value);

		void ShowNotification(Il2CppString*);

		void SetCameraSizeByOrientation(ScreenOrientation orientation);

		void CheckUIToFrameBufferBlitInstance();

		void AdjustSafeArea();

		void AdjustMissionClearContentsRootRect();

		void AdjustSafeAreaToAnnounceRect();

		void ReleaseRenderTexture();

		void LockGameCanvas();

		void UnlockGameCanvas();

		void UnlockAllCanvas();

		Il2CppObject* WaitResizeUI(bool isPortrait, bool isShowOrientationGuide = false);

		Il2CppArraySize_t<Il2CppObject*>* GetCanvasScalerList();

		static float GetCameraSizeByOrientation(ScreenOrientation orientation);

		static Vector2 DefaultResolution();

#ifdef _MSC_VER
		void ChangeResizeUIForPC(int width, int height);
#endif

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
