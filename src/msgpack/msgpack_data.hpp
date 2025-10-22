#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_map>
#include <queue>
#include <msgpack11.hpp>

#include "config/config.hpp"

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "string_utils.hpp"
#include "game.hpp"

#include "masterdb/masterdb.hpp"

#include "notification/DesktopNotificationManagerCompat.h"

#include "scripts/ScriptInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "scripts/umamusume/Gallop/PushNotificationManager.hpp"

using namespace std;
using namespace msgpack11;
using namespace Microsoft::WRL;

namespace MsgPackData
{
	MsgPack::object user_info;

	MsgPack::object tp_info;
	MsgPack::object rp_info;

	MsgPack::array jobs_going_info_array;

	Il2CppString* GetIconPath(Gallop::LocalPushDefine::LocalPushType localPushType)
	{
		return Gallop::PushNotificationManager::Instance().CreatePushIconFilePath(localPushType);
	}

	void DumpTexture2D(int unitId, Gallop::LocalPushDefine::LocalPushType localPushType, Il2CppObject* texture)
	{
		auto width = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_width", 0)->methodPointer(texture);

		auto height = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture->klass, "get_height", 0)->methodPointer(texture);

		auto renderTexture = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int, int, int, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "GetTemporary", 4)(width, height, 0, 0);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Graphics", "Blit", 2)(texture, renderTexture);

		auto previous = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "get_active", IgnoreNumberOfArguments)();

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "set_active", 1)(renderTexture);

		auto readableTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(readableTexture->klass, ".ctor", 2)->methodPointer(readableTexture, width, height);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Rect, int, int)>(readableTexture->klass, "ReadPixels", 3)->methodPointer(readableTexture, UnityEngine::Rect{ 0, 0, static_cast<float>(width), static_cast<float>(height) }, 0, 0);
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

		auto wPath = GetIconPath(localPushType);
		wstring path = wPath->chars;
		auto parentDir = filesystem::path(path).parent_path();

		if (!filesystem::exists(parentDir))
		{
			filesystem::create_directories(parentDir);
		}

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, Il2CppArraySize_t<uint8_t>*)>("mscorlib.dll", "System.IO", "File", "WriteAllBytes", 2)(il2cpp_string_new_utf16(path.data(), path.size()), pngData);
	}

	Il2CppObject* GetCharaPushIcon(int charaId, int dressId)
	{
		auto old_str = to_string(dressId);
		size_t n_zero = 6;
		auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;

		auto push_icon = "push_icon_"s + to_string(charaId) + "_" + new_str;
		auto path = "chara/chr"s + to_string(charaId) + "/" + push_icon;
		auto path2 = "chara/chr"s + to_string(charaId) + "/" + push_icon + "_01";

		auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
		auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path.data()), false);

		if (!asset)
		{
			auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path2.data()), false);

			if (!asset)
			{
				return nullptr;
			}
		}

		auto assetBundle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)->methodPointer(asset);
		if (!assetBundle)
		{
			return nullptr;
		}

		auto texture2DType = reinterpret_cast<Il2CppType*>(::GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
		return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, const Il2CppType*)>(il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)"))(assetBundle, il2cpp_string_new(push_icon.data()), texture2DType);
	}

	void RegisterTPScheduledToast()
	{
		if (!MsgPackData::user_info.empty() || !MsgPackData::tp_info.empty())
		{
			int leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
			int leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

			auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
			if (!texture2D)
			{
				return;
			}

			DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::LocalPushType::Tp, texture2D);

			if (config::notification_tp)
			{
				if ((!MsgPackData::user_info.empty() || !MsgPackData::tp_info.empty()) && config::notification_tp)
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
					auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
					auto content = u8_wide(MasterDB::GetTextData(184, leader_chara_id));
					DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"TP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Tp)->chars, MsgPackData::tp_info["max_recovery_time"].int64_value() * 1000);
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

			auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
			if (!texture2D)
			{
				return;
			}

			DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::LocalPushType::Rp, texture2D);

			if (config::notification_rp)
			{
				if ((!MsgPackData::user_info.empty() || !MsgPackData::rp_info.empty()) && config::notification_rp)
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
					auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
					auto content = u8_wide(MasterDB::GetTextData(185, leader_chara_id));
					DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"RP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Rp)->chars, MsgPackData::rp_info["max_recovery_time"].int64_value() * 1000);
				}
			}
		}
	}

	void RegisterJobsScheduledToast()
	{
		if (!MsgPackData::jobs_going_info_array.empty())
		{
			if (config::notification_rp)
			{
				if ((!MsgPackData::user_info.empty() || !MsgPackData::rp_info.empty()) && config::notification_rp)
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByGroup(L"Jobs");

					for (MsgPack msgPack : MsgPackData::jobs_going_info_array)
					{
						MsgPack::object info = msgPack.object_items();
						MsgPack::array attend_card_info_array = info["attend_card_info_array"].array_items();
						MsgPack::object leader_card_info = attend_card_info_array[0].object_items();

						int leader_chara_id = static_cast<int>(floorf(leader_card_info["card_id"].int_value() * 0.01f));
						int leader_chara_dress_id = leader_card_info["dress_id"].int_value();

						auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
						if (!texture2D)
						{
							continue;
						}

						auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
						auto jobs_reward_id = info["jobs_reward_id"].int_value();
						auto local_push_type_index = info["local_push_type_index"].int_value();

						DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index), texture2D);

						auto jobsInfo = MasterDB::GetJobsInfo(jobs_reward_id);
						auto raceTrackId = MasterDB::GetJobsPlaceRaceTrackId(std::get<0>(jobsInfo));
						auto raceTrack = u8_wide(MasterDB::GetTextData(34, raceTrackId));
						auto genre = u8_wide(MasterDB::GetTextData(357, std::get<1>(jobsInfo)));
						auto content = u8_wide(MasterDB::GetTextData(360, leader_chara_id));

						auto jobs_placename = L"\u3010" + raceTrack + L"\u3011" + genre;
						replaceAll(content, LR"(<jobs_placename>)", jobs_placename);
						replaceAll(content, L"\\n", L"\n");

						tm tm{};
						stringstream ss(info["end_time"].string_value());
						ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
						chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));
						int64_t end_time = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()).count();

						int notiId = Gallop::PushNotificationManager::Instance().MakeNotificationId(Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index), 0);

						DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), (L"Jobs" + to_wstring(notiId)).data(), GetIconPath(Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index))->chars, end_time, L"Jobs");
					}
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

					if (data["tp_info"].is_object() || data["rp_info"].is_object() ||
						data["jobs_load_info"].is_object() || data["jobs_going_info_array"].is_object() ||
						data["user_info"].is_object())
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

						if (data["jobs_load_info"].is_object())
						{
							MsgPack::object jobs_load_info = data["jobs_load_info"].object_items();
							MsgPackData::jobs_going_info_array = jobs_load_info["jobs_going_info_array"].array_items();
						}

						if (data["jobs_going_info_array"].is_array())
						{
							MsgPackData::jobs_going_info_array = data["jobs_going_info_array"].array_items();
						}

						leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
						leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

						auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
						if (!texture2D)
						{
							return;
						}

						if (config::notification_tp || config::notification_rp || config::notification_jobs)
						{
							auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
							if ((data["user_info"].is_object() || data["tp_info"].is_object()) && config::notification_tp)
							{
								DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::LocalPushType::Tp, texture2D);
								DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
								auto content = u8_wide(MasterDB::GetTextData(184, leader_chara_id));
								DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"TP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Tp)->chars, MsgPackData::tp_info["max_recovery_time"].int64_value() * 1000);
							}
							if ((data["user_info"].is_object() || data["rp_info"].is_object()) && config::notification_rp)
							{
								DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::LocalPushType::Rp, texture2D);
								DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
								auto content = u8_wide(MasterDB::GetTextData(185, leader_chara_id));
								DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"RP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Rp)->chars, MsgPackData::rp_info["max_recovery_time"].int64_value() * 1000);
							}
							if ((data["jobs_load_info"].is_object() || data["jobs_going_info_array"].is_array()) && config::notification_jobs)
							{
								DesktopNotificationManagerCompat::RemoveFromScheduleByGroup(L"Jobs");

								for (MsgPack msgPack : MsgPackData::jobs_going_info_array)
								{
									MsgPack::object info = msgPack.object_items();
									MsgPack::array attend_card_info_array = info["attend_card_info_array"].array_items();
									MsgPack::object leader_card_info = attend_card_info_array[0].object_items();

									int leader_chara_id = static_cast<int>(floorf(leader_card_info["card_id"].int_value() * 0.01f));
									int leader_chara_dress_id = leader_card_info["dress_id"].int_value();

									auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
									if (!texture2D)
									{
										continue;
									}

									auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
									auto jobs_reward_id = info["jobs_reward_id"].int_value();
									auto local_push_type_index = info["local_push_type_index"].int_value();

									DumpTexture2D(leader_chara_id, Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index), texture2D);

									auto jobsInfo = MasterDB::GetJobsInfo(jobs_reward_id);
									auto raceTrackId = MasterDB::GetJobsPlaceRaceTrackId(std::get<0>(jobsInfo));
									auto raceTrack = u8_wide(MasterDB::GetTextData(34, raceTrackId));
									auto genre = u8_wide(MasterDB::GetTextData(357, std::get<1>(jobsInfo)));
									auto content = u8_wide(MasterDB::GetTextData(360, leader_chara_id));

									auto jobs_placename = L"\u3010" + raceTrack + L"\u3011" + genre;
									replaceAll(content, LR"(<jobs_placename>)", jobs_placename);
									replaceAll(content, L"\\n", L"\n");

									tm tm{};
									stringstream ss(info["end_time"].string_value());
									ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
									chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));
									int64_t end_time = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()).count();

									int notiId = Gallop::PushNotificationManager::Instance().MakeNotificationId(Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index), 0);

									DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), (L"Jobs" + to_wstring(notiId)).data(), GetIconPath(Gallop::LocalPushDefine::GetJobsLocalPushTypeByIndex(local_push_type_index))->chars, end_time, L"Jobs");
								}
							}
						}
					}
				}
			}
		}
	}
}
