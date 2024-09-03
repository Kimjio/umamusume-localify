#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Coroutine.hpp"

Il2CppClass* CoroutineClass = nullptr;

static void InitAddress()
{
	CoroutineClass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine", "Coroutine");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Coroutine::Coroutine(Il2CppObject* object)
	{
		auto ptrField = il2cpp_class_get_field_from_name_wrap(CoroutineClass, "m_Ptr");
		il2cpp_field_get_value(object, ptrField, &m_Ptr);
	}
}
