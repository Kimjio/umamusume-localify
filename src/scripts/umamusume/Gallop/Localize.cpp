#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "Localize.hpp"

#include "local/local.hpp"
#include "logger/logger.hpp"

namespace
{
	void* Localize_Get_addr = nullptr;

	void* Localize_Get1_addr = nullptr;

	void* Localize_JP_Get_addr = nullptr;
	void* Localize_JP_Get_orig = nullptr;

	void* Localize_Set_addr = nullptr;
}

static Il2CppString* Localize_JP_Get_hook(uint64_t id)
{
	auto orig_result = reinterpret_cast<decltype(Localize_JP_Get_hook)*>(Localize_JP_Get_orig)(id);

	Il2CppString* result = nullptr;

	if (config::static_entries_use_text_id_name)
	{
		result = local::get_localized_string(GetTextIdNameById(id));
	}
	else if (config::static_entries_use_hash)
	{
		result = local::get_localized_string(orig_result);
	}
	else
	{
		result = local::get_localized_string(id);
	}

	return result ? result : orig_result;
}

static void InitAddress()
{
	// have to do this way because there's Get(TextId id) and Get(string id)
	// the string one looks like will not be called by elsewhere
	Localize_Get_addr = il2cpp_symbols::find_method(ASSEMBLY_NAME, "Gallop", "Localize", [](const MethodInfo* method)
		{
			return method->name == "Get"s && (*method->parameters)->type == IL2CPP_TYPE_VALUETYPE;
		});
	Localize_Get1_addr = il2cpp_symbols::find_method(ASSEMBLY_NAME, "Gallop", "Localize", [](const MethodInfo* method)
		{
			return method->name == "Get"s && (*method->parameters)->type == IL2CPP_TYPE_STRING;
		});
	Localize_JP_Get_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Localize/JP", "Get", 1);
	Localize_Set_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "Localize", "Set", 3);
}

static void HookMethods()
{
	// They store all localized texts that used by code in a dict
	ADD_HOOK(Localize_JP_Get, "Gallop.Localize.JP::Get at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	Il2CppString* Localize::Get(uint64_t id)
	{
		return reinterpret_cast<Il2CppString * (*)(uint64_t)>(Localize_Get_addr)(id);
	}

	Il2CppString* Localize::Get(Il2CppString* id)
	{
		return reinterpret_cast<Il2CppString * (*)(Il2CppString*)>(Localize_Get1_addr)(id);
	}

	void Localize::Set(Localize::Region region, Il2CppString* id, Il2CppString* value)
	{
		reinterpret_cast<decltype(Set)*>(Localize_Set_addr)(region, id, value);
	}

	void Localize::DumpAllEntries()
	{
		vector<il2cppstring> static_entries;
		vector<pair<const il2cppstring, const il2cppstring>> text_id_static_entries;
		vector<pair<const il2cppstring, const il2cppstring>> text_id_not_matched_entries;
		// 0 is None
		for (uint64_t i = 1;; i++)
		{
			auto textIdName = GetTextIdNameById(i);

			if (textIdName.empty())
			{
				break;
			}

			auto str = reinterpret_cast<decltype(Localize_JP_Get_hook)*>(Localize_JP_Get_orig)(i);

			if (str)
			{
				if (config::static_entries_use_text_id_name)
				{
					il2cppstring textIdName = GetTextIdNameById(i);
					text_id_static_entries.emplace_back(textIdName, str->chars);
					if (local::get_localized_string(textIdName) == nullptr ||
						il2cppstring(local::get_localized_string(textIdName)->chars) == str->chars)
					{
						text_id_not_matched_entries.emplace_back(textIdName, str->chars);
					}
				}
				else if (config::static_entries_use_hash)
				{
					static_entries.emplace_back(str->chars);
				}
				else
				{
					logger::write_entry(i, str->chars);
				}
			}
		}
		if (config::static_entries_use_text_id_name)
		{
			logger::write_text_id_static_dict(text_id_static_entries, text_id_not_matched_entries);
		}
		else if (config::static_entries_use_hash)
		{
			logger::write_static_dict(static_entries);
		}
	}
}
