#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "TextFontManager.hpp"
#include "config/config.hpp"

#include "string_utils.hpp"

#include <unordered_map>

#include "../../UnityEngine.AssetBundleModule/UnityEngine/AssetBundle.hpp"

namespace
{
	unordered_map<u16string, Il2CppObject*> loadedFontAssets;
}

static void InitAddress()
{
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Gallop
{
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

		if (!loadedFontAssets.contains(replaceFontAsset.assetBundlePath))
		{
			loadedFontAssets.emplace(replaceFontAsset.assetBundlePath, UnityEngine::AssetBundle::LoadFromFile(il2cpp_string_new16(replaceFontAsset.assetBundlePath.data())));
		}

		try
		{
			return UnityEngine::AssetBundle(loadedFontAssets[replaceFontAsset.assetBundlePath]).LoadAsset(il2cpp_string_new16(replaceFontAsset.assetName.data()), GetRuntimeType("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font"));
		}
		catch (const Il2CppExceptionWrapper& ex)
		{
			return nullptr;
		}
	}
}
