#pragma once

#define NOMINMAX

#include <iostream>
#include <string>
#include <cstdio>
#include <MinHook.h>

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-tabledefs.h"

#include "config/config.hpp"

#include "game.hpp"

inline static HWND currentHWnd = nullptr;

#define STATIC \
static void __static__();\
[[maybe_unused]] static const char __static_value__ = (__static__(), 0);\
static void __static__()

inline void PrintStackTrace()
{
	Il2CppString* (*trace)() = il2cpp_symbols::get_method_pointer<Il2CppString * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "StackTraceUtility", "ExtractStackTrace", 0);
	std::wcout << trace()->chars << std::endl;
}

template<typename... Ts, typename = Il2CppReflectionType*>
inline Il2CppClass* GetGenericClass(Il2CppReflectionType* baseRuntimeType, Ts... runtimeTypes)
{
	auto typeArray = reinterpret_cast<Il2CppArraySize*>(il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Type"), sizeof...(runtimeTypes)));

	int i = 0;
	for (const auto type : { runtimeTypes... })
	{
		il2cpp_array_setref(typeArray, i, type);
		i++;
	}

	auto runtimeType = il2cpp_class_get_method_from_name_type<Il2CppReflectionRuntimeType * (*)(Il2CppReflectionType*, Il2CppArraySize*)>(baseRuntimeType->object.klass, "MakeGenericType", 1)->methodPointer(baseRuntimeType, typeArray);
	auto newType = runtimeType->type.type;

	return il2cpp_class_from_type(newType);
}

template<typename... Ts, typename = Il2CppReflectionType*>
inline Il2CppClass* GetArrayClass(Il2CppReflectionType* runtimeType)
{
	auto arrayRuntimeType = il2cpp_class_get_method_from_name_type<Il2CppReflectionRuntimeType * (*)(Il2CppReflectionType*)>(runtimeType->object.klass, "MakeArrayType", 0)->methodPointer(runtimeType);
	auto newType = arrayRuntimeType->type.type;

	return il2cpp_class_from_type(newType);
}

template<typename... Ts, typename = Il2CppReflectionType*>
inline const MethodInfo* GetGenericMethod(const MethodInfo* baseMethodInfo, Ts... runtimeTypes)
{
	auto runtimeMethodInfo = il2cpp_method_get_object(baseMethodInfo, nullptr);
	auto typeArray = reinterpret_cast<Il2CppArraySize*>(il2cpp_array_new(il2cpp_symbols::get_class("mscorlib.dll", "System", "Type"), sizeof...(runtimeTypes)));

	int i = 0;
	for (const auto type : { runtimeTypes... })
	{
		il2cpp_array_setref(typeArray, i, type);
		i++;
	}

	auto runtimeType = il2cpp_class_get_method_from_name_type<Il2CppReflectionMethod * (*)(Il2CppObject*, Il2CppArraySize*)>(runtimeMethodInfo->object.klass, "MakeGenericMethod", 1)->methodPointer(&runtimeMethodInfo->object, typeArray);
	return il2cpp_method_get_from_reflection(runtimeType);
}

template<typename... T, typename R, size_t... S>
InvokerMethod GetInvokerMethod(R(*fn)(Il2CppObject*, T...), index_sequence<S...>)
{
	return *([](Il2CppMethodPointer fn, const MethodInfo* method, void* obj, void** params, void** returnValue)
		{
			*returnValue = reinterpret_cast<void* (*)(void*, ...)>(fn)(obj, params[S]...);
		});
}

template<typename... T, typename R, size_t... S>
InvokerMethod GetInvokerMethod(R(*fn)(void*, T...), index_sequence<S...>)
{
	return *([](Il2CppMethodPointer fn, const MethodInfo* method, void*, void** params, void** returnValue)
		{
			*returnValue = reinterpret_cast<void* (*)(void*, ...)>(fn)(nullptr, params[S]...);
		});
}

static void InvokeDelegateConstructor(Il2CppDelegate* delegate, Il2CppObject* target, const MethodInfo* method)
{
	const MethodInfo* ctor = il2cpp_class_get_method_from_name(delegate->object.klass, ".ctor", 2);

	if (!ctor) {
		return;
	}

	void* ctorArgs[2] = { target, reinterpret_cast<void*>(&method) };
	ctor->invoker_method(ctor->methodPointer, ctor, delegate, ctorArgs, nullptr);
}

template<typename... T, typename R>
inline Il2CppMulticastDelegate* CreateDelegateWithClass(Il2CppClass* klass, Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
{
	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->delegate.invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->virtualMethodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;
		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(&delegate->delegate, target, methodInfo);

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate2020*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(klass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(&delegate->delegate), target, reinterpret_cast<const MethodInfo*>(methodInfo));

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return reinterpret_cast<Il2CppMulticastDelegate*>(delegate);
	}
}

template<typename... T, typename R>
inline Il2CppDelegate* CreateDelegate(Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
{
	const auto delegateClass = il2cpp_symbols::get_class("mscorlib.dll", "System", "Action");

	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppDelegate*>(il2cpp_object_new(delegateClass));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = reinterpret_cast<MethodInfo*>(new MethodInfo{});
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->virtualMethodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(delegate, nullptr, methodInfo);

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		il2cpp_gc_wbarrier_set_field(object, reinterpret_cast<void**>(&(delegate)->target), target);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppDelegate2020*>(il2cpp_object_new(delegateClass));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->invoke_impl = methodInfo->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(delegate), target, reinterpret_cast<const MethodInfo*>(methodInfo));

		auto object = reinterpret_cast<Il2CppObject*>(delegate);

		il2cpp_gc_wbarrier_set_field(object, reinterpret_cast<void**>(&(delegate)->target), target);

		return reinterpret_cast<Il2CppDelegate*>(delegate);
	}
}

template<typename... T, typename R>
inline Il2CppMulticastDelegate* CreateUnityAction(Il2CppObject* target, R(*fn)(Il2CppObject*, T...))
{
	const auto delegateClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction");

	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->delegate.invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = reinterpret_cast<MethodInfo*>(new MethodInfo{});
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->virtualMethodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(&delegate->delegate, target, methodInfo);

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate2020*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(delegateClass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(&delegate->delegate), target, reinterpret_cast<const MethodInfo*>(methodInfo));

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		auto object1 = reinterpret_cast<Il2CppObject*>(&delegate->delegate);

		il2cpp_gc_wbarrier_set_field(object1, reinterpret_cast<void**>(&(delegate->delegate).target), target);

		return reinterpret_cast<Il2CppMulticastDelegate*>(delegate);
	}
}

template<typename... T, typename R>
inline Il2CppReflectionMethod* GetRuntimeMethodInfo(R(*fn)(void*, T...))
{
	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		MethodInfo* methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->virtualMethodPointer = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		return il2cpp_method_get_object(methodInfo, methodInfo->klass);
	}
	else
	{
		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousMethod";
		methodInfo->methodPointer = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		// methodInfo->flags = METHOD_ATTRIBUTE_PINVOKE_IMPL;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		return il2cpp_method_get_object(reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass);
	}
}

template<typename... T, typename R>
inline Il2CppMulticastDelegate* CreateDelegateWithClassStatic(Il2CppClass* klass, R(*fn)(void*, T...))
{
	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->delegate.invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->virtualMethodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(&delegate->delegate, nullptr, methodInfo);

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate2020*>(il2cpp_object_new(klass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(klass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(&delegate->delegate), nullptr, reinterpret_cast<const MethodInfo*>(methodInfo));

		delegate->delegates = il2cpp_array_new(klass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return reinterpret_cast<Il2CppMulticastDelegate*>(delegate);
	}
}

template<typename R>
inline Il2CppMulticastDelegate* CreateDelegateWithClassStatic(Il2CppClass* klass, R(*fn)())
{
	return CreateDelegateWithClassStatic(klass, reinterpret_cast<R(*)(void*)>(fn));
}

template<typename... T, typename R>
inline Il2CppDelegate* CreateDelegateStatic(R(*fn)(void*, T...))
{
	const auto delegateClass = il2cpp_symbols::get_class("mscorlib.dll", "System", "Action");

	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppDelegate*>(il2cpp_object_new(delegateClass));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->virtualMethodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(delegate, nullptr, methodInfo);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppDelegate2020*>(il2cpp_object_new(delegateClass));
		delegate->method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->invoke_impl = methodInfo->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(delegate), nullptr, reinterpret_cast<const MethodInfo*>(methodInfo));

		return reinterpret_cast<Il2CppDelegate*>(delegate);
	}
}

template<typename R>
inline Il2CppDelegate* CreateDelegateStatic(R(*fn)())
{
	return CreateDelegateStatic(reinterpret_cast<R(*)(void*)>(fn));
}

template<typename... T, typename R>
inline Il2CppMulticastDelegate* CreateUnityActionStatic(R(*fn)(void*, T...))
{
	const auto delegateClass = il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine.Events", "UnityAction");

	if (Game::CurrentGameRegion == Game::Region::JPN)
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		delegate->delegate.invoke_impl = reinterpret_cast<Il2CppMethodPointer>(fn);

		MethodInfo* methodInfo = new MethodInfo{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->virtualMethodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);
		methodInfo->return_type = il2cpp_class_get_type(il2cpp_defaults.void_class);

		// methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		// methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo*>(il2cpp_method_get_object(methodInfo, methodInfo->klass));

		InvokeDelegateConstructor(&delegate->delegate, nullptr, methodInfo);

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return delegate;
	}
	else
	{
		auto delegate = reinterpret_cast<Il2CppMulticastDelegate2020*>(il2cpp_object_new(delegateClass));
		delegate->delegate.method_ptr = reinterpret_cast<Il2CppMethodPointer>(fn);
		// delegate->delegate.invoke_impl = il2cpp_class_get_method_from_name(delegateClass, "Invoke", IgnoreNumberOfArguments)->invoker_method;

		MethodInfo2020* methodInfo = new MethodInfo2020{};
		methodInfo->name = "AnonymousStaticMethod";
		methodInfo->methodPointer = delegate->delegate.method_ptr;
		methodInfo->klass = il2cpp_defaults.method_info_class;
		// methodInfo->invoker_method = GetInvokerMethod(fn, index_sequence_for<T...>{});
		methodInfo->slot = kInvalidIl2CppMethodSlot;
		methodInfo->flags = METHOD_ATTRIBUTE_STATIC;
		methodInfo->parameters_count = sizeof...(T);

		methodInfo->nativeFunction = reinterpret_cast<Il2CppMethodPointer>(fn);
		methodInfo->is_marshaled_from_native = true;

		delegate->delegate.method = reinterpret_cast<const MethodInfo2020*>(il2cpp_method_get_object(
			reinterpret_cast<const MethodInfo*>(methodInfo), methodInfo->klass));

		// InvokeDelegateConstructor(reinterpret_cast<Il2CppDelegate*>(&delegate->delegate), nullptr, reinterpret_cast<const MethodInfo*>(methodInfo));

		delegate->delegates = il2cpp_array_new(delegateClass, 1);
		il2cpp_array_setref(delegate->delegates, 0, &delegate->delegate);

		return reinterpret_cast<Il2CppMulticastDelegate*>(delegate);
	}
}

template<typename R>
inline Il2CppMulticastDelegate* CreateUnityActionStatic(R(*fn)())
{
	return CreateUnityActionStatic(reinterpret_cast<R(*)(void*)>(fn));
}

inline Il2CppObject* GetSingletonInstance(Il2CppClass* klass)
{
	if (!klass || !klass->parent)
	{
		return nullptr;
	}
	auto instanceField = il2cpp_class_get_field_from_name_wrap(klass, "_instance");
	if (!instanceField)
	{
		instanceField = il2cpp_class_get_field_from_name_wrap(klass, "instance");
		if (!instanceField)
		{
			return nullptr;
		}
	}
	Il2CppObject* instance;
	il2cpp_field_static_get_value(instanceField, &instance);
	return instance;
}

inline Il2CppObject* GetSingletonInstanceByMethod(Il2CppClass* klass)
{
	if (!klass || !klass->parent)
	{
		return nullptr;
	}
	auto get_Instance = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)()>(klass, "get_Instance", 0);
	if (get_Instance)
	{
		return get_Instance->methodPointer();
	}
	return nullptr;
}

inline Il2CppReflectionType* GetRuntimeType(const char* assemblyName, const char* namespaze, const char* klassName)
{
	return reinterpret_cast<Il2CppReflectionType*>(il2cpp_type_get_object(il2cpp_class_get_type(il2cpp_symbols::get_class(assemblyName, namespaze, klassName))));
}

inline Il2CppReflectionType* GetRuntimeType(Il2CppClass* klass)
{
	return reinterpret_cast<Il2CppReflectionType*>(il2cpp_type_get_object(il2cpp_class_get_type(klass)));
}

inline Il2CppObject* ParseEnum(Il2CppReflectionType* runtimeType, const wstring& name)
{
	return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(Il2CppReflectionType*, Il2CppString*)>("mscorlib.dll", "System", "Enum", "Parse", 2)(runtimeType, il2cpp_string_new16(name.data()));
}

inline Il2CppString* GetEnumName(Il2CppReflectionType* runtimeType, int id)
{
	return il2cpp_symbols::get_method_pointer<Il2CppString * (*)(Il2CppReflectionType*, Il2CppObject*)>("mscorlib.dll", "System", "Enum", "GetName", 2)(runtimeType, il2cpp_value_box(il2cpp_defaults.int32_class, &id));
}

inline uint64_t GetEnumValue(Il2CppObject* runtimeEnum)
{
	return il2cpp_symbols::get_method_pointer<uint64_t(*)(Il2CppObject*)>("mscorlib.dll", "System", "Enum", "ToUInt64", 1)(runtimeEnum);
}

inline Il2CppDelegate* GetButtonCommonOnClickDelegate(Il2CppObject* object)
{
	if (!object)
	{
		return nullptr;
	}
	if (object->klass != il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ButtonCommon"))
	{
		return nullptr;
	}
	auto onClickField = il2cpp_class_get_field_from_name_wrap(object->klass, "m_OnClick");
	Il2CppObject* onClick;
	il2cpp_field_get_value(object, onClickField, &onClick);
	if (onClick)
	{
		auto callsField = il2cpp_class_get_field_from_name_wrap(onClick->klass, "m_Calls");
		Il2CppObject* calls;
		il2cpp_field_get_value(onClick, callsField, &calls);
		if (calls)
		{
			auto runtimeCallsField = il2cpp_class_get_field_from_name_wrap(calls->klass,
				"m_RuntimeCalls");
			Il2CppObject* runtimeCalls;
			il2cpp_field_get_value(calls, runtimeCallsField, &runtimeCalls);

			if (runtimeCalls)
			{
				FieldInfo* itemsField = il2cpp_class_get_field_from_name_wrap(runtimeCalls->klass,
					"_items");
				Il2CppArraySize* arr;
				il2cpp_field_get_value(runtimeCalls, itemsField, &arr);
				if (arr)
				{
					for (int i = 0; i < arr->max_length; i++)
					{
						auto value = reinterpret_cast<Il2CppObject*>(arr->vector[i]);
						if (value)
						{
							auto delegateField = il2cpp_class_get_field_from_name_wrap(value->klass,
								"Delegate");
							Il2CppDelegate* delegate;
							il2cpp_field_get_value(value, delegateField, &delegate);
							if (delegate)
							{
								// Unbox delegate
								auto callbackField = il2cpp_class_get_field_from_name_wrap(
									delegate->target->klass, "callback");
								Il2CppDelegate* callback;
								il2cpp_field_get_value(delegate->target, callbackField, &callback);

								return callback;
							}
						}
					}
				}
			}
		}
	}
	return nullptr;
}

inline uint64_t GetTextIdByName(const wstring& name)
{
	return GetEnumValue(ParseEnum(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), name));
}

inline wstring GetTextIdNameById(int id)
{
	return GetEnumName(GetRuntimeType("umamusume.dll", "Gallop", "TextId"), id)->chars;
}

inline Il2CppObject* GetCurrentViewController()
{
	auto sceneManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "SceneManager"));

	if (!sceneManager)
	{
		return nullptr;
	}

	auto GetCurrentViewController = il2cpp_symbols::find_method<Il2CppObject * (*)(Il2CppObject*)>("umamusume.dll", "Gallop", "SceneManager", [](const MethodInfo* info)
		{
			if (Game::CurrentGameRegion == Game::Region::KOR)
			{
				auto info2020 = reinterpret_cast<const MethodInfo2020*>(info);
				return info2020->name == "GetCurrentViewController"s && !info2020->is_generic;
			}

			return info->name == "GetCurrentViewController"s && !info->is_generic;
		});

	return GetCurrentViewController(sceneManager);
}

inline Il2CppObject* GetCurrentHubViewChildController()
{
	auto viewController = GetCurrentViewController();

	if (!viewController)
	{
		return nullptr;
	}

	if (viewController->klass->parent->klass->name == "HubViewControllerBase"s)
	{
		return il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(viewController->klass, "get_ChildCurrentController", 0)->methodPointer(viewController);
	}

	return nullptr;
}

inline Il2CppObject* GetFrontDialog()
{
	return il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "DialogManager", "GetForeFrontDialog", IgnoreNumberOfArguments)();
}

#ifdef _MSC_VER
inline HWND GetHWND()
{
	std::wstring title = config::custom_title_name;
	if (title.empty())
	{
		title = L"umamusume";
	}

	static HWND hWndFound;

	EnumWindows(reinterpret_cast<WNDENUMPROC>(*([](HWND hWnd, LPARAM lParam)
		{
			WCHAR buf[MAX_CLASS_NAME]{};
			GetClassNameW(hWnd, (LPWSTR)&buf, MAX_CLASS_NAME);

			if (std::wstring(buf).find(L"UnityWndClass") != std::wstring::npos)
			{
				DWORD dwWndProcID = 0;
				GetWindowThreadProcessId(hWnd, &dwWndProcID);

				if (dwWndProcID == GetCurrentProcessId())
				{
					hWndFound = hWnd;
					return FALSE;
				}
			}

			return TRUE;
		}
		)), NULL);

	return hWndFound;
}
#endif

inline void dump_bytes(void* pos)
{
	if (pos)
	{
		printf("Hex dump of %p\n", pos);

		char* memory = reinterpret_cast<char*>(pos);

		for (int i = 0; i < 0x20; i++)
		{
			if (i > 0 && i % 16 == 0)
				printf("\n");

			char byte = *(memory++);

			printf("%02hhX ", byte);
		}

	}
	printf("\n\n");
}

#pragma region HOOK_MACRO
#define ADD_HOOK(_name_, _fmt_) \
	auto _name_##_offset = reinterpret_cast<void*>(_name_##_addr); \
	\
	printf(_fmt_, _name_##_offset); \
	dump_bytes(_name_##_offset); \
	\
	auto _name_##_create_result = MH_CreateHook(_name_##_offset, _name_##_hook, &_name_##_orig); \
	if (_name_##_create_result != MH_OK) \
	{\
		cout << "WARN: " << #_name_ << " Create Failed: " << MH_StatusToString(_name_##_create_result) << endl; \
	}\
	auto _name_##_result = MH_EnableHook(_name_##_offset); \
	if (_name_##_result != MH_OK) \
	{\
		cout << "WARN: " << #_name_ << " Failed: " << MH_StatusToString(_name_##_result) << " LastError: " << GetLastError() << endl << endl; \
		_name_##_orig = _name_##_addr; \
	}
#pragma endregion
