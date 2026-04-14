#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "FriendViewController.hpp"

#include "DialogCommon.hpp"
#include "DialogManager.hpp"
#include "TextCommon.hpp"
#include "Localize.hpp"

#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredLong.hpp"

#include "string_utils.hpp"

namespace
{
	void* FriendViewController_OnClickIDSearch_addr = nullptr;
	void* FriendViewController_OnClickIDSearch_orig = nullptr;
}

static void FriendViewController_OnClickIDSearch_hook(Il2CppObject* self)
{
	auto viewerIdSearchFn = *[](void*)
		{
			auto fn = *[](void*, Il2CppString* text)
				{
					auto dialog = GetFrontDialog();
					auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

					auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
					Il2CppObject* ContentsObject;
					il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

					auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogIDSearch"));

					auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
					Il2CppObject* _inputField;
					il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

					auto text1 = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

					string searchId = il2cpp_u8(text1->chars);

					auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WorkDataManager"));

					auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
					auto viewerIdObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredLong(*)(Il2CppObject*)>(workUserData->klass, "get_ViewerId", 0)->methodPointer(workUserData);
					auto viewerId = viewerIdObscured.GetDecrypted();

					if (searchId == to_string(viewerId))
					{
						il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "FriendViewController", "PushErrorSearchOwn", IgnoreNumberOfArguments)();
						return;
					}

					auto onModifyFriendCloseFn = *[]()
						{
							auto controller = GetCurrentViewController();
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "UpdateCurrentTabView", 0)->methodPointer(controller);
						};

					il2cpp_symbols::get_method_pointer<void (*)(int64_t, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppDelegate*, Il2CppObject*, bool, Il2CppDelegate*, bool, Il2CppDelegate*, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogTrainerInfo", "PushDialog", 11)
						(stoll(searchId.data()), CreateDelegateStatic(onModifyFriendCloseFn), nullptr, nullptr, nullptr, nullptr, false, nullptr, false, nullptr, nullptr);
				};
			il2cpp_symbols::get_method_pointer<void (*)(Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogIDSearch", "PushDialog", 1)(CreateDelegateStatic(fn));

			auto dialog = GetFrontDialog();
			auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

			auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
			Il2CppObject* ContentsObject;
			il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

			auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogIDSearch"));

			auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
			Il2CppObject* _inputField;
			il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

			auto onValueChanged = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_inputField->klass, "get_onValueChanged", 0)->methodPointer(_inputField);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(onValueChanged->klass, "RemoveAllListeners", 0)->methodPointer(onValueChanged);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(_inputField->klass, "Start", 0)->methodPointer(_inputField);

			auto valueChangeFn = *[](Il2CppObject* obj)
				{
					auto dialog = GetFrontDialog();
					auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

					auto ContentsObjectField = il2cpp_class_get_field_from_name(data->klass, "ContentsObject");
					Il2CppObject* ContentsObject;
					il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

					auto idSearch = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType(ASSEMBLY_NAME, "Gallop", "DialogIDSearch"));

					auto _inputFieldField = il2cpp_class_get_field_from_name(idSearch->klass, "_inputField");
					Il2CppObject* _inputField;
					il2cpp_field_get_value(idSearch, _inputFieldField, &_inputField);

					auto value = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_inputField->klass, "get_text", 0)->methodPointer(_inputField);

					Il2CppObject* rightButton = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, uint64_t)>(dialog->klass, "GetButtonObj", 1)->methodPointer(dialog, 2);

					bool valid = false;

					if (value && value->length >= 9)
					{
						try
						{
							stoll(il2cpp_u8(value->chars).data());
							valid = true;
						}
						catch (invalid_argument e)
						{
						}
					}

					if (rightButton)
					{
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(rightButton->klass, "set_interactable", 1)->methodPointer(rightButton, valid);
						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*)>(rightButton->klass, "SetNotificationMessage", 1)->methodPointer(rightButton, valid ? il2cpp_string_new("") : Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Friend0013"))));
					}
				};

			auto delegateClass = GetGenericClass(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction`1"), GetRuntimeType(il2cpp_defaults.string_class));

			void** params = new void* [1];
			params[0] = CreateDelegateWithClass(delegateClass, _inputField, valueChangeFn);

			Il2CppException* exception;

			il2cpp_runtime_invoke_type<void>(il2cpp_class_get_method_from_name(onValueChanged->klass, "AddListener", 1), onValueChanged, params, &exception);

			auto _myIdTextField = il2cpp_class_get_field_from_name(idSearch->klass, "_myIdText");
			Il2CppObject* _myIdText;
			il2cpp_field_get_value(idSearch, _myIdTextField, &_myIdText);

			auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WorkDataManager"));

			auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
			auto viewerIdStringObscured = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workUserData->klass, "get_ViewerIdString", 0)->methodPointer(workUserData);

			auto viewerId = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(viewerIdStringObscured->klass, "InternalDecrypt", 0)->methodPointer(viewerIdStringObscured);

			Gallop::TextCommon(_myIdText).text(viewerId);
		};

	auto playerIdSearchFn = *[](Il2CppObject* self)
		{
			reinterpret_cast<decltype(FriendViewController_OnClickIDSearch_hook)*>(FriendViewController_OnClickIDSearch_orig)(self);
		};

	auto dialogData = Gallop::DialogCommon::Data();
	dialogData.SetSimpleTwoButtonMessage(
		Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Friend0039"))),
		il2cpp_string_new("트레이너 ID로 검색하시겠습니까?"),
		CreateDelegateStatic(viewerIdSearchFn),
		GetTextIdByName(IL2CPP_STRING("Outgame0002")),
		GetTextIdByName(IL2CPP_STRING("Common0003")),
		CreateDelegate(self, playerIdSearchFn)
	);

	Gallop::DialogManager::PushDialog(dialogData);
}

static void InitAddress()
{
	FriendViewController_OnClickIDSearch_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "FriendViewController", "OnClickIDSearch", 0);
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(FriendViewController_OnClickIDSearch, "Gallop.FriendViewController::OnClickIDSearch at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
