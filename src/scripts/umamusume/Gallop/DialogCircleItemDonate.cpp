#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogCircleItemDonate.hpp"

namespace
{
	void* DialogCircleItemDonate_Initialize_addr = nullptr;
	void* DialogCircleItemDonate_Initialize_orig = nullptr;

	void* DialogCircleItemDonate_CalcDonateItemMax_addr = nullptr;

	void* DialogCircleItemDonate_ValidateDonateItemCount_addr = nullptr;

	void* DialogCircleItemDonate_ApplyDonateItemCountText_addr = nullptr;

	void* DialogCircleItemDonate_OnClickPlusButton_addr = nullptr;

	FieldInfo* DialogCircleItemDonate__donateCountField = nullptr;
}

static void DialogCircleItemDonate_Initialize_hook(Il2CppObject* self, Il2CppObject* dialog, Il2CppObject* itemRequestInfo)
{
	reinterpret_cast<decltype(DialogCircleItemDonate_Initialize_hook)*>(DialogCircleItemDonate_Initialize_orig)(self, dialog, itemRequestInfo);
	auto donateCount = reinterpret_cast<int (*)(Il2CppObject*)>(DialogCircleItemDonate_CalcDonateItemMax_addr)(self);
	il2cpp_field_set_value(self, DialogCircleItemDonate__donateCountField, &donateCount);

	reinterpret_cast<void (*)(Il2CppObject*)>(DialogCircleItemDonate_ValidateDonateItemCount_addr)(self);
	reinterpret_cast<void (*)(Il2CppObject*)>(DialogCircleItemDonate_ApplyDonateItemCountText_addr)(self);
	reinterpret_cast<void (*)(Il2CppObject*)>(DialogCircleItemDonate_OnClickPlusButton_addr)(self);
}

static void InitAddress()
{
	auto DialogCircleItemDonate_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogCircleItemDonate");
	DialogCircleItemDonate_Initialize_addr = il2cpp_symbols::get_method_pointer(DialogCircleItemDonate_klass, "Initialize", 2);
	DialogCircleItemDonate_CalcDonateItemMax_addr = il2cpp_symbols::get_method_pointer(DialogCircleItemDonate_klass, "CalcDonateItemMax", 0);
	DialogCircleItemDonate_ValidateDonateItemCount_addr = il2cpp_symbols::get_method_pointer(DialogCircleItemDonate_klass, "ValidateDonateItemCount", 0);
	DialogCircleItemDonate_ApplyDonateItemCountText_addr = il2cpp_symbols::get_method_pointer(DialogCircleItemDonate_klass, "ApplyDonateItemCountText", 0);
	DialogCircleItemDonate_OnClickPlusButton_addr = il2cpp_symbols::get_method_pointer(DialogCircleItemDonate_klass, "OnClickPlusButton", 0);
	DialogCircleItemDonate__donateCountField = il2cpp_class_get_field_from_name(DialogCircleItemDonate_klass, "_donateCount");
}

static void HookMethods()
{
	ADD_HOOK(DialogCircleItemDonate_Initialize, "Gallop.DialogCircleItemDonate::Initialize at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
