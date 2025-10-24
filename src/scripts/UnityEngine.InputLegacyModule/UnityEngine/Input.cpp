#include "../UnityEngine.InputLegacyModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Input.hpp"

#include "config/config.hpp"

namespace
{
	void* Input_get_mousePosition_Injected_addr = nullptr;
	void* Input_GetMouseButtonDown_addr = nullptr;
	void* Input_GetMouseButton_addr = nullptr;
	void* Input_GetMouseButtonUp_addr = nullptr;
}

static void InitAddress()
{
	Input_get_mousePosition_Injected_addr = il2cpp_resolve_icall("UnityEngine.Input::get_mousePosition_Injected()");
	Input_GetMouseButtonDown_addr = il2cpp_resolve_icall("UnityEngine.Input::GetMouseButtonDown()");
	Input_GetMouseButton_addr = il2cpp_resolve_icall("UnityEngine.Input::GetMouseButton()");
	Input_GetMouseButtonUp_addr = il2cpp_resolve_icall("UnityEngine.Input::GetMouseButtonUp()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Vector3 Input::mousePosition()
	{
		Vector3 vector;
		reinterpret_cast<void (*)(Vector3*)>(Input_get_mousePosition_Injected_addr)(&vector);
		return vector;
	}

	bool Input::GetMouseButtonDown(int button)
	{
		return reinterpret_cast<bool (*)(int)>(Input_GetMouseButtonDown_addr)(button);
	}

	bool Input::GetMouseButton(int button)
	{
		return reinterpret_cast<bool (*)(int)>(Input_GetMouseButton_addr)(button);
	}

	bool Input::GetMouseButtonUp(int button)
	{
		return reinterpret_cast<bool (*)(int)>(Input_GetMouseButtonUp_addr)(button);
	}
}
