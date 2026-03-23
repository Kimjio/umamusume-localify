#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LiveTheaterViewController.hpp"
#include "DialogLiveStartConfirm.hpp"

namespace
{
	Il2CppClass* LiveTheaterViewController_klass = nullptr;

	void* LiveTheaterViewController_OnClickNext_addr = nullptr;
	void* LiveTheaterViewController_OnClickNext_orig = nullptr;

	void* LiveTheaterViewController_ChangeMenu_addr = nullptr;

	void* LiveTheaterViewController_ChangeLive_addr = nullptr;

	void* LiveTheaterViewController_get_CurrentInfo_addr = nullptr;

	FieldInfo* LiveTheaterViewController__currentMenu = nullptr;
}

static void LiveTheaterViewController_OnClickNext_hook(Il2CppObject* self)
{
	Gallop::LiveTheaterViewController::Menu _currentMenu;
	il2cpp_field_get_value(self, LiveTheaterViewController__currentMenu, &_currentMenu);

	Gallop::LiveTheaterViewController::Menu nextMenu = static_cast<Gallop::LiveTheaterViewController::Menu>(static_cast<uint32_t>(_currentMenu) + 1);

	if (nextMenu == Gallop::LiveTheaterViewController::Menu::Max)
	{
		auto CurrentInfo = reinterpret_cast<Il2CppObject * (*) (Il2CppObject*)>(LiveTheaterViewController_get_CurrentInfo_addr)(self);
		auto Id = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(CurrentInfo->klass, "get_Id", 0)->methodPointer(CurrentInfo);
		Gallop::DialogLiveStartConfirm::PushDialog(Id, false, CreateDelegate(self, *[](Il2CppObject* self)
			{
				reinterpret_cast<void (*)(Il2CppObject*)>(LiveTheaterViewController_ChangeLive_addr)(self);
			}
		));
		return;
	}

	reinterpret_cast<void (*)(Il2CppObject*, Gallop::LiveTheaterViewController::Menu)>(LiveTheaterViewController_ChangeMenu_addr)(self, nextMenu);
}

static void InitAddress()
{
	LiveTheaterViewController_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LiveTheaterViewController");
	LiveTheaterViewController__currentMenu = il2cpp_class_get_field_from_name(LiveTheaterViewController_klass, "_currentMenu");
	LiveTheaterViewController_OnClickNext_addr = il2cpp_symbols::get_method_pointer(LiveTheaterViewController_klass, "OnClickNext", 0);
	LiveTheaterViewController_ChangeMenu_addr = il2cpp_symbols::get_method_pointer(LiveTheaterViewController_klass, "ChangeMenu", 1);
	LiveTheaterViewController_ChangeLive_addr = il2cpp_symbols::get_method_pointer(LiveTheaterViewController_klass, "ChangeLive", 0);
	LiveTheaterViewController_get_CurrentInfo_addr = il2cpp_symbols::get_method_pointer(LiveTheaterViewController_klass, "get_CurrentInfo", 0);
}

static void HookMethods()
{
	ADD_HOOK(LiveTheaterViewController_OnClickNext, "Gallop.LiveTheaterViewController::OnClickNext at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
