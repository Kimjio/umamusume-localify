#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Sprite.hpp"

namespace
{
	void* Sprite_get_texture_addr = nullptr;
	void* Sprite_get_texture_orig = nullptr;
}

static Il2CppObject* Sprite_get_texture_hook(Il2CppObject* self)
{
	auto texture2D = reinterpret_cast<decltype(Sprite_get_texture_hook)*>(Sprite_get_texture_orig)(self);
	auto uobject_name = UnityEngine::Object::Name(texture2D);
	if (!il2cppstring(uobject_name->chars).empty())
	{
		auto newTexture = GetReplacementAssets(
			uobject_name,
			GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
		if (newTexture)
		{
			return newTexture;
		}
	}
	return texture2D;
}

static void InitAddress()
{
	Sprite_get_texture_addr = il2cpp_resolve_icall("UnityEngine.Sprite::get_texture(UnityEngine.Sprite)");
}

static void HookMethods()
{
	if (!config::replace_assetbundle_file_paths.empty())
	{
		ADD_HOOK(Sprite_get_texture, "UnityEngine.Sprite::get_texture at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	Il2CppObject* Sprite::texture()
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(Sprite_get_texture_addr)(instance);
	}
}
