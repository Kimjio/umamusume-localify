#pragma once

#define NOMINMAX

#include <Windows.h>

#include <functional>

#include <vector>

#include "il2cpp-object-internals.h"

template<typename T>
struct MethodInfo_t
{
	T methodPointer;
	void* (*invoker_method)(T, const MethodInfo_t<T>*, void*, void**);
	const char* name;
	Il2CppClass* klass;
	const Il2CppType* return_type;
	const ParameterInfo* parameters;

	union
	{
		const Il2CppRGCTXData* rgctx_data; /* is_inflated is true and is_generic is false, i.e. a generic instance method */
		Il2CppMetadataMethodDefinitionHandle methodMetadataHandle;
	};

	/* note, when is_generic == true and is_inflated == true the method represents an uninflated generic method on an inflated type. */
	union
	{
		const Il2CppGenericMethod* genericMethod; /* is_inflated is true */
		Il2CppMetadataGenericContainerHandle genericContainerHandle; /* is_inflated is false and is_generic is true */
		Il2CppMethodPointer nativeFunction; /* if is_marshaled_from_native is true */
	};

	uint32_t token;
	uint16_t flags;
	uint16_t iflags;
	uint16_t slot;
	uint8_t parameters_count;
	uint8_t is_generic : 1; /* true if method is a generic method definition */
	uint8_t is_inflated : 1; /* true if declaring_type is a generic instance or if method is a generic instance*/
	uint8_t wrapper_type : 1; /* always zero (MONO_WRAPPER_NONE) needed for the debugger */
	uint8_t is_marshaled_from_native : 1; /* a fake MethodInfo wrapping a native function pointer */
};

template<typename T>
struct Il2CppArraySize_t : public Il2CppArray
{
	ALIGN_TYPE(8) T vector[IL2CPP_ZERO_LEN_ARRAY];
};

template<typename T>
struct Il2CppDelegate_t
{
	Il2CppObject object;
	/* The compiled code of the target method */
	T method_ptr;
	/* The invoke code */
	InvokerMethod invoke_impl;
	Il2CppObject* target;
	const MethodInfo* method;

	void* delegate_trampoline;

	intptr_t extraArg;

	/*
	 * If non-NULL, this points to a memory location which stores the address of
	 * the compiled code of the method, or NULL if it is not yet compiled.
	 */
	uint8_t** method_code;
	Il2CppReflectionMethod* method_info;
	Il2CppReflectionMethod* original_method_info;
	Il2CppObject* data;

	bool method_is_virtual;
};

#define DO_API(r, n, p) extern r (*n) p
#include "il2cpp-api-functions.h"
#undef DO_API

char* il2cpp_array_addr_with_size(void* arr, int32_t size, uintptr_t idx);

// array macro
#define il2cpp_array_addr(array, type, index) ((type*)(void*) il2cpp_array_addr_with_size (array, sizeof (type), index))

#define il2cpp_array_setref(array, index, value)  \
    do {    \
        void* *__p = (void* *) il2cpp_array_addr ((array), void*, (index));; \
         *__p = (value);    \
    } while (0)

extern Il2CppDefaults il2cpp_defaults;

constexpr int IgnoreNumberOfArguments = -1;

Il2CppString* il2cpp_string_new16(const wchar_t* value);

FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name);

namespace il2cpp_symbols
{
	extern Il2CppDomain* il2cpp_domain;

	extern std::vector<std::function<void()>> init_callbacks;

	void init(HMODULE game_module);

	void call_init_callbacks();

	void init_defaults();

	Il2CppClass* get_class(const char* assemblyName, const char* namespaze, const char* klassName);

	Il2CppMethodPointer get_method_pointer(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount);

	template<typename T>
	T get_method_pointer(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount) {
		return reinterpret_cast<T>(get_method_pointer(assemblyName, namespaze, klassName, name,
			argsCount));
	}

	const MethodInfo* get_method(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount);

	template<typename T>
	const MethodInfo_t<T>* get_method(const char* assemblyName, const char* namespaze,
		const char* klassName, const char* name, int argsCount)
	{
		return reinterpret_cast<const MethodInfo_t<T>*>(get_method(assemblyName, namespaze, klassName, name, argsCount));
	}

	const Il2CppClass* find_class(const char* assemblyName, const char* namespaze,
		const std::function<bool(const Il2CppClass*)>& predict);

	Il2CppMethodPointer find_method(const char* assemblyName, const char* namespaze,
		const char* klassName,
		const std::function<bool(const MethodInfo*)>& predict);

	template<typename T>
	T find_method(const char* assemblyName, const char* namespaze,
		const char* klassName,
		const std::function<bool(const MethodInfo*)>& predict)
	{
		return reinterpret_cast<T>(find_method(assemblyName, namespaze, klassName, predict));
	}
}

#include "il2cpp-api-functions.hpp"
