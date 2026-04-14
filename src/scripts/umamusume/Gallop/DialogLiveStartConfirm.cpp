#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogLiveStartConfirm.hpp"
#include "Live/Master3dLive.hpp"

#include "DialogManager.hpp"
#include "Localize.hpp"
#include "TextCommon.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ResourcesAPIINternal.hpp"
#include "scripts/mscorlib/System/ValueTuple.hpp"

#include "config/config.hpp"

#include "localify/UIParts.hpp"

namespace
{
	Il2CppClass* DialogLiveStartConfirm_klass = nullptr;

	void* DialogLiveStartConfirm_Setup_addr = nullptr;
	void* DialogLiveStartConfirm_Setup_orig = nullptr;

	void* DialogLiveStartConfirm_SetupToggle_addr = nullptr;
	void* DialogLiveStartConfirm_SetupToggle_orig = nullptr;

	void* DialogLiveStartConfirm_OnClickOK_addr = nullptr;
	void* DialogLiveStartConfirm_OnClickOK_orig = nullptr;

	void* DialogLiveStartConfirm_SendApi_addr = nullptr;
	void* DialogLiveStartConfirm_SendApi_orig = nullptr;

	void* DialogLiveStartConfirm_PushDialog_addr = nullptr;
	void* DialogLiveStartConfirm_PushDialog_orig = nullptr;

	FieldInfo* DialogLiveStartConfirm__icon = nullptr;
	FieldInfo* DialogLiveStartConfirm__name = nullptr;
	FieldInfo* DialogLiveStartConfirm__musicId = nullptr;
	FieldInfo* DialogLiveStartConfirm__isSendApi = nullptr;
}

static void DialogLiveStartConfirm_SetupToggle_hook(Il2CppObject* self)
{
	int _musicId;
	il2cpp_field_get_value(self, DialogLiveStartConfirm__musicId, &_musicId);
	auto RadioButtonFullPortrait = UnityEngine::GameObject::Find(il2cpp_string_new("RadioButtonFullPortrait"));
	if (RadioButtonFullPortrait)
	{
		if (Gallop::Live::Master3dLive(_musicId).LiveData().ScreenMode() > 0 || config::runtime::LiveStartSettingIsOrientationFullPortrait.value_or(false))
		{
			auto _toggleOrientationFullPortrait = RadioButtonFullPortrait.GetComponent(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "ToggleCommon"));
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(_toggleOrientationFullPortrait->klass, "set_isOn", 1)->methodPointer(_toggleOrientationFullPortrait, true);
			return;
		}
	}

	reinterpret_cast<decltype(DialogLiveStartConfirm_SetupToggle_hook)*>(DialogLiveStartConfirm_SetupToggle_orig)(self);
}

static void DialogLiveStartConfirm_OnClickOK_hook(Il2CppObject* self)
{
	auto RadioButtonFullPortrait = UnityEngine::GameObject::Find(il2cpp_string_new("RadioButtonFullPortrait"));
	if (RadioButtonFullPortrait)
	{
		auto _toggleOrientationFullPortrait = RadioButtonFullPortrait.GetComponent(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "ToggleCommon"));
		bool isOn = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(_toggleOrientationFullPortrait->klass, "get_isOn", 0)->methodPointer(_toggleOrientationFullPortrait);

		if (isOn)
		{
			config::runtime::LiveStartSettingIsOrientationFullPortrait = true;
			return;
		}

		config::runtime::LiveStartSettingIsOrientationFullPortrait = false;
	}

	reinterpret_cast<decltype(DialogLiveStartConfirm_OnClickOK_hook)*>(DialogLiveStartConfirm_OnClickOK_orig)(self);
}

static void DialogLiveStartConfirm_SendApi_hook(Il2CppObject* self, Il2CppDelegate* finishCallback)
{
	auto RadioButtonFullPortrait = UnityEngine::GameObject::Find(il2cpp_string_new("RadioButtonFullPortrait"));
	if (RadioButtonFullPortrait)
	{
		auto _toggleOrientationFullPortrait = RadioButtonFullPortrait.GetComponent(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "ToggleCommon"));
		bool isOn = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(_toggleOrientationFullPortrait->klass, "get_isOn", 0)->methodPointer(_toggleOrientationFullPortrait);

		if (isOn)
		{
			int _musicId;
			il2cpp_field_get_value(self, DialogLiveStartConfirm__musicId, &_musicId);

			bool _isSendApi;
			il2cpp_field_get_value(self, DialogLiveStartConfirm__isSendApi, &_isSendApi);

			il2cpp_symbols::get_method_pointer<void (*)(int, bool, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogLiveStartConfirmFullPortrait", "PushDialog", 3)(_musicId, _isSendApi, finishCallback);
			return;
		}
	}

	reinterpret_cast<decltype(DialogLiveStartConfirm_SendApi_hook)*>(DialogLiveStartConfirm_SendApi_orig)(self, finishCallback);
}

static void InitAddress()
{
	DialogLiveStartConfirm_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogLiveStartConfirm");
	DialogLiveStartConfirm__icon = il2cpp_class_get_field_from_name(DialogLiveStartConfirm_klass, "_icon");
	DialogLiveStartConfirm__name = il2cpp_class_get_field_from_name(DialogLiveStartConfirm_klass, "_name");
	DialogLiveStartConfirm__musicId = il2cpp_class_get_field_from_name(DialogLiveStartConfirm_klass, "_musicId");
	DialogLiveStartConfirm__isSendApi = il2cpp_class_get_field_from_name(DialogLiveStartConfirm_klass, "_isSendApi");
	DialogLiveStartConfirm_Setup_addr = il2cpp_symbols::get_method_pointer(DialogLiveStartConfirm_klass, "Setup", 2);
	DialogLiveStartConfirm_SetupToggle_addr = il2cpp_symbols::get_method_pointer(DialogLiveStartConfirm_klass, "SetupToggle", 0);
	DialogLiveStartConfirm_OnClickOK_addr = il2cpp_symbols::get_method_pointer(DialogLiveStartConfirm_klass, "OnClickOK", 0);
	DialogLiveStartConfirm_SendApi_addr = il2cpp_symbols::get_method_pointer(DialogLiveStartConfirm_klass, "SendApi", 1);
	DialogLiveStartConfirm_PushDialog_addr = il2cpp_symbols::get_method_pointer(DialogLiveStartConfirm_klass, "PushDialog", 3);
}

static void HookMethods()
{
	ADD_HOOK(DialogLiveStartConfirm_SetupToggle, "Gallop.DialogLiveStartConfirm::SetupToggle at %p\n");
	ADD_HOOK(DialogLiveStartConfirm_OnClickOK, "Gallop.DialogLiveStartConfirm::OnClickOK at %p\n");
	ADD_HOOK(DialogLiveStartConfirm_SendApi, "Gallop.DialogLiveStartConfirm::SendApi at %p\n");
	auto DialogLiveStartConfirm_PushDialog_hook = Gallop::DialogLiveStartConfirm::PushDialog;
	ADD_HOOK(DialogLiveStartConfirm_PushDialog, "Gallop.DialogLiveStartConfirm::PushDialog at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	void DialogLiveStartConfirm::Setup(int musicId, bool isSendApi)
	{
		il2cpp_field_set_value(instance, DialogLiveStartConfirm__musicId, &musicId);
		il2cpp_field_set_value(instance, DialogLiveStartConfirm__isSendApi, &isSendApi);
		auto LoadOnView = GetGenericMethod(il2cpp_symbols::get_method(ASSEMBLY_NAME, "Gallop", "ResourceManager", "LoadOnView", 2), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));

		Il2CppString* jacketPath = nullptr;
		auto GetJacketPath = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, uint64_t, bool)>(ASSEMBLY_NAME, "Gallop", "ResourcePath", "GetJacketPath", 3);
		if (GetJacketPath)
		{
			jacketPath = GetJacketPath(musicId, 0, false);
		}
		else
		{
			jacketPath = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(int, uint64_t)>(ASSEMBLY_NAME, "Gallop", "ResourcePath", "GetJacketPath", 2)(musicId, 0);
		}

		Il2CppObject* _icon;
		il2cpp_field_get_value(instance, DialogLiveStartConfirm__icon, &_icon);

		auto jacketTexture = reinterpret_cast<Il2CppObject * (*)(Il2CppString*, uint32_t, const MethodInfo*)>(LoadOnView->methodPointer)(jacketPath, 0, LoadOnView);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(_icon->klass, "set_texture", 1)->methodPointer(_icon, jacketTexture);

		Il2CppObject* _name;
		il2cpp_field_get_value(instance, DialogLiveStartConfirm__name, &_name);

		auto masterLiveTitle = il2cpp_symbols::get_method_pointer<Il2CppString * (*)(uint64_t, int)>(ASSEMBLY_NAME, "Gallop", "TextUtil", "GetMasterText", 2)(GetEnumValue(ParseEnum(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "MasterString/Category"), IL2CPP_STRING("MasterLiveTitle"))), musicId);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(_name->klass, "set_text", 1)->methodPointer(_name, masterLiveTitle);

		SetupToggle();
	}

	void DialogLiveStartConfirm::SetupToggle()
	{
		reinterpret_cast<decltype(DialogLiveStartConfirm_SetupToggle_hook)*>(DialogLiveStartConfirm_SetupToggle_addr)(instance);
	}

	void DialogLiveStartConfirm::OnClickOK()
	{
		reinterpret_cast<decltype(DialogLiveStartConfirm_OnClickOK_hook)*>(DialogLiveStartConfirm_OnClickOK_addr)(instance);
	}

	void DialogLiveStartConfirm::SendApi(Il2CppDelegate* finishCallback)
	{
		reinterpret_cast<decltype(DialogLiveStartConfirm_SendApi_hook)*>(DialogLiveStartConfirm_SendApi_addr)(instance, finishCallback);
	}

	void DialogLiveStartConfirm::PushDialog(int musicId, bool isSendApi, Il2CppDelegate* onDecide)
	{
		UnityEngine::GameObject gameObject = UnityEngine::Object::Internal_CloneSingle(UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("UI/Parts/Live/DialogLiveStartConfirm"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject")));
		
		try
		{
			auto Live6620008 = GetTextIdByName(IL2CPP_STRING("Live6620008"));
		
			auto radioRootTransform = gameObject.transform().Find(il2cpp_string_new("orientation_radio_root"));

			if (radioRootTransform)
			{
				auto radioButtonPortrait = radioRootTransform.Find(il2cpp_string_new("RadioButtonPortrait"));
				UnityEngine::GameObject radioButtonPortraitCloned = UnityEngine::Object::Internal_CloneSingle(radioButtonPortrait.gameObject());
				UnityEngine::Object::Name(radioButtonPortraitCloned, il2cpp_string_new("RadioButtonFullPortrait"));

				auto array = radioButtonPortraitCloned.GetComponentsInChildren(GetRuntimeType(ASSEMBLY_NAME, "Gallop", "TextCommon"), false);
				Gallop::TextCommon(array->vector[0]).text(Gallop::Localize::Get(Live6620008));

				Localify::UIParts::AddToLayout(radioRootTransform, { radioButtonPortraitCloned }, false);

				UnityEngine::Object::DestroyImmediate(radioRootTransform.gameObject().GetComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "HorizontalLayoutGroup")));
				auto gridLayoutGroup = radioRootTransform.gameObject().AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 50 });
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 32 });

				auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);
			}
		}
		catch (const Il2CppExceptionWrapper& ex)
		{
			wcout << ex.ex->message->chars << endl;
		}
		
		DialogLiveStartConfirm dialogContent = gameObject.GetComponent(GetRuntimeType(DialogLiveStartConfirm_klass));
		dialogContent.Setup(musicId, isSendApi);

		DialogCommon::Data data = dialogContent.CreateDialogData();
		data.Title(Localize::Get(GetTextIdByName(IL2CPP_STRING("Live0003"))));
		data.AutoClose(false);
		data.LeftButtonText(Localize::Get(GetTextIdByName(IL2CPP_STRING("Common0004"))));
		data.RightButtonText(Localize::Get(GetTextIdByName(IL2CPP_STRING("Common0003"))));

		static bool isDecide;
		isDecide = false;

		data.LeftButtonCallBack(CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				DialogCommon(dialog).Close();
			}
		));
		data.RightButtonCallBack(CreateDelegate(dialogContent, *[](Il2CppObject* dialogContent, Il2CppObject* dialog)
			{
				auto ValueTuple2Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`2"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
				System::ValueTuple<Il2CppObject*, Il2CppObject*> tuple = { dialogContent, dialog };
				auto boxed = il2cpp_value_box(ValueTuple2Class, &tuple);
				DialogLiveStartConfirm(dialogContent).SendApi(CreateDelegate(boxed, *[](Il2CppObject* self)
					{
						auto tuple = *il2cpp_object_unbox_type<System::ValueTuple<Il2CppObject*, Il2CppObject*>*>(self);
						DialogLiveStartConfirm dialogContent = tuple.Item1;
						DialogCommon dialog = tuple.Item2;
						dialogContent.OnClickOK();
						isDecide = true;
						dialog.Close();
					}
				));
			}
		));
		data.DestroyCallBack(CreateDelegate(reinterpret_cast<Il2CppObject*>(onDecide), *[](Il2CppObject* self)
			{
				if (!isDecide)
				{
					return;
				}

				if (self)
				{
					auto onDecide = reinterpret_cast<Il2CppDelegate*>(self);
					reinterpret_cast<void (*)(Il2CppObject*, const MethodInfo*)>(onDecide->method_ptr)(onDecide->target, onDecide->method);
				}
			}
		));

		DialogManager::PushDialog(data);
	}
}
