/*
** CAPI3REF: Result Codes
** KEYWORDS: {result code definitions}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicate success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [extended result code definitions]
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* Generic error */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Internal use only */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Not used */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */

/*
** CAPI3REF: Flags For File Open Operations
**
** These bit values are intended for use in the
** 3rd parameter to the [sqlite3_open_v2()] interface and
** in the 4th parameter to the [sqlite3_vfs.xOpen] method.
**
** Only those flags marked as "Ok for sqlite3_open_v2()" may be
** used as the third argument to the [sqlite3_open_v2()] interface.
** The other flags have historically been ignored by sqlite3_open_v2(),
** though future versions of SQLite might change so that an error is
** raised if any of the disallowed bits are passed into sqlite3_open_v2().
** Applications should not depend on the historical behavior.
**
** Note in particular that passing the SQLITE_OPEN_EXCLUSIVE flag into
** [sqlite3_open_v2()] does *not* cause the underlying database file
** to be opened using O_EXCL.  Passing SQLITE_OPEN_EXCLUSIVE into
** [sqlite3_open_v2()] has historically be a no-op and might become an
** error in future versions of SQLite.
*/
#define SQLITE_OPEN_READONLY         0x00000001  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_READWRITE        0x00000002  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_CREATE           0x00000004  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_DELETEONCLOSE    0x00000008  /* VFS only */
#define SQLITE_OPEN_EXCLUSIVE        0x00000010  /* VFS only */
#define SQLITE_OPEN_AUTOPROXY        0x00000020  /* VFS only */
#define SQLITE_OPEN_URI              0x00000040  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_MEMORY           0x00000080  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_MAIN_DB          0x00000100  /* VFS only */
#define SQLITE_OPEN_TEMP_DB          0x00000200  /* VFS only */
#define SQLITE_OPEN_TRANSIENT_DB     0x00000400  /* VFS only */
#define SQLITE_OPEN_MAIN_JOURNAL     0x00000800  /* VFS only */
#define SQLITE_OPEN_TEMP_JOURNAL     0x00001000  /* VFS only */
#define SQLITE_OPEN_SUBJOURNAL       0x00002000  /* VFS only */
#define SQLITE_OPEN_SUPER_JOURNAL    0x00004000  /* VFS only */
#define SQLITE_OPEN_NOMUTEX          0x00008000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_FULLMUTEX        0x00010000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_SHAREDCACHE      0x00020000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_PRIVATECACHE     0x00040000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_WAL              0x00080000  /* VFS only */
#define SQLITE_OPEN_NOFOLLOW         0x01000000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_EXRESCODE        0x02000000  /* Extended result codes */

/*
** Symbols for ciphers
*/
#define CODEC_TYPE_UNKNOWN     0
#define CODEC_TYPE_AES128      1
#define CODEC_TYPE_AES256      2
#define CODEC_TYPE_CHACHA20    3
#define CODEC_TYPE_SQLCIPHER   4
#define CODEC_TYPE_RC4         5
#define CODEC_TYPE_ASCON128    6
#define CODEC_TYPE_AEGIS       7
#define CODEC_TYPE_MAX_BUILTIN 7

#ifdef _MSC_VER
#define NOMINMAX
#include <Windows.h>

#define GET_PROC(name) reinterpret_cast<decltype(name)*>(name##_addr)
#define SET_PROC(name) name##_addr = reinterpret_cast<decltype(name)*>(GetProcAddress(libnative, #name))
#endif

#define ORIG_SYM(name) reinterpret_cast<decltype(name)*>(name##_orig)

#include <string>
#include <filesystem>

#include "il2cpp/il2cpp_symbols.hpp"

using namespace std;

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3_vfs sqlite3_vfs;

/*
** The type for a callback function.
** This is legacy and deprecated.  It is included for historical
** compatibility and is not documented.
*/
typedef int (*sqlite3_callback)(void*, int, char**, char**);

#ifdef SQLITE_INT64_TYPE
typedef SQLITE_INT64_TYPE sqlite_int64;
# ifdef SQLITE_UINT64_TYPE
typedef SQLITE_UINT64_TYPE sqlite_uint64;
# else
typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
# endif
#elif defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 sqlite_int64;
typedef unsigned __int64 sqlite_uint64;
#else
typedef long long int sqlite_int64;
typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;
typedef sqlite_uint64 sqlite3_uint64;


#ifdef _MSC_VER
#define MODULE HMODULE
#define PROC FARPROC
#else
#define MODULE void*
#define PROC void*
#endif

#define DEFINE_PROC(name) decltype(name)* name##_addr = nullptr; \
void* name##_orig = nullptr

#define CALL_ORIG_PROC(name) if (name##_orig) return reinterpret_cast<decltype(name)*>(name##_orig)

MODULE libnative;

int sqlite3_bind_blob(
	sqlite3_stmt* pStmt,
	int i,
	const void* zData,
	int nData,
	void (*xDel)(void*)
);

int sqlite3_bind_double(sqlite3_stmt* pStmt, int i, double rValue);

int sqlite3_bind_int(sqlite3_stmt* pStmt, int i, int iValue);

int sqlite3_bind_int64(sqlite3_stmt* pStmt, int i, sqlite_int64 iValue);

int sqlite3_bind_null(sqlite3_stmt* pStmt, int i);

int sqlite3_bind_text(
	sqlite3_stmt* pStmt,
	int i,
	const char* zData,
	int nData,
	void (*xDel)(void*)
);

int sqlite3_close(sqlite3* db);

const void* sqlite3_column_blob(sqlite3_stmt* pStmt, int i);

int sqlite3_column_bytes(sqlite3_stmt* pStmt, int i);

double sqlite3_column_double(sqlite3_stmt* pStmt, int i);

int sqlite3_column_int(sqlite3_stmt* pStmt, int i);

sqlite_int64 sqlite3_column_int64(sqlite3_stmt* pStmt, int i);

const unsigned char* sqlite3_column_text(sqlite3_stmt* pStmt, int i);

int sqlite3_column_type(sqlite3_stmt* pStmt, int i);

int sqlite3_exec(
	sqlite3* db,                /* The database on which the SQL executes */
	const char* zSql,           /* The SQL to be executed */
	sqlite3_callback xCallback, /* Invoke this callback routine */
	void* pArg,                 /* First argument to xCallback() */
	char** pzErrMsg             /* Write error messages here */
);

int sqlite3_finalize(sqlite3_stmt* pStmt);

int sqlite3_key(sqlite3* db, const void* zKey, int nKey);

int sqlite3_key_v2(sqlite3* db, const char* zDbName, const void* zKey, int nKey);

int sqlite3_open(
	const char* zFilename,
	sqlite3** ppDb
);

int sqlite3_open_v2(
	const char* filename,   /* Database filename (UTF-8) */
	sqlite3** ppDb,         /* OUT: SQLite db handle */
	int flags,              /* Flags */
	const char* zVfs        /* Name of VFS module to use */
);

int sqlite3_prepare_v2(
	sqlite3* db,              /* Database handle. */
	const char* zSql,         /* UTF-8 encoded SQL statement. */
	int nBytes,               /* Length of zSql in bytes. */
	sqlite3_stmt** ppStmt,    /* OUT: A pointer to the prepared statement */
	const char** pzTail       /* OUT: End of parsed string */
);

int sqlite3_rekey(sqlite3* db, const void* zKey, int nKey);

int sqlite3_rekey_v2(sqlite3* db, const char* zDbName, const void* zKey, int nKey);

int sqlite3_reset(sqlite3_stmt* pStmt);

int sqlite3_step(sqlite3_stmt* pStmt);

int sqlite3_threadsafe(void);

sqlite3_vfs* sqlite3_vfs_find(const char* zVfs);

int sqlite3_vfs_register(sqlite3_vfs* pVfs, int makeDflt);

int sqlite3_vfs_unregister(sqlite3_vfs* pVfs);

int sqlite3_wal_checkpoint(sqlite3* db, const char* zDb);

int sqlite3_wal_checkpoint_v2(
	sqlite3* db,                    /* Database handle */
	const char* zDb,                /* Name of attached database (or NULL) */
	int eMode,                      /* SQLITE_CHECKPOINT_* value */
	int* pnLog,                     /* OUT: Size of WAL log in frames */
	int* pnCkpt                     /* OUT: Total number of frames checkpointed */
);

unsigned char* sqlite3mc_codec_data(sqlite3* db, const char* zDbName, const char* paramName);

int sqlite3mc_config(sqlite3* db, const char* paramName, int newValue);

int sqlite3mc_config_cipher(sqlite3* db, const char* cipherName, const char* paramName, int newValue);

DEFINE_PROC(sqlite3_bind_blob);
DEFINE_PROC(sqlite3_bind_double);
DEFINE_PROC(sqlite3_bind_int);
DEFINE_PROC(sqlite3_bind_int64);
DEFINE_PROC(sqlite3_bind_null);
DEFINE_PROC(sqlite3_bind_text);
DEFINE_PROC(sqlite3_close);
DEFINE_PROC(sqlite3_column_blob);
DEFINE_PROC(sqlite3_column_bytes);
DEFINE_PROC(sqlite3_column_double);
DEFINE_PROC(sqlite3_column_int);
DEFINE_PROC(sqlite3_column_int64);
DEFINE_PROC(sqlite3_column_text);
DEFINE_PROC(sqlite3_column_type);
DEFINE_PROC(sqlite3_exec);
DEFINE_PROC(sqlite3_finalize);
DEFINE_PROC(sqlite3_key);
DEFINE_PROC(sqlite3_key_v2);
DEFINE_PROC(sqlite3_open);
DEFINE_PROC(sqlite3_open_v2);
DEFINE_PROC(sqlite3_prepare_v2);
DEFINE_PROC(sqlite3_rekey);
DEFINE_PROC(sqlite3_rekey_v2);
DEFINE_PROC(sqlite3_reset);
DEFINE_PROC(sqlite3_step);
DEFINE_PROC(sqlite3_threadsafe);
DEFINE_PROC(sqlite3_vfs_find);
DEFINE_PROC(sqlite3_vfs_register);
DEFINE_PROC(sqlite3_vfs_unregister);
DEFINE_PROC(sqlite3_wal_checkpoint);
DEFINE_PROC(sqlite3_wal_checkpoint_v2);
DEFINE_PROC(sqlite3mc_codec_data);
DEFINE_PROC(sqlite3mc_config);
DEFINE_PROC(sqlite3mc_config_cipher);

void init_sqlite3()
{
#ifdef _MSC_VER
	wstring module_name;
	module_name.resize(MAX_PATH);
	module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

	filesystem::path module_path(module_name);

	wstring name = module_path.filename().replace_extension();

	SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());
	libnative = LoadLibraryW(L"libnative.dll");
	SetDllDirectoryW(nullptr);
#endif

	SET_PROC(sqlite3_bind_blob);
	SET_PROC(sqlite3_bind_double);
	SET_PROC(sqlite3_bind_int);
	SET_PROC(sqlite3_bind_int64);
	SET_PROC(sqlite3_bind_null);
	SET_PROC(sqlite3_bind_text);
	SET_PROC(sqlite3_close);
	SET_PROC(sqlite3_column_blob);
	SET_PROC(sqlite3_column_bytes);
	SET_PROC(sqlite3_column_double);
	SET_PROC(sqlite3_column_int);
	SET_PROC(sqlite3_column_int64);
	SET_PROC(sqlite3_column_text);
	SET_PROC(sqlite3_column_type);
	SET_PROC(sqlite3_exec);
	SET_PROC(sqlite3_finalize);
	SET_PROC(sqlite3_key);
	SET_PROC(sqlite3_key_v2);
	SET_PROC(sqlite3_open);
	SET_PROC(sqlite3_open_v2);
	SET_PROC(sqlite3_prepare_v2);
	SET_PROC(sqlite3_rekey);
	SET_PROC(sqlite3_rekey_v2);
	SET_PROC(sqlite3_reset);
	SET_PROC(sqlite3_step);
	SET_PROC(sqlite3_threadsafe);
	SET_PROC(sqlite3_vfs_find);
	SET_PROC(sqlite3_vfs_register);
	SET_PROC(sqlite3_vfs_unregister);
	SET_PROC(sqlite3_wal_checkpoint);
	SET_PROC(sqlite3_wal_checkpoint_v2);
	SET_PROC(sqlite3mc_codec_data);
	SET_PROC(sqlite3mc_config);
	SET_PROC(sqlite3mc_config_cipher);
}

int sqlite3_bind_blob(
	sqlite3_stmt* pStmt,
	int i,
	const void* zData,
	int nData,
	void (*xDel)(void*)
)
{
	if (sqlite3_bind_blob_orig)
	{
		return ORIG_SYM(sqlite3_bind_blob)(pStmt, i, zData, nData, xDel);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_blob)(pStmt, i, zData, nData, xDel);
	}

	return SQLITE_OK;
}

int sqlite3_bind_double(sqlite3_stmt* pStmt, int i, double rValue)
{
	if (sqlite3_bind_double_orig)
	{
		return ORIG_SYM(sqlite3_bind_double)(pStmt, i, rValue);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_double)(pStmt, i, rValue);
	}

	return SQLITE_OK;
}

int sqlite3_bind_int(sqlite3_stmt* pStmt, int i, int iValue)
{
	if (sqlite3_bind_int_orig)
	{
		return ORIG_SYM(sqlite3_bind_int)(pStmt, i, iValue);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_int)(pStmt, i, iValue);
	}

	return SQLITE_OK;
}

int sqlite3_bind_int64(sqlite3_stmt* pStmt, int i, sqlite_int64 iValue)
{
	if (sqlite3_bind_int64_orig)
	{
		return ORIG_SYM(sqlite3_bind_int64)(pStmt, i, iValue);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_int64)(pStmt, i, iValue);
	}

	return SQLITE_OK;
}

int sqlite3_bind_null(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_bind_null_orig)
	{
		return ORIG_SYM(sqlite3_bind_null)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_null)(pStmt, i);
	}

	return SQLITE_OK;
}

int sqlite3_bind_text(
	sqlite3_stmt* pStmt,
	int i,
	const char* zData,
	int nData,
	void (*xDel)(void*)
)
{
	if (sqlite3_bind_text_orig)
	{
		return ORIG_SYM(sqlite3_bind_text)(pStmt, i, zData, nData, xDel);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_bind_text)(pStmt, i, zData, nData, xDel);
	}

	return SQLITE_OK;
}

int sqlite3_close(sqlite3* db)
{
	if (sqlite3_close_orig)
	{
		return ORIG_SYM(sqlite3_close)(db);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_close)(db);
	}

	return SQLITE_OK;
}

const void* sqlite3_column_blob(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_blob_orig)
	{
		return ORIG_SYM(sqlite3_column_blob)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_blob)(pStmt, i);
	}

	return SQLITE_OK;
}

int sqlite3_column_bytes(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_bytes_orig)
	{
		return ORIG_SYM(sqlite3_column_bytes)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_bytes)(pStmt, i);
	}

	return SQLITE_OK;
}

double sqlite3_column_double(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_double_orig)
	{
		return ORIG_SYM(sqlite3_column_double)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_double)(pStmt, i);
	}

	return SQLITE_OK;
}

int sqlite3_column_int(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_int_orig)
	{
		return ORIG_SYM(sqlite3_column_int)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_int)(pStmt, i);
	}

	return SQLITE_OK;
}

sqlite_int64 sqlite3_column_int64(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_int64_orig)
	{
		return ORIG_SYM(sqlite3_column_int64)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_int64)(pStmt, i);
	}

	return SQLITE_OK;
}

const unsigned char* sqlite3_column_text(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_text_orig)
	{
		return ORIG_SYM(sqlite3_column_text)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_text)(pStmt, i);
	}

	return SQLITE_OK;
}

int sqlite3_column_type(sqlite3_stmt* pStmt, int i)
{
	if (sqlite3_column_type_orig)
	{
		return ORIG_SYM(sqlite3_column_type)(pStmt, i);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_column_type)(pStmt, i);
	}

	return SQLITE_OK;
}

int sqlite3_exec(
	sqlite3* db,                /* The database on which the SQL executes */
	const char* zSql,           /* The SQL to be executed */
	sqlite3_callback xCallback, /* Invoke this callback routine */
	void* pArg,                 /* First argument to xCallback() */
	char** pzErrMsg             /* Write error messages here */
)
{
	if (sqlite3_exec_orig)
	{
		return ORIG_SYM(sqlite3_exec)(db, zSql, xCallback, pArg, pzErrMsg);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_exec)(db, zSql, xCallback, pArg, pzErrMsg);
	}

	return SQLITE_OK;
}

int sqlite3_finalize(sqlite3_stmt* pStmt)
{
	if (sqlite3_finalize_orig)
	{
		return ORIG_SYM(sqlite3_finalize)(pStmt);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_finalize)(pStmt);
	}

	return SQLITE_OK;
}

int sqlite3_key(sqlite3* db, const void* zKey, int nKey)
{
	if (sqlite3_key_orig)
	{
		return ORIG_SYM(sqlite3_key)(db, zKey, nKey);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_key)(db, zKey, nKey);
	}

	return SQLITE_OK;
}

int sqlite3_key_v2(sqlite3* db, const char* zDbName, const void* zKey, int nKey)
{
	if (sqlite3_key_v2_orig)
	{
		return ORIG_SYM(sqlite3_key_v2)(db, zDbName, zKey, nKey);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_key_v2)(db, zDbName, zKey, nKey);
	}

	return SQLITE_OK;
}

int sqlite3_open(
	const char* zFilename,
	sqlite3** ppDb
)
{
	if (sqlite3_open_orig)
	{
		return ORIG_SYM(sqlite3_open)(zFilename, ppDb);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_open)(zFilename, ppDb);
	}

	return SQLITE_OK;
}

int sqlite3_open_v2(
	const char* filename,   /* Database filename (UTF-8) */
	sqlite3** ppDb,         /* OUT: SQLite db handle */
	int flags,              /* Flags */
	const char* zVfs        /* Name of VFS module to use */
)
{
	if (sqlite3_open_v2_orig)
	{
		return ORIG_SYM(sqlite3_open_v2)(filename, ppDb, flags, zVfs);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_open_v2)(filename, ppDb, flags, zVfs);
	}

	return SQLITE_OK;
}

int sqlite3_prepare_v2(
	sqlite3* db,              /* Database handle. */
	const char* zSql,         /* UTF-8 encoded SQL statement. */
	int nBytes,               /* Length of zSql in bytes. */
	sqlite3_stmt** ppStmt,    /* OUT: A pointer to the prepared statement */
	const char** pzTail       /* OUT: End of parsed string */
)
{
	if (sqlite3_prepare_v2_orig)
	{
		return ORIG_SYM(sqlite3_prepare_v2)(db, zSql, nBytes, ppStmt, pzTail);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_prepare_v2)(db, zSql, nBytes, ppStmt, pzTail);
	}

	return SQLITE_OK;
}

int sqlite3_rekey(sqlite3* db, const void* zKey, int nKey)
{
	if (sqlite3_rekey_orig)
	{
		return ORIG_SYM(sqlite3_rekey)(db, zKey, nKey);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_rekey)(db, zKey, nKey);
	}

	return SQLITE_OK;
}

int sqlite3_rekey_v2(sqlite3* db, const char* zDbName, const void* zKey, int nKey)
{
	if (sqlite3_rekey_v2_orig)
	{
		return ORIG_SYM(sqlite3_rekey_v2)(db, zDbName, zKey, nKey);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_rekey_v2)(db, zDbName, zKey, nKey);
	}

	return SQLITE_OK;
}

int sqlite3_reset(sqlite3_stmt* pStmt)
{
	if (sqlite3_reset_orig)
	{
		return ORIG_SYM(sqlite3_reset)(pStmt);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_reset)(pStmt);
	}

	return SQLITE_OK;
}

int sqlite3_step(sqlite3_stmt* pStmt)
{
	if (sqlite3_step_orig)
	{
		return ORIG_SYM(sqlite3_step)(pStmt);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_step)(pStmt);
	}

	return SQLITE_OK;
}

int sqlite3_threadsafe(void)
{
	if (sqlite3_threadsafe_orig)
	{
		return ORIG_SYM(sqlite3_threadsafe)();
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_threadsafe)();
	}

	return SQLITE_OK;
}

sqlite3_vfs* sqlite3_vfs_find(const char* zVfs)
{
	if (sqlite3_vfs_find_orig)
	{
		return ORIG_SYM(sqlite3_vfs_find)(zVfs);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_vfs_find)(zVfs);
	}

	return SQLITE_OK;
}

int sqlite3_vfs_register(sqlite3_vfs* pVfs, int makeDflt)
{
	if (sqlite3_vfs_register_orig)
	{
		return ORIG_SYM(sqlite3_vfs_register)(pVfs, makeDflt);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_vfs_register)(pVfs, makeDflt);
	}

	return SQLITE_OK;
}

int sqlite3_vfs_unregister(sqlite3_vfs* pVfs)
{
	if (sqlite3_vfs_unregister_orig)
	{
		return ORIG_SYM(sqlite3_vfs_unregister)(pVfs);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_vfs_unregister)(pVfs);
	}

	return SQLITE_OK;
}

int sqlite3_wal_checkpoint(sqlite3* db, const char* zDb)
{
	if (sqlite3_wal_checkpoint_orig)
	{
		return ORIG_SYM(sqlite3_wal_checkpoint)(db, zDb);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_wal_checkpoint)(db, zDb);
	}

	return SQLITE_OK;
}

int sqlite3_wal_checkpoint_v2(
	sqlite3* db,                    /* Database handle */
	const char* zDb,                /* Name of attached database (or NULL) */
	int eMode,                      /* SQLITE_CHECKPOINT_* value */
	int* pnLog,                     /* OUT: Size of WAL log in frames */
	int* pnCkpt                     /* OUT: Total number of frames checkpointed */
)
{
	if (sqlite3_wal_checkpoint_v2_orig)
	{
		return ORIG_SYM(sqlite3_wal_checkpoint_v2)(db, zDb, eMode, pnLog, pnCkpt);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3_wal_checkpoint_v2)(db, zDb, eMode, pnLog, pnCkpt);
	}

	return SQLITE_OK;
}

unsigned char* sqlite3mc_codec_data(sqlite3* db, const char* zDbName, const char* paramName)
{
	if (sqlite3mc_codec_data_orig)
	{
		return ORIG_SYM(sqlite3mc_codec_data)(db, zDbName, paramName);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3mc_codec_data)(db, zDbName, paramName);
	}

	return nullptr;
}

int sqlite3mc_config(sqlite3* db, const char* paramName, int newValue)
{
	if (sqlite3mc_config_orig)
	{
		return ORIG_SYM(sqlite3mc_config)(db, paramName, newValue);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3mc_config)(db, paramName, newValue);
	}

	return SQLITE_OK;
}

int sqlite3mc_config_cipher(sqlite3* db, const char* cipherName, const char* paramName, int newValue)
{
	if (sqlite3mc_config_cipher_orig)
	{
		return ORIG_SYM(sqlite3mc_config_cipher)(db, cipherName, paramName, newValue);
	}

	if (libnative)
	{
		return GET_PROC(sqlite3mc_config_cipher)(db, cipherName, paramName, newValue);
	}

	return SQLITE_OK;
}
