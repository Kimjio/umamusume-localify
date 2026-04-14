#include "../../UnityEngine.UnityWebRequestModule.hpp"
#include "../../../ScriptInternal.hpp"
#include "DownloadHandler.hpp"

#include "msgpack/msgpack_modify.hpp"
#include "msgpack/msgpack_data.hpp"

#include "notifier/notifier.hpp"

#include "game.hpp"

namespace
{
	void* DownloadHandler_InternalGetByteArray_addr = nullptr;
	void* DownloadHandler_InternalGetByteArray_orig = nullptr;
}

static const char* DownloadHandler_InternalGetByteArray_hook(Il2CppObject* self, int* length)
{
	auto data = reinterpret_cast<decltype(DownloadHandler_InternalGetByteArray_hook)*>(DownloadHandler_InternalGetByteArray_orig)(self, length);

	try
	{
		if (config::msgpack_notifier)
		{
			notifier::notify_response(string(data, *length));
		}

		if (config::dump_msgpack)
		{
			string out_path =
				"msgpack_dump/"s.append(to_string(current_time())).append("R.msgpack");

			MsgPackData::DumpMsgPackFile(out_path, data, *length);
		}

		MsgPackData::ReadResponse(data, *length);

#ifdef EXPERIMENTS
		if (config::unlock_live_chara)
		{
			auto modified = MsgPackModify::ModifyResponse(data, *length);

			if (!modified.empty())
			{
				data = new char[modified.size()];
				memcpy(const_cast<char*>(data), modified.data(), modified.size());
			}
		}
#endif
	}
	catch (...)
	{
	}

	return data;
}

static void InitAddress()
{
	DownloadHandler_InternalGetByteArray_addr = il2cpp_resolve_icall("UnityEngine.Networking.DownloadHandler::InternalGetByteArray()");
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(DownloadHandler_InternalGetByteArray, "UnityEngine.Networking.DownloadHandler::InternalGetByteArray at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
