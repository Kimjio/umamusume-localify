#include "../UnityEngine.AssetBundleModule.hpp"
#include "../../ScriptInternal.hpp"
#include "AssetBundle.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Shader.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Material.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector2.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Vector4.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"

#include "config/config.hpp"
#include "string_utils.hpp"

#include "local/local.hpp"

#include <msgpack11.hpp>

using namespace msgpack11;

namespace
{
	void* LoadFromFile_Internal_addr = nullptr;
	void* LoadFromFile_Internal_orig = nullptr;

	void* LoadFromStreamInternal_addr = nullptr;
	void* LoadFromStreamInternal_orig = nullptr;

	void* LoadAsset_Internal_addr = nullptr;
	void* LoadAsset_Internal_orig = nullptr;

	void* LoadAssetAsync_Internal_addr = nullptr;
	void* LoadAssetAsync_Internal_orig = nullptr;

	void* AssetBundleRequest_GetResult_addr = nullptr;
	void* AssetBundleRequest_GetResult_orig = nullptr;

	void* GetAllAssetNames_addr = nullptr;

	void* Unload_addr = nullptr;
	void* Unload_orig = nullptr;

	Il2CppClass* AtlasReferenceClass;
	Il2CppClass* GameObjectClass;
	Il2CppClass* MaterialClass;
	Il2CppClass* FontClass;

	Il2CppClass* StoryTimelineDataClass;
}

static void ReplaceMaterialTextureProperty(Il2CppObject* material, Il2CppString* property)
{
	if (UnityEngine::Material{ material }.HasProperty(UnityEngine::Shader::PropertyToID(property)))
	{
		auto texture = UnityEngine::Material{ material }.GetTextureImpl(UnityEngine::Shader::PropertyToID(property));
		if (texture)
		{
			auto uobject_name = UnityEngine::Object::Name(texture);
			if (!il2cppstring(uobject_name->chars).empty())
			{
				auto newTexture = GetReplacementAssets(
					uobject_name,
					GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
				if (newTexture)
				{
					UnityEngine::Material{ material }.SetTextureImpl(UnityEngine::Shader::PropertyToID(property), newTexture);
				}
			}
		}
	}
}

static void ReplaceMaterialTexture(Il2CppObject* material)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(material))
	{
		return;
	}

	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_DirtTex"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_EmissiveTex"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_EnvMap"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_MainTex"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_MaskColorTex"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_OptionMaskMap"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_ToonMap"));
	ReplaceMaterialTextureProperty(material, il2cpp_string_new("_TripleMaskMap"));
}

static void ReplaceAssetHolderTextures(Il2CppObject* holder)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(holder))
	{
		return;
	}

	auto objectList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(holder->klass, "get_ObjectList", 0)->methodPointer(holder);

	FieldInfo* itemsField = il2cpp_class_get_field_from_name(objectList->klass, "_items");
	Il2CppArraySize_t<Il2CppObject*>* arr;
	il2cpp_field_get_value(objectList, itemsField, &arr);

	for (int i = 0; i < arr->max_length; i++)
	{
		auto pair = arr->vector[i];

		if (!pair)
		{
			continue;
		}

		auto field = il2cpp_class_get_field_from_name(pair->klass, "Value");
		Il2CppObject* obj;
		il2cpp_field_get_value(pair, field, &obj);

		if (obj)
		{
			if (obj->klass->name == "Texture2D"s)
			{
				auto uobject_name = UnityEngine::Object::Name(obj);
				if (!il2cppstring(uobject_name->chars).empty())
				{
					auto newTexture = GetReplacementAssets(
						uobject_name,
						GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
					if (newTexture)
					{
						il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "Object", "set_hideFlags", 1)
							(newTexture, 32);
						il2cpp_field_set_value(pair, field, newTexture);
					}
				}
			}
			if (obj->klass->name == "Material"s)
			{
				ReplaceMaterialTexture(obj);
			}
		}
	}
}

static void ReplaceAnimateToUnityTextures(Il2CppObject* anRoot)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(anRoot))
	{
		return;
	}

	auto meshParameterGroup = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(anRoot->klass, "get_MeshParameterGroup", 0)->methodPointer(anRoot);

	if (meshParameterGroup)
	{
		auto meshParameterList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(meshParameterGroup->klass, "get_MeshParameterList", 0)->methodPointer(meshParameterGroup);

		if (meshParameterList)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(meshParameterList->klass, "_items");
			Il2CppArraySize_t<Il2CppObject*>* items;
			il2cpp_field_get_value(meshParameterList, itemsField, &items);

			if (items)
			{
				for (int i = 0; i < items->max_length; i++)
				{
					auto parameter = items->vector[i];

					if (parameter)
					{
						auto meshParameterGroupList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parameter->klass, "get_MeshParameterGroupList", 0)->methodPointer(parameter);

						if (meshParameterGroupList)
						{
							FieldInfo* items1Field = il2cpp_class_get_field_from_name(meshParameterGroupList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* items1;
							il2cpp_field_get_value(meshParameterGroupList, items1Field, &items1);

							if (items1)
							{
								for (int j = 0; j < items1->max_length; j++)
								{
									auto group = items1->vector[j];

									if (group)
									{
										auto textureSetColor = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(group->klass, "get_TextureSetColor", 0)->methodPointer(group);

										if (textureSetColor)
										{
											auto uobject_name = UnityEngine::Object::Name(textureSetColor);
											if (!il2cppstring(uobject_name->chars).empty())
											{
												auto newTexture = GetReplacementAssets(
													uobject_name,
													GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
												if (newTexture)
												{
													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(group->klass, "set_TextureSetColor", 1)->methodPointer(group, newTexture);
												}
											}
										}

										auto textureSetAlpha = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(group->klass, "get_TextureSetAlpha", 0)->methodPointer(group);

										if (textureSetAlpha)
										{
											auto uobject_name = UnityEngine::Object::Name(textureSetAlpha);
											if (!il2cppstring(uobject_name->chars).empty())
											{
												auto newTexture = GetReplacementAssets(
													uobject_name,
													GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
												if (newTexture)
												{
													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(group->klass, "set_TextureSetAlpha", 1)->methodPointer(group, newTexture);
												}
											}
										}
									}
								}
							}
						}

						auto customMeshInfoParameterList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(parameter->klass, "get_CustomMeshInfoParameterList", 0)->methodPointer(parameter);

						if (customMeshInfoParameterList)
						{
							FieldInfo* items1Field = il2cpp_class_get_field_from_name(customMeshInfoParameterList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* items1;
							il2cpp_field_get_value(customMeshInfoParameterList, items1Field, &items1);

							if (items1)
							{
								for (int j = 0; j < items1->max_length; j++)
								{
									auto customParameter = items1->vector[j];

									if (customParameter)
									{
										auto textureColor = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(customParameter->klass, "get_TextureColor", 0)->methodPointer(customParameter);

										if (textureColor)
										{
											auto uobject_name = UnityEngine::Object::Name(textureColor);
											if (!il2cppstring(uobject_name->chars).empty())
											{
												auto newTexture = GetReplacementAssets(
													uobject_name,
													GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
												if (newTexture)
												{
													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(customParameter->klass, "set_TextureColor", 1)->methodPointer(customParameter, newTexture);
												}
											}
										}

										auto textureAlpha = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(customParameter->klass, "get_TextureAlpha", 0)->methodPointer(customParameter);

										if (textureAlpha)
										{
											auto uobject_name = UnityEngine::Object::Name(textureAlpha);
											if (!il2cppstring(uobject_name->chars).empty())
											{
												auto newTexture = GetReplacementAssets(
													uobject_name,
													GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
												if (newTexture)
												{
													il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(customParameter->klass, "set_TextureAlpha", 1)->methodPointer(customParameter, newTexture);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

static void ReplaceRendererTexture(Il2CppObject* renderer)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(renderer))
	{
		return;
	}

	auto count = il2cpp_resolve_icall_type<int (*)(Il2CppObject*)>("UnityEngine.Renderer::GetMaterialCount()")(renderer);

	if (count > 0)
	{
		Il2CppArraySize_t<Il2CppObject*>* materials = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Material"), count);
		il2cpp_resolve_icall_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>("UnityEngine.Renderer::CopySharedMaterialArray()")(renderer, materials);

		for (int i = 0; i < materials->max_length; i++)
		{
			auto material = materials->vector[i];
			if (material)
			{
				ReplaceMaterialTexture(material);
			}
		}
	}
}

static void ReplaceGameObjectTextures(Il2CppObject* gameObject, bool isChild = false);

static void ReplaceCutInTimelineControllerTextures(Il2CppObject* controller)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(controller))
	{
		return;
	}

	auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(controller->klass, "get_Data", 0)->methodPointer(controller);

	if (data)
	{
		auto worksheetList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(data->klass, "GetWorkSheetList", 0)->methodPointer(data);

		if (worksheetList)
		{
			FieldInfo* itemsField = il2cpp_class_get_field_from_name(worksheetList->klass, "_items");
			Il2CppArraySize_t<Il2CppObject*>* items;
			il2cpp_field_get_value(worksheetList, itemsField, &items);

			if (items)
			{
				for (int i = 0; i < items->max_length; i++)
				{
					auto worksheet = items->vector[i];

					if (worksheet)
					{
						auto effectListField = il2cpp_class_get_field_from_name(worksheet->klass, "_effectList");
						Il2CppObject* effectList;
						il2cpp_field_get_value(worksheet, effectListField, &effectList);

						if (effectList)
						{
							FieldInfo* items1Field = il2cpp_class_get_field_from_name(effectList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* items1;
							il2cpp_field_get_value(effectList, items1Field, &items1);


							if (items1)
							{
								for (int j = 0; j < items1->max_length; j++)
								{
									auto effectData = items1->vector[j];

									if (effectData)
									{
										auto keysField = il2cpp_class_get_field_from_name(effectData->klass, "_keys");
										Il2CppObject* keys;
										il2cpp_field_get_value(effectData, keysField, &keys);

										if (keys)
										{
											auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(keys->klass, "get_Prefab", 0)->methodPointer(keys);

											if (gameObject)
											{
												ReplaceGameObjectTextures(gameObject);
											}
										}
									}
								}
							}
						}

						auto bgListField = il2cpp_class_get_field_from_name(worksheet->klass, "_bgList");
						Il2CppObject* bgList;
						il2cpp_field_get_value(worksheet, bgListField, &bgList);

						if (bgList)
						{
							FieldInfo* items1Field = il2cpp_class_get_field_from_name(bgList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* items1;
							il2cpp_field_get_value(bgList, items1Field, &items1);


							if (items1)
							{
								for (int j = 0; j < items1->max_length; j++)
								{
									auto effectData = items1->vector[j];

									if (effectData)
									{
										auto keysField = il2cpp_class_get_field_from_name(effectData->klass, "_keys");
										Il2CppObject* keys;
										il2cpp_field_get_value(effectData, keysField, &keys);

										if (keys)
										{
											auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(keys->klass, "get_Prefab", 0)->methodPointer(keys);

											if (gameObject)
											{
												ReplaceGameObjectTextures(gameObject);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

static void ReplaceTransformTextures(Il2CppObject* transform)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(transform))
	{
		return;
	}

	auto childCount = il2cpp_class_get_method_from_name_type<int(*)(Il2CppObject*)>(transform->klass, "get_childCount", 0)->methodPointer(transform);

	if (!childCount)
	{
		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(transform->klass, "get_gameObject", 0)->methodPointer(transform);
		ReplaceGameObjectTextures(gameObject, true);
	}

	for (int i = 0; i < childCount; i++)
	{
		auto child = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(transform->klass, "GetChild", 1)->methodPointer(transform, i);

		ReplaceTransformTextures(child);
	}
}

static void ReplaceRawImageTexture(Il2CppObject* rawImage)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(rawImage))
	{
		return;
	}

	auto textureField = il2cpp_class_get_field_from_name(rawImage->klass, "m_Texture");
	Il2CppObject* texture;
	il2cpp_field_get_value(rawImage, textureField, &texture);

	if (texture)
	{
		auto uobject_name = UnityEngine::Object::Name(texture);
		if (uobject_name)
		{
			auto nameU8 = il2cpp_u8(uobject_name->chars);
			if (!nameU8.empty())
			{
				do
				{
					stringstream pathStream(nameU8);
					string segment;
					vector<string> split;
					while (getline(pathStream, segment, '/'))
					{
						split.emplace_back(segment);
					}
					auto& textureName = split.back();
					if (!textureName.empty())
					{
						auto texture2D = GetReplacementAssets(il2cpp_string_new(split.back().data()),
							GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
						if (texture2D)
						{
							il2cpp_field_set_value(rawImage, textureField, texture2D);
						}
					}
				} while (false);
			}
		}
	}
}

static void ReplaceImageTexture(Il2CppObject* image)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(image))
	{
		return;
	}

	auto spriteField = il2cpp_class_get_field_from_name(image->klass, "m_Sprite");
	Il2CppObject* sprite;
	il2cpp_field_get_value(image, spriteField, &sprite);

	if (sprite)
	{
		auto uobject_name = UnityEngine::Object::Name(sprite);
		if (uobject_name)
		{
			auto nameU8 = il2cpp_u8(uobject_name->chars);
			if (!nameU8.empty())
			{
				do
				{
					stringstream pathStream(nameU8);
					string segment;
					vector<string> split;
					while (getline(pathStream, segment, '/'))
					{
						split.emplace_back(segment);
					}
					auto& spriteName = split.back();
					if (!spriteName.empty())
					{
						auto spriteNew = GetReplacementAssets(il2cpp_string_new(split.back().data()),
							GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Sprite"));
						if (spriteNew)
						{
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppObject*)>(image->klass, "set_sprite", 1)->methodPointer(image, spriteNew);
						}
					}
				} while (false);
			}
		}
	}
}

static void ReplaceGameObjectTextures(Il2CppObject* gameObject, bool isChild)
{
	auto getComponent = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppType*)>(gameObject->klass, "GetComponent", 1)->methodPointer;
	auto getComponents =
		il2cpp_class_get_method_from_name_type<Il2CppArraySize_t<Il2CppObject*> *(*)(Il2CppObject*, Il2CppType*, bool, bool, bool, bool, Il2CppObject*)>(gameObject->klass, "GetComponentsInternal", 6)->methodPointer;

	auto array = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType(
		"UnityEngine.CoreModule.dll", "UnityEngine", "Object")), true, true, true, false, nullptr);

	if (array)
	{
		for (int i = 0; i < array->max_length; i++)
		{
			auto obj =
				il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, long index)>("mscorlib.dll", "System", "Array", "GetValue", 1)(array, i);

			if (!obj) continue;

			if (string(obj->klass->name).find("MeshRenderer") != string::npos)
			{
				ReplaceRendererTexture(obj);
			}
		}
	}

	auto rawImages = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop", "RawImageCommon")),
		true, true, true, false, nullptr);

	if (rawImages && rawImages->max_length)
	{
		for (int i = 0; i < rawImages->max_length; i++)
		{
			auto rawImage = rawImages->vector[i];
			if (rawImage)
			{
				ReplaceRawImageTexture(rawImage);
			}
		}
	}

	auto images = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop", "ImageCommon")),
		true, true, true, false, nullptr);

	if (images && images->max_length)
	{
		for (int i = 0; i < images->max_length; i++)
		{
			auto image = images->vector[i];
			if (image)
			{
				ReplaceImageTexture(image);
			}
		}
	}

	auto anRoots = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("Plugins.dll", "AnimateToUnity", "AnRoot")),
		true, true, true, false, nullptr);

	if (anRoots && anRoots->max_length)
	{
		for (int i = 0; i < anRoots->max_length; i++)
		{
			auto anRoot = anRoots->vector[i];
			if (anRoot)
			{
				ReplaceAnimateToUnityTextures(anRoot);
			}
		}
	}

	auto cutInTimelineControllers = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop.CutIn.Cutt", "CutInTimelineController")),
		true, true, true, false, nullptr);

	if (cutInTimelineControllers && cutInTimelineControllers->max_length)
	{
		for (int i = 0; i < cutInTimelineControllers->max_length; i++)
		{
			auto cutInTimelineController = cutInTimelineControllers->vector[i];
			if (cutInTimelineController)
			{
				ReplaceCutInTimelineControllerTextures(cutInTimelineController);
			}
		}
	}

	if (!isChild)
	{
		auto transforms = getComponents(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Transform")),
			true, true, true, false, nullptr);

		if (transforms && transforms->max_length)
		{
			for (int i = 0; i < transforms->max_length; i++)
			{
				auto transform = transforms->vector[i];
				if (transform)
				{
					ReplaceTransformTextures(transform);
				}
			}
		}
	}

	auto assetholder = getComponent(gameObject, reinterpret_cast<Il2CppType*>(GetRuntimeType("umamusume.dll", "Gallop", "AssetHolder")));

	if (assetholder)
	{
		ReplaceAssetHolderTextures(assetholder);
	}
}

static void ReplaceAtlasReferenceSprites(Il2CppObject* atlasReference)
{
	auto spritesField = il2cpp_class_get_field_from_name(atlasReference->klass, "sprites");
	Il2CppArraySize_t<Il2CppObject*>* sprites;
	il2cpp_field_get_value(atlasReference, spritesField, &sprites);

	if (sprites)
	{
		il2cppstringstream pathStream(UnityEngine::Object::Name(atlasReference)->chars);
		il2cppstring segment;
		vector<il2cppstring> splited;
		while (getline(pathStream, segment, IL2CPP_STRING('.')))
		{
			splited.emplace_back(segment);
		}

		auto& atlasName = splited.front();

		auto atlas = GetReplacementAtlasAssets(
			il2cpp_string_new16((IL2CPP_STRING("_") + atlasName).data()),
			GetRuntimeType(AtlasReferenceClass));

		if (!atlas)
		{
			return;
		}

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(atlas->klass, "OnEnable", 0)->methodPointer(atlas);

		auto newSprites = il2cpp_array_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Sprite"), sprites->max_length);

		for (int i = 0; i < sprites->max_length; i++)
		{
			auto sprite = sprites->vector[i];
			if (sprite)
			{
				auto uobject_name = UnityEngine::Object::Name(sprite);
				if (!il2cppstring(uobject_name->chars).empty())
				{
					auto newSprite = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*)>(atlas->klass, "GetSprite", 1)->methodPointer(atlas, uobject_name);

					if (newSprite)
					{
						il2cpp_array_setref_type(newSprites, Il2CppObject*, i, newSprite);
					}
					else
					{
						il2cpp_array_setref_type(newSprites, Il2CppObject*, i, sprite);
					}
				}
			}
		}

		il2cpp_field_set_value(atlasReference, spritesField, newSprites);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(atlasReference->klass, "OnEnable", 0)->methodPointer(atlasReference);
	}
}

static void ReplaceFontTexture(Il2CppObject* font)
{
	if (!UnityEngine::Object::IsNativeObjectAlive(font))
	{
		return;
	}

	auto material = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(font->klass, "get_material", 0)->methodPointer(font);

	if (material)
	{
		ReplaceMaterialTexture(material);
	}
}

static void ReplaceTimelineData(Il2CppObject* timelineData)
{
	auto objectName = UnityEngine::Object::Name(timelineData);

	il2cppstring timelinePath = config::replace_timeline_path;
	if (filesystem::path(timelinePath.data()).is_relative())
	{
		timelinePath.insert(0, filesystem::current_path().IL2CPP_BASIC_STRING().append(IL2CPP_STRING("/")));
	}

	auto filePath = filesystem::path(timelinePath + IL2CPP_STRING("/") + objectName->chars + IL2CPP_STRING(".msgpack"));

	auto object = MsgPack();
	if (filesystem::exists(filePath))
	{
		ifstream fileStream(filePath, ios::in | ios::binary);
		object = MsgPack::parse(fileStream);
	}

	MsgPack::array ReplacementBlockList = object.is_null() ? MsgPack::array{} : object["BlockList"].array_items();

	auto StoryTimelineDataClass_TitleField = il2cpp_class_get_field_from_name(timelineData->klass, "Title");
	Il2CppString* TimelineTitle;

	if (object.is_null())
	{
		il2cpp_field_get_value(timelineData, StoryTimelineDataClass_TitleField, &TimelineTitle);
	}
	else
	{
		TimelineTitle = il2cpp_string_new(object["TItle"].string_value().data());
	}
	il2cpp_field_set_value(timelineData, StoryTimelineDataClass_TitleField, local::get_localized_string(TimelineTitle));

	auto StoryTimelineDataClass_BlockListField = il2cpp_class_get_field_from_name(timelineData->klass, "BlockList");

	Il2CppObject* blockList;
	il2cpp_field_get_value(timelineData, StoryTimelineDataClass_BlockListField, &blockList);

	Il2CppArraySize_t<Il2CppObject*>* blockArray;
	il2cpp_field_get_value(blockList, il2cpp_class_get_field_from_name(blockList->klass, "_items"), &blockArray);

	auto StoryTimelineTextClipDataClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineTextClipData");
	auto NameField = il2cpp_class_get_field_from_name(StoryTimelineTextClipDataClass, "Name");
	auto TextField = il2cpp_class_get_field_from_name(StoryTimelineTextClipDataClass, "Text");
	auto ChoiceDataListField = il2cpp_class_get_field_from_name(StoryTimelineTextClipDataClass, "ChoiceDataList");
	auto ColorTextInfoListField = il2cpp_class_get_field_from_name(StoryTimelineTextClipDataClass, "ColorTextInfoList");
	auto AdditionalGradientListField = il2cpp_class_get_field_from_name(StoryTimelineTextClipDataClass, "AdditionalGradientList");

	auto BlockListSize = object.is_null() ? blockArray->max_length : ReplacementBlockList.size();

	for (size_t i = 0; i < BlockListSize; i++)
	{
		MsgPack::object ReplacementBlockData = object.is_null() ? MsgPack::object{} : ReplacementBlockList[i].object_items();
		auto blockData = blockArray->vector[i];

		MsgPack::object ReplacementTextTrack = object.is_null() ? MsgPack::object{} : ReplacementBlockData["TextTrack"].object_items();
		MsgPack::array ReplacementClipList = object.is_null() ? MsgPack::array{} : ReplacementTextTrack["ClipList"].array_items();

		auto TextTrackField = il2cpp_class_get_field_from_name(blockData->klass, "TextTrack");
		Il2CppObject* TextTrack;
		il2cpp_field_get_value(blockData, TextTrackField, &TextTrack);

		auto ClipListField = il2cpp_class_get_field_from_name(TextTrack->klass, "ClipList");
		Il2CppObject* ClipList;
		il2cpp_field_get_value(TextTrack, ClipListField, &ClipList);

		Il2CppArraySize_t<Il2CppObject*>* ClipArray;
		il2cpp_field_get_value(ClipList, il2cpp_class_get_field_from_name(ClipList->klass, "_items"), &ClipArray);

		auto ClipListSize = object.is_null() ? ClipArray->max_length : ReplacementClipList.size();

		for (size_t i = 0; i < ClipListSize; i++)
		{
			MsgPack::object ReplacementClipData = object.is_null() ? MsgPack::object{} : ReplacementClipList[i].object_items();
			auto clipData = ClipArray->vector[i];

			Il2CppString* ClipDataName;
			if (object.is_null())
			{
				il2cpp_field_get_value(clipData, NameField, &ClipDataName);
			}
			else
			{
				ClipDataName = il2cpp_string_new(ReplacementClipData["Name"].string_value().data());
			}
			il2cpp_field_set_value(clipData, NameField, local::get_localized_string(ClipDataName));

			Il2CppString* ClipDataText;
			if (object.is_null())
			{
				il2cpp_field_get_value(clipData, TextField, &ClipDataText);
			}
			else
			{
				ClipDataText = il2cpp_string_new(ReplacementClipData["Text"].string_value().data());
			}
			il2cpp_field_set_value(clipData, TextField, local::get_localized_string(ClipDataText));

			Il2CppObject* choiceDataList;
			il2cpp_field_get_value(clipData, ChoiceDataListField, &choiceDataList);

			if (choiceDataList)
			{
				MsgPack::array ReplacementChoiceDataList = object.is_null() ? MsgPack::array{} : ReplacementClipData["ChoiceDataList"].array_items();
				Il2CppArraySize_t<Il2CppObject*>* choiceDataArray;
				il2cpp_field_get_value(choiceDataList, il2cpp_class_get_field_from_name(choiceDataList->klass, "_items"), &choiceDataArray);

				auto ChoiceDataListSize = object.is_null() ? choiceDataArray->max_length : ReplacementChoiceDataList.size();

				for (size_t i = 0; i < ChoiceDataListSize; i++)
				{
					MsgPack::object ReplacementChoiceData = object.is_null() ? MsgPack::object{} : ReplacementChoiceDataList[i].object_items();
					auto choiceData = choiceDataArray->vector[i];
					auto choiceDataTextField = il2cpp_class_get_field_from_name(choiceData->klass, "Text");

					Il2CppString* ChoiceDataText;
					if (object.is_null())
					{
						il2cpp_field_get_value(choiceData, choiceDataTextField, &ChoiceDataText);
					}
					else
					{
						ChoiceDataText = il2cpp_string_new(ReplacementChoiceData["Text"].string_value().data());
					}
					il2cpp_field_set_value(choiceData, choiceDataTextField, local::get_localized_string(ChoiceDataText));
				}
			}

			Il2CppObject* colorTextInfoList;
			il2cpp_field_get_value(clipData, ColorTextInfoListField, &colorTextInfoList);

			if (colorTextInfoList)
			{
				MsgPack::array ReplacementColorTextInfoList = object.is_null() ? MsgPack::array{} : ReplacementClipData["ColorTextInfoList"].array_items();
				Il2CppArraySize_t<Il2CppObject*>* colorTextInfoArray;
				il2cpp_field_get_value(colorTextInfoList, il2cpp_class_get_field_from_name(colorTextInfoList->klass, "_items"), &colorTextInfoArray);

				auto ColorTextInfoListSize = object.is_null() ? colorTextInfoArray->max_length : ReplacementColorTextInfoList.size();

				for (size_t i = 0; i < ColorTextInfoListSize; i++)
				{
					MsgPack::object ReplacementColorTextInfo = object.is_null() ? MsgPack::object{} : ReplacementColorTextInfoList[i].object_items();
					auto colorTextInfo = colorTextInfoArray->vector[i];
					auto colorTextInfoTextField = il2cpp_class_get_field_from_name(colorTextInfo->klass, "Text");

					Il2CppString* ColorTextInfoText;
					if (object.is_null())
					{
						il2cpp_field_get_value(colorTextInfo, colorTextInfoTextField, &ColorTextInfoText);
					}
					else
					{
						ColorTextInfoText = il2cpp_string_new(ReplacementColorTextInfo["Text"].string_value().data());
					}
					il2cpp_field_set_value(colorTextInfo, colorTextInfoTextField, local::get_localized_string(ColorTextInfoText));
				}
			}

			Il2CppObject* additionalGradientList;
			il2cpp_field_get_value(clipData, AdditionalGradientListField, &additionalGradientList);

			if (additionalGradientList)
			{
				MsgPack::array ReplacementAdditionalGradientList = object.is_null() ? MsgPack::array{} : ReplacementClipData["AdditionalGradientList"].array_items();
				Il2CppArraySize_t<Il2CppObject*>* additionalGradientArray;
				il2cpp_field_get_value(additionalGradientList, il2cpp_class_get_field_from_name(additionalGradientList->klass, "_items"), &additionalGradientArray);

				auto AdditionalGradientListSize = object.is_null() ? additionalGradientArray->max_length : ReplacementAdditionalGradientList.size();

				for (size_t i = 0; i < AdditionalGradientListSize; i++)
				{
					MsgPack::object ReplacementAdditionalGradient = object.is_null() ? MsgPack::object{} : ReplacementAdditionalGradientList[i].object_items();
					auto additionalGradient = additionalGradientArray->vector[i];
					auto additionalGradientColorTextField = il2cpp_class_get_field_from_name(additionalGradient->klass, "ColorText");

					Il2CppString* AdditionalGradientColorText;
					if (object.is_null())
					{
						il2cpp_field_get_value(additionalGradient, additionalGradientColorTextField, &AdditionalGradientColorText);
					}
					else
					{
						AdditionalGradientColorText = il2cpp_string_new(ReplacementAdditionalGradient["ColorText"].string_value().data());
					}
					il2cpp_field_set_value(additionalGradient, additionalGradientColorTextField, local::get_localized_string(AdditionalGradientColorText));
				}
			}
		}
	}
}

static Il2CppObject* LoadAsset_Internal_hook(Il2CppObject* self, Il2CppString* name, Il2CppReflectionType* type);

static Il2CppObject* GetReplacementAssets(Il2CppString* name, Il2CppReflectionType* type)
{
	for (auto it = config::runtime::replaceAssets.begin(); it != config::runtime::replaceAssets.end(); it++)
	{
		auto assets = reinterpret_cast<decltype(LoadAsset_Internal_hook)*>(LoadAsset_Internal_orig)(*it, name, type);
		if (assets)
		{
			return assets;
		}
	}

	return nullptr;
}

static Il2CppObject* GetReplacementAtlasAssets(Il2CppString* name, Il2CppReflectionType* type)
{
	if (config::runtime::replaceAtlas)
	{
		auto assets = reinterpret_cast<decltype(LoadAsset_Internal_hook)*>(LoadAsset_Internal_orig)(config::runtime::replaceAtlas, name, type);
		if (assets)
		{
			return assets;
		}
	}

	return nullptr;
}

static Il2CppObject* LoadAssetAsync_Internal_hook(Il2CppObject* self, Il2CppString* name, Il2CppReflectionType* type);

static Il2CppObject* GetReplacementAssetsAsync(Il2CppString* name, Il2CppReflectionType* type)
{
	for (auto it = config::runtime::replaceAssets.begin(); it != config::runtime::replaceAssets.end(); it++)
	{
		auto assets = reinterpret_cast<decltype(LoadAssetAsync_Internal_hook)*>(LoadAssetAsync_Internal_orig)(*it, name, type);
		if (assets)
		{
			return assets;
		}
	}

	return nullptr;
}

static Il2CppObject* LoadFromFile_Internal_hook(Il2CppString* path, uint32_t crc, uint64_t offset)
{
	il2cppstringstream pathStream(path->chars);
	il2cppstring segment;
	vector<il2cppstring> splited;
	while (getline(pathStream, segment, IL2CPP_STRING('\\')))
	{
		splited.emplace_back(segment);
	}

	auto& name = splited.back();
	if (config::replace_assets.find(name) != config::replace_assets.end())
	{
		auto& replaceAsset = config::replace_assets.at(name);
		auto assets = reinterpret_cast<decltype(LoadFromFile_Internal_hook)*>(LoadFromFile_Internal_orig)(il2cpp_string_new16(replaceAsset.path.data()), crc, offset);
		replaceAsset.asset = assets;
		return assets;
	}

	auto assetBundle = reinterpret_cast<decltype(LoadFromFile_Internal_hook)*>(LoadFromFile_Internal_orig)(path, crc, offset);
	return assetBundle;
}

static Il2CppObject* LoadFromStreamInternal_hook(Il2CppObject* stream, uint32_t crc, uint32_t managedReadBufferSize)
{
	return reinterpret_cast<decltype(LoadFromStreamInternal_hook)*>(LoadFromStreamInternal_orig)(stream, crc, managedReadBufferSize);
}

static Il2CppObject* LoadAsset_Internal_hook(Il2CppObject* self, Il2CppString* name, Il2CppReflectionType* type)
{
	il2cppstringstream pathStream(name->chars);
	il2cppstring segment;
	vector<il2cppstring> splited;
	while (getline(pathStream, segment, IL2CPP_STRING('/')))
	{
		splited.emplace_back(segment);
	}

	auto& fileName = splited.back();
	if (find_if(config::runtime::replaceAssetNames.begin(), config::runtime::replaceAssetNames.end(), [fileName](const il2cppstring& item)
		{
			return item.find(fileName) != il2cppstring::npos;
		}) != config::runtime::replaceAssetNames.end())
	{
		auto result = GetReplacementAssets(il2cpp_string_new16(fileName.data()), type);

		if (result)
		{
			return result;
		}
	}

	auto obj = reinterpret_cast<decltype(LoadAsset_Internal_hook)*>(LoadAsset_Internal_orig)(self, name, type);

	if (!obj)
	{
		return nullptr;
	}

	if (obj->klass == AtlasReferenceClass)
	{
		ReplaceAtlasReferenceSprites(obj);
	}

	if (obj->klass == GameObjectClass)
	{
		ReplaceGameObjectTextures(obj);
	}

	if (obj->klass == MaterialClass)
	{
		ReplaceMaterialTexture(obj);
	}

	if (obj->klass == FontClass)
	{
		ReplaceFontTexture(obj);
	}

	if (obj->klass == StoryTimelineDataClass)
	{
		ReplaceTimelineData(obj);
	}

	return obj;
}

static Il2CppObject* LoadAssetAsync_Internal_hook(Il2CppObject* self, Il2CppString* name, Il2CppReflectionType* type)
{
	il2cppstringstream pathStream(name->chars);
	il2cppstring segment;
	vector<il2cppstring> splited;
	while (getline(pathStream, segment, IL2CPP_STRING('/')))
	{
		splited.emplace_back(segment);
	}
	auto& fileName = splited.back();
	if (find_if(config::runtime::replaceAssetNames.begin(), config::runtime::replaceAssetNames.end(), [fileName](const il2cppstring& item)
		{
			return item.find(fileName) != il2cppstring::npos;
		}) != config::runtime::replaceAssetNames.end())
	{
		return GetReplacementAssetsAsync(il2cpp_string_new16(fileName.data()), type);
	}
	return reinterpret_cast<decltype(LoadAssetAsync_Internal_hook)*>(LoadAssetAsync_Internal_orig)(self, name, type);
}

static Il2CppObject* AssetBundleRequest_GetResult_hook(Il2CppObject* self)
{
	auto obj = reinterpret_cast<decltype(AssetBundleRequest_GetResult_hook)*>(AssetBundleRequest_GetResult_orig)(self);

	if (obj->klass == StoryTimelineDataClass)
	{
		ReplaceTimelineData(obj);
	}
	else if (obj)
	{
		auto name = UnityEngine::Object::Name(obj);
		il2cppstring wName = name->chars;
		if (find(config::runtime::replaceAssetNames.begin(), config::runtime::replaceAssetNames.end(), wName) != config::runtime::replaceAssetNames.end())
		{
			return GetReplacementAssets(name, reinterpret_cast<Il2CppReflectionType*>(il2cpp_type_get_object(il2cpp_class_get_type(obj->klass))));
		}

		if (obj->klass == GameObjectClass)
		{
			ReplaceGameObjectTextures(obj);
		}
	}

	return obj;
}

static void Unload_hook(Il2CppObject* self, bool unloadAllLoadedObjects)
{
	for (auto& pair : config::replace_assets)
	{
		if (pair.second.asset == self)
		{
			reinterpret_cast<decltype(Unload_hook)*>(Unload_orig)(self, unloadAllLoadedObjects);
			pair.second.asset = nullptr;
			return;
		}
	}

	reinterpret_cast<decltype(Unload_hook)*>(Unload_orig)(self, unloadAllLoadedObjects);
}

static void InitAddress()
{
	LoadFromFile_Internal_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadFromFile_Internal(System.String,System.UInt32,System.UInt64)");
	LoadFromStreamInternal_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadFromStreamInternal()");
	LoadAsset_Internal_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)");
	LoadAssetAsync_Internal_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAssetAsync_Internal(System.String,System.Type)");
	AssetBundleRequest_GetResult_addr = il2cpp_resolve_icall("UnityEngine.AssetBundleRequest::GetResult()");
	GetAllAssetNames_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::GetAllAssetNames()");
	Unload_addr = il2cpp_resolve_icall("UnityEngine.AssetBundle::Unload()");

	AtlasReferenceClass = il2cpp_symbols::get_class("Cute.UI.Assembly.dll", "Cute.UI", "AtlasReference");
	GameObjectClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject");
	MaterialClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Material");
	FontClass = il2cpp_symbols::get_class("UnityEngine.TextRenderingModule.dll", "UnityEngine", "Font");
	StoryTimelineDataClass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "StoryTimelineData");
}

static void HookMethods()
{
	ADD_HOOK(LoadFromFile_Internal, "UnityEngine.AssetBundle::LoadFromFile_Internal at %p\n");
	// ADD_HOOK(LoadFromStreamInternal, "UnityEngine.AssetBundle::LoadFromStreamInternal at %p\n");
	ADD_HOOK(LoadAsset_Internal, "UnityEngine.AssetBundle::LoadAsset_Internal at %p\n");
	ADD_HOOK(LoadAssetAsync_Internal, "UnityEngine.AssetBundle::LoadAssetAsync_Internal at %p\n");
	ADD_HOOK(AssetBundleRequest_GetResult, "UnityEngine.AssetBundleRequest::GetResult at %p\n");
	ADD_HOOK(Unload, "UnityEngine.AssetBundle::Unload at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	AssetBundle AssetBundle::LoadFromFile(Il2CppString* path)
	{
		auto obj = reinterpret_cast<decltype(LoadFromFile_Internal_hook)*>(LoadFromFile_Internal_orig)(path, 0, 0);
		return AssetBundle{ obj };
	}

	Il2CppObject* AssetBundle::LoadAsset(Il2CppString* name, Il2CppReflectionType* runtimeType)
	{
		return reinterpret_cast<decltype(LoadAsset_Internal_hook)*>(LoadAsset_Internal_orig)(instance, name, runtimeType);
	}

	Il2CppObject* AssetBundle::LoadAssetAsync(Il2CppString* name, Il2CppReflectionType* runtimeType)
	{
		return reinterpret_cast<decltype(LoadAssetAsync_Internal_hook)*>(LoadAssetAsync_Internal_orig)(instance, name, runtimeType);
	}

	Il2CppArraySize_t<Il2CppString*>* AssetBundle::GetAllAssetNames()
	{
		return reinterpret_cast<Il2CppArraySize_t<Il2CppString*>*(*)(Il2CppObject*)>(GetAllAssetNames_addr)(instance);
	}

	void AssetBundle::Unload(bool unloadAllLoadedObjects)
	{
		reinterpret_cast<decltype(Unload_hook)*>(Unload_orig)(instance, unloadAllLoadedObjects);
	}
}
