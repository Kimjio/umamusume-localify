#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "DialogInnerBase.hpp"

namespace
{
	void* DialogInnerBase_CreateDialogData_addr = nullptr;
}

static void InitAddress()
{
	const auto DialogInnerBase = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "DialogInnerBase");

	DialogInnerBase_CreateDialogData_addr = il2cpp_symbols::get_method_pointer(DialogInnerBase, "CreateDialogData", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	DialogCommon::Data DialogInnerBase::CreateDialogData()
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(DialogInnerBase_CreateDialogData_addr)(instance);
	}
}
