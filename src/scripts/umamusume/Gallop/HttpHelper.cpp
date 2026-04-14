#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "HttpHelper.hpp"

#include "msgpack/msgpack_modify.hpp"
#include "msgpack/msgpack_data.hpp"

#include "notifier/notifier.hpp"

#include "game.hpp"

namespace
{
	void* HttpHelper_DecompressResponse_addr = nullptr;
	void* HttpHelper_DecompressResponse_orig = nullptr;

	void* HttpHelper_CompressRequest_addr = nullptr;
	void* HttpHelper_CompressRequest_orig = nullptr;
}

static Il2CppArraySize_t<int8_t>* HttpHelper_CompressRequest_hook(Il2CppArraySize_t<int8_t>* data)
{
	auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

	try
	{
		if (config::msgpack_notifier && config::msgpack_notifier_request)
		{
			notifier::notify_request(string(buf, data->max_length));
		}

		if (config::dump_msgpack && config::dump_msgpack_request)
		{
			string out_path = "msgpack_dump/"s.append(to_string(current_time())).append("Q.msgpack");

			MsgPackData::DumpMsgPackFile(out_path, buf, data->max_length);
		}

		MsgPackData::ReadRequest(buf, data->max_length);

#ifdef EXPERIMENTS
		if (config::unlock_live_chara)
		{
			auto modified = MsgPackModify::ModifyRequest(buf, data->max_length);

			if (!modified.empty())
			{
				data = il2cpp_array_new_type<int8_t>(il2cpp_defaults.byte_class, modified.size());

				char* buf1 = reinterpret_cast<char*>(data) + kIl2CppSizeOfArray;
				memcpy(buf1, modified.data(), modified.size());
			}
		}
#endif
	}
	catch (...)
	{
	}

	return reinterpret_cast<decltype(HttpHelper_CompressRequest_hook)*>(HttpHelper_CompressRequest_orig)(data);
}

static Il2CppArraySize_t<int8_t>* HttpHelper_DecompressResponse_hook(Il2CppArraySize_t<int8_t>* compressed)
{
	auto data = reinterpret_cast<decltype(HttpHelper_DecompressResponse_hook)*>(HttpHelper_DecompressResponse_orig)(compressed);

	try
	{
		auto buf = reinterpret_cast<const char*>(data) + kIl2CppSizeOfArray;

		if (config::msgpack_notifier)
		{
			notifier::notify_response(string(buf, data->max_length));
		}

		if (config::dump_msgpack)
		{
			string out_path = "msgpack_dump/"s.append(to_string(current_time())).append("R.msgpack");

			MsgPackData::DumpMsgPackFile(out_path, buf, data->max_length);
		}

		MsgPackData::ReadResponse(buf, data->max_length);

#ifdef EXPERIMENTS
		if (config::unlock_live_chara)
		{
			auto modified = MsgPackModify::ModifyResponse(buf, data->max_length);

			if (!modified.empty())
			{
				data = il2cpp_array_new_type<int8_t>(il2cpp_defaults.byte_class, modified.size());

				char* buf1 = reinterpret_cast<char*>(data) + kIl2CppSizeOfArray;
				memcpy(buf1, modified.data(), modified.size());
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
	HttpHelper_DecompressResponse_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "HttpHelper", "DecompressResponse", 1);
	HttpHelper_CompressRequest_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "HttpHelper", "CompressRequest", 1);
}

static void HookMethods()
{
	if (Game::CurrentGameRegion != Game::Region::KOR)
	{
		ADD_HOOK(HttpHelper_DecompressResponse, "Gallop.HttpHelper::DecompressResponse at %p\n");
		ADD_HOOK(HttpHelper_CompressRequest, "Gallop.HttpHelper::CompressRequest at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
