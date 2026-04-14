#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "GallopFrameBuffer.hpp"

#include <algorithm>
#include <vector>

namespace
{
	void* GallopFrameBuffer_Initialize_addr = nullptr;
	void* GallopFrameBuffer_Initialize_orig = nullptr;

	void* GallopFrameBuffer_Initialize2_addr = nullptr;
	void* GallopFrameBuffer_Initialize2_orig = nullptr;

	void* GallopFrameBuffer_Release_addr = nullptr;
	void* GallopFrameBuffer_Release_orig = nullptr;

	void* GallopFrameBuffer_ResizeRenderTexture_addr = nullptr;
	void* GallopFrameBuffer_ResizeRenderTexture_orig = nullptr;

	void* GallopFrameBuffer_get_ColorBuffer_addr = nullptr;

	void* GallopFrameBuffer_RemakeRenderTexture_addr = nullptr;

	FieldInfo* GallopFrameBuffer__drawPassField = nullptr;

	vector<Il2CppObject*> frameBuffers;
}

static void GallopFrameBuffer_Initialize_hook(Il2CppObject* self, Il2CppObject* parent)
{
	auto value = find(frameBuffers.begin(), frameBuffers.end(), self);
	if (value == frameBuffers.end())
	{
		frameBuffers.emplace_back(self);
	}

	reinterpret_cast<decltype(GallopFrameBuffer_Initialize_hook)*>(GallopFrameBuffer_Initialize_orig)(self, parent);
}

static void GallopFrameBuffer_Initialize2_hook(Il2CppObject* self, Il2CppObject* parent, bool copyUIFrameBuffer)
{
	auto value = find(frameBuffers.begin(), frameBuffers.end(), self);
	if (value == frameBuffers.end())
	{
		frameBuffers.emplace_back(self);
	}

	reinterpret_cast<decltype(GallopFrameBuffer_Initialize2_hook)*>(GallopFrameBuffer_Initialize2_orig)(self, parent, copyUIFrameBuffer);
}

static void GallopFrameBuffer_Release_hook(Il2CppObject* self)
{
	auto value = find(frameBuffers.begin(), frameBuffers.end(), self);
	if (value != frameBuffers.end())
	{
		frameBuffers.erase(value);
	}

	reinterpret_cast<decltype(GallopFrameBuffer_Release_hook)*>(GallopFrameBuffer_Release_orig)(self);
}

static void GallopFrameBuffer_ResizeRenderTexture_hook()
{
	for (auto buffer : frameBuffers)
	{
		if (buffer && reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(GallopFrameBuffer_get_ColorBuffer_addr)(buffer))
		{
			Il2CppObject* _drawPath;
			il2cpp_field_get_value(buffer, GallopFrameBuffer__drawPassField, &_drawPath);

			reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(GallopFrameBuffer_RemakeRenderTexture_addr)(buffer, _drawPath);
		}
	}
}

static void InitAddress()
{
	auto GallopFrameBuffer_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "GallopFrameBuffer");
	GallopFrameBuffer_Initialize_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "Initialize", 1);
	GallopFrameBuffer_Initialize2_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "Initialize", 2);
	GallopFrameBuffer_Release_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "Release", 0);
	GallopFrameBuffer_ResizeRenderTexture_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "ResizeRenderTexture", 0);
	GallopFrameBuffer_get_ColorBuffer_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "get_ColorBuffer", 0);
	GallopFrameBuffer_RemakeRenderTexture_addr = il2cpp_symbols::get_method_pointer(GallopFrameBuffer_klass, "RemakeRenderTexture", 1);
	GallopFrameBuffer__drawPassField = il2cpp_class_get_field_from_name(GallopFrameBuffer_klass, "_drawPass");
}

static void HookMethods()
{
	if (config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(GallopFrameBuffer_Initialize, "Gallop.GallopFrameBuffer::Initialize at %p\n");
		ADD_HOOK(GallopFrameBuffer_Initialize2, "Gallop.GallopFrameBuffer::Initialize2 at %p\n");
		ADD_HOOK(GallopFrameBuffer_Release, "Gallop.GallopFrameBuffer::Release at %p\n");
		ADD_HOOK(GallopFrameBuffer_ResizeRenderTexture, "Gallop.GallopFrameBuffer::ResizeRenderTexture at %p\n");
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
