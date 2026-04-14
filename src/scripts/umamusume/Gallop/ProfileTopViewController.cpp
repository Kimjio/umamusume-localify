#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "ProfileTopViewController.hpp"

#include "DialogCommon.hpp"
#include "DialogManager.hpp"
#include "TextCommon.hpp"
#include "Localize.hpp"

#include "scripts/Plugins/CodeStage/AntiCheat/ObscuredTypes/ObscuredLong.hpp"

namespace
{
	void* ProfileTopViewController_SetupUI_addr = nullptr;
	void* ProfileTopViewController_SetupUI_orig = nullptr;
}

static void ProfileTopViewController_SetupUI_hook(Il2CppObject* self)
{
	reinterpret_cast<decltype(ProfileTopViewController_SetupUI_hook)*>(ProfileTopViewController_SetupUI_orig)(self);
	auto view = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(self->klass, "GetViewBase", 0)->methodPointer(self);

	auto ViewerIdTextField = il2cpp_class_get_field_from_name(view->klass, "ViewerIdText");
	Il2CppObject* ViewerIdText;
	il2cpp_field_get_value(view, ViewerIdTextField, &ViewerIdText);

	auto CopyIdButtonField = il2cpp_class_get_field_from_name(view->klass, "CopyIdButton");
	Il2CppObject* CopyIdButton;
	il2cpp_field_get_value(view, CopyIdButtonField, &CopyIdButton);

	if (!GetButtonCommonOnClickDelegate(CopyIdButton))
	{
		return;
	}

	auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WorkDataManager"));

	auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
	auto viewerIdStringObscured = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workUserData->klass, "get_ViewerIdString", 0)->methodPointer(workUserData);

	auto viewerId = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(viewerIdStringObscured->klass, "InternalDecrypt", 0)->methodPointer(viewerIdStringObscured);

	if (viewerId)
	{
		auto textCommon = Gallop::TextCommon(ViewerIdText);
		auto newText = textCommon.text()->chars + il2cppstring(IL2CPP_STRING("\n(")) + viewerId->chars + IL2CPP_STRING(")");
		textCommon.text(il2cpp_string_new16(newText.data()));
	}

	auto fn = *[](void*)
		{
			auto viewerIdCopyFn = *[](void*)
				{
					auto workDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "WorkDataManager"));

					auto workUserData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(workDataManager->klass, "get_UserData", 0)->methodPointer(workDataManager);
					auto viewerIdObscured = il2cpp_class_get_method_from_name_type<CodeStage::AntiCheat::ObscuredTypes::ObscuredLong(*)(Il2CppObject*)>(workUserData->klass, "get_ViewerId", 0)->methodPointer(workUserData);
					auto viewerId = viewerIdObscured.GetDecrypted();

					il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*)>("UnityEngine.IMGUIModule.dll", "UnityEngine", "GUIUtility", "set_systemCopyBuffer", 1)(il2cpp_string_new(to_string(viewerId).data()));

					auto dialogData = Gallop::DialogCommon::Data();
					dialogData.SetSimpleOneButtonMessage(GetTextIdByName(IL2CPP_STRING("Outgame0031")), il2cpp_string_new16(IL2CPP_STRING("트레이너 ID를 복사했습니다.")), nullptr, GetTextIdByName(IL2CPP_STRING("Common0007")));

					Gallop::DialogManager::PushDialog(dialogData);
				};

			auto playerIdCopyFn = *[](void*)
				{
					auto controller = GetCurrentViewController();
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(controller->klass, "OnClickCopyIdButton", 0)->methodPointer(controller);
				};

			auto dialogData = Gallop::DialogCommon::Data();
			dialogData.SetSimpleTwoButtonMessage(
				Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0031"))),
				il2cpp_string_new("트레이너 ID를 복사하시겠습니까?"),
				CreateDelegateStatic(viewerIdCopyFn),
				GetTextIdByName(IL2CPP_STRING("Outgame0002")),
				GetTextIdByName(IL2CPP_STRING("Common0003")),
				CreateDelegateStatic(playerIdCopyFn)
			);

			Gallop::DialogManager::PushDialog(dialogData);
		};

	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(CopyIdButton->klass, "SetOnClick", 1)->methodPointer(CopyIdButton, &CreateUnityActionStatic(fn)->delegate);
}

static void InitAddress()
{
	ProfileTopViewController_SetupUI_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ProfileTopViewController", "SetupUI", 0);
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(ProfileTopViewController_SetupUI, "Gallop.ProfileTopViewController::SetupUI at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
