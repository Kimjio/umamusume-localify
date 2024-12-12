#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "MonoBehaviour.hpp"
#include "Color.hpp"
#include "Coroutine.hpp"
#include "Screen.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"

#include "config/config.hpp"

void* StartCoroutineManaged2_addr = nullptr;
void* StartCoroutineManaged2_orig = nullptr;

static Il2CppObject* StartCoroutineManaged2_hook(Il2CppObject* _this, Il2CppObject* enumerator)
{
	if (!config::runtime::allowStart && string(enumerator->klass->name).find("BootCoroutine") != string::npos)
	{
		auto _bgCanvasObjectField = il2cpp_class_get_field_from_name_wrap(_this->klass, "_bgCanvasObject");
		Il2CppObject* _bgCanvasObject;
		il2cpp_field_get_value(_this, _bgCanvasObjectField, &_bgCanvasObject);

		GameObject{ _bgCanvasObject }.SetActive(true);

		auto onComplete = *[]()
			{
				auto gameSystem = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GameSystem"));

				reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(gameSystem, il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gameSystem->klass, "InitializeGame", 0)->methodPointer(gameSystem));

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

						auto dialogData = il2cpp_object_new(
							il2cpp_symbols::get_class("umamusume.dll", "Gallop",
								"DialogCommon/Data"));
						il2cpp_runtime_object_init(dialogData);

						auto text = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(unsigned long)>(
							"umamusume.dll", "Gallop", "LocalizeExtention", "Text", 1
						);

						dialogData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject * thisObj,
							Il2CppString * headerTextArg,
							Il2CppString * message)>(
								il2cpp_class_get_method_from_name(dialogData->klass,
									"SetSimpleNoButtonMessage",
									2)->methodPointer
								)(dialogData,
									text(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), L"Common0071"))),
									text(GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), L"Error0032"))));

						auto AutoCloseField = il2cpp_class_get_field_from_name_wrap(dialogData->klass, "AutoClose");
						bool AutoClose = false;

						il2cpp_field_set_value(dialogData, AutoCloseField, &AutoClose);

						il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(Il2CppObject*, bool)>(
							"umamusume.dll", "Gallop", "DialogManager", "PushSystemDialog", 2)(
								dialogData, true);
					})
				);

				il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(1, &callback->delegate, true);
			};

		auto newEnumerator = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppDelegate*)>("umamusume.dll", "Gallop", "MonoBehaviourExtension", "WaitForEndFrameAsync", 1)(CreateDelegateStatic(onComplete));

		return reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(_this, newEnumerator);
	}

	auto coroutine = reinterpret_cast<decltype(StartCoroutineManaged2_hook)*>(StartCoroutineManaged2_orig)(_this, enumerator);

	if (string(enumerator->klass->name).find("ChangeOrientation") != string::npos)
	{
		auto uiManager = Gallop::UIManager::Instance();
		if (uiManager)
		{
			uiManager.UnlockGameCanvas();
		}
	}

	return coroutine;
}

static void InitAddress()
{
	StartCoroutineManaged2_addr = il2cpp_resolve_icall("UnityEngine.MonoBehaviour::StartCoroutineManaged2()");
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(StartCoroutineManaged2, "UnityEngine.MonoBehaviour::StartCoroutineManaged2 at %p\n");
	}
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
}
