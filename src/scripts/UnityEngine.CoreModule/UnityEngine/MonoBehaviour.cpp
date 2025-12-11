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
#include "scripts/umamusume/Gallop/StandaloneWindowResize.hpp"
#include "scripts/umamusume/Gallop/DialogCommon.hpp"
#include "scripts/umamusume/Gallop/DialogManager.hpp"

#include "config/config.hpp"

#include "string_utils.hpp"

namespace
{
	void* StartCoroutineManaged2_addr = nullptr;
	void* StartCoroutineManaged2_orig = nullptr;

	void* StopCoroutineManaged_addr = nullptr;

	static Il2CppObject* BootSystem;
	static bool font_asset_loaded = false;
	static bool replace_assetbundle_file_path_loaded = false;
	static bool replace_atlas_assetbundle_file_path_loaded = false;
	static bool replace_assetbundle_file_paths_loaded = false;
}

static void LoadAssets()
{
	auto dialogCommon = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "DialogManager", "GetForeFrontDialog", IgnoreNumberOfArguments)();
	auto _currentDialogObjField = il2cpp_class_get_field_from_name_wrap(dialogCommon->klass, "_currentDialogObj");
	Il2CppObject* _currentDialogObj;
	il2cpp_field_get_value(dialogCommon, _currentDialogObjField, &_currentDialogObj);

	auto text = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_currentDialogObj->klass, "get_Text", 0)->methodPointer(_currentDialogObj);
#pragma region LOAD_ASSETBUNDLE
	if (!config::runtime::fontAssets && !config::font_assetbundle_path.empty() && config::replace_to_custom_font && !font_asset_loaded)
	{
		u16string assetbundlePath = config::font_assetbundle_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
			assetbundlePath.insert(0, filesystem::current_path().u16string().append(u"/"));
		}


		if (filesystem::exists(assetbundlePath))
		{
			u16stringstream ss;
			ss << u"Loading font asset: " << assetbundlePath << u"... ";
			wcout << u16_wide(ss.str());
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(text->klass, "set_text", 1)->methodPointer(text, il2cpp_string_new16(ss.str().data()));

			std::thread([text, assetbundlePath]()
				{
					Sleep(100);

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					config::runtime::fontAssets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(text->klass, "FontTextureChanged", 0)->methodPointer(text);

					if (!config::runtime::fontAssets)
					{
						if (filesystem::exists(assetbundlePath))
						{
							wcout << L"Asset founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
						}
						else
						{
							wcout << endl;
						}
					}
					else
					{
						wcout << L"OK: " << config::runtime::fontAssets << endl;
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
		u16string assetbundlePath = config::replace_assetbundle_file_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
			assetbundlePath.insert(0, filesystem::current_path().u16string().append(u"/"));
		}

		if (filesystem::exists(assetbundlePath))
		{
			u16stringstream ss;
			ss << u"Loading replacement AssetBundle: " << assetbundlePath << u"... ";
			wcout << u16_wide(ss.str());
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(text->klass, "set_text", 1)->methodPointer(text, il2cpp_string_new16(ss.str().data()));

			std::thread([text, assetbundlePath]()
				{
					Sleep(100);

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

					if (!assets)
					{
						if (filesystem::exists(assetbundlePath))
						{
							wcout << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
						}
						else
						{
							wcout << endl;
						}
					}
					else
					{
						wcout << L"OK: " << assets.NativeObject() << endl;
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
		u16string assetbundlePath = config::replace_atlas_assetbundle_file_path;
		if (filesystem::path(assetbundlePath.data()).is_relative())
		{
			assetbundlePath.insert(0, filesystem::current_path().u16string().append(u"/"));
		}

		if (filesystem::exists(assetbundlePath))
		{
			u16stringstream ss;
			ss << u"Loading replacement atlas AssetBundle: " << assetbundlePath << u"... ";
			wcout << u16_wide(ss.str());
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(text->klass, "set_text", 1)->methodPointer(text, il2cpp_string_new16(ss.str().data()));

			std::thread([text, assetbundlePath]()
				{
					Sleep(100);

					auto t = il2cpp_thread_attach(il2cpp_domain_get());
					auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

					if (!assets)
					{
						if (filesystem::exists(assetbundlePath))
						{
							wcout << L"Replacement atlas AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
						}
						else
						{
							wcout << endl;
						}
					}
					else
					{
						wcout << L"OK: " << assets.NativeObject() << endl;
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
			u16string assetbundlePath = *it;
			if (filesystem::path(assetbundlePath.data()).is_relative())
			{
				assetbundlePath.insert(0, filesystem::current_path().u16string().append(u"/"));
			}

			if (filesystem::exists(assetbundlePath))
			{
				u16stringstream ss;
				ss << u"Loading replacement AssetBundle: " << assetbundlePath << u"... ";
				wcout << u16_wide(ss.str());
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(text->klass, "set_text", 1)->methodPointer(text, il2cpp_string_new16(ss.str().data()));

				std::thread([text, assetbundlePath]()
					{
						Sleep(100);

						auto t = il2cpp_thread_attach(il2cpp_domain_get());
						auto assets = UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new_utf16(assetbundlePath.data(), assetbundlePath.length()));

						if (!assets)
						{
							if (filesystem::exists(assetbundlePath))
							{
								wcout << L"Replacement AssetBundle founded but not loaded. Maybe Asset BuildTarget is not for Windows" << endl;
							}
							else
							{
								wcout << endl;
							}
						}
						else
						{
							wcout << L"OK: " << assets.NativeObject() << endl;
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
		auto _bgCanvasObjectField = il2cpp_class_get_field_from_name_wrap(self->klass, "_bgCanvasObject");
		Il2CppObject* _bgCanvasObject;
		il2cpp_field_get_value(self, _bgCanvasObjectField, &_bgCanvasObject);

		GameObject{ _bgCanvasObject }.SetActive(true);

		auto onComplete = *[]()
			{
				auto GameSystem = Gallop::GameSystem::Instance();
				reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(GameSystem, GameSystem.InitializeGame(nullptr));

				auto callback = CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([]()
					{
						auto uiManager = Gallop::UIManager::Instance();

						if (config::unlock_size || config::freeform_window)
						{
							int width = UnityEngine::Screen::width();
							int height = UnityEngine::Screen::height();

							bool isVirt = width < height;

							uiManager.ChangeResizeUIForPC(isVirt ? height : width, isVirt ? width : height);
						}

						Il2CppObject* _bgCamera = uiManager._bgCamera();
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Color)>(_bgCamera->klass, "set_backgroundColor", 1)->methodPointer(_bgCamera,
							il2cpp_symbols::get_method_pointer<UnityEngine::Color(*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "Color", "get_clear", IgnoreNumberOfArguments)());

						auto bgManager = GetSingletonInstanceByMethod(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "BGManager"));
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, uint32_t, bool)>(bgManager->klass, "SetBg", 2)->methodPointer(bgManager, 1, false);
						il2cpp_class_get_method_from_name_type<void (*)(bool)>(bgManager->klass, "SetBgCameraEnable", 1)->methodPointer(true);


						auto dialogData = Gallop::DialogCommon::Data();
						dialogData.AutoClose(false);

						auto text = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(unsigned long)>(
							"umamusume.dll", "Gallop", "LocalizeExtention", "Text", 1
						);

						if (!config::runtime::allowStart)
						{
							dialogData.SetSimpleNoButtonMessage(
								text(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), u"Common0071"))),
								text(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), u"Error0032")))
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
					})
				);

				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(1, &callback->delegate, true);
			};

		auto newEnumerator = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(CreateDelegateStatic(onComplete));

		return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(self, newEnumerator);
	}

	/*if (config::freeform_window && string(enumerator->klass->name).find("ChangeScreenOrientation") != string::npos)
	{
		auto newEnumerator = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(CreateDelegateStatic(*[]() {}));

		return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(self, newEnumerator);
	}*/
	if (string(enumerator->klass->name).find("SoftwareResetAsync") != string::npos)
	{
		Gallop::StandaloneWindowResize::IsVirt(false);
	}

	if (config::freeform_window && string(enumerator->klass->name).find("ChangeOrientation") != string::npos)
	{
		auto uiManager = Gallop::UIManager::Instance();
		if (uiManager)
		{
			uiManager.UnlockGameCanvas();
		}
	}

	return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(self, enumerator);
}

static void InitAddress()
{
	StartCoroutineManaged2_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StartCoroutineManaged2()");
	StopCoroutineManaged_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StopCoroutineManaged()");
}

static void HookMethods()
{
	ADD_HOOK(StartCoroutineManaged2, "UnityEngine.MonoBehaviour::StartCoroutineManaged2 at %p\n");
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
