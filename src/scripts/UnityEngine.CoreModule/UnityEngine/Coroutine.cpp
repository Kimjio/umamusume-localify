#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Coroutine.hpp"

namespace
{
	Il2CppClass* CoroutineClass = nullptr;
	
	void* ReleaseCoroutine_addr = nullptr;

	FieldInfo* m_PtrField = nullptr;
}

static void InitAddress()
{
	CoroutineClass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine", "Coroutine");
	ReleaseCoroutine_addr = il2cpp_resolve_icall("UnityEngine.Coroutine::ReleaseCoroutine()");
	m_PtrField = il2cpp_class_get_field_from_name(CoroutineClass, "m_Ptr");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Coroutine::Coroutine(Il2CppObject* object) : YieldInstruction(object)
	{
		if (object)
		{
			il2cpp_field_get_value(object, m_PtrField, &m_Ptr);
		}
	}

	void Coroutine::ReleaseCoroutine()
	{
		ReleaseCoroutine(m_Ptr);
	}

	void Coroutine::ReleaseCoroutine(void* ptr)
	{
		reinterpret_cast<void (*)(void*)>(ReleaseCoroutine_addr)(ptr);
	}
}
