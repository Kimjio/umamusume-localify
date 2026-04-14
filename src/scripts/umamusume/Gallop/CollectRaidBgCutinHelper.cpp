#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CollectRaidBgCutinHelper.hpp"

#include "Screen.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/RenderTexture.hpp"

namespace
{
	Il2CppClass* CollectRaidBgCutinHelper_klass = nullptr;

	void* CollectRaidBgCutinHelper_InstantiateTimeline_addr = nullptr;
	void* CollectRaidBgCutinHelper_InstantiateTimeline_orig = nullptr;

	void* CollectRaidBgCutinHelper_InstantiateTimeline4_addr = nullptr;
	void* CollectRaidBgCutinHelper_InstantiateTimeline4_orig = nullptr;
}

static bool CollectRaidBgCutinHelper_InstantiateTimeline_hook(Il2CppObject* self, Il2CppString* path, Il2CppObject* parent, Il2CppObject* renderTexture, float fovFactor, int initCuttIndex)
{
	if (renderTexture)
	{
		auto texture = UnityEngine::RenderTexture(renderTexture);
		auto width = texture.width();
		auto height = texture.height();

		auto ratio = static_cast<float>(width) / static_cast<float>(height);

		auto newWidth = Gallop::Screen::Height();
		auto newHeight = static_cast<int>(static_cast<float>(newWidth) / ratio);

		texture.width(newWidth);
		texture.height(newHeight);
	}

	return reinterpret_cast<decltype(CollectRaidBgCutinHelper_InstantiateTimeline_hook)*>(CollectRaidBgCutinHelper_InstantiateTimeline_orig)(self, path, parent, renderTexture, fovFactor, initCuttIndex);
}

static bool CollectRaidBgCutinHelper_InstantiateTimeline4_hook(Il2CppObject* self, Il2CppString* path, Il2CppObject* parent, Il2CppObject* renderTexture, float fovFactor)
{
	if (renderTexture)
	{
		auto texture = UnityEngine::RenderTexture(renderTexture);
		auto width = texture.width();
		auto height = texture.height();

		auto ratio = static_cast<float>(width) / static_cast<float>(height);

		auto newWidth = Gallop::Screen::Height();
		auto newHeight = static_cast<int>(static_cast<float>(newWidth) / ratio);

		texture.width(newWidth);
		texture.height(newHeight);
	}

	return reinterpret_cast<decltype(CollectRaidBgCutinHelper_InstantiateTimeline4_hook)*>(CollectRaidBgCutinHelper_InstantiateTimeline4_orig)(self, path, parent, renderTexture, fovFactor);
}

static void InitAddress()
{
	CollectRaidBgCutinHelper_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CollectRaidBgCutinHelper");
	if (CollectRaidBgCutinHelper_klass)
	{
		auto CollectRaidBgCutinHelper_InstantiateTimeline = il2cpp_symbols::get_method(CollectRaidBgCutinHelper_klass, "InstantiateTimeline", 5);

		if (CollectRaidBgCutinHelper_InstantiateTimeline->klass == CollectRaidBgCutinHelper_klass)
		{
			CollectRaidBgCutinHelper_InstantiateTimeline_addr = CollectRaidBgCutinHelper_InstantiateTimeline->methodPointer;
		}

		CollectRaidBgCutinHelper_InstantiateTimeline4_addr = il2cpp_symbols::get_method_pointer(CollectRaidBgCutinHelper_klass, "InstantiateTimeline", 4);
	}
}

static void HookMethods()
{
	if (config::unlock_size || config::freeform_window)
	{
		ADD_HOOK(CollectRaidBgCutinHelper_InstantiateTimeline, "Gallop.CollectRaidBgCutinHelper::InstantiateTimeline at %p\n");
		ADD_HOOK(CollectRaidBgCutinHelper_InstantiateTimeline4, "Gallop.CollectRaidBgCutinHelper::InstantiateTimeline4 at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
