#include "../ScriptInternal.hpp"
#include "UncheaterInit.hpp"

namespace
{
	void* UncheaterInit_Init_addr = nullptr;
	void* UncheaterInit_Init_orig = nullptr;

	void* UncheaterInit_InitializeUncheaterSDK_addr = nullptr;
	void* UncheaterInit_InitializeUncheaterSDK_orig = nullptr;

	void* UncheaterInit_CheckUncheaterSystem_addr = nullptr;
	void* UncheaterInit_CheckUncheaterSystem_orig = nullptr;

	void* UncheaterInit_setUserName_addr = nullptr;
	void* UncheaterInit_setUserName_orig = nullptr;
}

static void UncheaterInit_Init_hook(Il2CppObject* self, Il2CppObject* system)
{
}

static void UncheaterInit_InitializeUncheaterSDK_hook(Il2CppObject* self)
{
	auto flagField = il2cpp_class_get_field_from_name_wrap(self->klass, "flag");
	bool flag = true;
	il2cpp_field_set_value(self, flagField, &flag);
}

static bool UncheaterInit_CheckUncheaterSystem_hook()
{
	return true;
}

static void UncheaterInit_setUserName_hook(Il2CppString* username)
{
}

static void InitAddress()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		UncheaterInit_Init_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "UncheaterInit", "Init", 1);
		UncheaterInit_InitializeUncheaterSDK_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "UncheaterInit", "InitializeUncheaterSDK", 0);
		UncheaterInit_CheckUncheaterSystem_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "UncheaterInit", "CheckUncheaterSystem", 0);
		UncheaterInit_setUserName_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "UncheaterInit", "setUserName", 1);
	}
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(UncheaterInit_Init, "UncheaterInit::Init at %p\n");
		ADD_HOOK(UncheaterInit_InitializeUncheaterSDK, "UncheaterInit::InitializeUncheaterSDK at %p\n");
		ADD_HOOK(UncheaterInit_CheckUncheaterSystem, "UncheaterInit::CheckUncheaterSystem at %p\n");
		ADD_HOOK(UncheaterInit_setUserName, "UncheaterInit::setUserName at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace
{

}
