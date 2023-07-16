#pragma once
template<typename T>
T il2cpp_resolve_icall_type(const char* name) {
	return reinterpret_cast<T>(il2cpp_resolve_icall(name));
}

template<typename T>
T il2cpp_object_new_type(const Il2CppClass* klass) {
	return reinterpret_cast<T>(il2cpp_object_new(klass));
}

template<typename T>
T il2cpp_object_unbox_type(Il2CppObject* obj) {
	return reinterpret_cast<T>(il2cpp_object_unbox(obj));
}

template<typename T>
T il2cpp_value_box_type(Il2CppClass* klass, void* data) {
	return reinterpret_cast<T>(il2cpp_value_box(klass, data));
}

template<typename T>
MethodInfo_t<T>* il2cpp_class_get_method_from_name_type(Il2CppClass* klass, const char* name, int argsCount) {
	return reinterpret_cast<MethodInfo_t<T>*>(il2cpp_class_get_method_from_name(klass, name, argsCount));
}
