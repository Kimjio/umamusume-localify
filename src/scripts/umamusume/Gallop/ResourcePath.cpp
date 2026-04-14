#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "ResourcePath.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"

#include "config/config.hpp"

namespace
{
	void* ResourcePath_GetPreInTitleMoviePath_addr = nullptr;
	void* ResourcePath_GetPreInTitleMoviePath_orig = nullptr;

	void* ResourcePath_GetLatestAssetBundleTitleMoviePath_addr = nullptr;
	void* ResourcePath_GetLatestAssetBundleTitleMoviePath_orig = nullptr;
}

static Il2CppString* ResourcePath_GetPreInTitleMoviePath_hook()
{
	auto GameDefine = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "GameDefine");
	auto PREIN_TITLE_OP_VERSION_Field = il2cpp_class_get_field_from_name(GameDefine, "PREIN_TITLE_OP_VERSION");
	uint64_t PREIN_TITLE_OP_VERSION;
	il2cpp_field_static_get_value(PREIN_TITLE_OP_VERSION_Field, &PREIN_TITLE_OP_VERSION);

	auto ResourcePath = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "ResourcePath");

	auto _stringBuilderField = il2cpp_class_get_field_from_name(ResourcePath, "_stringBuilder");
	Il2CppObject* _stringBuilder;
	il2cpp_field_static_get_value(_stringBuilderField, &_stringBuilder);
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

	auto boxedOpVersion = il2cpp_value_box(il2cpp_defaults.int32_class, &PREIN_TITLE_OP_VERSION);

	Il2CppString* titleMovieFormat = nullptr;

	auto TITLE_MOVIE_FORMAT_DEFAULT_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT_DEFAULT");

	if (TITLE_MOVIE_FORMAT_DEFAULT_Field)
	{
		Il2CppString* TITLE_MOVIE_FORMAT_DEFAULT;
		il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_DEFAULT_Field, &TITLE_MOVIE_FORMAT_DEFAULT);

		auto TITLE_MOVIE_FORMAT_LANDSCAPE_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT_LANDSCAPE");
		Il2CppString* TITLE_MOVIE_FORMAT_LANDSCAPE;
		il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_LANDSCAPE_Field, &TITLE_MOVIE_FORMAT_LANDSCAPE);

		if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
		{
			titleMovieFormat = TITLE_MOVIE_FORMAT_LANDSCAPE;
		}
		else
		{
			titleMovieFormat = TITLE_MOVIE_FORMAT_DEFAULT;
		}
	}
	else
	{
		auto TITLE_MOVIE_FORMAT_Field = il2cpp_class_get_field_from_name(ResourcePath, "TITLE_MOVIE_FORMAT");
		Il2CppString* TITLE_MOVIE_FORMAT;
		il2cpp_field_static_get_value(TITLE_MOVIE_FORMAT_Field, &TITLE_MOVIE_FORMAT);

		titleMovieFormat = TITLE_MOVIE_FORMAT;

		if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
		{
			auto streamingAssetsPath = UnityEngine::Application::streamingAssetsPath()->chars;

			il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);
			auto titleMovie = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder)->chars;
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

			if (filesystem::exists(streamingAssetsPath + il2cppstring(IL2CPP_STRING("/PreinResource/Movie/m/")) + titleMovie + IL2CPP_STRING("_landscape.usm")))
			{
				titleMovieFormat = il2cpp_string_new("Title/title_movie_{0:D2}_landscape");
			}
			else
			{
				titleMovieFormat = TITLE_MOVIE_FORMAT;
			}
		}
		else
		{
			titleMovieFormat = TITLE_MOVIE_FORMAT;
		}
	}

	il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);

	return il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder);
}

static Il2CppString* ResourcePath_GetLatestAssetBundleTitleMoviePath_hook()
{
	auto SaveDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SaveDataManager"));
	auto SaveLoader = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(SaveDataManager->klass, "get_SaveLoader", 0)->methodPointer(SaveDataManager);
	int NewOpProgress = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(SaveLoader->klass, "get_NewOpProgress", 0)->methodPointer(SaveLoader);

	auto ResourcePath = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "ResourcePath");

	auto _stringBuilderField = il2cpp_class_get_field_from_name(ResourcePath, "_stringBuilder");
	Il2CppObject* _stringBuilder;
	il2cpp_field_static_get_value(_stringBuilderField, &_stringBuilder);
	il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

	auto boxedOpVersion = il2cpp_value_box(il2cpp_defaults.int32_class, &NewOpProgress);

	Il2CppString* titleMovieFormat = nullptr;

	auto ASSET_BUNDLE_TITLE_MOVIE_FORMAT_Field = il2cpp_class_get_field_from_name(ResourcePath, "ASSET_BUNDLE_TITLE_MOVIE_FORMAT");
	Il2CppString* ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
	il2cpp_field_static_get_value(ASSET_BUNDLE_TITLE_MOVIE_FORMAT_Field, &ASSET_BUNDLE_TITLE_MOVIE_FORMAT);

	titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;

	if (UnityEngine::Screen::width() > UnityEngine::Screen::height())
	{
		auto streamingAssetsPath = UnityEngine::Application::streamingAssetsPath()->chars;

		il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);
		auto titleMovie = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder)->chars;
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(_stringBuilder->klass, "set_Length", 1)->methodPointer(_stringBuilder, 0);

		if (filesystem::exists(streamingAssetsPath + il2cppstring(IL2CPP_STRING("/PreinResource/Movie/m/")) + titleMovie + IL2CPP_STRING("_landscape.usm")))
		{
			titleMovieFormat = il2cpp_string_new("Title/title_movie_{0:D2}_landscape");
		}
		else
		{
			titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
		}
	}
	else
	{
		titleMovieFormat = ASSET_BUNDLE_TITLE_MOVIE_FORMAT;
	}

	il2cpp_class_get_method_from_name_type<Il2CppObject* (*)(Il2CppObject*, Il2CppString*, Il2CppObject*)>(_stringBuilder->klass, "AppendFormat", 2)->methodPointer(_stringBuilder, titleMovieFormat, boxedOpVersion);

	return il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(_stringBuilder->klass, "ToString", 0)->methodPointer(_stringBuilder);
}

static void InitAddress()
{
	auto ResourcePath_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "ResourcePath");
	ResourcePath_GetPreInTitleMoviePath_addr = il2cpp_symbols::get_method_pointer(ResourcePath_klass, "GetPreInTitleMoviePath", 0);
	ResourcePath_GetLatestAssetBundleTitleMoviePath_addr = il2cpp_symbols::get_method_pointer(ResourcePath_klass, "GetLatestAssetBundleTitleMoviePath", 0);
}

static void HookMethods()
{
	if (config::freeform_window)
	{
		ADD_HOOK(ResourcePath_GetPreInTitleMoviePath, "Gallop.ResourcePath::GetPreInTitleMoviePath at %p\n");
		ADD_HOOK(ResourcePath_GetLatestAssetBundleTitleMoviePath, "Gallop.ResourcePath::GetLatestAssetBundleTitleMoviePath at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
