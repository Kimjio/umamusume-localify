#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"
#include "BGManager.hpp"
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

		BGManager _bgManager();

		Il2CppObject* _uiToFrameBufferRenderCameraData();

		Il2CppObject* _uiCommandBuffer();

		Il2CppObject* _blitToFrameMaterial();

		Il2CppObject* _noImageEffectUICamera();

		Il2CppObject* _noImageEffectCanvas();

		Il2CppObject* _uiToFrameBufferBlitCamera();

		Il2CppObject* _mainCanvas();

		Il2CppObject* _gameCanvas();

		Il2CppObject* _systemCanvas();

		Il2CppObject* UITexture();

		Il2CppObject* CommonHeaderTitle();

#ifdef _MSC_VER
		Il2CppObject* LandscapeUIManager();
#endif

		void UITexture(Il2CppObject* value);

		void ShowNotification(Il2CppString* text);

		void SetCameraSizeByOrientation(ScreenOrientation orientation);

		void CheckUIToFrameBufferBlitInstance();

		void SetupSafeArea();

		void AdjustSafeArea();

        static void UpdateCanvasScaler(Il2CppObject* canvasScaler);

		void AdjustMissionClearContentsRootRect();

		void AdjustSafeAreaToAnnounceRect();

		void SetBgCameraRenderTexture(Il2CppObject* texture);

		void CreateRenderTextureFromScreen();

		void ReleaseRenderTexture();

		bool IsLockGameCanvas();

		void LockGameCanvas();

		void UnlockGameCanvas();

		void UnlockAllCanvas();

		void ChangeResolution();

		Il2CppObject* WaitResizeUI(bool isPortrait, bool isShowOrientationGuide = false);

		Il2CppArraySize_t<Il2CppObject*>* GetCanvasScalerList();

		static float GetCameraSizeByOrientation(ScreenOrientation orientation);

		static Vector2 DefaultResolution();

		static bool IsLandscapeMode();

#ifdef _MSC_VER
		void ChangeResizeUIForPC(int width, int height);
#endif

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
