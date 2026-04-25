#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "ModelLoader.hpp"

namespace
{
	void* ModelLoader_LoadZekkenCompositeResourceInternal_addr = nullptr;
	void* ModelLoader_LoadZekkenCompositeResourceInternal_orig = nullptr;
}

static void ModelLoader_LoadZekkenCompositeResourceInternal_hook(Il2CppObject* self)
{
	if (config::replace_to_custom_font && config::runtime::fontAssets)
	{
		auto font = GetCustomFont();
		if (font)
		{
			FieldInfo* zekkenFontField = il2cpp_class_get_field_from_name(self->klass, "_fontZekken");
			il2cpp_field_set_value(self, zekkenFontField, font);
		}
	}
	reinterpret_cast<decltype(ModelLoader_LoadZekkenCompositeResourceInternal_hook)*>(ModelLoader_LoadZekkenCompositeResourceInternal_orig)(self);
}

static void InitAddress()
{
	ModelLoader_LoadZekkenCompositeResourceInternal_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ModelLoader", "LoadZekkenCompositeResourceInternal", 0);
}

static void HookMethods()
{
	ADD_HOOK(ModelLoader_LoadZekkenCompositeResourceInternal, "Gallop.ModelLoader::LoadZekkenCompositeResourceInternal at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* ModelLoader::namespaze = "Gallop";
	const char* ModelLoader::klassName = "ModelLoader";
}
