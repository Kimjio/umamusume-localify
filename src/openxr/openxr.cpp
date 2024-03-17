#include "openxr.hpp"

#include <algorithm>

#include "il2cpp/il2cpp-tabledefs.h"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "local/local.hpp"

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

	template<typename... T, typename R>
	Il2CppDelegate* CreateUnityActionStatic(R(*fn)(void*, T...))
	{
		auto delegate = reinterpret_cast<MulticastDelegate*>(
			il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction")));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);

		const auto methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_symbols::get_class("mscorlib.dll", "System.Reflection", "MonoMethod");
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		delegate->method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		return delegate;
	}

	template<typename R>
	Il2CppDelegate* CreateUnityActionStatic(R(*fn)())
	{
		return CreateUnityActionStatic(reinterpret_cast<R(*)(void*)>(fn));
	}

	void OpenXR::InitLibrary(IUnityInterfaces* interfaces)
	{
		auto productName = il2cpp_resolve_icall_type<Il2CppString * (*)()>("UnityEngine.Application::get_productName")()->start_char;
		module = LoadLibraryW((productName + L"_Data\\Plugins\\x86_64\\UnityOpenXR.dll"s).data());

		if (module)
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
		}

		Feature::Internal_SetProcAddressPtrAndLoadStage1(Feature::Internal_GetProcAddressPtr(true));

		if (!Internal_InitializeSession())
		{
			cout << "Fail Internal_InitializeSession" << endl;
			return;
		}

		auto productName = local::wide_u8(il2cpp_resolve_icall_type<Il2CppString * (*)()>("UnityEngine.Application::get_productName")()->start_char);

		auto version = local::wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Application", "get_version", -1)()->start_char);

		auto unityVersion = local::wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>(
			"UnityEngine.CoreModule.dll", "UnityEngine",
			"Application", "get_unityVersion", -1)()->start_char);

		cout << productName << " " << version << " " << unityVersion << endl;

		auto MD5 = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("mscorlib.dll", "System.Security.Cryptography", "MD5", "Create", -1)();

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

					wcout << id << endl;

					if (wstring(id->start_char).find(L"Display"))
					{
						xrDisplaySubsystem = subsystem;
					}

					if (wstring(id->start_char).find(L"Input"))
					{
						xrInputSubsystem = subsystem;
					}
				}
			}
		}

		if (!xrDisplaySubsystem || !xrInputSubsystem)
		{
			cout << "xrDisplaySubsystem or xrInputSubsystem is null" << endl;
			return;
		}

		auto beforeRenderHelper = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "BeforeRenderHelper");
		auto s_OrderBlocksField = il2cpp_class_get_field_from_name(beforeRenderHelper->klass, "s_OrderBlocks");
		Il2CppObject* s_OrderBlocks;
		il2cpp_field_static_get_value(s_OrderBlocksField, &s_OrderBlocks);

		auto itemField = il2cpp_class_get_field_from_name(s_OrderBlocks->klass, "_items");

		Il2CppArraySize_t<OrderBlock>* array1;
		il2cpp_field_get_value(s_OrderBlocks, itemField, &array1);

		auto orderClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "BeforeRenderHelper/OrderBlock");

		auto array2 = il2cpp_array_new(orderClass, array1->max_length + 1);

		for (int i = 0; i < array1->max_length; i++) {
			il2cpp_array_setref(array2, i, &array1->vector[i]);
		}

		auto orderBlock = reinterpret_cast<OrderBlock*>(il2cpp_object_new(orderClass));
		orderBlock->order = 0;
		orderBlock->callback = CreateUnityActionStatic(*([]()
			{
				cout << "BeforeRender" << endl;
				Internal_PumpMessageLoop();
			}));

		il2cpp_array_setref(array2, array1->max_length, orderBlock);

		il2cpp_field_set_value(s_OrderBlocks, itemField, array2);

		auto sizeField = il2cpp_class_get_field_from_name(s_OrderBlocks->klass, "_size");
		int size = array2->max_length;
		il2cpp_field_set_value(s_OrderBlocks, sizeField, &size);

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

				Start(xrDisplaySubsystem);
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

		auto actionMap = oculusTouchConfig;
		for (int i = 0; i < actionMap.deviceInfos.size(); i++)
		{
			auto deviceInfo = actionMap.deviceInfos[i];
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

		unordered_map<const char*, vector<SerializedBinding*>> interactionProfiles;

		auto& actions = actionMap.actions;
		for (int i = 0; i < actions.size(); i++)
		{
			CoCreateGuid(&guid);

			auto action = actions[i];
			auto allUserPaths = vector<const char*>{ "/user/hand/left" , "/user/hand/right" };

			string name = action.name;
			transform(name.begin(), name.end(), name.begin(), tolower);

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
				auto binding = action.bindings[j];

				for (int k = 0; k < binding.userPaths.size(); k++)
				{
					auto userPath = binding.userPaths[k];

					if (!interactionProfiles.contains(userPath))
					{
						interactionProfiles.emplace(userPath, vector<SerializedBinding*>{});
					}

					auto& exist = interactionProfiles.at(userPath);

					auto str = userPath + string(binding.interactionPath);
					vector<char> writable(str.begin(), str.end());
					writable.push_back('\0');

					const char* ptr = &writable[0];

					cout << "SerializedBinding interactionProfiles " << ptr << endl;

					exist.emplace_back(new SerializedBinding{ actionId, ptr });
				}
			}
		}

		for (auto& [key, value] : interactionProfiles) {
			cout << "Internal_SuggestBindings " << key << endl;
			for (auto& v : value)
			{
				cout << "Internal_SuggestBindings actionId " << v->actionId << endl;
				cout << "Internal_SuggestBindings path " << v->path << endl;
			}

			bool result = Internal_SuggestBindings(key, value.data(), value.size());
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

	bool OpenXR::Input::Internal_SuggestBindings(const char* interactionProfile, SerializedBinding** serializedBindings, uint32_t serializedBindingCount)
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
