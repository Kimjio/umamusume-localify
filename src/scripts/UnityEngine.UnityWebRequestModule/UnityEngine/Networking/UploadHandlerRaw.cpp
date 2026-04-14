#include "../../UnityEngine.UnityWebRequestModule.hpp"
#include "../../../ScriptInternal.hpp"
#include "UploadHandlerRaw.hpp"

#include "msgpack/msgpack_modify.hpp"
#include "msgpack/msgpack_data.hpp"

#include "notifier/notifier.hpp"

#include "game.hpp"

namespace
{
	void* UploadHandlerRaw_Create_addr = nullptr;
	void* UploadHandlerRaw_Create_orig = nullptr;
}

static void* UploadHandlerRaw_Create_hook(Il2CppObject* self, const char* data, int dataLength)
{
	try
	{
		if (config::msgpack_notifier && config::msgpack_notifier_request)
		{
			notifier::notify_request(string(data, dataLength));
		}

		if (config::dump_msgpack && config::dump_msgpack_request)
		{
			string out_path = "msgpack_dump/"s.append(to_string(current_time())).append("Q.msgpack");

			MsgPackData::DumpMsgPackFile(out_path, data, dataLength);
		}

		MsgPackData::ReadRequest(data, dataLength);

#ifdef EXPERIMENTS
		if (config::unlock_live_chara)
		{
			auto modified = MsgPackModify::ModifyRequest(data, dataLength);

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

	return reinterpret_cast<decltype(UploadHandlerRaw_Create_hook)*>(UploadHandlerRaw_Create_orig)(self, data, dataLength);
}

static void InitAddress()
{
	UploadHandlerRaw_Create_addr = il2cpp_resolve_icall("UnityEngine.Networking.UploadHandlerRaw::Create()");
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(UploadHandlerRaw_Create, "UnityEngine.Networking.UploadHandlerRaw::Create at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
