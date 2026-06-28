#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "MonoBehaviour.hpp"
#include "Color.hpp"
#include "Coroutine.hpp"
#include "Screen.hpp"

#include <thread>

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"
#include "scripts/umamusume/Gallop/GameSystem.hpp"
#include "scripts/umamusume/Gallop/Screen.hpp"
#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"
#include "scripts/umamusume/Gallop/DialogCommon.hpp"
#include "scripts/umamusume/Gallop/DialogManager.hpp"
#include "scripts/umamusume/Gallop/TextCommon.hpp"
#include "scripts/umamusume/Gallop/TextFontManager.hpp"
#include "scripts/umamusume/Gallop/Localize.hpp"

#include "config/config.hpp"

#include "string_utils.hpp"

namespace
{
	Il2CppMethodPointer StartCoroutineManaged2_addr = nullptr;

	Il2CppMethodPointer StopCoroutineManaged_addr = nullptr;

	static Il2CppObject* BootSystem;
	static bool font_asset_loaded = false;
	static bool replace_assetbundle_file_path_loaded = false;
	static bool replace_atlas_assetbundle_file_path_loaded = false;
	static bool replace_assetbundle_file_paths_loaded = false;
}

static void LoadAssets()
{
	auto dialogCommon = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "DialogManager", "GetForeFrontDialog", IgnoreNumberOfArguments)();
	auto _currentDialogObjField = il2cpp_class_get_field_from_name(dialogCommon->klass, "_currentDialogObj");
	Il2CppObject* _currentDialogObj;
	il2cpp_field_get_value(dialogCommon, _currentDialogObjField, &_currentDialogObj);

	auto text = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObj->klass, "get_Text", 0)(_currentDialogObj);
#pragma region LOAD_ASSETBUNDLE
	if (!config::runtime::fontAssets && !config::font_assetbundle_path.empty() && config::replace_to_custom_font && !font_asset_loaded)
	{
		il2cppstring assetbundlePath = config::font_assetbundle_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
#ifdef _MSC_VER
			assetbundlePath.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
#else
			assetbundlePath.insert(0, u"/sdcard/Android/data/"s.append(u8_il2cpp(Game::GetCurrentPackageName()))).append(u"/");
#endif
		}

		if (filesystem::exists(assetbundlePath))
		{
#ifdef _MSC_VER
			il2cppstringstream ss;
			ss << IL2CPP_STRING("Loading font asset: ") << assetbundlePath << IL2CPP_STRING("... ");
			wcout << ss.str();
			Gallop::TextCommon(text).text(il2cpp_string_new16(ss.str().data()));
#else
			stringstream ss;
			ss << "Loading font asset: " << il2cpp_u8(assetbundlePath) << "... ";
			LOGI("%s", ss.str().data());
			Gallop::TextCommon(text).text(il2cpp_string_new(ss.str().data()));
#endif

			std::thread([text, assetbundlePath]()
				{
#ifdef _MSC_VER
					Sleep(100);
#else
					usleep(100 * 1000);
#endif

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					config::runtime::fontAssets = Gallop::TextFontManager::GetReplacementFontAssetBundle(assetbundlePath);
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(text->klass, "FontTextureChanged", 0)(text);

					if (!config::runtime::fontAssets)
					{
						if (filesystem::exists(assetbundlePath))
						{
#ifdef _MSC_VER
							wcout << endl << L"Asset founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
#else
							LOGW("Asset founded but not loaded. Maybe Asset BuildTarget is not for Android: %s", il2cpp_u8(assetbundlePath).data());
#endif
						}
#ifdef _MSC_VER
						else
						{
							wcout << endl;
						}
#endif
					}
					else
					{
#ifdef _MSC_VER
						wcout << L"OK: " << config::runtime::fontAssets << endl;
#else
						LOGI("%s OK: %p", il2cpp_u8(assetbundlePath).data(), config::runtime::fontAssets);
#endif
					}

					font_asset_loaded = true;

					LoadAssets();
					il2cpp_thread_detach(t);
				}).detach();
		}
		else
		{
			font_asset_loaded = true;
			LoadAssets();
		}
		return;
	}

	if (!config::replace_assetbundle_file_path.empty() && !replace_assetbundle_file_path_loaded)
	{
		il2cppstring assetbundlePath = config::replace_assetbundle_file_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
#ifdef _MSC_VER
			assetbundlePath.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
#else
			assetbundlePath.insert(0, u"/sdcard/Android/data/"s.append(u8_il2cpp(Game::GetCurrentPackageName()))).append(u"/");
#endif
		}

		if (filesystem::exists(assetbundlePath))
		{
#ifdef _MSC_VER
			il2cppstringstream ss;
			ss << IL2CPP_STRING("Loading replacement AssetBundle: ") << assetbundlePath << IL2CPP_STRING("... ");
			wcout << ss.str();
			Gallop::TextCommon(text).text(il2cpp_string_new16(ss.str().data()));
#else
			stringstream ss;
			ss << "Loading replacement AssetBundle: " << il2cpp_u8(assetbundlePath) << "... ";
			LOGI("%s", ss.str().data());
			Gallop::TextCommon(text).text(il2cpp_string_new(ss.str().data()));
#endif

			std::thread([text, assetbundlePath]()
				{
#ifdef _MSC_VER
					Sleep(100);
#else
					usleep(100 * 1000);
#endif

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

					if (!assets)
					{
						if (filesystem::exists(assetbundlePath))
						{
#ifdef _MSC_VER
							wcout << endl << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
#else
							LOGW("Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Android: %s", il2cpp_u8(assetbundlePath).data());
#endif
						}
#ifdef _MSC_VER
						else
						{
							wcout << endl;
						}
#endif
					}
					else
					{
#ifdef _MSC_VER
						wcout << L"OK: " << assets.NativeObject() << endl;
#else
						LOGI("%s OK: %p", il2cpp_u8(assetbundlePath).data(), assets.NativeObject());
#endif
						config::runtime::replaceAssets.emplace_back(assets);
					}

					replace_assetbundle_file_path_loaded = true;

					LoadAssets();
					il2cpp_thread_detach(t);
				}).detach();
		}
		else
		{
			replace_assetbundle_file_path_loaded = true;
			LoadAssets();
		}
		return;
	}

	if (!config::replace_atlas_assetbundle_file_path.empty() && !replace_atlas_assetbundle_file_path_loaded)
	{
		il2cppstring assetbundlePath = config::replace_atlas_assetbundle_file_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
#ifdef _MSC_VER
			assetbundlePath.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
#else
			assetbundlePath.insert(0, u"/sdcard/Android/data/"s.append(u8_il2cpp(Game::GetCurrentPackageName()))).append(u"/");
#endif
		}

		if (filesystem::exists(assetbundlePath))
		{
#ifdef _MSC_VER
			il2cppstringstream ss;
			ss << IL2CPP_STRING("Loading replacement atlas AssetBundle: ") << assetbundlePath << IL2CPP_STRING("... ");
			wcout << ss.str();
			Gallop::TextCommon(text).text(il2cpp_string_new16(ss.str().data()));
#else
			stringstream ss;
			ss << "Loading replacement atlas AssetBundle: " << il2cpp_u8(assetbundlePath) << "... ";
			LOGI("%s", ss.str().data());
			Gallop::TextCommon(text).text(il2cpp_string_new(ss.str().data()));
#endif

			std::thread([text, assetbundlePath]()
				{
#ifdef _MSC_VER
					Sleep(100);
#else
					usleep(100 * 1000);
#endif

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

					if (!assets)
					{
						if (filesystem::exists(assetbundlePath))
						{
#ifdef _MSC_VER
							wcout << endl << L"Replacement atlas AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
#else
							LOGW("Replacement atlas AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Android: %s", il2cpp_u8(assetbundlePath).data());
#endif
						}
#ifdef _MSC_VER
						else
						{
							wcout << endl;
						}
#endif
					}
					else
					{
#ifdef _MSC_VER
						wcout << L"OK: " << assets.NativeObject() << endl;
#else
						LOGI("%s OK: %p", il2cpp_u8(assetbundlePath).data(), assets.NativeObject());
#endif
						config::runtime::replaceAtlas = assets;
					}

					replace_atlas_assetbundle_file_path_loaded = true;

					LoadAssets();
					il2cpp_thread_detach(t);
				}).detach();
		}
		else
		{
			replace_atlas_assetbundle_file_path_loaded = true;
			LoadAssets();
		}
		return;
	}

	if (!config::replace_assetbundle_file_paths.empty() && !replace_assetbundle_file_paths_loaded)
	{
		static auto it = config::replace_assetbundle_file_paths.begin();
		if (it != config::replace_assetbundle_file_paths.end())
		{
			il2cppstring assetbundlePath = *it;
			if (filesystem::path(assetbundlePath.data()).is_relative())
			{
#ifdef _MSC_VER
				assetbundlePath.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
#else
				assetbundlePath.insert(0, u"/sdcard/Android/data/"s.append(u8_il2cpp(Game::GetCurrentPackageName()))).append(u"/");
#endif
			}

			if (filesystem::exists(assetbundlePath))
			{
#ifdef _MSC_VER
				il2cppstringstream ss;
				ss << IL2CPP_STRING("Loading replacement AssetBundle: ") << assetbundlePath << IL2CPP_STRING("... ");
				wcout << ss.str();
				Gallop::TextCommon(text).text(il2cpp_string_new16(ss.str().data()));
#else
				stringstream ss;
				ss << "Loading replacement AssetBundle: " << il2cpp_u8(assetbundlePath) << "... ";
				LOGI("%s", ss.str().data());
				Gallop::TextCommon(text).text(il2cpp_string_new(ss.str().data()));
#endif

				std::thread([text, assetbundlePath]()
					{
#ifdef _MSC_VER
						Sleep(100);
#else
						usleep(100 * 1000);
#endif

						auto t = il2cpp_thread_attach(il2cpp_domain_get());
						auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

						if (!assets)
						{
							if (filesystem::exists(assetbundlePath))
							{
#ifdef _MSC_VER
								wcout << endl << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
#else
								LOGW("Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Android: %s", il2cpp_u8(assetbundlePath).data());
#endif
							}
#ifdef _MSC_VER
							else
							{
								wcout << endl;
							}
#endif
						}
						else
						{
#ifdef _MSC_VER
							wcout << L"OK: " << assets.NativeObject() << endl;
#else
							LOGI("%s OK: %p", il2cpp_u8(assetbundlePath).data(), assets.NativeObject());
#endif
							config::runtime::replaceAssets.emplace_back(assets);
						}

						it++;

						LoadAssets();
						il2cpp_thread_detach(t);
					}).detach();
				return;
			}
		}

		replace_assetbundle_file_paths_loaded = true;
	}

	if (!config::runtime::replaceAssets.empty())
	{
		for (auto it = config::runtime::replaceAssets.begin(); it != config::runtime::replaceAssets.end(); it++)
		{
			if (*it)
			{
				auto names = UnityEngine::AssetBundle{ *it }.GetAllAssetNames();
				for (int i = 0; i < names->max_length; i++)
				{
					config::runtime::replaceAssetNames.emplace_back(names->vector[i]->chars);
				}
			}
		}
	}

	Gallop::GameSystem::Instance().SoftwareReset();
#pragma endregion
}

static Il2CppObject* StartCoroutineManaged2_hook(Il2CppObject* self, Il2CppObject* enumerator)
{
	if (!BootSystem && string(enumerator->klass->name).find("BootCoroutine") != string::npos)
	{
		BootSystem = self;
		auto _bgCanvasObjectField = il2cpp_class_get_field_from_name(self->klass, "_bgCanvasObject");
		Il2CppObject* _bgCanvasObject;
		il2cpp_field_get_value(self, _bgCanvasObjectField, &_bgCanvasObject);

		if (_bgCanvasObject)
		{
			GameObject{ _bgCanvasObject }.SetActive(true);
		}

		auto onComplete = *[]()
			{
				auto GameSystem = Gallop::GameSystem::Instance();
				reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_addr)(GameSystem, GameSystem.InitializeGame(nullptr));

				auto Func = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "Func`1"),
					GetRuntimeType("mscorlib.dll", "System", "Boolean"));
				auto predicate = CreateDelegateWithClass(Func, GameSystem, *[](Il2CppObject* gameSystem)
					{
                    if (Gallop::GameSystem(gameSystem)._systemState() == Gallop::GameSystem::SystemState::Active)
						{
							auto uiManager = Gallop::UIManager::Instance();
#ifdef _MSC_VER
							if (config::unlock_size || config::freeform_window)
							{
								int width = UnityEngine::Screen::width();
								int height = UnityEngine::Screen::height();

								bool isVirt = width < height;

								uiManager.ChangeResizeUIForPC(isVirt ? height : width, isVirt ? width : height);
							}
#endif

							Il2CppObject* _bgCamera = uiManager._bgCamera();
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, UnityEngine::Color)>(_bgCamera->klass, "set_backgroundColor", 1)(_bgCamera,
								il2cpp_symbols::get_method_pointer<UnityEngine::Color(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());
							auto bgManager = GetSingletonInstanceByMethod(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "BGManager"));
							il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, uint32_t, bool)>(bgManager->klass, "SetBg", 2)(bgManager, 1, false);
							il2cpp_symbols::get_method_pointer<void (*)(bool)>(bgManager->klass, "SetBgCameraEnable", 1)(true);

							auto dialogData = Gallop::DialogCommon::Data();
							dialogData.AutoClose(false);

							if (!config::runtime::allowStart)
							{
								dialogData.SetSimpleNoButtonMessage(
									Gallop::Localize::Get(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), IL2CPP_STRING("Common0071")))),
									Gallop::Localize::Get(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), IL2CPP_STRING("Error0032"))))
								);

								Gallop::DialogManager::PushSystemDialog(dialogData, true);
							}
							else
							{
								dialogData.SetSimpleNoButtonMessage(il2cpp_string_new(""), il2cpp_string_new("Loading..."));

								dialogData.AddOpenCallback(
									&CreateDelegateWithClassStatic(
										GetGenericClass(
											GetRuntimeType(il2cpp_symbols::get_class("mscorlib.dll", "System", "Action`1")),
											GetRuntimeType(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "DialogCommon"))),
										*[]()
										{
											LoadAssets();
										}
									)->delegate
								);
							}

							Gallop::DialogManager::PushSystemDialog(dialogData, true);

							return true;
						}

						return false;
					});

				auto WaitUntil = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitUntil"));
				il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(WaitUntil->klass, ".ctor", 1)(WaitUntil, &predicate->delegate);
				reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_addr)(GameSystem, WaitUntil);
			};

		auto newEnumerator = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(CreateDelegateStatic(onComplete));

		return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_addr)(self, newEnumerator);
	}

	/*if (config::freeform_window && string(enumerator->klass->name).find("ChangeScreenOrientation") != string::npos)
	{
		auto newEnumerator = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(CreateDelegateStatic(*[]() {}));

		return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_addr)(self, newEnumerator);
	}*/
#ifdef _MSC_VER
	if (config::freeform_window && string(enumerator->klass->name).find("SoftwareResetAsync") != string::npos)
	{
		Gallop::StandaloneWindowResize::IsVirt(Gallop::Screen::IsVertical());
	}
#endif

	if (config::freeform_window && string(enumerator->klass->name).find("ChangeOrientation") != string::npos)
	{
		auto uiManager = Gallop::UIManager::Instance();
		if (uiManager)
		{
			uiManager.UnlockGameCanvas();
		}
	}

	return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_addr)(self, enumerator);
}

static void InitAddress()
{
	StartCoroutineManaged2_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StartCoroutineManaged2");
	StopCoroutineManaged_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StopCoroutineManaged");
}

static void HookMethods()
{
	il2cpp_add_internal_call("UnityEngine.MonoBehaviour::StartCoroutineManaged2", reinterpret_cast<Il2CppMethodPointer>(StartCoroutineManaged2_hook));
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	Coroutine MonoBehaviour::StartCoroutineManaged2(Il2CppObject* enumerator)
	{
		auto object = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppObject*)>(StartCoroutineManaged2_addr)(instance, enumerator);
		return Coroutine(object);
	}

	void MonoBehaviour::StopCoroutineManaged(Coroutine coroutine)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(StopCoroutineManaged_addr)(instance, coroutine);
	}
}
