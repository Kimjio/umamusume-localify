#include "ResourcesAPIInternal.hpp"
#include "../../ScriptInternal.hpp"

#include "scripts/UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"
#include "scripts/umamusume/Gallop/TextFontManager.hpp"

namespace
{
	void* ResourcesAPIInternal_Load_addr = nullptr;
	void* ResourcesAPIInternal_Load_orig = nullptr;

	Il2CppReflectionType* FontReflectionType = nullptr;
}

static Il2CppObject* ResourcesAPIInternal_Load_hook(Il2CppString* path, Il2CppReflectionType* systemTypeInstance)
{
	il2cppstring u16Name = path->chars;

	if (u16Name == il2cppstring(IL2CPP_STRING("ui/views/titleview")))
	{
		if (find_if(config::runtime::replaceAssetNames.begin(), config::runtime::replaceAssetNames.end(), [](const il2cppstring& item)
			{
				return item.find(IL2CPP_STRING("utx_obj_title_logo_umamusume")) != wstring::npos;
			}) != config::runtime::replaceAssetNames.end())
		{
			auto gameObj = reinterpret_cast<decltype(ResourcesAPIInternal_Load_hook)*>(ResourcesAPIInternal_Load_orig)(path, systemTypeInstance);
			auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(gameObj->klass, "GetComponent", 1)->methodPointer;
			auto component = getComponent(gameObj, GetRuntimeType("umamusume.dll", "Gallop", "TitleView"));

			auto imgField = il2cpp_class_get_field_from_name(component->klass, "TitleLogoImage");
			Il2CppObject* imgCommon;
			il2cpp_field_get_value(component, imgField, &imgCommon);
			auto texture = GetReplacementAssets(
				il2cpp_string_new("utx_obj_title_logo_umamusume.png"),
				GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			auto m_TextureField = il2cpp_class_get_field_from_name(imgCommon->klass->parent, "m_Texture");
			il2cpp_field_set_value(imgCommon, m_TextureField, texture);
			return gameObj;
		}
	}

	if (config::replace_to_custom_font)
	{
		if (config::runtime::fontAssets && u16Name == IL2CPP_STRING("TMP Settings"))
		{
			auto object = reinterpret_cast<decltype(ResourcesAPIInternal_Load_hook)*>(ResourcesAPIInternal_Load_orig)(path, systemTypeInstance);
			auto fontAssetField = il2cpp_class_get_field_from_name(object->klass, "m_defaultFontAsset");
			il2cpp_field_set_value(object, fontAssetField, GetCustomTMPFont());
			return object;
		}

		if (systemTypeInstance == FontReflectionType || u16Name.starts_with(IL2CPP_STRING("Font")))
		{
			auto replacement = Gallop::TextFontManager::GetReplacementFontAsset(path);
			if (replacement)
			{
				return replacement;
			}
		}
	}

	return reinterpret_cast<decltype(ResourcesAPIInternal_Load_hook)*>(ResourcesAPIInternal_Load_orig)(path, systemTypeInstance);
}

static void InitAddress()
{
	ResourcesAPIInternal_Load_addr = il2cpp_resolve_icall("UnityEngine.ResourcesAPIInternal::Load(System.String,System.Type)");

	if (!ResourcesAPIInternal_Load_addr)
	{
		ResourcesAPIInternal_Load_addr = il2cpp_resolve_icall("UnityEngine.Resources::Load()");
	}

	FontReflectionType = GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font");
}

static void HookMethods()
{
	ADD_HOOK(ResourcesAPIInternal_Load, "UnityEngine.ResourcesAPIInternal::Load at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	Il2CppObject* ResourcesAPIInternal::Load(Il2CppString* path, Il2CppReflectionType* systemTypeInstance)
	{
		return reinterpret_cast<Il2CppObject * (*)(Il2CppString*, Il2CppReflectionType*)>(ResourcesAPIInternal_Load_addr)(path, systemTypeInstance);
	}
}
