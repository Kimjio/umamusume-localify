#include <stdinclude.hpp>

il2cpp_string_new_utf16_t il2cpp_string_new_utf16;
il2cpp_string_new_t il2cpp_string_new;
il2cpp_domain_get_t il2cpp_domain_get;
il2cpp_domain_get_assemblies_t il2cpp_domain_get_assemblies;
il2cpp_domain_assembly_open_t il2cpp_domain_assembly_open;
il2cpp_assembly_get_image_t il2cpp_assembly_get_image;
il2cpp_class_from_name_t il2cpp_class_from_name;
il2cpp_class_get_methods_t il2cpp_class_get_methods;
il2cpp_class_get_method_from_name_t il2cpp_class_get_method_from_name;
il2cpp_method_get_from_reflection_t il2cpp_method_get_from_reflection;
il2cpp_method_get_param_t il2cpp_method_get_param;
il2cpp_object_new_t il2cpp_object_new;
il2cpp_add_internal_call_t il2cpp_add_internal_call;
il2cpp_resolve_icall_t il2cpp_resolve_icall;
il2cpp_array_new_t il2cpp_array_new;
il2cpp_thread_attach_t il2cpp_thread_attach;
il2cpp_thread_detach_t il2cpp_thread_detach;
il2cpp_class_get_type_t il2cpp_class_get_type;
il2cpp_class_get_type_token_t il2cpp_class_get_type_token;
il2cpp_class_get_field_from_name_t il2cpp_class_get_field_from_name;
il2cpp_field_get_value_t il2cpp_field_get_value;
il2cpp_field_set_value_t il2cpp_field_set_value;
il2cpp_field_static_get_value_t il2cpp_field_static_get_value;
il2cpp_field_static_set_value_t il2cpp_field_static_set_value;
il2cpp_field_get_type_t il2cpp_field_get_type;
il2cpp_type_get_object_t il2cpp_type_get_object;
il2cpp_image_get_name_t il2cpp_image_get_name;
il2cpp_image_get_class_count_t il2cpp_image_get_class_count;
il2cpp_image_get_class_t il2cpp_image_get_class;
il2cpp_type_is_byref_t il2cpp_type_is_byref;
il2cpp_method_get_flags_t il2cpp_method_get_flags;
il2cpp_method_get_return_type_t il2cpp_method_get_return_type;
il2cpp_class_from_type_t il2cpp_class_from_type;
il2cpp_class_get_name_t il2cpp_class_get_name;
il2cpp_class_get_properties_t il2cpp_class_get_properties;
il2cpp_class_is_enum_t il2cpp_class_is_enum;
il2cpp_class_get_fields_t il2cpp_class_get_fields;
il2cpp_method_get_name_t il2cpp_method_get_name;
il2cpp_method_get_param_count_t il2cpp_method_get_param_count;
il2cpp_method_get_param_name_t il2cpp_method_get_param_name;
il2cpp_class_get_parent_t il2cpp_class_get_parent;
il2cpp_class_get_interfaces_t il2cpp_class_get_interfaces;
il2cpp_class_get_namespace_t il2cpp_class_get_namespace;
il2cpp_class_get_flags_t il2cpp_class_get_flags;
il2cpp_class_is_valuetype_t il2cpp_class_is_valuetype;
il2cpp_property_get_flags_t il2cpp_property_get_flags;
il2cpp_property_get_get_method_t il2cpp_property_get_get_method;
il2cpp_property_get_set_method_t il2cpp_property_get_set_method;
il2cpp_property_get_name_t il2cpp_property_get_name;
il2cpp_property_get_parent_t il2cpp_property_get_parent;
 il2cpp_field_get_flags_t il2cpp_field_get_flags;
 il2cpp_field_get_name_t il2cpp_field_get_name;
 il2cpp_field_get_parent_t il2cpp_field_get_parent;
 il2cpp_field_get_offset_t il2cpp_field_get_offset;


char* il2cpp_array_addr_with_size(void* array, int32_t size, uintptr_t idx)
{
	return ((char*)array) + kIl2CppSizeOfArray + size * idx;
}

namespace il2cpp_symbols
{
#define RESOLVE_IMPORT(name) name = reinterpret_cast<name##_t>(GetProcAddress(game_module, #name))

	void* il2cpp_domain = nullptr;

	void init(HMODULE game_module)
	{
		RESOLVE_IMPORT(il2cpp_string_new_utf16);
		RESOLVE_IMPORT(il2cpp_string_new);
		RESOLVE_IMPORT(il2cpp_domain_get);
		RESOLVE_IMPORT(il2cpp_domain_get_assemblies);
		RESOLVE_IMPORT(il2cpp_domain_assembly_open);
		RESOLVE_IMPORT(il2cpp_assembly_get_image);
		RESOLVE_IMPORT(il2cpp_class_from_name);
		RESOLVE_IMPORT(il2cpp_class_get_methods);
		RESOLVE_IMPORT(il2cpp_class_get_method_from_name);
		RESOLVE_IMPORT(il2cpp_method_get_from_reflection);
		RESOLVE_IMPORT(il2cpp_method_get_param);
		RESOLVE_IMPORT(il2cpp_object_new);
		RESOLVE_IMPORT(il2cpp_add_internal_call);
		RESOLVE_IMPORT(il2cpp_resolve_icall);
		RESOLVE_IMPORT(il2cpp_array_new);
		RESOLVE_IMPORT(il2cpp_thread_attach);
		RESOLVE_IMPORT(il2cpp_thread_detach);
		RESOLVE_IMPORT(il2cpp_class_get_type);
		RESOLVE_IMPORT(il2cpp_class_get_type_token);
		RESOLVE_IMPORT(il2cpp_class_get_field_from_name);
		RESOLVE_IMPORT(il2cpp_field_get_value);
		RESOLVE_IMPORT(il2cpp_field_set_value);
		RESOLVE_IMPORT(il2cpp_field_get_type);
		RESOLVE_IMPORT(il2cpp_type_get_object);
		RESOLVE_IMPORT(il2cpp_image_get_name);
		RESOLVE_IMPORT(il2cpp_image_get_class_count);
		RESOLVE_IMPORT(il2cpp_image_get_class);
		RESOLVE_IMPORT(il2cpp_type_is_byref);
		RESOLVE_IMPORT(il2cpp_method_get_flags);
		RESOLVE_IMPORT(il2cpp_method_get_return_type);
		RESOLVE_IMPORT(il2cpp_class_from_type);
		RESOLVE_IMPORT(il2cpp_class_get_name);
		RESOLVE_IMPORT(il2cpp_class_get_properties);
		RESOLVE_IMPORT(il2cpp_class_is_enum);
		RESOLVE_IMPORT(il2cpp_class_get_fields);
		RESOLVE_IMPORT(il2cpp_method_get_name);
		RESOLVE_IMPORT(il2cpp_method_get_param_count);
		RESOLVE_IMPORT(il2cpp_method_get_param_name);
		RESOLVE_IMPORT(il2cpp_class_get_parent);
		RESOLVE_IMPORT(il2cpp_class_get_interfaces);
		RESOLVE_IMPORT(il2cpp_class_get_namespace);
		RESOLVE_IMPORT(il2cpp_class_get_flags);
		RESOLVE_IMPORT(il2cpp_class_is_valuetype);
		RESOLVE_IMPORT(il2cpp_property_get_flags);
		RESOLVE_IMPORT(il2cpp_property_get_get_method);
		RESOLVE_IMPORT(il2cpp_property_get_set_method);
		RESOLVE_IMPORT(il2cpp_property_get_name);
		RESOLVE_IMPORT(il2cpp_property_get_parent);
		RESOLVE_IMPORT(il2cpp_field_get_flags);
		RESOLVE_IMPORT(il2cpp_field_get_name);
		RESOLVE_IMPORT(il2cpp_field_get_parent);
		RESOLVE_IMPORT(il2cpp_field_get_offset);

		il2cpp_domain = il2cpp_domain_get();
	}

	Il2CppClass* get_class(const char* assemblyName, const char* namespaze, const char* klassName)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		auto image = il2cpp_assembly_get_image(assembly);
		return il2cpp_class_from_name(image, namespaze, klassName);
	}

	uintptr_t get_method_pointer(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		auto image = il2cpp_assembly_get_image(assembly);
		auto klass = il2cpp_class_from_name(image, namespaze, klassName);

		if (klass)
		{
			auto methodInfo = il2cpp_class_get_method_from_name(klass, name, argsCount);
			if (methodInfo)
			{
				return methodInfo->methodPointer;
			}
		}
		return 0;
	}

	MethodInfo* get_method(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		auto image = il2cpp_assembly_get_image(assembly);
		auto klass = il2cpp_class_from_name(image, namespaze, klassName);

		if (klass)
		{
			return il2cpp_class_get_method_from_name(klass, name, argsCount);
		}
		return nullptr;
	}

	uintptr_t find_method(const char* assemblyName, const char* namespaze,
		const char* klassName, std::function<bool(const MethodInfo*)> predict)
	{
		auto assembly = il2cpp_domain_assembly_open(il2cpp_domain, assemblyName);
		auto image = il2cpp_assembly_get_image(assembly);
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

		return 0;
	}
}
