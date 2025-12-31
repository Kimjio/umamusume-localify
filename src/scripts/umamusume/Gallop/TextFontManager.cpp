#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TextFontManager.hpp"
#include "config/config.hpp"

#include "string_utils.hpp"

#include <unordered_map>

#include "../../UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

namespace
{
	void* TextFontManager_get_DefaultFont_addr = nullptr;

	unordered_map<u16string, Il2CppObject*> loadedFontAssets;
}

static void InitAddress()
{
	TextFontManager_get_DefaultFont_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "TextFontManager", "get_DefaultFont", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
	Il2CppObject* TextFontManager::GetReplacementFontAssetBundle(u16string path)
	{
		if (!loadedFontAssets.contains(path))
		{
			loadedFontAssets.emplace(path, UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new16(path.data())));
		}

		return loadedFontAssets[path];
	}

	Il2CppObject* TextFontManager::GetReplacementFontAsset(Il2CppString* path)
	{
		if (config::font_asset_by_path.empty())
		{
			return nullptr;
		}

		auto& replaceFontAsset = config::font_asset_by_path[path->chars];

		if (replaceFontAsset.assetBundlePath.empty() || replaceFontAsset.assetName.empty())
		{
			return nullptr;
		}

		if (auto assetBundle = GetReplacementFontAssetBundle(replaceFontAsset.assetBundlePath))
		{
			try
			{
				return UnityEngine::AssetBundle(assetBundle).LoadAsset(il2cpp_string_new16(replaceFontAsset.assetName.data()), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font"));
			}
			catch (const Il2CppExceptionWrapper& ex)
			{
				return nullptr;
			}
		}

		return nullptr;
	}

	Il2CppObject* TextFontManager::DefaultFont()
	{
		return reinterpret_cast<decltype(DefaultFont)*>(TextFontManager_get_DefaultFont_addr)();
	}
}
