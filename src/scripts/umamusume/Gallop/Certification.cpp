#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Certification.hpp"
#include "TitleViewController.hpp"

#include "game.hpp"

namespace
{
	void* Certification_initDmmPlatformData_addr = nullptr;
	void* Certification_initDmmPlatformData_orig = nullptr;

	void* Certification_get_dmmViewerId_addr = nullptr;

	void* Certification_set_dmmViewerId_addr = nullptr;

	void* Certification_get_dmmOnetimeToken_addr = nullptr;

	void* Certification_set_dmmOnetimeToken_addr = nullptr;
}

static void Certification_initDmmPlatformData_hook()
{
	reinterpret_cast<decltype(Certification_initDmmPlatformData_hook)*>(Certification_initDmmPlatformData_orig)();

	if (!Gallop::TitleViewController::viewerId.empty() && !Gallop::TitleViewController::onetimeToken.empty())
	{
		Gallop::Certification::dmmViewerId(il2cpp_string_new(Gallop::TitleViewController::viewerId.data()));
		Gallop::Certification::dmmOnetimeToken(il2cpp_string_new(Gallop::TitleViewController::onetimeToken.data()));
	}
}

static void InitAddress()
{
	auto Certification_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "Certification");
	Certification_initDmmPlatformData_addr = il2cpp_symbols::get_method_pointer(Certification_klass, "initDmmPlatformData", 0);
	Certification_get_dmmViewerId_addr = il2cpp_symbols::get_method_pointer(Certification_klass, "get_dmmViewerId", 0);
	Certification_set_dmmViewerId_addr = il2cpp_symbols::get_method_pointer(Certification_klass, "set_dmmViewerId", 1);
	Certification_get_dmmOnetimeToken_addr = il2cpp_symbols::get_method_pointer(Certification_klass, "get_dmmOnetimeToken", 0);
	Certification_set_dmmOnetimeToken_addr = il2cpp_symbols::get_method_pointer(Certification_klass, "set_dmmOnetimeToken", 1);
}

static void HookMethods()
{
	if (Game::CurrentGameStore != Game::Store::Steam)
	{
		ADD_HOOK(Certification_initDmmPlatformData, "Gallop.Certification::initDmmPlatformData at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* Certification::namespaze = "Gallop";
	const char* Certification::klassName = "Certification";

	void Certification::initDmmPlatformData()
	{
		reinterpret_cast<void (*)()>(Certification_initDmmPlatformData_addr)();
	}

	Il2CppString* Certification::dmmViewerId()
	{
		return reinterpret_cast<Il2CppString* (*)()>(Certification_get_dmmViewerId_addr)();
	}

	void Certification::dmmViewerId(Il2CppString* viewerId)
	{
		reinterpret_cast<void (*)(Il2CppString*)>(Certification_set_dmmViewerId_addr)(viewerId);
	}

	Il2CppString* Certification::dmmOnetimeToken()
	{
		return reinterpret_cast<Il2CppString* (*)()>(Certification_get_dmmOnetimeToken_addr)();
	}

	void Certification::dmmOnetimeToken(Il2CppString* onetimeToken)
	{
		reinterpret_cast<void (*)(Il2CppString*)>(Certification_set_dmmOnetimeToken_addr)(onetimeToken);
	}
}
