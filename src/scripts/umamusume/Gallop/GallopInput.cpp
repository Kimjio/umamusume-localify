#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GallopInput.hpp"

#include "WindowsGamepadControl.hpp"
#include "Screen.hpp"
#include "UIManager.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.InputLegacyModule/UnityEngine/Input.hpp"

namespace
{
	void* GallopInput_mousePosition_addr = nullptr;
	void* GallopInput_mousePosition_orig = nullptr;
}

static UnityEngine::Vector3 GallopInput_mousePosition_hook()
{
	if (auto WindowsGamepadControl = Gallop::WindowsGamepadControl::Instance())
	{
		WindowsGamepadControl.UpdateInputControls();
	}

	auto position = UnityEngine::Input::mousePosition();

	if (!config::freeform_window)
	{
		if (Gallop::Screen::IsVertical() && Gallop::UIManager::IsLandscapeMode())
		{
			return position * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
		}

		return position * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
	}
	return position;
}

static void InitAddress()
{
	GallopInput_mousePosition_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GallopInput", "mousePosition", 0);
}

static void HookMethods()
{
	if (config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(GallopInput_mousePosition, "Gallop.GallopInput::mousePosition at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	UnityEngine::Vector3 GallopInput::mousePosition()
	{
		return reinterpret_cast<UnityEngine::Vector3(*)()>(GallopInput_mousePosition_addr)();
	}
}
