#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Cryptographer.hpp"

void* MakeMd5_addr = nullptr;

static void InitAddress()
{
	MakeMd5_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Cryptographer", "MakeMd5", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	Il2CppString* Cryptographer::MakeMd5(Il2CppString* input)
	{
		return reinterpret_cast<Il2CppString * (*)(Il2CppString*)>(MakeMd5_addr)(input);
	}
}
