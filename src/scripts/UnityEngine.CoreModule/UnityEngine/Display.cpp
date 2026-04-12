#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Display.hpp"

namespace
{
	Il2CppClass* Display_klass = nullptr;

	void* Display_get_main_addr = nullptr;

	void* Display_get_renderingWidth_addr = nullptr;

	void* Display_get_renderingHeight_addr = nullptr;

	void* Display_get_systemWidth_addr = nullptr;

	void* Display_get_systemHeight_addr = nullptr;
}

static void InitAddress()
{
	Display_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine", "Display");
	Display_get_main_addr = il2cpp_symbols::get_method_pointer(Display_klass, "get_main", 0);
	Display_get_renderingWidth_addr = il2cpp_symbols::get_method_pointer(Display_klass, "get_renderingWidth", 0);
	Display_get_renderingHeight_addr = il2cpp_symbols::get_method_pointer(Display_klass, "get_renderingHeight", 0);
	Display_get_systemWidth_addr = il2cpp_symbols::get_method_pointer(Display_klass, "get_systemWidth", 0);
	Display_get_systemHeight_addr = il2cpp_symbols::get_method_pointer(Display_klass, "get_systemHeight", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Display Display::main()
	{
		return reinterpret_cast<Il2CppObject * (*)()>(Display_get_main_addr)();
	}

	int Display::renderingWidth()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(Display_get_renderingWidth_addr)(instance);
	}

	int Display::renderingHeight()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(Display_get_renderingHeight_addr)(instance);
	}

	int Display::systemWidth()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(Display_get_systemWidth_addr)(instance);
	}

	int Display::systemHeight()
	{
		return reinterpret_cast<int (*)(Il2CppObject*)>(Display_get_systemHeight_addr)(instance);
	}
}
