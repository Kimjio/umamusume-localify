#include "../ScriptInternal.hpp"
#include "InjectFix_BUMA.hpp"

namespace
{
	Il2CppMethodPointer InjectFix_BUMA_LoadLocalPatch_addr = nullptr;
	void* InjectFix_BUMA_LoadLocalPatch_orig = nullptr;

	Il2CppMethodPointer InjectFix_BUMA_LoadRemotePatch_addr = nullptr;
	void* InjectFix_BUMA_LoadRemotePatch_orig = nullptr;
}

static void InjectFix_BUMA_LoadLocalPatch_hook()
{

}

static Il2CppObject* InjectFix_BUMA_LoadRemotePatch_hook()
{
	auto yield = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "WaitWhile"));
	il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppDelegate*)>(yield->klass, ".ctor", 1)(yield, CreateDelegateStatic(*[]() { return false; }));
	return yield;
}

static void InitAddress()
{
	if (Game::CurrentGameRegion == Game::Region::TWN)
	{
		InjectFix_BUMA_LoadLocalPatch_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "InjectFix_BUMA", "LoadLoaclPatch", 0);
		InjectFix_BUMA_LoadRemotePatch_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "", "InjectFix_BUMA", "LoadRemotePatch", 0);
	}
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::TWN)
	{
		ADD_HOOK(InjectFix_BUMA_LoadLocalPatch, "InjectFix_BUMA::LoadLocalPatch at %p\n");
		ADD_HOOK(InjectFix_BUMA_LoadRemotePatch, "InjectFix_BUMA::LoadRemotePatch at %p\n");
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
