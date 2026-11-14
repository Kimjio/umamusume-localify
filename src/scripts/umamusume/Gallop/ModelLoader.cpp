#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "ModelLoader.hpp"

namespace
{
	Il2CppMethodPointer ModelLoader_LoadZekkenCompositeResourceInternal_addr = nullptr;
	void* ModelLoader_LoadZekkenCompositeResourceInternal_orig = nullptr;

	Il2CppMethodPointer ModelLoader_CreateModel_addr = nullptr;
	void* ModelLoader_CreateModel_orig = nullptr;

	Il2CppMethodPointer ModelLoader_CreateModelAsync_addr = nullptr;
	void* ModelLoader_CreateModelAsync_orig = nullptr;
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

static Il2CppObject* ModelLoader_CreateModel_hook(Il2CppObject* charInfo)
{
	if (config::remove_head_decorations_on_common_outfit)
	{
		int CharaId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_CharaId", 0)->methodPointer(charInfo);
		int HeadModelSubId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_HeadModelSubId", 0)->methodPointer(charInfo);
		int DressId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_DressId", 0)->methodPointer(charInfo);
		bool IsUseDressDataHeadModelSubId = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(charInfo->klass, "get_IsUseDressDataHeadModelSubId", 0)->methodPointer(charInfo);

		if (CharaId < 9000 && !il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>(charInfo->klass, "IsMini", 0)(charInfo))
		{
			string path = "3d/chara/head/chr" + to_string(CharaId) + "_80/pfb_chr" + to_string(CharaId) + "_80";

			if ((DressId < 100 || DressId > 900000) && HeadModelSubId == 0)
			{
				if (il2cpp_symbols::get_method_pointer<bool (*)(Il2CppString*)>("umamusume.dll", "Gallop", "ResourceManager", "IsExistAsset", 1)(il2cpp_string_new(path.data())))
				{
					auto _headModelSubIdField = il2cpp_class_get_field_from_name(charInfo->klass, "_headModelSubId");
					HeadModelSubId = 80;
					il2cpp_field_set_value(charInfo, _headModelSubIdField, &HeadModelSubId);
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(charInfo->klass, "Rebuild", 0)(charInfo);
				}
			}
		}
	}

	return reinterpret_cast<decltype(ModelLoader_CreateModel_hook)*>(ModelLoader_CreateModel_orig)(charInfo);
}

static Il2CppObject* ModelLoader_CreateModelAsync_hook(Il2CppObject* charInfo, Il2CppDelegate* onLoaded)
{
	if (config::remove_head_decorations_on_common_outfit)
	{
		int CharaId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_CharaId", 0)->methodPointer(charInfo);
		int HeadModelSubId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_HeadModelSubId", 0)->methodPointer(charInfo);
		int DressId = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(charInfo->klass, "get_DressId", 0)->methodPointer(charInfo);
		bool IsUseDressDataHeadModelSubId = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*)>(charInfo->klass, "get_IsUseDressDataHeadModelSubId", 0)->methodPointer(charInfo);

		if (CharaId < 9000 && !il2cpp_symbols::get_method_pointer<bool (*)(Il2CppObject*)>(charInfo->klass, "IsMini", 0)(charInfo))
		{
			string path = "3d/chara/head/chr" + to_string(CharaId) + "_80/pfb_chr" + to_string(CharaId) + "_80";

			if ((DressId < 100 || DressId > 900000) && HeadModelSubId == 0)
			{
				if (il2cpp_symbols::get_method_pointer<bool (*)(Il2CppString*)>("umamusume.dll", "Gallop", "ResourceManager", "IsExistAsset", 1)(il2cpp_string_new(path.data())))
				{
					auto _headModelSubIdField = il2cpp_class_get_field_from_name(charInfo->klass, "_headModelSubId");
					HeadModelSubId = 80;
					il2cpp_field_set_value(charInfo, _headModelSubIdField, &HeadModelSubId);
					il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(charInfo->klass, "Rebuild", 0)(charInfo);
				}
			}
		}
	}

	return reinterpret_cast<decltype(ModelLoader_CreateModelAsync_hook)*>(ModelLoader_CreateModelAsync_orig)(charInfo, onLoaded);
}

static void InitAddress()
{
	ModelLoader_LoadZekkenCompositeResourceInternal_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ModelLoader", "LoadZekkenCompositeResourceInternal", 0);
	ModelLoader_CreateModel_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ModelLoader", "CreateModel", 1);
	ModelLoader_CreateModelAsync_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "ModelLoader", "CreateModelAsync", 2);
}

static void HookMethods()
{
	if (config::replace_to_custom_font && config::runtime::fontAssets)
	{
		ADD_HOOK(ModelLoader_LoadZekkenCompositeResourceInternal, "Gallop.ModelLoader::LoadZekkenCompositeResourceInternal at %p\n");
	}

	ADD_HOOK(ModelLoader_CreateModel, "Gallop.ModelLoader::CreateModel at %p");
	ADD_HOOK(ModelLoader_CreateModelAsync, "Gallop.ModelLoader::CreateModelAsync at %p");
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
