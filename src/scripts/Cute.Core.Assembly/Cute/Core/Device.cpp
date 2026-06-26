#include "../../Cute.Core.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "Device.hpp"

namespace
{
    Il2CppMethodPointer Cute_Core_Device_IsIllegalUser_addr = nullptr;
    void* Cute_Core_Device_IsIllegalUser_orig = nullptr;
}

static bool Cute_Core_Device_IsIllegalUser_hook() {
    return false;
}

static void InitAddress()
{
    Cute_Core_Device_IsIllegalUser_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Cute.Core", "Device", "IsIllegalUser", -1);
}

static void HookMethods()
{
#ifndef _MSC_VER
    ADD_HOOK(Cute_Core_Device_IsIllegalUser, "Cute.Core.Device::IsIllegalUser at %p\n");
#endif
}

STATIC
{
    il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
