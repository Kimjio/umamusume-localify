#include "il2cpp_symbols.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/encodings.h>
#include <rapidjson/document.h>

#include <winver.h>
#include <wincrypt.h>

#include "Signature.h"

#define DO_API(r, n, p) r (*n) p
#include "il2cpp-api-functions.h"
#undef DO_API

Il2CppDefaults il2cpp_defaults;

Il2CppString* il2cpp_string_new16(const Il2CppChar* value)
{
	return il2cpp_string_new_utf16(value, std::char_traits<Il2CppChar>::length(value));
}

#include "config/config.hpp"
#include "string_utils.hpp"
#include "game.hpp"

#include "pe_lib/pe_base.h"
#include "pe_lib/pe_properties_generic.h"

FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name)
{
	if (config::code_map.IsNull() || config::code_map.HasParseError())
	{
		return il2cpp_class_get_field_from_name(klass, name);
	}
	string className = string(klass->namespaze).append(".").append(klass->name);

	if (config::code_map.HasMember("!common"))
	{
		auto commonMap = config::code_map["!common"].GetObj();
		if (commonMap.HasMember(name))
		{
			auto field = il2cpp_class_get_field_from_name(klass, commonMap[name].GetString());
			if (field)
			{
				return field;
			}
		}
	}

	if (!config::code_map.HasMember(className.data()))
	{
		return il2cpp_class_get_field_from_name(klass, name);
	}

	auto classMap = config::code_map[className.data()].GetObj();

	if (classMap.HasMember(name))
	{
		auto field = il2cpp_class_get_field_from_name(klass, classMap[name].GetString());
		if (field)
		{
			return field;
		}
	}

	if (classMap.HasMember((name + ".index"s).data()))
	{
		void* iter = nullptr;
		int i = 0;
		int index = classMap[(name + ".index"s).data()].GetInt();
		while (FieldInfo* field = il2cpp_class_get_fields(klass, &iter))
		{
			if (index == i)
			{
				return field;
			}
			i++;
		}
	}

	if (classMap.HasMember("!extends"))
	{
		auto parentName = classMap["!extends"].GetString();
		auto parentMap = config::code_map[parentName].GetObj();
		auto parentClass = klass->parent;

		if (parentMap.HasMember((name + ".index"s).data()))
		{
			void* iter = nullptr;
			int i = 0;
			int index = parentMap[(name + ".index"s).data()].GetInt();
			while (FieldInfo* field = il2cpp_class_get_fields(parentClass, &iter))
			{
				if (index == i)
				{
					return field;
				}
				i++;
			}
		}
	}

	return il2cpp_class_get_field_from_name(klass, name);;
}

std::string il2cpp_fn_name(const char* name)
{
	if (config::fn_map.IsNull() || config::fn_map.HasParseError())
	{
		return name;
	}

	if (!config::fn_map.HasMember(name))
	{
		return name;
	}

	auto mapped = config::fn_map[name].GetString();

	if (mapped)
	{
		return mapped;
	}

	return name;
}

namespace il2cpp_symbols
{
	Il2CppDomain* il2cpp_domain = nullptr;
	uint64_t base = 0;

	std::vector<std::function<void()>> init_callbacks;

	static bool HasValidCert(filesystem::path path)
	{
		HCERTSTORE hStore = nullptr;
		HCRYPTMSG hMsg = nullptr;
		BOOL fResult;
		DWORD dwEncoding, dwContentType, dwFormatType;
		DWORD dwSignerInfo;
		CERT_INFO CertInfo{};

		fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
			path.wstring().data(),
			CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
			CERT_QUERY_FORMAT_FLAG_BINARY,
			0,
			&dwEncoding,
			&dwContentType,
			&dwFormatType,
			&hStore,
			&hMsg,
			nullptr);

		if (fResult)
		{
			// Get signer information size.
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				nullptr,
				&dwSignerInfo);

			if (fResult)
			{
				// Allocate memory for signer information.
				PCMSG_SIGNER_INFO pSignerInfo = reinterpret_cast<PCMSG_SIGNER_INFO>(LocalAlloc(LPTR, dwSignerInfo));
				if (pSignerInfo)
				{
					// Get Signer Information.
					fResult = CryptMsgGetParam(hMsg,
						CMSG_SIGNER_INFO_PARAM,
						0,
						(PVOID)pSignerInfo,
						&dwSignerInfo);

					if (fResult)
					{
						// Search for the signer certificate in the temporary
						// certificate store.
						CertInfo.Issuer = pSignerInfo->Issuer;
						CertInfo.SerialNumber = pSignerInfo->SerialNumber;

						PCCERT_CONTEXT pCertContext = CertFindCertificateInStore(hStore,
							X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
							0,
							CERT_FIND_SUBJECT_CERT,
							(PVOID)&CertInfo,
							nullptr);

						if (pCertContext)
						{
							DWORD dwData;

							// Get Subject name size.
							if (dwData = CertGetNameStringW(pCertContext,
								CERT_NAME_SIMPLE_DISPLAY_TYPE,
								0,
								nullptr,
								nullptr,
								0))
							{
								// Allocate memory for subject name.
								wstring subjectName;
								subjectName.resize(dwData * sizeof(wchar_t));

								// Get subject name.
								if (CertGetNameStringW(pCertContext,
									CERT_NAME_SIMPLE_DISPLAY_TYPE,
									0,
									nullptr,
									subjectName.data(),
									dwData))
								{
									return subjectName.starts_with(L"Unity Technologies");
								}
							}
						}
					}
				}
			}
		}

		return false;
	}

	void load_symbols(filesystem::path& path)
	{
		if (!config::fn_map.IsNull())
		{
			return;
		}

		auto bufSize = GetFileVersionInfoSizeW(path.wstring().data(), nullptr);

		vector<BYTE> versionInfoBuffer(bufSize);

		GetFileVersionInfoW(path.wstring().data(), 0, bufSize, versionInfoBuffer.data());

		UINT versionInfoSize = 0;
		VS_FIXEDFILEINFO* pVSFileInfo = nullptr;
		PWSTR versionInfo = nullptr;

		VerQueryValueW(versionInfoBuffer.data(), L"\\", reinterpret_cast<void**>(&pVSFileInfo), &versionInfoSize);

		if (!pVSFileInfo)
		{
			return;
		}

		auto major = HIWORD(pVSFileInfo->dwProductVersionMS);
		auto patch = HIWORD(pVSFileInfo->dwProductVersionLS);

		switch (major)
		{
		case 2022:
			Game::CurrentUnityVersion = Game::UnityVersion::Unity22;
			break;
		default:
			break;
		}

		if (!HasValidCert(path))
		{
			// Modified UnityPlayer
			return;
		}

		try
		{
			config::fn_map.SetObject();

			auto file = CreateFileW(path.wstring().data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			auto map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
			CloseHandle(file);

			auto view = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);

			MEMORY_BASIC_INFORMATION memInfo;
			VirtualQuery(view, &memInfo, sizeof(memInfo));

			int startOffset = SigScanOffset("\x48\x83\xC4\x28\xC3\x48\x89\x5C\x24\x00\x48\x8d\x15", "xxxxxxxxx?xxx", memInfo);
			int nextOffset = SigScanOffset("\x48\x8d\x15", "xxx", memInfo, startOffset);

			int firstGap = nextOffset - startOffset;

			int nextOffset2 = SigScanOffset("\x48\x8d\x15", "xxx", memInfo, nextOffset);

			int gap = nextOffset2 - nextOffset;

			stringstream stream{ string(reinterpret_cast<char*>(view), memInfo.RegionSize) };

			auto base = pe_bliss::pe_base(stream, pe_bliss::pe_properties_64(), false);

			int startRva = base.file_offset_to_rva(startOffset);
			int rva = startRva;

			vector<const char*> symbol_names;

#define DO_API(r, n, p) symbol_names.emplace_back(#n)
#define DO_API_NEW(r, n, p) if (patch >= 62) symbol_names.emplace_back(#n)
			// PROFILER is only used in debug builds
#undef IL2CPP_ENABLE_PROFILER
#define IL2CPP_ENABLE_PROFILER 0
			// DO_API_NO_RETURN is not obfuscated
#define DO_API_NO_RETURN(r, n, p) symbol_names.emplace_back("_"#n)
#include "il2cpp-api-functions.h"
#undef DO_API_NO_RETURN
#undef IL2CPP_ENABLE_PROFILER
#define IL2CPP_ENABLE_PROFILER !IL2CPP_TINY
#undef DO_API
#undef DO_API_NEW

			for (auto name : symbol_names)
			{
				auto offset = base.rva_to_file_offset(rva);

				uint8_t bytes[4]
				{
					reinterpret_cast<uint8_t*>(view)[offset],
					reinterpret_cast<uint8_t*>(view)[offset + 1],
					reinterpret_cast<uint8_t*>(view)[offset + 2],
					reinterpret_cast<uint8_t*>(view)[offset + 3]
				};

				uint32_t rip_offset;
				memcpy(&rip_offset, bytes, sizeof(uint32_t));

				auto name_offset = base.rva_to_file_offset(rva + 0x4 + rip_offset);

				string real_symbol_name = string(&reinterpret_cast<char*>(view)[name_offset]);
				if (name == real_symbol_name)
				{
					// Symbol is not obfuscated
					break;
				}

				char* real_name = new char[real_symbol_name.size() + 1];
				strcpy(real_name, real_symbol_name.data());

				config::fn_map.AddMember(
					rapidjson::StringRef(name),
					rapidjson::StringRef(real_name),
					config::fn_map.GetAllocator());

				rva += rva == startRva ? firstGap : gap;
			}

			UnmapViewOfFile(view);
			CloseHandle(map);
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
		}
	}

	void init_functions(HMODULE game_module);
	void init_defaults();

	void init(HMODULE game_module)
	{
		init_functions(game_module);
	}

	void call_init_callbacks()
	{
		for (auto& init_callback : init_callbacks)
		{
			init_callback();
		}
	}

	void init_functions(HMODULE game_module)
	{
#define DO_API(r, n, p) n = reinterpret_cast<decltype(n)>(GetProcAddress(game_module, il2cpp_fn_name(#n).data()))
#include "il2cpp-api-functions.h"
#undef DO_API

		HMODULE hMod = nullptr;
		if (GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCTSTR>(il2cpp_domain_get_assemblies), &hMod))
		{
			base = reinterpret_cast<uint64_t>(hMod);
		}
	}

	void init_defaults()
	{
		memset(&il2cpp_defaults, 0, sizeof(Il2CppDefaults));

#define DEFAULTS_INIT(field, ns, n) do { il2cpp_defaults.field = il2cpp_class_from_name(il2cpp_defaults.corlib, ns, n); } while (0)

#define DEFAULTS_INIT_TYPE(field, ns, n, nativetype) do { DEFAULTS_INIT(field, ns, n); } while (0)

#define DEFAULTS_INIT_OPTIONAL(field, ns, n) do { il2cpp_defaults.field = il2cpp_class_from_name(il2cpp_defaults.corlib, ns, n); } while (0)

#define DEFAULTS_INIT_TYPE_OPTIONAL(field, ns, n, nativetype) do { DEFAULTS_INIT_OPTIONAL(field, ns, n); } while (0)

#define DEFAULTS_GEN_INIT_TYPE(field, ns, n, nativetype) do { il2cpp_defaults.field = il2cpp_class_from_name(il2cpp_defaults.corlib_gen, ns, n); } while (0)

#define DEFAULTS_GEN_INIT_OPTIONAL(field, ns, n) do { il2cpp_defaults.field = il2cpp_class_from_name(il2cpp_defaults.corlib_gen, ns, n); } while (0)

		il2cpp_defaults.corlib = const_cast<Il2CppImage*>(il2cpp_get_corlib());
		auto gen_assembly = il2cpp_domain_assembly_open(il2cpp_domain, "__Generated");
		if (gen_assembly)
		{
			il2cpp_defaults.corlib_gen = const_cast<Il2CppImage*>(il2cpp_assembly_get_image(gen_assembly));
		}
		DEFAULTS_INIT(object_class, "System", "Object");
		DEFAULTS_INIT(void_class, "System", "Void");
		DEFAULTS_INIT_TYPE(boolean_class, "System", "Boolean", bool);
		DEFAULTS_INIT_TYPE(byte_class, "System", "Byte", uint8_t);
		DEFAULTS_INIT_TYPE(sbyte_class, "System", "SByte", int8_t);
		DEFAULTS_INIT_TYPE(int16_class, "System", "Int16", int16_t);
		DEFAULTS_INIT_TYPE(uint16_class, "System", "UInt16", uint16_t);
		DEFAULTS_INIT_TYPE(int32_class, "System", "Int32", int32_t);
		DEFAULTS_INIT_TYPE(uint32_class, "System", "UInt32", uint32_t);
		DEFAULTS_INIT(uint_class, "System", "UIntPtr");
		DEFAULTS_INIT_TYPE(int_class, "System", "IntPtr", intptr_t);
		DEFAULTS_INIT_TYPE(int64_class, "System", "Int64", int64_t);
		DEFAULTS_INIT_TYPE(uint64_class, "System", "UInt64", uint64_t);
		DEFAULTS_INIT_TYPE(single_class, "System", "Single", float);
		DEFAULTS_INIT_TYPE(double_class, "System", "Double", double);
		DEFAULTS_INIT_TYPE(char_class, "System", "Char", Il2CppChar);
		DEFAULTS_INIT(string_class, "System", "String");
		DEFAULTS_INIT(enum_class, "System", "Enum");
		DEFAULTS_INIT(array_class, "System", "Array");
		DEFAULTS_INIT(value_type_class, "System", "ValueType");
#if !IL2CPP_TINY
		DEFAULTS_INIT_TYPE(delegate_class, "System", "Delegate", Il2CppDelegate);
		DEFAULTS_INIT_TYPE(multicastdelegate_class, "System", "MulticastDelegate",
			Il2CppMulticastDelegate);
		DEFAULTS_INIT(asyncresult_class, "System.Runtime.Remoting.Messaging", "AsyncResult");
		DEFAULTS_INIT_TYPE(async_call_class, "System", "MonoAsyncCall", Il2CppAsyncCall);
		DEFAULTS_INIT(manualresetevent_class, "System.Threading", "ManualResetEvent");
#endif // !IL2CPP_TINY
		DEFAULTS_INIT(typehandle_class, "System", "RuntimeTypeHandle");
		DEFAULTS_INIT(methodhandle_class, "System", "RuntimeMethodHandle");
		DEFAULTS_INIT(fieldhandle_class, "System", "RuntimeFieldHandle");
		DEFAULTS_INIT(systemtype_class, "System", "Type");
#if !IL2CPP_TINY
		DEFAULTS_INIT_TYPE(monotype_class, "System", "MonoType", Il2CppReflectionMonoType);
#endif
		DEFAULTS_INIT(exception_class, "System", "Exception");
		DEFAULTS_INIT(threadabortexception_class, "System.Threading", "ThreadAbortException");
		DEFAULTS_INIT_TYPE(thread_class, "System.Threading", "Thread", Il2CppThread);
		DEFAULTS_INIT_TYPE(internal_thread_class, "System.Threading", "InternalThread",
			Il2CppInternalThread);
		DEFAULTS_INIT_TYPE(runtimetype_class, "System", "RuntimeType", Il2CppReflectionRuntimeType);
#if !IL2CPP_TINY
		DEFAULTS_INIT(appdomain_class, "System", "AppDomain");
		DEFAULTS_INIT(appdomain_setup_class, "System", "AppDomainSetup");
		DEFAULTS_INIT(member_info_class, "System.Reflection", "MemberInfo");
		DEFAULTS_INIT(field_info_class, "System.Reflection", "FieldInfo");
		DEFAULTS_INIT(method_info_class, "System.Reflection", "MethodInfo");
		DEFAULTS_INIT(property_info_class, "System.Reflection", "PropertyInfo");
		DEFAULTS_INIT_TYPE(event_info_class, "System.Reflection", "EventInfo",
			Il2CppReflectionEvent);
		DEFAULTS_INIT_TYPE(stringbuilder_class, "System.Text", "StringBuilder",
			Il2CppStringBuilder);
		DEFAULTS_INIT_TYPE(stack_frame_class, "System.Diagnostics", "StackFrame", Il2CppStackFrame);
		DEFAULTS_INIT(stack_trace_class, "System.Diagnostics", "StackTrace");
		DEFAULTS_INIT_TYPE(typed_reference_class, "System", "TypedReference", Il2CppTypedRef);
#endif
		DEFAULTS_INIT(generic_ilist_class, "System.Collections.Generic", "IList`1");
		DEFAULTS_INIT(generic_icollection_class, "System.Collections.Generic", "ICollection`1");
		DEFAULTS_INIT(generic_ienumerable_class, "System.Collections.Generic", "IEnumerable`1");
		DEFAULTS_INIT(generic_ireadonlylist_class, "System.Collections.Generic", "IReadOnlyList`1");
		DEFAULTS_INIT(generic_ireadonlycollection_class, "System.Collections.Generic",
			"IReadOnlyCollection`1");
		DEFAULTS_INIT(generic_nullable_class, "System", "Nullable`1");
#if !IL2CPP_TINY
		DEFAULTS_INIT(version, "System", "Version");
		DEFAULTS_INIT(culture_info, "System.Globalization", "CultureInfo");
		DEFAULTS_INIT_TYPE(assembly_class, "System.Reflection", "RuntimeAssembly", Il2CppReflectionAssembly);
		DEFAULTS_INIT_TYPE_OPTIONAL(assembly_name_class, "System.Reflection", "AssemblyName",
			Il2CppReflectionAssemblyName);
#endif // !IL2CPP_TINY
#if !IL2CPP_TINY
		DEFAULTS_INIT_TYPE(parameter_info_class, "System.Reflection", "RuntimeParameterInfo",
			Il2CppReflectionParameter);
		DEFAULTS_INIT_TYPE(module_class, "System.Reflection", "RuntimeModule", Il2CppReflectionModule);

		DEFAULTS_INIT_TYPE(exception_class, "System", "Exception", Il2CppException);
		DEFAULTS_INIT_TYPE(system_exception_class, "System", "SystemException",
			Il2CppSystemException);
		DEFAULTS_INIT_TYPE(argument_exception_class, "System", "ArgumentException",
			Il2CppArgumentException);
		DEFAULTS_INIT_TYPE(marshalbyrefobject_class, "System", "MarshalByRefObject",
			Il2CppMarshalByRefObject);
		DEFAULTS_GEN_INIT_TYPE(il2cpp_com_object_class, "System", "__Il2CppComObject", Il2CppComObject);
		DEFAULTS_INIT_TYPE(safe_handle_class, "System.Runtime.InteropServices", "SafeHandle",
			Il2CppSafeHandle);
		DEFAULTS_INIT_TYPE(sort_key_class, "System.Globalization", "SortKey", Il2CppSortKey);
		DEFAULTS_INIT(dbnull_class, "System", "DBNull");
		DEFAULTS_INIT_TYPE_OPTIONAL(error_wrapper_class, "System.Runtime.InteropServices",
			"ErrorWrapper", Il2CppErrorWrapper);
		DEFAULTS_INIT(missing_class, "System.Reflection", "Missing");
		DEFAULTS_INIT(attribute_class, "System", "Attribute");
		DEFAULTS_INIT_OPTIONAL(customattribute_data_class, "System.Reflection", "CustomAttributeData");
		DEFAULTS_INIT_OPTIONAL(customattribute_typed_argument_class, "System.Reflection", "CustomAttributeTypedArgument");
		DEFAULTS_INIT_OPTIONAL(customattribute_named_argument_class, "System.Reflection", "CustomAttributeNamedArgument");
		DEFAULTS_INIT(key_value_pair_class, "System.Collections.Generic", "KeyValuePair`2");
		DEFAULTS_INIT(system_guid_class, "System", "Guid");
#endif // !IL2CPP_TINY

#if !IL2CPP_TINY
		DEFAULTS_INIT(threadpool_wait_callback_class, "System.Threading",
			"_ThreadPoolWaitCallback");
		DEFAULTS_INIT(mono_method_message_class, "System.Runtime.Remoting.Messaging",
			"MonoMethodMessage");

		il2cpp_defaults.threadpool_perform_wait_callback_method = const_cast<MethodInfo*>(il2cpp_class_get_method_from_name(
			il2cpp_defaults.threadpool_wait_callback_class, "PerformWaitCallback", 0));
#endif

		DEFAULTS_INIT_OPTIONAL(sbyte_shared_enum, "System", "SByteEnum");
		DEFAULTS_INIT_OPTIONAL(int16_shared_enum, "System", "Int16Enum");
		DEFAULTS_INIT_OPTIONAL(int32_shared_enum, "System", "Int32Enum");
		DEFAULTS_INIT_OPTIONAL(int64_shared_enum, "System", "Int64Enum");

		DEFAULTS_INIT_OPTIONAL(byte_shared_enum, "System", "ByteEnum");
		DEFAULTS_INIT_OPTIONAL(uint16_shared_enum, "System", "UInt16Enum");
		DEFAULTS_INIT_OPTIONAL(uint32_shared_enum, "System", "UInt32Enum");
		DEFAULTS_INIT_OPTIONAL(uint64_shared_enum, "System", "UInt64Enum");

		DEFAULTS_GEN_INIT_OPTIONAL(il2cpp_fully_shared_type, "Unity.IL2CPP.Metadata", "__Il2CppFullySharedGenericType");
		DEFAULTS_GEN_INIT_OPTIONAL(il2cpp_fully_shared_struct_type, "Unity.IL2CPP.Metadata", "__Il2CppFullySharedGenericStructType");
	}

	Il2CppClass* get_class(const char* assemblyName, const char* namespaze, const char* klassName)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		if (assembly)
		{
			auto image = il2cpp_assembly_get_image(assembly);
			if (image)
			{
				return il2cpp_class_from_name(image, namespaze, klassName);
			}
		}
		return nullptr;
	}

	Il2CppMethodPointer get_method_pointer(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		if (assembly)
		{
			auto image = il2cpp_assembly_get_image(assembly);
			if (image)
			{
				auto klass = il2cpp_class_from_name(image, namespaze, klassName);
				if (klass)
				{
					auto method = il2cpp_class_get_method_from_name(klass, name, argsCount);
					if (method)
					{
						return method->methodPointer;
					}
				}
			}
		}
		return nullptr;
	}

	Il2CppMethodPointer get_method_pointer(Il2CppClass* klass, const char* name, int argsCount)
	{
		if (klass)
		{
			auto method = il2cpp_class_get_method_from_name(klass, name, argsCount);
			if (method)
			{
				return method->methodPointer;
			}
		}
		return nullptr;
	}

	const MethodInfo* get_method(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		if (assembly)
		{
			auto image = il2cpp_assembly_get_image(assembly);
			if (image)
			{
				auto klass = il2cpp_class_from_name(image, namespaze, klassName);
				if (klass)
				{
					return il2cpp_class_get_method_from_name(klass, name, argsCount);
				}
			}
		}
		return nullptr;
	}

	const Il2CppClass* find_class(const char* assemblyName, const char* namespaze,
		const std::function<bool(const Il2CppClass*)>& predict)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		if (assembly)
		{
			auto image = il2cpp_assembly_get_image(assembly);
			if (image)
			{
				auto classCount = il2cpp_image_get_class_count(image);
				for (int i = 0; i < classCount; i++)
				{
					if (predict(il2cpp_image_get_class(image, i)))
					{
						return il2cpp_image_get_class(image, i);
					}
				}
			}
		}

		return nullptr;
	}

	Il2CppMethodPointer find_method(const char* assemblyName, const char* namespaze,
		const char* klassName,
		const std::function<bool(const MethodInfo*)>& predict)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		if (assembly)
		{
			auto image = il2cpp_assembly_get_image(assembly);
			if (image)
			{
				auto klass = il2cpp_class_from_name(image, namespaze, klassName);
				if (klass)
				{
					void* iter = nullptr;
					while (const MethodInfo* method = il2cpp_class_get_methods(klass, &iter))
					{
						if (predict(method))
							return method->methodPointer;
					}
				}
			}
		}

		return nullptr;
	}
}
