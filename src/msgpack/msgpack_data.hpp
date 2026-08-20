#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_map>
#include <queue>
#include <filesystem>
#include <msgpack11.hpp>

#include "config/config.hpp"

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "string_utils.hpp"
#include "game.hpp"

#include "masterdb/masterdb.hpp"

#ifdef _MSC_VER
#include "notification/DesktopNotificationManagerCompat.h"
#endif

#include "scripts/ScriptInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RenderTexture.hpp"
#include "scripts/umamusume/Gallop/PushNotificationManager.hpp"

using namespace std;
using namespace msgpack11;

#ifdef _MSC_VER
using namespace Microsoft::WRL;
#endif

namespace MsgPackData
{
	inline void DumpMsgPackFile(string& file_path, const char* buffer, const size_t len) {
#ifndef _MSC_VER
		if (filesystem::path(file_path).is_relative()) {
			file_path.insert(0, "/sdcard/Android/data/"s.append(Game::GetCurrentPackageName()).append("/"));
		}
#endif

		auto parent_path = filesystem::path(file_path).parent_path();
		if (!filesystem::exists(parent_path)) {
			filesystem::create_directories(parent_path);
		}
		ofstream file{ file_path, ios::binary };
		file.write(buffer, static_cast<int>(len));
		file.flush();
		file.close();
	}

	inline MsgPack::object user_info;

	inline MsgPack::object tp_info;
	inline MsgPack::object rp_info;

	inline MsgPack::array jobs_going_info_array;

	inline MsgPack::object idle_single_mode_load_info;

#ifdef _MSC_VER
	inline Il2CppString* GetIconPath(Gallop::LocalPushDefine::LocalPushType localPushType)
	{
		return Gallop::PushNotificationManager::Instance().CreatePushIconFilePath(localPushType);
	}

	inline void DumpTexture2D(int unitId, Gallop::LocalPushDefine::LocalPushType localPushType, Il2CppObject* texture)
	{
		auto width = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(texture->klass, "get_width", 0)(texture);

		auto height = il2cpp_symbols::get_method_pointer<int (*)(Il2CppObject*)>(texture->klass, "get_height", 0)(texture);

		auto renderTexture = UnityEngine::RenderTexture::GetTemporary(width, height);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Graphics", "Blit", 2)(texture, renderTexture);

		auto previous = UnityEngine::RenderTexture::GetActive();

		UnityEngine::RenderTexture::SetActive(renderTexture);

		auto readableTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, int, int)>(readableTexture->klass, ".ctor", 2)(readableTexture, width, height);

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, UnityEngine::Rect, int, int)>(readableTexture->klass, "ReadPixels", 3)(readableTexture, UnityEngine::Rect{ 0, 0, static_cast<float>(width), static_cast<float>(height) }, 0, 0);
		il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>(readableTexture->klass, "Apply", 0)(readableTexture);

		UnityEngine::RenderTexture::SetActive(previous);

		UnityEngine::RenderTexture::ReleaseTemporary(renderTexture);

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

		auto u16Path = GetIconPath(localPushType);
		il2cppstring path = u16Path->chars;
		auto parentDir = filesystem::path(path).parent_path();

		if (!filesystem::exists(parentDir))
		{
			filesystem::create_directories(parentDir);
		}

		il2cpp_symbols::get_method_pointer<void (*)(Il2CppString*, Il2CppArraySize_t<uint8_t>*)>("mscorlib.dll", "System.IO", "File", "WriteAllBytes", 2)(il2cpp_string_new_utf16(path.data(), path.size()), pngData);
	}

	inline Il2CppObject* GetCharaPushIcon(int charaId, int dressId)
	{
		auto old_str = to_string(dressId);
		size_t n_zero = 6;
		auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;

		auto push_icon = "push_icon_"s + to_string(charaId) + "_" + new_str;
		auto path = "chara/chr"s + to_string(charaId) + "/" + push_icon;
		auto path2 = "chara/chr"s + to_string(charaId) + "/" + push_icon + "_01";

		auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
		auto asset = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)(loader, il2cpp_string_new(path.data()), false);

		if (!asset)
		{
			auto asset = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)(loader, il2cpp_string_new(path2.data()), false);

			if (!asset)
			{
				return nullptr;
			}
		}

		UnityEngine::AssetBundle assetBundle = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)(asset);
		if (!assetBundle)
		{
			return nullptr;
		}

		return assetBundle.LoadAsset(il2cpp_string_new(push_icon.data()), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
	}

	inline void RegisterTPScheduledToast()
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

			if ((!MsgPackData::user_info.empty() || !MsgPackData::tp_info.empty()) && config::notification_tp)
			{
				DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
				auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
				auto content = u8_wide(MasterDB::GetTextData(184, leader_chara_id));
				DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"TP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Tp)->chars, MsgPackData::tp_info["max_recovery_time"].int64_value() * 1000);
			}
		}
	}

	inline void RegisterRPScheduledToast()
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

			if ((!MsgPackData::user_info.empty() || !MsgPackData::rp_info.empty()) && config::notification_rp)
			{
				DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
				auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
				auto content = u8_wide(MasterDB::GetTextData(185, leader_chara_id));
				DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"RP", GetIconPath(Gallop::LocalPushDefine::LocalPushType::Rp)->chars, MsgPackData::rp_info["max_recovery_time"].int64_value() * 1000);
			}
		}
	}

	inline void RegisterJobsScheduledToast()
	{
		if (!MsgPackData::jobs_going_info_array.empty() && config::notification_jobs)
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

	inline void RegisterIdleSingleModeScheduledToast()
	{
		if (!MsgPackData::idle_single_mode_load_info.empty())
		{
			MsgPack::object chara_info;

			if (MsgPackData::idle_single_mode_load_info.contains("single_mode_chara_light"))
			{
				chara_info = MsgPackData::idle_single_mode_load_info["single_mode_chara_light"].object_items();
			}
			else if (MsgPackData::idle_single_mode_load_info.contains("progress_info"))
			{
				chara_info = MsgPackData::idle_single_mode_load_info["progress_info"].object_items();
			}

			if (chara_info.empty())
			{
				return;
			}

			int card_id = chara_info["card_id"].int_value();
			int chara_id = MasterDB::GetCharaIdByCardId(card_id);
			int rarity = chara_info["rarity"].int_value();
			int dress_id = MasterDB::GetDressIdByCardIdAndRarity(card_id, rarity);

			auto texture2D = GetCharaPushIcon(chara_id, dress_id);
			if (!texture2D)
			{
				return;
			}

			DumpTexture2D(chara_id, Gallop::LocalPushDefine::LocalPushType::IdleSingleMode, texture2D);

			if (!MsgPackData::idle_single_mode_load_info.empty() && config::notification_idle_single_mode)
			{
				DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"IdleSingleMode");
				auto title = u8_wide(MasterDB::GetTextData(6, chara_id));
				auto content = u8_wide(MasterDB::GetTextData(469, chara_id));
				tm tm{};
				stringstream ss(MsgPackData::idle_single_mode_load_info["end_time"].string_value());
				ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
				chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));
				int64_t end_time = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()).count();
				DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"IdleSingleMode", GetIconPath(Gallop::LocalPushDefine::LocalPushType::IdleSingleMode)->chars, end_time);
			}
		}
	}
#endif

	inline void ReadRequest(const char* data, size_t size)
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

	inline void ReadResponse(const char* data, size_t size)
	{
		string error;
		auto parsed = MsgPack::parse(data, size, error);

		if (error.empty())
		{
			if (parsed.is_object())
			{
				MsgPack::object object = parsed.object_items();

#ifdef _MSC_VER
				if (object["data"].is_object())
				{
					MsgPack::object data = object["data"].object_items();

					if (data["tp_info"].is_object() || data["rp_info"].is_object() ||
						data["jobs_load_info"].is_object() || data["jobs_going_info_array"].is_object() ||
						data["idle_single_mode_load_info"].is_object() || data["progress_info"].is_object() ||
						data["user_info"].is_object())
					{
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

						if (data["idle_single_mode_load_info"].is_object())
						{
							MsgPackData::idle_single_mode_load_info = data["idle_single_mode_load_info"].object_items();
						}

						if (data["progress_info"].is_object())
						{
							MsgPackData::idle_single_mode_load_info = data["progress_info"].object_items();
						}

						if (config::notification_tp || config::notification_rp)
						{
							auto leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();
							auto leader_chara_dress_id = MsgPackData::user_info["leader_chara_dress_id"].int_value();

							auto texture2D = GetCharaPushIcon(leader_chara_id, leader_chara_dress_id);
							if (!texture2D)
							{
								return;
							}

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
						}

						if (config::notification_jobs || config::notification_idle_single_mode)
						{
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

							if ((data["idle_single_mode_load_info"].is_object() || data["progress_info"].is_object()) && config::notification_idle_single_mode)
							{
								MsgPack::object chara_info;

								if (MsgPackData::idle_single_mode_load_info.contains("single_mode_chara_light"))
								{
									chara_info = MsgPackData::idle_single_mode_load_info["single_mode_chara_light"].object_items();
								}
								else if (MsgPackData::idle_single_mode_load_info.contains("chara_info"))
								{
									chara_info = MsgPackData::idle_single_mode_load_info["chara_info"].object_items();
								}

								if (chara_info.empty())
								{
									return;
								}

								int card_id = chara_info["card_id"].int_value();
								int chara_id = MasterDB::GetCharaIdByCardId(card_id);
								int rarity = chara_info["rarity"].int_value();
								int dress_id = MasterDB::GetDressIdByCardIdAndRarity(card_id, rarity);

								auto texture2D = GetCharaPushIcon(chara_id, dress_id);
								if (!texture2D)
								{
									return;
								}

								DumpTexture2D(chara_id, Gallop::LocalPushDefine::LocalPushType::IdleSingleMode, texture2D);
								DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"IdleSingleMode");
								auto content = u8_wide(MasterDB::GetTextData(469, chara_id));
								tm tm{};
								stringstream ss(MsgPackData::idle_single_mode_load_info["end_time"].string_value());
								ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
								chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));
								int64_t end_time = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()).count();

								auto title = u8_wide(MasterDB::GetTextData(6, chara_id));
								DesktopNotificationManagerCompat::AddScheduledToastNotification(title.data(), content.data(), L"IdleSingleMode", GetIconPath(Gallop::LocalPushDefine::LocalPushType::IdleSingleMode)->chars, end_time);
							}
						}
					}
				}
#endif
			}
		}
	}
}
