#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_map>
#include <queue>
#include <SQLiteCpp/SQLiteCpp.h>
#include <msgpack11.hpp>

#include "config/config.hpp"

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "local/local.hpp"
#include "game.hpp"

#include "masterdb/masterdb.hpp"

#include "hook.h"

#include "notification/DesktopNotificationManagerCompat.h"

using namespace std;
using namespace msgpack11;
using namespace Microsoft::WRL;

namespace MsgPackData
{
	MsgPack::object user_info;

	MsgPack::object tp_info;
	MsgPack::object rp_info;

	Il2CppString* GetIconPath(int unitId)
	{
		return il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*, int)>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "PushNotificationManager"), "createFavIconFilePath", 1)->methodPointer(nullptr, unitId);
	}

	void DumpTexture2D(int unitId, Il2CppObject* texture)
	{
		auto width = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_width", 0)->methodPointer(texture);

		auto height = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_height", 0)->methodPointer(texture);

		auto renderTexture = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int, int, int, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "GetTemporary", 4)(width, height, 0, 0);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Graphics", "Blit", 2)(texture, renderTexture);

		auto previous = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "get_active", IgnoreNumberOfArguments)();

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "set_active", 1)(renderTexture);

		auto readableTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(readableTexture->klass, ".ctor", 2)->methodPointer(readableTexture, width, height);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Rect_t, int, int)>(readableTexture->klass, "ReadPixels", 3)->methodPointer(readableTexture, Rect_t{ 0, 0, static_cast<float>(width), static_cast<float>(height) }, 0, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(readableTexture->klass, "Apply", 0)->methodPointer(readableTexture);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "set_active", 1)(previous);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "ReleaseTemporary", 1)(renderTexture);

		auto method = il2cpp_symbols::get_method("UnityEngine.ImageConversionModule.dll", "UnityEngine", "ImageConversion", "EncodeToPNG", 1);

		void** params = new void* [1];
		params[0] = readableTexture;

		Il2CppException* exception;

		auto pngData = reinterpret_cast<Il2CppArraySize_t<uint8_t>*>(il2cpp_runtime_invoke(method, nullptr, params, &exception));

		delete[] params;

		if (exception)
		{
			wcout << "EncodeToPNG Error: " << exception->message << endl;
			return;
		}

		auto wPath = GetIconPath(unitId);
		wstring path = wPath->chars;
		auto parentDir = filesystem::path(path).parent_path();

		if (!filesystem::exists(parentDir))
		{
			filesystem::create_directories(parentDir);
		}

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, Il2CppArraySize_t<uint8_t>*)>("mscorlib.dll", "System.IO", "File", "WriteAllBytes", 2)(il2cpp_string_new_utf16(path.data(), path.size()), pngData);
	}

	void RegisterTPScheduledToast()
	{
		if (!MsgPackData::user_info.empty() || !MsgPackData::tp_info.empty())
		{
			int leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
			int leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

			auto old_str = to_string(leader_chara_dress_id);
			size_t n_zero = 6;
			auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;

			auto push_icon = "push_icon_"s + to_string(leader_chara_id) + "_" + new_str;
			auto path = "chara/chr"s + to_string(leader_chara_id) + "/" + push_icon;

			auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
			auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path.data()), false);
			if (!asset)
			{
				return;
			}

			auto assetBundle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)->methodPointer(asset);
			if (!assetBundle)
			{
				return;
			}

			auto texture2DType = reinterpret_cast<Il2CppType*>(::GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			auto texture2D = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, const Il2CppType*)>(il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)"))(assetBundle, il2cpp_string_new(push_icon.data()), texture2DType);

			DumpTexture2D(leader_chara_id, texture2D);

			if (config::notification_tp)
			{
				// unique_ptr<DesktopNotificationHistoryCompat> history;
				// DesktopNotificationManagerCompat::get_History(&history);

				if ((!MsgPackData::user_info.empty() || !MsgPackData::tp_info.empty()) && config::notification_tp)
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
					auto title = local::u8_wide(MasterDB::GetTextData(6, leader_chara_id));
					auto content = local::u8_wide(MasterDB::GetTextData(184, leader_chara_id));
					// history->RemoveGroupedTag(L"TP", L"Generic");
					DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"TP", GetIconPath(leader_chara_id)->chars, MsgPackData::tp_info["max_recovery_time"].int64_value() * 1000);
				}
			}
		}
	}

	void RegisterRPScheduledToast()
	{
		if (!MsgPackData::user_info.empty() || !MsgPackData::rp_info.empty())
		{
			int leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
			int leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

			auto old_str = to_string(leader_chara_dress_id);
			size_t n_zero = 6;
			auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;

			auto push_icon = "push_icon_"s + to_string(leader_chara_id) + "_" + new_str;
			auto path = "chara/chr"s + to_string(leader_chara_id) + "/" + push_icon;

			auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
			auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path.data()), false);
			if (!asset)
			{
				return;
			}

			auto assetBundle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)->methodPointer(asset);
			if (!assetBundle)
			{
				return;
			}

			auto texture2DType = reinterpret_cast<Il2CppType*>(::GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			auto texture2D = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, const Il2CppType*)>(il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)"))(assetBundle, il2cpp_string_new(push_icon.data()), texture2DType);

			DumpTexture2D(leader_chara_id, texture2D);

			if (config::notification_rp)
			{
				// unique_ptr<DesktopNotificationHistoryCompat> history;
				// DesktopNotificationManagerCompat::get_History(&history);

				if ((!MsgPackData::user_info.empty() || !MsgPackData::rp_info.empty()) && config::notification_rp)
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
					auto title = local::u8_wide(MasterDB::GetTextData(6, leader_chara_id));
					auto content = local::u8_wide(MasterDB::GetTextData(185, leader_chara_id));
					// history->RemoveGroupedTag(L"TP", L"Generic");
					DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"RP", GetIconPath(leader_chara_id)->chars, MsgPackData::rp_info["max_recovery_time"].int64_value() * 1000);
				}
			}
		}
	}

	void ReadRequest(const char* data, size_t size)
	{
		string error;
		auto parsed = MsgPack::parse(data, size, error);

		if (error.empty())
		{
			if (parsed.is_object())
			{
				MsgPack::object object = parsed.object_items();
			}
		}
	}

	void ReadResponse(const char* data, size_t size)
	{
		string error;
		auto parsed = MsgPack::parse(data, size, error);

		if (error.empty())
		{
			if (parsed.is_object())
			{
				MsgPack::object object = parsed.object_items();

				if (object["data"].is_object())
				{
					MsgPack::object data = object["data"].object_items();

					if (data["tp_info"].is_object() || data["rp_info"].is_object() || data["user_info"].is_object())
					{
						int leader_chara_id;
						int leader_chara_dress_id;

						if (data["user_info"].is_object())
						{
							MsgPackData::user_info = data["user_info"].object_items();
						}

						if (data["tp_info"].is_object())
						{
							MsgPackData::tp_info = data["tp_info"].object_items();
						}

						if (data["rp_info"].is_object())
						{
							MsgPackData::rp_info = data["rp_info"].object_items();
						}

						leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
						leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

						auto old_str = to_string(leader_chara_dress_id);
						size_t n_zero = 6;
						auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;

						auto push_icon = "push_icon_"s + to_string(leader_chara_id) + "_" + new_str;
						auto path = "chara/chr"s + to_string(leader_chara_id) + "/" + push_icon;

						auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
						auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path.data()), false);

						if (!asset)
						{
							return;
						}

						auto assetBundle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)->methodPointer(asset);

						auto texture2DType = reinterpret_cast<Il2CppType*>(::GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
						auto texture2D = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, const Il2CppType*)>(il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)"))(assetBundle, il2cpp_string_new(push_icon.data()), texture2DType);

						DumpTexture2D(leader_chara_id, texture2D);

						if (config::notification_tp || config::notification_rp)
						{
							// unique_ptr<DesktopNotificationHistoryCompat> history;
							// DesktopNotificationManagerCompat::get_History(&history);

							auto title = local::u8_wide(MasterDB::GetTextData(6, leader_chara_id));
							if ((data["user_info"].is_object() || data["tp_info"].is_object()) && config::notification_tp)
							{
								DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
								auto content = local::u8_wide(MasterDB::GetTextData(184, leader_chara_id));
								// history->RemoveGroupedTag(L"TP", L"Generic");
								DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"TP", GetIconPath(leader_chara_id)->chars, MsgPackData::tp_info["max_recovery_time"].int64_value() * 1000);
							}
							if ((data["user_info"].is_object() || data["rp_info"].is_object()) && config::notification_rp)
							{
								DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
								auto content = local::u8_wide(MasterDB::GetTextData(185, leader_chara_id));
								// history->RemoveGroupedTag(L"RP", L"Generic");
								DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"RP", GetIconPath(leader_chara_id)->chars, MsgPackData::rp_info["max_recovery_time"].int64_value() * 1000);
							}
						}
					}
				}
			}
		}
	}
}
