#include "../../LibNative.Runtime.hpp"
#include "../../../ScriptInternal.hpp"
#include "Plugin.hpp"

#include "libnative_sqlite3.hpp"

#include "config/config.hpp"

#include "masterdb/masterdb.hpp"

#include "local/local.hpp"

#include "string_utils.hpp"

namespace
{
	unordered_map<sqlite3_stmt*, tuple<sqlite3_stmt*, string, bool>> text_queries;
	unordered_map<sqlite3_stmt*, bool> replacement_queries_can_next;

	sqlite3_stmt* select_character_system_text_characterId;
	tuple<sqlite3_stmt*, bool> select_character_system_text_characterId_replacement;
	bool select_character_system_text_characterId_replacement_can_next = true;
}

static void DecryptManifestDB()
{
	auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
	Il2CppString* persistentDataPath = il2cpp_class_get_method_from_name_type<Il2CppString * (*)()>(CuteCoreDevice, "GetPersistentDataPath", 0)->methodPointer();

	il2cppstring path = persistentDataPath->chars + il2cppstring(IL2CPP_STRING("/meta"));

	if (!filesystem::exists(path))
	{
		return;
	}

	ifstream file{ filesystem::path(path).string(), ios::binary };
	string magic = string(16, '\0');
	file.read(magic.data(), 16);
	file.close();

	if (!magic.starts_with("SQLite format 3"))
	{
		sqlite3* db;
		sqlite3_open(il2cpp_u8(path).data(), &db);

		sqlite3mc_config(db, "cipher", CODEC_TYPE_CHACHA20);

		vector<char> key;

		if (Game::CurrentGameRegion == Game::Region::JPN)
		{
			key = { 0x6D, 0x5B, 0x65, 0x33, 0x63, 0x36, 0x63, 0x25, 0x54, 0x71, 0x2D, 0x73, 0x50, 0x53, 0x63, 0x38, 0x6D, 0x34, 0x37, 0x7B, 0x35, 0x63, 0x70, 0x23, 0x37, 0x34, 0x53, 0x29, 0x73, 0x43, 0x36, 0x33 };
		}
		else if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			key = { 0x56, 0x63, 0x6B, 0x63, 0x42, 0x72, 0x37, 0x76, 0x65, 0x70, 0x41, 0x62 };
		}

		if (!key.empty())
		{
			sqlite3_key(db, key.data(), key.size());
			sqlite3_rekey(db, nullptr, 0);
		}

		sqlite3_close(db);
	}
}

static int sqlite3_prepare_v2_hook(sqlite3* db, const char* zSql, int nBytes, sqlite3_stmt** ppStmt, const char** pzTail)
{
	auto result = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(db, zSql, nBytes, ppStmt, pzTail);

	string sql = string(zSql, nBytes);

	if (sql.find("text_data") != string::npos ||
		sql.find("race_jikkyo_comment") != string::npos ||
		sql.find("race_jikkyo_message") != string::npos)
	{
		sqlite3_stmt* pStmt = *ppStmt;

		if (MasterDB::replacementMasterDB)
		{
			sqlite3_stmt* stmt;
			auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &stmt, pzTail);
			text_queries.emplace(pStmt, make_tuple(stmt, sql, false));

			if (res != SQLITE_OK)
			{
				cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
			}
		}
	}

	if (sql.find("character_system_text") != string::npos)
	{
		sqlite3_stmt* pStmt = *ppStmt;

		if (sql.find("`voice_id`=?") != string::npos)
		{
			if (MasterDB::replacementMasterDB)
			{
				sqlite3_stmt* stmt;
				auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &stmt, pzTail);
				text_queries.emplace(pStmt, make_tuple(stmt, sql, false));

				if (res != SQLITE_OK)
				{
					cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
				}
			}
		}
		else
		{
			select_character_system_text_characterId = pStmt;


			if (MasterDB::replacementMasterDB)
			{
				auto res = reinterpret_cast<decltype(sqlite3_prepare_v2_hook)*>(sqlite3_prepare_v2_orig)(MasterDB::replacementMasterDB, zSql, nBytes, &get<0>(select_character_system_text_characterId_replacement), pzTail);
				if (res != SQLITE_OK)
				{
					cout << "ERROR: sqlite3_prepare_v2_hook SQL:" << sql << endl;
				}
			}
		}
	}

	return result;
}

static int sqlite3_step_hook(sqlite3_stmt* pStmt)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		if (get<1>(stmt).find("`race_jikkyo_message`;") != string::npos ||
			get<1>(stmt).find("`race_jikkyo_comment`;") != string::npos)
		{
			if (replacement_queries_can_next.find(pStmt) == replacement_queries_can_next.end())
			{
				replacement_queries_can_next.emplace(pStmt, true);
			}
			if (replacement_queries_can_next.at(pStmt))
			{
				get<2>(stmt) = (reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(get<0>(stmt)) == SQLITE_ROW);
			}
		}
		else
		{
			get<2>(stmt) = (reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(get<0>(stmt)) == SQLITE_ROW);
		}
	}

	if (pStmt == select_character_system_text_characterId)
	{
		if (get<0>(select_character_system_text_characterId_replacement))
		{
			auto res = reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(pStmt);

			if (res == SQLITE_ROW)
			{
				int voiceId = sqlite3_column_int(pStmt, 0);

				if (select_character_system_text_characterId_replacement_can_next)
				{
					get<1>(select_character_system_text_characterId_replacement) = (sqlite3_step(get<0>(select_character_system_text_characterId_replacement)) == SQLITE_ROW);
				}

				if (get<1>(select_character_system_text_characterId_replacement))
				{
					select_character_system_text_characterId_replacement_can_next = voiceId == sqlite3_column_int(get<0>(select_character_system_text_characterId_replacement), 0);
				}
			}

			return res;
		}
	}

	return reinterpret_cast<decltype(sqlite3_step_hook)*>(sqlite3_step_orig)(pStmt);
}

static int sqlite3_reset_hook(sqlite3_stmt* pStmt)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(get<0>(stmt));
		get<2>(stmt) = false;
		replacement_queries_can_next.insert_or_assign(pStmt, true);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(get<0>(select_character_system_text_characterId_replacement));
		get<1>(select_character_system_text_characterId_replacement) = false;
		select_character_system_text_characterId_replacement_can_next = true;
	}

	return reinterpret_cast<decltype(sqlite3_reset_hook)*>(sqlite3_reset_orig)(pStmt);
}

static int sqlite3_bind_text_hook(sqlite3_stmt* pStmt, int i, const char* zData, int nData, void (*xDel)(void*))
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(get<0>(stmt), i, zData, nData, xDel);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(get<0>(select_character_system_text_characterId_replacement), i, zData, nData, xDel);
	}

	return reinterpret_cast<decltype(sqlite3_bind_text_hook)*>(sqlite3_bind_text_orig)(pStmt, i, zData, nData, xDel);
}

static int sqlite3_bind_int_hook(sqlite3_stmt* pStmt, int i, int iValue)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(get<0>(stmt), i, iValue);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(get<0>(select_character_system_text_characterId_replacement), i, iValue);
	}

	return reinterpret_cast<decltype(sqlite3_bind_int_hook)*>(sqlite3_bind_int_orig)(pStmt, i, iValue);
}

static int sqlite3_bind_int64_hook(sqlite3_stmt* pStmt, int i, sqlite_int64 iValue)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(stmt), i, iValue);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(select_character_system_text_characterId_replacement), i, iValue);
	}

	return reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(pStmt, i, iValue);
}

static int sqlite3_bind_double_hook(sqlite3_stmt* pStmt, int i, double rValue)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_bind_double_hook)*>(sqlite3_bind_double_orig)(get<0>(stmt), i, rValue);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_bind_int64_hook)*>(sqlite3_bind_int64_orig)(get<0>(select_character_system_text_characterId_replacement), i, rValue);
	}

	return reinterpret_cast<decltype(sqlite3_bind_double_hook)*>(sqlite3_bind_double_orig)(pStmt, i, rValue);
}

static int sqlite3_finalize_hook(sqlite3_stmt* pStmt)
{
	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(get<0>(stmt));
		text_queries.erase(pStmt);
	}

	if (pStmt == select_character_system_text_characterId && get<0>(select_character_system_text_characterId_replacement))
	{
		reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(get<0>(select_character_system_text_characterId_replacement));
		get<0>(select_character_system_text_characterId_replacement) = nullptr;
	}

	return reinterpret_cast<decltype(sqlite3_finalize_hook)*>(sqlite3_finalize_orig)(pStmt);
}

static int sqlite3_key_hook(sqlite3* db, const void* pKey, int nKey)
{
	// no-op
	return SQLITE_OK;
}

static const unsigned char* sqlite3_column_text_hook(sqlite3_stmt* pStmt, int i)
{
	auto result = reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(pStmt, i);

	if (text_queries.contains(pStmt))
	{
		auto& stmt = text_queries.at(pStmt);
		const char* text;
		if (get<2>(stmt))
		{
			text = reinterpret_cast<const char*>(reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(get<0>(stmt), i));
			if (text && !string(text).empty())
			{
				if (get<1>(stmt).find("`race_jikkyo_message`;") != string::npos ||
					get<1>(stmt).find("`race_jikkyo_comment`;") != string::npos)
				{
					int id = sqlite3_column_int(pStmt, 0);
					int id1 = sqlite3_column_int(get<0>(stmt), 0);
					int groupId = sqlite3_column_int(pStmt, 1);
					int groupId1 = sqlite3_column_int(get<0>(stmt), 1);

					if (id == id1 && groupId == groupId1)
					{
						replacement_queries_can_next.insert_or_assign(pStmt, true);
						return reinterpret_cast<const unsigned char*>(text);
					}
					else
					{
						replacement_queries_can_next.insert_or_assign(pStmt, false);
					}
				}
				else
				{
					return reinterpret_cast<const unsigned char*>(text);
				}
			}
		}

		return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
	}

	if (pStmt == select_character_system_text_characterId)
	{
		if (get<1>(select_character_system_text_characterId_replacement) && select_character_system_text_characterId_replacement_can_next)
		{
			return reinterpret_cast<decltype(sqlite3_column_text_hook)*>(sqlite3_column_text_orig)(get<0>(select_character_system_text_characterId_replacement), i);
		}

		return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
	}

	return reinterpret_cast<const unsigned char*>(local::get_localized_string(reinterpret_cast<const char*>(result)));
}

static void InitAddress()
{
	init_sqlite3();
}

static void HookMethods()
{
	ADD_HOOK(sqlite3_key, "Plugin::sqlite3_key at %p\n");

	if (!config::replace_text_db_path.empty())
	{
		if (MasterDB::InitReplacementMasterDB(il2cpp_u8(config::replace_text_db_path.data())))
		{
			ADD_HOOK(sqlite3_prepare_v2, "Plugin::sqlite3_prepare_v2 at %p\n");
			ADD_HOOK(sqlite3_step, "Plugin::sqlite3_step at %p\n");
			ADD_HOOK(sqlite3_reset, "Plugin::sqlite3_reset at %p\n");
			ADD_HOOK(sqlite3_bind_text, "Plugin::sqlite3_bind_text at %p\n");
			ADD_HOOK(sqlite3_bind_int, "Plugin::sqlite3_bind_int at %p\n");
			ADD_HOOK(sqlite3_bind_int64, "Plugin::sqlite3_bind_int64 at %p\n");
			ADD_HOOK(sqlite3_bind_double, "Plugin::sqlite3_bind_double at %p\n");
			ADD_HOOK(sqlite3_column_text, "Plugin::sqlite3_column_text at %p\n");
			ADD_HOOK(sqlite3_finalize, "Plugin::sqlite3_finalize at %p\n");
		}
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::late_init_callbacks.emplace_back(HookMethods);
}
