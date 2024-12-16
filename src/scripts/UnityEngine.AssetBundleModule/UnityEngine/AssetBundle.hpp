#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"

extern Il2CppObject* GetReplacementAssets(Il2CppString* name, Il2CppObject* type);

extern void ReplaceMaterialTextureProperty(Il2CppObject* material, Il2CppString* property);

namespace UnityEngine
{
	class AssetBundle : public Object
	{
	public:
		static AssetBundle LoadFromFile(Il2CppString* path);
		Il2CppObject* LoadAsset(Il2CppString* name, Il2CppObject* runtimeType);
		Il2CppObject* LoadAssetAsync(Il2CppString* name, Il2CppObject* runtimeType);
		Il2CppArraySize_t<Il2CppString*>* GetAllAssetNames();
		void Unload(bool unloadAllLoadedObjects);

		using Object::Object;
	};
}