#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "LowResolutionCameraFrameBuffer.hpp"

#include "config/config.hpp"

namespace
{
	void* LowResolutionCameraFrameBuffer_Initialize_addr = nullptr;
	void* LowResolutionCameraFrameBuffer_Initialize_orig = nullptr;
}

static void LowResolutionCameraFrameBuffer_Initialize_hook(Il2CppObject* self, Il2CppObject* view, Il2CppObject* frameBuffer, bool isPortraitFullScreen)
{
	reinterpret_cast<decltype(LowResolutionCameraFrameBuffer_Initialize_hook)*>(LowResolutionCameraFrameBuffer_Initialize_orig)(self, view, frameBuffer, false);
}

static void InitAddress()
{
	LowResolutionCameraFrameBuffer_Initialize_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "LowResolutionCameraFrameBuffer", "Initialize", 3);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(LowResolutionCameraFrameBuffer_Initialize, "Gallop.LowResolutionCameraFrameBuffer::Initialize at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
