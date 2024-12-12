#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <filesystem>

#include "libnative_sqlite3.hpp"

#include "config/config.hpp"

#include "il2cpp/il2cpp_symbols.hpp"
#include "string_utils.hpp"
#include "game.hpp"

namespace MasterDB
{
	sqlite3* replacementMasterDB;
	sqlite3* masterDB;
	sqlite3* metaDB;

	string masterDBPath;

	void InitMasterDB()
	{
		auto path = wide_u8(il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("Cute.Core.Assembly.dll", "Cute.Core", "Device", "GetPersistentDataPath", IgnoreNumberOfArguments)()->chars);
		auto metaDBPath = path + R"(\meta)";
		masterDBPath = path + R"(\master\master.mdb)";

		auto res = sqlite3_open_v2(metaDBPath.data(), &metaDB, SQLITE_OPEN_READONLY, nullptr);
		if (res != SQLITE_OK)
		{
			metaDB = nullptr;
		}

		if (config::unlock_live_chara)
		{
			auto masterDBOrigPath = path + R"(\master\master_orig.mdb)";

			filesystem::copy(masterDBPath, masterDBOrigPath, filesystem::copy_options::skip_existing);

			res = sqlite3_open_v2(masterDBOrigPath.data(), &masterDB, SQLITE_OPEN_READONLY, nullptr);
			if (res != SQLITE_OK)
			{
				masterDB = nullptr;
			}
		}
		else
		{
			res = sqlite3_open_v2(masterDBPath.data(), &masterDB, SQLITE_OPEN_READONLY, nullptr);
			if (res != SQLITE_OK)
			{
				masterDB = nullptr;
			}
		}
	}

	bool InitReplacementMasterDB(string path)
	{
		auto res = sqlite3_open_v2(path.data(), &replacementMasterDB, SQLITE_OPEN_READONLY, nullptr);

		bool isOk = res == SQLITE_OK;
		if (!isOk)
		{
			replacementMasterDB = nullptr;
		}

		return isOk;
	}

	void ReloadMasterDB()
	{
		sqlite3_close(masterDB);
		sqlite3_open_v2(masterDBPath.data(), &masterDB, SQLITE_OPEN_READONLY, nullptr);
	}

	vector<string> GetChampionsResources()
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		vector<string> pairs;

		auto query = R"(SELECT t.text FROM champions_schedule c LEFT OUTER JOIN text_data t on t.category = 206 AND t."index" = c.id GROUP BY c.resource_id)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			pairs.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		}

		sqlite3_finalize(stmt);

		return pairs;
	}

	string GetTextData(int category, int index)
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		if (replacementMasterDB)
		{
			auto query = R"(SELECT text FROM text_data WHERE "category" = ?1 AND "index" = ?2)"s;
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(replacementMasterDB, query.data(), query.size(), &stmt, nullptr);

			sqlite3_bind_int(stmt, 1, category);
			sqlite3_bind_int(stmt, 2, index);

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				string text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				sqlite3_finalize(stmt);
				return text;
			}

			sqlite3_finalize(stmt);
		}

		auto query = R"(SELECT text FROM text_data WHERE "category" = ?1 AND "index" = ?2)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		sqlite3_bind_int(stmt, 1, category);
		sqlite3_bind_int(stmt, 2, index);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto text = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
			sqlite3_finalize(stmt);
			return text;
		}

		sqlite3_finalize(stmt);

		return "";
	}

	tuple<int, int> GetJobsInfo(int rewardId)
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		auto query = R"(SELECT place_id, genre_id FROM jobs_reward WHERE "id" = ?1)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		sqlite3_bind_int(stmt, 1, rewardId);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto placeId = sqlite3_column_int(stmt, 0);
			auto genreId = sqlite3_column_int(stmt, 1);
			sqlite3_finalize(stmt);
			return tuple{ placeId, genreId };
		}

		sqlite3_finalize(stmt);

		return tuple{ 0, 0 };
	}

	int GetJobsPlaceRaceTrackId(int placeId)
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		auto query = R"(SELECT race_track_id FROM jobs_place WHERE "id" = ?1)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		sqlite3_bind_int(stmt, 1, placeId);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto raceTrackId = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			return raceTrackId;
		}

		sqlite3_finalize(stmt);

		return 0;
	}

	int GetSingleModeRaceLiveMusicId(int raceInstanceId, int grade)
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		auto query = R"(SELECT music_id FROM single_mode_race_live WHERE "race_instance_id" = ?1)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		sqlite3_bind_int(stmt, 1, raceInstanceId);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto musicId = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			return musicId;
		}

		sqlite3_finalize(stmt);

		auto query1 = R"(SELECT music_id FROM single_mode_race_live WHERE "grade" = ?1 AND "race_instance_id" = 0)"s;
		sqlite3_stmt* stmt1;
		sqlite3_prepare_v2(masterDB, query1.data(), query1.size(), &stmt1, nullptr);

		sqlite3_bind_int(stmt1, 1, grade);

		while (sqlite3_step(stmt1) == SQLITE_ROW)
		{
			auto musicId = sqlite3_column_int(stmt1, 0);
			sqlite3_finalize(stmt1);
			return musicId;
		}

		sqlite3_finalize(stmt1);

		return 1001;
	}

	bool HasLivePermission(int musicId, int charaId)
	{
		if (!masterDB)
		{
			InitMasterDB();
		}

		auto query = R"(SELECT music_id FROM live_permission_data WHERE "music_id" = ?1 AND "chara_id" = ?2)"s;
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(masterDB, query.data(), query.size(), &stmt, nullptr);

		sqlite3_bind_int(stmt, 1, musicId);
		sqlite3_bind_int(stmt, 2, charaId);

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			sqlite3_finalize(stmt);
			return true;
		}

		sqlite3_finalize(stmt);

		auto query1 = R"(SELECT song_chara_type FROM live_data WHERE "music_id" = ?1)"s;
		sqlite3_stmt* stmt1;
		sqlite3_prepare_v2(masterDB, query1.data(), query1.size(), &stmt1, nullptr);

		sqlite3_bind_int(stmt1, 1, musicId);

		while (sqlite3_step(stmt1) == SQLITE_ROW)
		{
			auto songCharaType = sqlite3_column_int(stmt1, 0);
			sqlite3_finalize(stmt1);
			return songCharaType == 1;
		}

		sqlite3_finalize(stmt1);

		return false;
	}
}
