#include "openxr.hpp"

#include <algorithm>

#include "il2cpp/il2cpp-tabledefs.h"
#include "il2cpp/il2cpp-api-functions.hpp"

#include "game.hpp"

#include "string_utils.hpp"

#include "scripts/ScriptInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/BeforeRenderHelper.hpp"
#include "scripts/UnityEngine.XRModule/UnityEngine/XR/InputTracking.hpp"
#include "scripts/UnityEngine.XRModule/UnityEngine/XR/XRNodeState.hpp"

namespace Unity
{
	HMODULE OpenXR::module;

	Il2CppObject* OpenXR::xrDisplaySubsystem;
	Il2CppObject* OpenXR::xrInputSubsystem;

	bool OpenXR::initialized = false;
	bool OpenXR::started = false;

	Unity::OpenXR::ActionMapConfig OpenXR::oculusTouchConfig = ActionMapConfig
	{
		"oculustouchcontroller",
		"Oculus Touch Controller OpenXR",
		"/interaction_profiles/oculus/touch_controller",
		"Oculus",
		"",
		vector<DeviceConfig>
		{
			DeviceConfig
			{
				(InputDeviceCharacteristics::HeldInHand | InputDeviceCharacteristics::TrackedDevice | InputDeviceCharacteristics::Controller | InputDeviceCharacteristics::Left),
				"/user/hand/left"
			},
			DeviceConfig
			{
				(InputDeviceCharacteristics::HeldInHand | InputDeviceCharacteristics::TrackedDevice | InputDeviceCharacteristics::Controller | InputDeviceCharacteristics::Right),
				"/user/hand/right"
			}
		},
		vector<ActionConfig>
		{
		ActionConfig
		{
			"thumbstick",
			"Thumbstick",
			ActionType::Axis2D,
			vector<const char*> { "Primary2DAxis" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/thumbstick",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"grip",
			"Grip",
			ActionType::Axis1D,
			vector<const char*> { "Grip" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/squeeze/value",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"gripPressed",
			"Grip Pressed",
			ActionType::Binary,
			vector<const char*> { "GripButton" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/squeeze/value",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"menu",
			"Menu",
			ActionType::Binary,
			vector<const char*> { "MenuButton" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/menu/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/left" }
				},
				ActionBinding
				{
					"/input/system/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/right" }
				}
			}
		},
		ActionConfig
		{
			"primaryButton",
			"Primary Button",
			ActionType::Binary,
			vector<const char*> { "PrimaryButton" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/x/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/left" }
				},
				ActionBinding
				{
					"/input/a/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/right" }
				}
			}
		},
		ActionConfig
		{
			"primaryTouched",
			"Primary Touched",
			ActionType::Binary,
			vector<const char*> { "PrimaryTouch" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/x/touch",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/left" }
				},
				ActionBinding
				{
					"/input/a/touch",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/right" }
				}
			}
		},
		ActionConfig
		{
			"secondaryButton",
			"Secondary Button",
			ActionType::Binary,
			vector<const char*> { "SecondaryButton" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/y/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/left" }
				},
				ActionBinding
				{
					"/input/b/click",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/right" }
				}
			}
		},
		ActionConfig
		{
			"secondaryTouched",
			"Secondary Touched",
			ActionType::Binary,
			vector<const char*> { "SecondaryTouch" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/y/touch",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/left" }
				},
				ActionBinding
				{
					"/input/b/touch",
					"/interaction_profiles/oculus/touch_controller",
					vector<const char*> { "/user/hand/right" }
				}
			}
		},
		ActionConfig
		{
			"trigger",
			"Trigger",
			ActionType::Axis1D,
			vector<const char*> { "Trigger" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/trigger/value",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"triggerPressed",
			"Trigger Pressed",
			ActionType::Binary,
			vector<const char*> { "TriggerButton" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/trigger/value",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"triggerTouched",
			"Trigger Touched",
			ActionType::Binary,
			vector<const char*> { "TriggerTouch" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/trigger/touch",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"thumbstickClicked",
			"Thumbstick Clicked",
			ActionType::Binary,
			vector<const char*> { "Primary2DAxisClick" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/thumbstick/click",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"thumbstickTouched",
			"Thumbstick Touched",
			ActionType::Binary,
			vector<const char*> { "Primary2DAxisTouch" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/thumbstick/touch",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"thumbrestTouched",
			"Thumbrest Touched",
			ActionType::Binary,
			vector<const char*> { "ThumbrestTouch" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/thumbrest/touch",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"devicePose",
			"Device Pose",
			ActionType::Pose,
			vector<const char*> { "Device" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/grip/pose",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"pointer",
			"Pointer Pose",
			ActionType::Pose,
			vector<const char*> { "Pointer" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/input/aim/pose",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		},
		ActionConfig
		{
			"haptic",
			"Haptic Output",
			ActionType::Vibrate,
			vector<const char*> { "Haptic" },
			vector<ActionBinding>
			{
				ActionBinding
				{
					"/output/haptic",
					"/interaction_profiles/oculus/touch_controller"
				}
			}
		}
	}
	};

	void OpenXR::InitLibrary(IUnityInterfaces* interfaces)
	{
		if (module)
		{
			return;
		}

		auto productName = UnityEngine::Application::productName()->chars;
		SetDllDirectoryW((productName + L"_Data\\Plugins\\x86_64\\"s).data());
		module = LoadLibraryW(L"UnityOpenXR.dll");
		SetDllDirectoryW(nullptr);

		if (module && interfaces)
		{
			reinterpret_cast<decltype(UnityPluginLoad)*>(GetProcAddress(module, "UnityPluginLoad"))(interfaces);
		}
	}

	void OpenXR::UnityPluginLoad(IUnityInterfaces* interfaces)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(UnityPluginLoad)*>(GetProcAddress(module, "UnityPluginLoad"))(interfaces);
	}

	void OpenXR::InitModule(HMODULE module)
	{
		OpenXR::module = module;
	}

	static Il2CppObject* nodeStates;

	static void UpdateHeadPosition()
	{
		//if (!nodeStates)
		//{
		//	// GetRuntimeType("UnityEngine.XRModule.dll", "UnityEngine.XR", "XRNodeState")
		//	auto listClass = GetGenericClass(GetRuntimeType("mscorlib.dll", "System.Collections.Generic", "List`1"), GetRuntimeType("mscorlib.dll", "System", "ValueType"));
		//	il2cpp_runtime_class_init(listClass);
		//	nodeStates = il2cpp_object_new(listClass);
		//	/*cout << "listClass " << listClass << endl;
		//	cout << "listClass " << listClass->name << endl;
		//	cout << "nodeStates " << nodeStates << endl;*/

		//	il2cpp_runtime_object_init(nodeStates);
		//}

		//auto Clear = il2cpp_class_get_method_from_name(nodeStates->klass, "Clear", 0);

		//Il2CppException* exception;
		//il2cpp_runtime_invoke(Clear, nodeStates, nullptr, &exception);

		//UnityEngine::XR::InputTracking::GetNodeStates_Internal(nodeStates);

		//FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(nodeStates->klass, "_items");
		//Il2CppArraySize_t<UnityEngine::XR::XRNodeState>* array;
		//il2cpp_field_get_value(nodeStates, itemsField, &array);

		//if (array)
		//{
		//	cout << "List: " << array->max_length << endl;

		//	for (int i = 0; i < array->max_length; i++)
		//	{
		//		auto state = array->vector[i];
		//	}
		//}
		UnityEngine::Vector3 position;
		UnityEngine::Quaternion rotation;

		UnityEngine::XR::InputTracking::GetLocalPosition_Injected(UnityEngine::XR::XRNode::Head, &position);
		UnityEngine::XR::InputTracking::GetLocalRotation_Injected(UnityEngine::XR::XRNode::Head, &rotation);

		cout << "position " << position.x << " " << position.y << " " << position.z << endl;
		cout << "rotation " << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << endl;
	}

	void OpenXR::Init()
	{
		if (!module)
		{
			return;
		}

		Internal_SetSuccessfullyInitialized(false);

		if (!Internal_LoadOpenXRLibrary(L"openxr_loader"))
		{
			cout << "Fail Internal_LoadOpenXRLibrary" << endl;
			return;
		}

		Feature::Internal_SetProcAddressPtrAndLoadStage1(Feature::Internal_GetProcAddressPtr(true));

		if (!Internal_InitializeSession())
		{
			cout << "Fail Internal_InitializeSession" << endl;
			return;
		}

		auto productName = wide_u8(il2cpp_resolve_icall_type<Il2CppString * (*)()>("UnityEngine.Application::get_productName")()->chars);

		auto version = wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Application", "get_version", IgnoreNumberOfArguments)()->chars);

		auto unityVersion = wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Application", "get_unityVersion", IgnoreNumberOfArguments)()->chars);

		cout << productName << " " << version << " " << unityVersion << endl;

		auto MD5 = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("mscorlib.dll", "System.Security.Cryptography", "MD5", "Create", IgnoreNumberOfArguments)();

		auto versionByteArray = il2cpp_array_new_type<int8_t>(il2cpp_symbols::get_class("mscorlib.dll", "System", "Byte"), version.size());

		for (int i = 0; i < version.size(); i++)
		{
			auto c = version.at(i);
			il2cpp_array_setref(versionByteArray, i, &c);
		}

		auto versionMD5Data = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<int8_t>*(*)(Il2CppObject*, Il2CppArraySize_t<int8_t>*, int, int)>(MD5->klass, "ComputeHash", 3)->methodPointer(MD5, versionByteArray, 0, versionByteArray->max_length);

		auto versionHash = il2cpp_symbols::get_method_pointer<uint32_t(*)(Il2CppArraySize_t<int8_t>*, int)>("mscorlib.dll", "System", "BitConverter", "ToUInt32", 2)(versionMD5Data, 0);

		Internal_SetApplicationInfo(productName.data(), version.data(), versionHash, unityVersion.data());

		// Internal_RequestEnableExtensionString("XR_FB_touch_controller_pro");

		Internal_SetCallbacks(*ReceiveNativeEvent);

		Settings::ApplyRenderSettings();

		auto SubsystemDescriptorStore = il2cpp_symbols::get_class("UnityEngine.SubsystemsModule.dll", "UnityEngine.SubsystemsImplementation", "SubsystemDescriptorStore");

		auto s_IntegratedDescriptorsField = il2cpp_class_get_field_from_name(SubsystemDescriptorStore, "s_IntegratedDescriptors");

		Il2CppObject* s_IntegratedDescriptors;
		il2cpp_field_static_get_value(s_IntegratedDescriptorsField, &s_IntegratedDescriptors);

		cout << "s_IntegratedDescriptors " << s_IntegratedDescriptors << endl;

		if (!s_IntegratedDescriptors)
		{
			return;
		}

		FieldInfo* itemsField = il2cpp_class_get_field_from_name(s_IntegratedDescriptors->klass, "_items");
		Il2CppArraySize_t<Il2CppObject*>* arr;
		il2cpp_field_get_value(s_IntegratedDescriptors, itemsField, &arr);
		cout << "s_IntegratedDescriptors _items " << arr->max_length << endl;

		if (arr)
		{
			for (int i = 0; i < arr->max_length; i++)
			{
				auto descriptor = arr->vector[i];

				if (descriptor)
				{
					auto id = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(descriptor->klass, "get_id", 0)->methodPointer(descriptor);

					auto m_PtrField = il2cpp_class_get_field_from_name(descriptor->klass, "m_Ptr");

					void* subSystemPtr;
					il2cpp_field_get_value(descriptor, m_PtrField, &subSystemPtr);

					auto SubsystemDescriptorBindings_Create = il2cpp_resolve_icall_type<void* (*)(void*)>("UnityEngine.SubsystemDescriptorBindings::Create()");

					auto ptr = SubsystemDescriptorBindings_Create(subSystemPtr);

					auto subsystem = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(void*)>("UnityEngine.SubsystemsModule.dll", "UnityEngine", "SubsystemManager", "GetIntegratedSubsystemByPtr", 1)(ptr);

					wcout << id->chars << endl;

					if (wstring(id->chars).find(L"Display") != wstring::npos)
					{
						xrDisplaySubsystem = subsystem;
					}

					if (wstring(id->chars).find(L"Input") != wstring::npos)
					{
						xrInputSubsystem = subsystem;
					}
				}
			}
		}

		cout << "xrDisplaySubsystem " << xrDisplaySubsystem << endl;
		cout << "xrInputSubsystem " << xrInputSubsystem << endl;

		if (!xrDisplaySubsystem || !xrInputSubsystem)
		{
			cout << "xrDisplaySubsystem or xrInputSubsystem is null" << endl;
			return;
		}

		auto beforeRenderHelper = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "BeforeRenderHelper");
		auto s_OrderBlocksField = il2cpp_class_get_field_from_name(beforeRenderHelper->klass, "s_OrderBlocks");
		Il2CppObject* s_OrderBlocks;
		il2cpp_field_static_get_value(s_OrderBlocksField, &s_OrderBlocks);

		auto orderBlock = UnityEngine::BeforeRenderHelper::OrderBlock{};
		orderBlock.order = -3000;
		orderBlock.callback = &CreateUnityActionStatic(*([]()
			{
				Internal_PumpMessageLoop();
				UpdateHeadPosition();
			}))->delegate;

		auto listAdd = il2cpp_class_get_method_from_name(s_OrderBlocks->klass, "Add", 1);

		void** params = new void* [1];
		params[0] = &orderBlock;

		Il2CppException* exception;

		il2cpp_runtime_invoke_type<void>(listAdd, s_OrderBlocks, params, &exception);

		initialized = true;
	}

	void OpenXR::Start()
	{
		if (!module || !initialized)
		{
			return;
		}

		if (!started)
		{
			if (Internal_CreateSessionIfNeeded())
			{
				auto Start = il2cpp_resolve_icall_type<void (*)(Il2CppObject*)>("UnityEngine.IntegratedSubsystem::Start()");

				Start(xrDisplaySubsystem);

				Internal_BeginSession();

				Input::AttachActionSets();

				Start(xrInputSubsystem);

				started = true;
			}
			else
			{
				cout << "Fail Internal_CreateSessionIfNeeded" << endl;
			}
		}
	}

	void OpenXR::Stop()
	{
		if (!module)
		{
			return;
		}

		if (started)
		{
			auto Stop = il2cpp_resolve_icall_type<void (*)(Il2CppObject*)>("UnityEngine.IntegratedSubsystem::Stop()");
			Stop(xrDisplaySubsystem);
			Stop(xrInputSubsystem);

			Internal_EndSession();
			Internal_PumpMessageLoop();

			started = false;
		}
	}

	void OpenXR::DiagnosticReport::StartReport()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(StartReport)*>(GetProcAddress(module, "DiagnosticReport_StartReport"))();
	}

	void OpenXR::DiagnosticReport::Internal_DumpReport(const char* reason)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_DumpReport)*>(GetProcAddress(module, "DiagnosticReport_DumpReportWithReason"))(reason);
	}

	bool OpenXR::Internal_GetLastError(const char** error)
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_GetLastError)*>(GetProcAddress(module, "session_GetLastError"))(error);
	}

	bool OpenXR::Internal_LoadOpenXRLibrary(const wchar_t* loaderPath)
	{
		if (!module)
		{
			return false;
		}

		if (Game::CurrentGameRegion == Game::Region::JAP)
		{
			return reinterpret_cast<bool (*)(const char* loaderPath)>(GetProcAddress(module, "main_LoadOpenXRLibrary"))(wide_u8(loaderPath).data());
		}

		return reinterpret_cast<decltype(Internal_LoadOpenXRLibrary)*>(GetProcAddress(module, "main_LoadOpenXRLibrary"))(loaderPath);
	}

	void OpenXR::Internal_UnloadOpenXRLibrary()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_UnloadOpenXRLibrary)*>(GetProcAddress(module, "main_UnloadOpenXRLibrary"))();
	}

	void OpenXR::Internal_SetCallbacks(ReceiveNativeEventDelegate callback)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_SetCallbacks)*>(GetProcAddress(module, "NativeConfig_SetCallbacks"))(callback);
	}

	void OpenXR::Internal_SetApplicationInfo(const char* applicationName, const char* applicationVersion, UINT applicationVersionHash, const char* engineVersion)
	{
		if (!module)
		{
			return;
		}

		if (Game::CurrentGameRegion == Game::Region::JAP)
		{
			auto engineVersionString = string(engineVersion);

			auto MD5 = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("mscorlib.dll", "System.Security.Cryptography", "MD5", "Create", IgnoreNumberOfArguments)();

			auto versionByteArray = il2cpp_array_new_type<int8_t>(il2cpp_symbols::get_class("mscorlib.dll", "System", "Byte"), engineVersionString.size());

			for (int i = 0; i < engineVersionString.size(); i++)
			{
				auto c = engineVersionString.at(i);
				il2cpp_array_setref(versionByteArray, i, &c);
			}

			auto versionMD5Data = il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<int8_t>*(*)(Il2CppObject*, Il2CppArraySize_t<int8_t>*, int, int)>(MD5->klass, "ComputeHash", 3)->methodPointer(MD5, versionByteArray, 0, versionByteArray->max_length);

			auto versionHash = il2cpp_symbols::get_method_pointer<uint32_t(*)(Il2CppArraySize_t<int8_t>*, int)>("mscorlib.dll", "System", "BitConverter", "ToUInt32", 2)(versionMD5Data, 0);
			reinterpret_cast<void (*)(const char* applicationName, UINT applicationVersionHash, UINT engineVersionHash)>(GetProcAddress(module, "NativeConfig_SetApplicationInfo"))(applicationName, applicationVersionHash, versionHash);
			return;
		}

		reinterpret_cast<decltype(Internal_SetApplicationInfo)*>(GetProcAddress(module, "NativeConfig_SetApplicationInfo"))(applicationName, applicationVersion, applicationVersionHash, engineVersion);
	}

	bool OpenXR::Internal_InitializeSession()
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_InitializeSession)*>(GetProcAddress(module, "session_InitializeSession"))();
	}

	bool OpenXR::Internal_CreateSessionIfNeeded()
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_CreateSessionIfNeeded)*>(GetProcAddress(module, "session_CreateSessionIfNeeded"))();
	}

	void OpenXR::Internal_BeginSession()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_BeginSession)*>(GetProcAddress(module, "session_BeginSession"))();
	}

	void OpenXR::Internal_EndSession()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_EndSession)*>(GetProcAddress(module, "session_EndSession"))();
	}

	void OpenXR::Internal_DestroySession()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_DestroySession)*>(GetProcAddress(module, "session_DestroySession"))();
	}

	void OpenXR::Internal_PumpMessageLoop()
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_PumpMessageLoop)*>(GetProcAddress(module, "messagepump_PumpMessageLoop"))();
	}

	void OpenXR::Internal_SetSuccessfullyInitialized(bool value)
	{
		if (!module)
		{
			return;
		}

		if (Game::CurrentGameRegion == Game::Region::JAP)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_SetSuccessfullyInitialized)*>(GetProcAddress(module, "session_SetSuccessfullyInitialized"))(value);
	}

	bool OpenXR::Internal_RequestEnableExtensionString(const char* extensionString)
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_RequestEnableExtensionString)*>(GetProcAddress(module, "unity_ext_RequestEnableExtensionString"))(extensionString);
	}

	void OpenXR::ReceiveNativeEvent(Feature::NativeEvent e, ULONG payload)
	{
		cout << "ReceiveNativeEvent " << e << " payload " << payload << endl;
		switch (e)
		{
		case Feature::XrReady:
			Start();
			break;
		case Feature::XrFocused:
			DiagnosticReport::Internal_DumpReport("System Startup Completed");
			break;
		case Feature::XrStopping:
			Stop();
			break;
		case Feature::XrRestartRequested:
			break;
		case Feature::XrRequestRestartLoop:
			break;
		case Feature::XrRequestGetSystemLoop:
			break;
		default:
			break;
		}
	}

	void* OpenXR::Feature::Internal_GetProcAddressPtr(bool loaderDefault)
	{
		if (!module)
		{
			return nullptr;
		}

		return reinterpret_cast<decltype(Internal_GetProcAddressPtr)*>(GetProcAddress(module, "NativeConfig_GetProcAddressPtr"))(loaderDefault);
	}

	void OpenXR::Feature::Internal_SetProcAddressPtrAndLoadStage1(void* func)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_SetProcAddressPtrAndLoadStage1)*>(GetProcAddress(module, "NativeConfig_SetProcAddressPtrAndLoadStage1"))(func);
	}

	void OpenXR::Input::AttachActionSets()
	{
		if (!module)
		{
			return;
		}

		ActionMapConfig actionMap = oculusTouchConfig;
		for (int i = 0; i < actionMap.deviceInfos.size(); i++)
		{
			DeviceConfig deviceInfo = actionMap.deviceInfos[i];
			auto res = Internal_RegisterDeviceDefinition(deviceInfo.userPath, actionMap.desiredInteractionProfile, deviceInfo.characteristics, actionMap.localizedName, actionMap.manufacturer, actionMap.serialNumber);
			if (res == 0)
			{
				const char* error;
				Internal_GetLastError(&error);

				cout << "Internal_RegisterDeviceDefinition Error: " << error << endl;
			}
		}

		GUID guid;
		CoCreateGuid(&guid);

		auto actionSetId = Internal_CreateActionSet(actionMap.name, actionMap.localizedName, guid);
		if (actionSetId == 0)
		{
			const char* error;
			Internal_GetLastError(&error);

			cout << "Internal_CreateActionSet Error: " << error << endl;
		}

		unordered_map<const char*, vector<SerializedBinding>> interactionProfiles;

		vector<ActionConfig> actions = actionMap.actions;
		for (int i = 0; i < actions.size(); i++)
		{
			CoCreateGuid(&guid);

			ActionConfig action = actions[i];
			auto allUserPaths = vector<const char*>{ "/user/hand/left" , "/user/hand/right" };

			string name = action.name;
			transform(name.begin(), name.end(), name.begin(), [](auto c)
				{
					return tolower(c);
				}
			);

			auto actionId = Internal_CreateAction(actionSetId, name.data(), action.localizedName, action.type, guid, allUserPaths.data(), allUserPaths.size(), action.usages.data(), action.usages.size());

			cout << "Action " << name << " " << action.localizedName << " " << actionId << endl;

			if (actionId == 0)
			{
				const char* error;
				Internal_GetLastError(&error);

				cout << "Internal_CreateAction Error: " << error << endl;
				return;
			}

			for (int j = 0; j < action.bindings.size(); j++)
			{
				ActionBinding binding = action.bindings[j];

				for (int k = 0; k < binding.userPaths.size(); k++)
				{
					auto userPath = binding.userPaths[k];

					if (!interactionProfiles.contains(userPath))
					{
						interactionProfiles.emplace(userPath, vector<SerializedBinding>{});
					}

					vector<SerializedBinding> exist = interactionProfiles.at(userPath);

					auto str = userPath + string(binding.interactionPath);
					vector<char> writable(str.begin(), str.end());
					writable.push_back('\0');

					const char* ptr = &writable[0];

					cout << "SerializedBinding interactionProfiles " << ptr << endl;

					exist.emplace_back(SerializedBinding{ actionId, ptr });
				}
			}
		}

		for (auto& [key, value] : interactionProfiles) {
			cout << "Internal_SuggestBindings " << key << endl;
			auto array = il2cpp_array_new_type<SerializedBinding>(il2cpp_symbols::get_class("mscorlib.dll", "System", "ValueType"), value.size());

			int i = 0;
			for (auto& v : value)
			{
				cout << "Internal_SuggestBindings actionId " << v.actionId << endl;
				cout << "Internal_SuggestBindings path " << v.path << endl;
				il2cpp_array_setref(array, i, &v);
				i++;
			}

			bool result = Internal_SuggestBindings(key, array, value.size());
			if (!result)
			{
				const char* error;
				Internal_GetLastError(&error);

				cout << "Internal_SuggestBindings Error: " << error << endl;
				// return;
			}
		}

		bool result = Internal_AttachActionSets();
		if (!result)
		{
			const char* error;
			Internal_GetLastError(&error);

			cout << "Internal_AttachActionSets Error: " << error << endl;
			return;
		}
	}

	void OpenXR::Settings::Internal_SetRenderMode(RenderMode renderMode)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_SetRenderMode)*>(GetProcAddress(module, "NativeConfig_SetRenderMode"))(renderMode);
	}

	void OpenXR::Settings::Internal_SetDepthSubmissionMode(DepthSubmissionMode depthSubmissionMode)
	{
		if (!module)
		{
			return;
		}

		reinterpret_cast<decltype(Internal_SetDepthSubmissionMode)*>(GetProcAddress(module, "NativeConfig_SetDepthSubmissionMode"))(depthSubmissionMode);
	}

	uint64_t OpenXR::Input::Internal_RegisterDeviceDefinition(const char* userPath, const char* interactionProfile, uint32_t characteristics, const char* name, const char* manufacturer, const char* serialNumber)
	{
		if (!module)
		{
			return 0;
		}

		return reinterpret_cast<decltype(Internal_RegisterDeviceDefinition)*>(GetProcAddress(module, "OpenXRInputProvider_RegisterDeviceDefinition"))(userPath, interactionProfile, characteristics, name, manufacturer, serialNumber);
	}

	uint64_t OpenXR::Input::Internal_CreateActionSet(const char* name, const char* localizedName, GUID guid)
	{
		if (!module)
		{
			return 0;
		}

		return reinterpret_cast<decltype(Internal_CreateActionSet)*>(GetProcAddress(module, "OpenXRInputProvider_CreateActionSet"))(name, localizedName, guid);
	}

	uint64_t OpenXR::Input::Internal_CreateAction(uint64_t actionSetId, const char* name, const char* localizedName, uint32_t actionType, GUID guid, const char** userPaths, uint32_t userPathCount, const char** usages, uint32_t usageCount)
	{
		if (!module)
		{
			return 0;
		}

		return reinterpret_cast<decltype(Internal_CreateAction)*>(GetProcAddress(module, "OpenXRInputProvider_CreateAction"))(actionSetId, name, localizedName, actionType, guid, userPaths, userPathCount, usages, usageCount);
	}

	bool OpenXR::Input::Internal_SuggestBindings(const char* interactionProfile, Il2CppArraySize_t<SerializedBinding>* serializedBindings, uint32_t serializedBindingCount)
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_SuggestBindings)*>(GetProcAddress(module, "OpenXRInputProvider_SuggestBindings"))(interactionProfile, serializedBindings, serializedBindingCount);
	}

	bool OpenXR::Input::Internal_AttachActionSets()
	{
		if (!module)
		{
			return false;
		}

		return reinterpret_cast<decltype(Internal_AttachActionSets)*>(GetProcAddress(module, "OpenXRInputProvider_AttachActionSets"))();
	}

	void OpenXR::Settings::ApplyRenderSettings()
	{
		if (!module)
		{
			return;
		}

		OpenXR::Settings::Internal_SetRenderMode(OpenXR::Settings::RenderMode::MultiPass);
		OpenXR::Settings::Internal_SetDepthSubmissionMode(OpenXR::Settings::DepthSubmissionMode::Depth24Bit);
	}

	/*
	[DllImport("UnityOpenXR", EntryPoint = "main_LoadOpenXRLibrary")]
		[return: MarshalAs(UnmanagedType.U1)]
		internal static extern bool Internal_LoadOpenXRLibrary(byte[] loaderPath);

		// Token: 0x06000051 RID: 81
		[DllImport("UnityOpenXR", EntryPoint = "main_UnloadOpenXRLibrary")]
		internal static extern void Internal_UnloadOpenXRLibrary();

		// Token: 0x06000052 RID: 82
		[DllImport("UnityOpenXR", EntryPoint = "NativeConfig_SetCallbacks")]
		private static extern void Internal_SetCallbacks(OpenXRLoaderBase.ReceiveNativeEventDelegate callback);

		// Token: 0x06000053 RID: 83
		[DllImport("UnityOpenXR", CharSet = CharSet.Ansi, EntryPoint = "NativeConfig_SetApplicationInfo")]
		private static extern void Internal_SetApplicationInfo(const char* applicationName, const char* applicationVersion, uint applicationVersionHash, const char* engineVersion);

		// Token: 0x06000054 RID: 84
		[DllImport("UnityOpenXR", EntryPoint = "session_RequestExitSession")]
		internal static extern void Internal_RequestExitSession();

		// Token: 0x06000055 RID: 85
		[DllImport("UnityOpenXR", EntryPoint = "session_InitializeSession")]
		[return: MarshalAs(UnmanagedType.U1)]
		internal static extern bool Internal_InitializeSession();

		// Token: 0x06000056 RID: 86
		[DllImport("UnityOpenXR", EntryPoint = "session_CreateSessionIfNeeded")]
		[return: MarshalAs(UnmanagedType.U1)]
		internal static extern bool Internal_CreateSessionIfNeeded();

		// Token: 0x06000057 RID: 87
		[DllImport("UnityOpenXR", EntryPoint = "session_BeginSession")]
		internal static extern void Internal_BeginSession();

		// Token: 0x06000058 RID: 88
		[DllImport("UnityOpenXR", EntryPoint = "session_EndSession")]
		internal static extern void Internal_EndSession();

		// Token: 0x06000059 RID: 89
		[DllImport("UnityOpenXR", EntryPoint = "session_DestroySession")]
		internal static extern void Internal_DestroySession();

		// Token: 0x0600005A RID: 90
		[DllImport("UnityOpenXR", EntryPoint = "messagepump_PumpMessageLoop")]
		private static extern void Internal_PumpMessageLoop();

		// Token: 0x0600005B RID: 91
		[DllImport("UnityOpenXR", EntryPoint = "session_SetSuccessfullyInitialized")]
		internal static extern void Internal_SetSuccessfullyInitialized(bool value);

		// Token: 0x0600005C RID: 92
		[DllImport("UnityOpenXR", CharSet = CharSet.Ansi, EntryPoint = "unity_ext_RequestEnableExtensionString")]
		[return: MarshalAs(UnmanagedType.U1)]
		internal static extern bool Internal_RequestEnableExtensionString(const char* extensionString);
	*/
}
