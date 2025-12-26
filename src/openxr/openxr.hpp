#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <Windows.h>
#include <guiddef.h>

#include "unity/IUnityInterface.h"
#include "il2cpp/il2cpp_symbols.hpp"

using namespace std;

namespace Unity
{
	class OpenXR
	{

	public:
		class DiagnosticReport
		{
		public:
			static void StartReport();

			static void Internal_DumpReport(const char* reason);
		};

		enum LoaderState
		{
			Uninitialized,
			InitializeAttempted,
			Initialized,
			StartAttempted,
			Started,
			StopAttempted,
			Stopped,
			DeinitializeAttempted
		};

		class Feature
		{
		public:
			enum LoaderEvent
			{
				SubsystemCreate,
				SubsystemDestroy,
				SubsystemStart,
				SubsystemStop
			};

			enum NativeEvent
			{
				XrSetupConfigValues,
				XrSystemIdChanged,
				XrInstanceChanged,
				XrSessionChanged,
				XrBeginSession,
				XrSessionStateChanged,
				XrChangedSpaceApp,
				XrEndSession,
				XrDestroySession,
				XrDestroyInstance,
				XrIdle,
				XrReady,
				XrSynchronized,
				XrVisible,
				XrFocused,
				XrStopping,
				XrExiting,
				XrLossPending,
				XrInstanceLossPending,
				XrRestartRequested,
				XrRequestRestartLoop,
				XrRequestGetSystemLoop
			};

			static void* Internal_GetProcAddressPtr(bool loaderDefault);

			static void Internal_SetProcAddressPtrAndLoadStage1(void* func);
		};

		enum InputDeviceCharacteristics
		{
			None = 0U,
			HeadMounted = 1U,
			Camera = 2U,
			HeldInHand = 4U,
			HandTracking = 8U,
			EyeTracking = 16U,
			TrackedDevice = 32U,
			Controller = 64U,
			TrackingReference = 128U,
			Left = 256U,
			Right = 512U,
			Simulated6DOF = 1024U
		};

		enum ActionType
		{
			Binary,
			Axis1D,
			Axis2D,
			Pose,
			Vibrate,
			Count
		};

		class ActionBinding
		{
		public:
			ActionBinding(const char* interactionProfileName,
				const char* interactionPath,
				vector<const char*> userPaths = vector<const char*>{})
			{
				this->interactionProfileName = interactionProfileName;
				this->interactionPath = interactionPath;
				this->userPaths = userPaths;
			}

			const char* interactionProfileName;
			const char* interactionPath;
			vector<const char*> userPaths;
		};

		class ActionConfig
		{
		public:
			ActionConfig(const char* name,
				const char* localizedName,
				ActionType type,
				vector<const char*> usages = vector<const char*>{},
				vector<ActionBinding> bindings = vector<ActionBinding>{})
			{
				this->name = name;
				this->localizedName = localizedName;
				this->type = type;
				this->bindings = bindings;
				this->usages = usages;
			}

			const char* name;
			const char* localizedName;
			ActionType type;
			vector<const char*> usages;
			vector<ActionBinding> bindings;
		};

		class DeviceConfig
		{
		public:
			DeviceConfig(uint32_t characteristics,
				const char* userPath)
			{
				this->characteristics = characteristics;
				this->userPath = userPath;
			}

			uint32_t characteristics;
			const char* userPath;
		};

		class ActionMapConfig
		{
		public:
			ActionMapConfig(const char* name,
				const char* localizedName,
				const char* desiredInteractionProfile,
				const char* manufacturer,
				const char* serialNumber,
				vector<DeviceConfig> deviceInfos,
				vector<ActionConfig> actions)
			{
				this->name = name;
				this->localizedName = localizedName;
				this->deviceInfos = deviceInfos;
				this->actions = actions;
				this->desiredInteractionProfile = desiredInteractionProfile;
				this->manufacturer = manufacturer;
				this->serialNumber = serialNumber;
			}

			const char* name;
			const char* localizedName;
			vector<DeviceConfig> deviceInfos;
			vector<ActionConfig> actions;
			const char* desiredInteractionProfile;
			const char* manufacturer;
			const char* serialNumber;
		};

		static ActionMapConfig oculusTouchConfig;

		class Input
		{
		public:
			struct SerializedBinding
			{
				uint64_t actionId;
				const char* path;
			};

			static uint64_t Internal_RegisterDeviceDefinition(const char* userPath, const char* interactionProfile, uint32_t characteristics, const char* name, const char* manufacturer, const char* serialNumber);

			static uint64_t Internal_CreateActionSet(const char* name, const char* localizedName, GUID guid);

			static uint64_t Internal_CreateAction(uint64_t actionSetId, const char* name, const char* localizedName, uint32_t actionType, GUID guid, const char** userPaths, uint32_t userPathCount, const char** usages, uint32_t usageCount);

			static bool Internal_SuggestBindings(const char* interactionProfile, Il2CppArraySize_t<SerializedBinding>* serializedBindings, uint32_t serializedBindingCount);

			static bool Internal_AttachActionSets();

			static void AttachActionSets();
		};

		class Settings
		{
		public:
			enum RenderMode
			{
				MultiPass,
				SinglePassInstanced
			};

			enum DepthSubmissionMode
			{
				None,
				Depth16Bit,
				Depth24Bit
			};

			static void ApplyRenderSettings();

		private:
			static void Internal_SetRenderMode(RenderMode renderMode);

			static void Internal_SetDepthSubmissionMode(DepthSubmissionMode depthSubmissionMode);
		};

		typedef void (*ReceiveNativeEventDelegate)(Unity::OpenXR::Feature::NativeEvent, ULONG payload);

	public:
		static bool initialized;

		static bool started;

		static void InitLibrary(IUnityInterfaces* interfaces);

		static void UnityPluginLoad(IUnityInterfaces* interfaces);

		static void InitModule(HMODULE module);

		static void Init();

		static void Start();

		static void Stop();

		static bool Internal_GetLastError(const char** error);

		static bool Internal_LoadOpenXRLibrary(const wchar_t* loaderPath);

		static void Internal_UnloadOpenXRLibrary();

		static void Internal_SetCallbacks(ReceiveNativeEventDelegate callback);

		static void Internal_SetApplicationInfo(const char* applicationName, const char* applicationVersion, UINT applicationVersionHash, const char* engineVersion);

		static bool Internal_InitializeSession();

		static bool Internal_CreateSessionIfNeeded();

		static void Internal_BeginSession();

		static void Internal_EndSession();

		static void Internal_DestroySession();

		static void Internal_PumpMessageLoop();

		static bool Internal_RequestEnableExtensionString(const char* extensionString);

		static void ReceiveNativeEvent(Unity::OpenXR::Feature::NativeEvent, ULONG payload);

	private:
		static HMODULE module;
		static Il2CppObject* xrDisplaySubsystem;
		static Il2CppObject* xrInputSubsystem;
	};
}
