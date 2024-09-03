#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "RaceCameraManager.hpp"

#ifdef _MSC_VER
#include "StandaloneWindowResize.hpp"
#endif

#include "config/config.hpp"

void* SetupOrientation_addr = nullptr;

void* ChangeOrientationCoroutine_addr = nullptr;
void* ChangeOrientationCoroutine_orig = nullptr;

static Il2CppObject* ChangeOrientationCoroutine_hook(Il2CppObject* _this, Gallop::LowResolutionCameraUtil::DrawDirection direction, bool isFade,
	Il2CppDelegate_t<void (*)(Gallop::LowResolutionCameraUtil::DrawDirection, bool)>* onChangeResolution)
{
	cout << "ChangeOrientationCoroutine" << endl;
	Gallop::LowResolutionCameraUtil::DrawDirection newDirection = direction;

#ifdef _MSC_VER
	newDirection = Gallop::StandaloneWindowResize::IsVirt() ? Gallop::LowResolutionCameraUtil::DrawDirection::Portrait : Gallop::LowResolutionCameraUtil::DrawDirection::Landscape;
#endif

	return reinterpret_cast<Il2CppObject * (*)
		(Il2CppObject*, Gallop::LowResolutionCameraUtil::DrawDirection, bool, Il2CppDelegate_t<void (*)(Gallop::LowResolutionCameraUtil::DrawDirection, bool)>*)>
		(ChangeOrientationCoroutine_orig)(_this, newDirection, isFade, onChangeResolution);
}

static void InitAddress()
{
	SetupOrientation_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceCameraManager", "SetupOrientation", 1);
	ChangeOrientationCoroutine_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "RaceCameraManager", "ChangeOrientationCoroutine", 3);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(ChangeOrientationCoroutine, "Gallop.Screen::set_OriginalScreenWidth at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* RaceCameraManager::namespaze = "Gallop";
	const char* RaceCameraManager::klassName = "RaceCameraManager";

	void RaceCameraManager::SetupOrientation(LowResolutionCameraUtil::DrawDirection drawDirection)
	{
		reinterpret_cast<void (*)(Il2CppObject*, LowResolutionCameraUtil::DrawDirection)>(SetupOrientation_addr)(instance, drawDirection);
	}

	Il2CppObject* RaceCameraManager::ChangeOrientationCoroutine(LowResolutionCameraUtil::DrawDirection direction, bool isFade,
		Il2CppDelegate_t<void (*)(LowResolutionCameraUtil::DrawDirection, bool)>* onChangeResolution)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, LowResolutionCameraUtil::DrawDirection, bool, Il2CppDelegate_t<void (*)(LowResolutionCameraUtil::DrawDirection, bool)>*)>(ChangeOrientationCoroutine_addr)(instance, direction, isFade, onChangeResolution);
	}
}