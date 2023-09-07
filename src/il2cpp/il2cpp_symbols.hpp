#pragma once

#if _MSC_VER
typedef wchar_t Il2CppChar;
#elif __has_feature(cxx_unicode_literals)
typedef char16_t Il2CppChar;
#else
typedef uint16_t Il2CppChar;
#endif

struct Int32Object;

struct Boolean
{
	bool m_value;
};

struct Byte
{
	uint8_t m_value;
};

// UnityEngine.Color
struct Color_t
{
public:
	// System.Single UnityEngine.Color::r
	float r;
	// System.Single UnityEngine.Color::g
	float g;
	// System.Single UnityEngine.Color::b
	float b;
	// System.Single UnityEngine.Color::a
	float a;
};

// UnityEngine.Color32
struct Color32_t
{
public:
	// System.Single UnityEngine.Color32::rgba
	unsigned int rgba;
};


// UnityEngine.ScreenOrientation
enum class ScreenOrientation {
	Unknown,
	Portrait,
	PortraitUpsideDown,
	LandscapeLeft,
	LandscapeRight,
	AutoRotation,
	Landscape = 3
};

// UnityEngine.Vector2
struct Vector2_t
{
public:
	// System.Single UnityEngine.Vector2::x
	float x;
	// System.Single UnityEngine.Vector2::y
	float y;
};

// UnityEngine.Vector2Int
struct Vector2Int_t
{
public:
	// System.Int32 UnityEngine.Vector2Int::m_X
	int x;
	// System.Int32 UnityEngine.Vector2Int::m_Y
	int y;
};

// UnityEngine.Vector3
struct Vector3_t
{
public:
	// System.Single UnityEngine.Vector3::x
	float x;
	// System.Single UnityEngine.Vector3::y
	float y;
	// System.Single UnityEngine.Vector3::z
	float z;
};

// UnityEngine.Vector4
struct Vector4_t
{
public:
	// System.Single UnityEngine.Vector4::x
	float x;
	// System.Single UnityEngine.Vector4::y
	float y;
	// System.Single UnityEngine.Vector4::z
	float z;
	// System.Single UnityEngine.Vector4::w
	float w;
};

struct Rect_t
{
public:
	float x;
	float y;
	float width;
	float height;
};

struct Resolution_t
{
public:
	int width;
	int height;
	int herz;
};

// UnityEngine.TextGenerationSettings
struct TextGenerationSettings_t
{
public:
	// UnityEngine.Font UnityEngine.TextGenerationSettings::font
	void* font;
	// UnityEngine.Color UnityEngine.TextGenerationSettings::color
	Color_t color;
	// System.Int32 UnityEngine.TextGenerationSettings::fontSize
	int32_t fontSize;
	// System.Single UnityEngine.TextGenerationSettings::lineSpacing
	float lineSpacing;
	// System.Boolean UnityEngine.TextGenerationSettings::richText
	bool richText;
	// System.Single UnityEngine.TextGenerationSettings::scaleFactor
	float scaleFactor;
	// UnityEngine.FontStyle UnityEngine.TextGenerationSettings::fontStyle
	int32_t fontStyle;
	// UnityEngine.TextAnchor UnityEngine.TextGenerationSettings::textAnchor
	int32_t textAnchor;
	// System.Boolean UnityEngine.TextGenerationSettings::alignByGeometry
	bool alignByGeometry;
	// System.Boolean UnityEngine.TextGenerationSettings::resizeTextForBestFit
	bool resizeTextForBestFit;
	// System.Int32 UnityEngine.TextGenerationSettings::resizeTextMinSize
	int32_t resizeTextMinSize;
	// System.Int32 UnityEngine.TextGenerationSettings::resizeTextMaxSize
	int32_t resizeTextMaxSize;
	// System.Boolean UnityEngine.TextGenerationSettings::updateBounds
	bool updateBounds;
	// UnityEngine.VerticalWrapMode UnityEngine.TextGenerationSettings::verticalOverflow
	int32_t verticalOverflow;
	// UnityEngine.HorizontalWrapMode UnityEngine.TextGenerationSettings::horizontalOverflow
	int32_t horizontalOverflow;
	// UnityEngine.Vector2 UnityEngine.TextGenerationSettings::generationExtents
	Vector2_t  generationExtents;
	// UnityEngine.Vector2 UnityEngine.TextGenerationSettings::pivot
	Vector2_t  pivot;
	// System.Boolean UnityEngine.TextGenerationSettings::generateOutOfBounds
	bool generateOutOfBounds;
};

enum Il2CppTypeEnum
{
	IL2CPP_TYPE_END = 0x00,       /* End of List */
	IL2CPP_TYPE_VOID = 0x01,
	IL2CPP_TYPE_BOOLEAN = 0x02,
	IL2CPP_TYPE_CHAR = 0x03,
	IL2CPP_TYPE_I1 = 0x04,
	IL2CPP_TYPE_U1 = 0x05,
	IL2CPP_TYPE_I2 = 0x06,
	IL2CPP_TYPE_U2 = 0x07,
	IL2CPP_TYPE_I4 = 0x08,
	IL2CPP_TYPE_U4 = 0x09,
	IL2CPP_TYPE_I8 = 0x0a,
	IL2CPP_TYPE_U8 = 0x0b,
	IL2CPP_TYPE_R4 = 0x0c,
	IL2CPP_TYPE_R8 = 0x0d,
	IL2CPP_TYPE_STRING = 0x0e,
	IL2CPP_TYPE_PTR = 0x0f,
	IL2CPP_TYPE_BYREF = 0x10,
	IL2CPP_TYPE_VALUETYPE = 0x11,
	IL2CPP_TYPE_CLASS = 0x12,
	IL2CPP_TYPE_VAR = 0x13,
	IL2CPP_TYPE_ARRAY = 0x14,
	IL2CPP_TYPE_GENERICINST = 0x15,
	IL2CPP_TYPE_TYPEDBYREF = 0x16,
	IL2CPP_TYPE_I = 0x18,
	IL2CPP_TYPE_U = 0x19,
	IL2CPP_TYPE_FNPTR = 0x1b,
	IL2CPP_TYPE_OBJECT = 0x1c,
	IL2CPP_TYPE_SZARRAY = 0x1d,
	IL2CPP_TYPE_MVAR = 0x1e,
	IL2CPP_TYPE_CMOD_REQD = 0x1f,
	IL2CPP_TYPE_CMOD_OPT = 0x20,
	IL2CPP_TYPE_INTERNAL = 0x21,

	IL2CPP_TYPE_MODIFIER = 0x40,
	IL2CPP_TYPE_SENTINEL = 0x41,
	IL2CPP_TYPE_PINNED = 0x45,

	IL2CPP_TYPE_ENUM = 0x55
};

typedef struct Il2CppType
{
	void* dummy;
	unsigned int attrs : 16;
	Il2CppTypeEnum type : 8;
	unsigned int num_mods : 6;
	unsigned int byref : 1;
	unsigned int pinned : 1;
} Il2CppType;

typedef struct FieldInfo
{
	const char* name;
	const Il2CppType* type;
	void* parent;
	int32_t offset; // If offset is -1, then it's thread static
	uint32_t token;
} FieldInfo;

struct MethodInfo;

typedef struct Il2CppClass
{
	// The following fields are always valid for a Il2CppClass structure
	const void* image;
	void* gc_desc;
	const char* name;
	const char* namespaze;
	Il2CppType byval_arg;
	Il2CppType this_arg;
	Il2CppClass* element_class;
	Il2CppClass* castClass;
	Il2CppClass* declaringType;
	Il2CppClass* parent;
	void* generic_class;
	void* typeMetadataHandle; // non-NULL for Il2CppClass's constructed from type defintions
	const void* interopData;
	Il2CppClass* klass; // hack to pretend we are a MonoVTable. Points to ourself
	// End always valid fields

	// The following fields need initialized before access. This can be done per field or as an aggregate via a call to Class::Init
	FieldInfo* fields; // Initialized in SetupFields
	const void* events; // Initialized in SetupEvents
	const void* properties; // Initialized in SetupProperties
	const MethodInfo** methods; // Initialized in SetupMethods
	Il2CppClass** nestedTypes; // Initialized in SetupNestedTypes
	Il2CppClass** implementedInterfaces; // Initialized in SetupInterfaces
	void* interfaceOffsets; // Initialized in Init
	void* static_fields; // Initialized in Init
	const void* rgctx_data; // Initialized in Init
	// used for fast parent checks
	Il2CppClass** typeHierarchy; // Initialized in SetupTypeHierachy
	// End initialization required fields

	void* unity_user_data;

	uint32_t initializationExceptionGCHandle;

	uint32_t cctor_started;
	uint32_t cctor_finished;
	size_t cctor_thread;

	// Remaining fields are always valid except where noted
	void* genericContainerHandle;
	uint32_t instance_size; // valid when size_inited is true
	uint32_t actualSize;
	uint32_t element_size;
	int32_t native_size;
	uint32_t static_fields_size;
	uint32_t thread_static_fields_size;
	int32_t thread_static_fields_offset;
	uint32_t flags;
	uint32_t token;

	uint16_t method_count; // lazily calculated for arrays, i.e. when rank > 0
	uint16_t property_count;
	uint16_t field_count;
	uint16_t event_count;
	uint16_t nested_type_count;
	uint16_t vtable_count; // lazily calculated for arrays, i.e. when rank > 0
	uint16_t interfaces_count;
	uint16_t interface_offsets_count; // lazily calculated for arrays, i.e. when rank > 0

	uint8_t typeHierarchyDepth; // Initialized in SetupTypeHierachy
	uint8_t genericRecursionDepth;
	uint8_t rank;
	uint8_t minimumAlignment; // Alignment of this type
	uint8_t naturalAligment; // Alignment of this type without accounting for packing
	uint8_t packingSize;

	// this is critical for performance of Class::InitFromCodegen. Equals to initialized && !has_initialization_error at all times.
	// Use Class::UpdateInitializedAndNoError to update
	uint8_t initialized_and_no_error : 1;

	uint8_t valuetype : 1;
	uint8_t initialized : 1;
	uint8_t enumtype : 1;
	uint8_t is_generic : 1;
	uint8_t has_references : 1; // valid when size_inited is true
	uint8_t init_pending : 1;
	uint8_t size_init_pending : 1;
	uint8_t size_inited : 1;
	uint8_t has_finalize : 1;
	uint8_t has_cctor : 1;
	uint8_t is_blittable : 1;
	uint8_t is_import_or_windows_runtime : 1;
	uint8_t is_vtable_initialized : 1;
	uint8_t has_initialization_error : 1;
	void* vtable[0];
} Il2CppClass;

struct ParameterInfo
{
	const char* name;
	int32_t position;
	uint32_t token;
	const Il2CppType* parameter_type;
};

typedef struct Il2CppGenericContainer
{
	/* index of the generic type definition or the generic method definition corresponding to this container */
	int32_t ownerIndex; // either index into Il2CppClass metadata array or Il2CppMethodDefinition array
	int32_t type_argc;
	/* If true, we're a generic method, otherwise a generic type definition. */
	int32_t is_method;
	/* Our type parameters. */
	int32_t genericParameterStart;
} Il2CppGenericContainer;

typedef void (*Il2CppMethodPointer)();

typedef void* (*InvokerMethod)(Il2CppMethodPointer, const MethodInfo*, void*, void**);

struct MethodInfo
{
	Il2CppMethodPointer methodPointer;
	InvokerMethod invoker_method;
	const char* name;
	Il2CppClass* klass;
	const Il2CppType* return_type;
	const ParameterInfo* parameters;
	union
	{
		uintptr_t rgctx_data;
		uintptr_t methodDefinition;
	};
	union
	{
		uintptr_t genericMethod;
		Il2CppGenericContainer* genericContainer;
	};
	uint32_t token;
	uint16_t flags;
	uint16_t iflags;
	uint16_t slot;
	uint8_t parameters_count;
	uint8_t is_generic : 1;
	uint8_t is_inflated : 1;
	uint8_t wrapper_type : 1;
	uint8_t is_marshaled_from_native : 1;
};

template<typename T>
struct MethodInfo_t
{
	T methodPointer;
	InvokerMethod invoker_method;
	const char* name;
	Il2CppClass* klass;
	const Il2CppType* return_type;
	const ParameterInfo* parameters;
	union
	{
		uintptr_t rgctx_data;
		uintptr_t methodDefinition;
	};
	union
	{
		uintptr_t genericMethod;
		Il2CppGenericContainer* genericContainer;
	};
	uint32_t token;
	uint16_t flags;
	uint16_t iflags;
	uint16_t slot;
	uint8_t parameters_count;
	uint8_t is_generic : 1;
	uint8_t is_inflated : 1;
	uint8_t wrapper_type : 1;
	uint8_t is_marshaled_from_native : 1;
};



struct Il2CppObject
{
	union
	{
		Il2CppClass* klass;
		void* vtable;
	};
	void* monitor;
};

// not real Il2CppString class
struct Il2CppString
{
	Il2CppObject object;
	int32_t length;                             ///< Length of string *excluding* the trailing null (which is included in 'chars').
	Il2CppChar start_char[0];
};

typedef struct PropertyInfo {
	Il2CppClass* parent;
	const char* name;
	const MethodInfo* get;
	const MethodInfo* set;
	uint32_t attrs;
	uint32_t token;
} PropertyInfo;

typedef struct Il2CppArraySize
{
	Il2CppObject obj;
	void* bounds;
	uintptr_t max_length;
	alignas(8)
		void* vector[0];
} Il2CppArraySize;


template<typename T>
struct Il2CppArraySize_t
{
	Il2CppObject obj;
	void* bounds;
	uintptr_t max_length;
	alignas(8)
	T vector[0];
};

static const size_t kIl2CppSizeOfArray = (offsetof(Il2CppArraySize, vector));

template<typename K, typename V>
struct Entry
{
	int hashCode;
	int next;
	K key;
	V value;
};

struct CourseBaseObjectContext
{
	Il2CppObject* coursePrefab;
	Il2CppObject* courseGrassFurPrefab;
	Il2CppObject* monitorRenderTexture;
	Il2CppArraySize* swapTextures;
	Il2CppArraySize* swapSubTextures;
	Il2CppObject* postFilmSetGroup;
	Il2CppObject* grassParam;
};

struct RaceLoaderManagerCourceContext
{
	int courseId;
	int timeEnum;
	int seasonEnum;
	int turfGoalGate;
	int turfGoalFlower;
	int dirtGoalGate;
	int dirtGoalFlower;
	int skydomeCourseId;
	int skydomeSeasonEnum;
	int skydomeWeatherEnum;
	int skydomeTimeEnum;
	int audienceEnum;
	int audienceWeatherEnum;
	int audienceSeasonEnum;
	int treeWeaterEnum;
	int treeTimeEnum;
	int RotationCategoryEnum;
	int lightProbeId;
	Il2CppArraySize* materialTeturePairs;
	Il2CppArraySize* materialSubTexturePairs;
	bool halfStartGate;
	int CourseStartGateBaseId;
};

struct MoviePlayerHandle
{
	uint32_t id;
};

struct CriAtomExPlayback
{
	uint32_t id;
};

struct AudioPlayback
{
	CriAtomExPlayback criAtomExPlayback;
	bool isError;
	int soundGroup;
};

struct Randomize3dConfig
{
};

struct CuePos3dInfo
{
	float coneInsideAngle;
	float coneOutsideAngle;
	float minAttenuationDistance;
	float maxAttenuationDistance;
	float sourceRadius;
	float interiorDistance;
	float dopplerFactor;
	Randomize3dConfig randomPos;
	USHORT distanceAisacControl;
	USHORT listenerBaseAngleAisacControl;
	USHORT sourceBaseAngleAisacControl;
	USHORT listenerBaseElevationAisacControl;
	USHORT sourceBaseElevationAisacControl;
	USHORT reserved[1];
};

struct GameVariableInfo
{
	char* name;
	UINT id;
	float gameValue;
};

struct CueInfo
{
	int id;
	int type;
	char* name;
	char* userData;
	size_t length;
	USHORT categories[16];
	short numLimits;
	USHORT numBlocks;
	USHORT numTracks;
	USHORT numRelatedWaveForms;
	char priority;
	char headerVisibility;
	char ignore_player_parameter;
	char probability;
	int panType;
	CuePos3dInfo pos3dInfo;
	GameVariableInfo gameVariableInfo;
};

struct Scene
{
	int handle;
};

typedef struct Il2CppReflectionMethod Il2CppReflectionMethod;

typedef struct Il2CppDelegate
{
	Il2CppObject object;
	/* The compiled code of the target method */
	Il2CppMethodPointer method_ptr;
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
} Il2CppDelegate;

typedef struct MulticastDelegate : Il2CppDelegate {
	Il2CppArraySize* delegates;
} MulticastDelegate;

// function types
typedef Il2CppString* (*il2cpp_string_new_utf16_t)(const wchar_t* str, unsigned int len);
typedef Il2CppString* (*il2cpp_string_new_t)(const char* str);
typedef void* (*il2cpp_domain_get_t)();
typedef void** (*il2cpp_domain_get_assemblies_t)(void* domain, std::size_t* size);
typedef void* (*il2cpp_domain_assembly_open_t)(void* domain, const char* name);
typedef void* (*il2cpp_assembly_get_image_t)(void* assembly);
typedef Il2CppClass* (*il2cpp_class_from_name_t)(void* image, const char* namespaze, const char* name);
typedef MethodInfo* (*il2cpp_class_get_methods_t)(Il2CppClass* klass, void** iter);
typedef MethodInfo* (*il2cpp_class_get_method_from_name_t)(Il2CppClass* klass, const char* name, int argsCount);
typedef MethodInfo* (*il2cpp_method_get_from_reflection_t)(Il2CppObject* ref);
typedef const Il2CppType* (*il2cpp_method_get_param_t)(const MethodInfo* method, uint32_t index);
typedef Il2CppObject* (*il2cpp_object_new_t)(Il2CppClass* klass);
typedef void (*il2cpp_add_internal_call_t)(const char* name, uintptr_t pointer);
typedef void* (*il2cpp_resolve_icall_t)(const char* name);
typedef Il2CppArraySize* (*il2cpp_array_new_t)(Il2CppClass* klass, uintptr_t count);
typedef void* (*il2cpp_thread_attach_t)(void* domain);
typedef void (*il2cpp_thread_detach_t)(void* thread);
typedef const Il2CppType* (*il2cpp_class_get_type_t)(Il2CppClass* klass);
typedef uint32_t(*il2cpp_class_get_type_token_t)(Il2CppClass* klass);
typedef FieldInfo* (*il2cpp_class_get_field_from_name_t)(Il2CppClass* klass, const char* name);
typedef void (*il2cpp_field_get_value_t)(Il2CppObject* obj, FieldInfo* field, void* value);
typedef void (*il2cpp_field_set_value_t)(Il2CppObject* obj, FieldInfo* field, void* value);
typedef void (*il2cpp_field_set_value_object_t)(Il2CppObject* obj, FieldInfo* field, Il2CppObject* value);
typedef void (*il2cpp_field_static_get_value_t)(FieldInfo* field, void* value);
typedef void (*il2cpp_field_static_set_value_t)(FieldInfo* field, void* value);
typedef const Il2CppType* (*il2cpp_field_get_type_t)(FieldInfo* field);
typedef Il2CppObject* (*il2cpp_type_get_object_t)(const Il2CppType* type);
typedef const char* (*il2cpp_image_get_name_t)(void* image);
typedef size_t(*il2cpp_image_get_class_count_t)(void* image);
typedef Il2CppClass* (*il2cpp_image_get_class_t)(void* image, size_t index);
typedef bool (*il2cpp_type_is_byref_t)(const Il2CppType* type);
typedef uint32_t(*il2cpp_method_get_flags_t)(const MethodInfo* mehod, uint32_t* iflags);
typedef const Il2CppType* (*il2cpp_method_get_return_type_t)(const MethodInfo* method);
typedef Il2CppClass* (*il2cpp_class_from_type_t)(const Il2CppType* type);
typedef const char* (*il2cpp_class_get_name_t)(Il2CppClass* klass);
typedef const PropertyInfo* (*il2cpp_class_get_properties_t)(Il2CppClass* klass, void** iter);
typedef bool (*il2cpp_class_is_enum_t)(const Il2CppClass* klass);
typedef FieldInfo* (*il2cpp_class_get_fields_t)(Il2CppClass* klass, void** iter);
typedef const char* (*il2cpp_method_get_name_t)(const MethodInfo* method);
typedef uint32_t(*il2cpp_method_get_param_count_t)(const MethodInfo* method);
typedef const char* (*il2cpp_method_get_param_name_t)(const MethodInfo* method, uint32_t index);
typedef Il2CppClass* (*il2cpp_class_get_parent_t)(Il2CppClass* klass);
typedef Il2CppClass* (*il2cpp_class_get_interfaces_t)(Il2CppClass* klass, void** iter);
typedef const char* (*il2cpp_class_get_namespace_t)(Il2CppClass* klass);
typedef int (*il2cpp_class_get_flags_t)(const Il2CppClass* klass);
typedef bool (*il2cpp_class_is_valuetype_t)(const Il2CppClass* klass);
typedef uint32_t(*il2cpp_property_get_flags_t) (PropertyInfo* prop);
typedef const MethodInfo* (*il2cpp_property_get_get_method_t) (const PropertyInfo* prop);
typedef const MethodInfo* (*il2cpp_property_get_set_method_t) (const PropertyInfo* prop);
typedef const char* (*il2cpp_property_get_name_t) (const PropertyInfo* prop);
typedef Il2CppClass* (*il2cpp_property_get_parent_t) (const PropertyInfo* prop);
typedef int (*il2cpp_field_get_flags_t)(FieldInfo* field);
typedef const char* (*il2cpp_field_get_name_t)(FieldInfo* field);
typedef Il2CppClass* (*il2cpp_field_get_parent_t)(FieldInfo* field);
typedef size_t (*il2cpp_field_get_offset_t)(FieldInfo* field);
typedef const PropertyInfo* (*il2cpp_class_get_property_from_name_t)(Il2CppClass* klass, const char* name);
typedef void (*il2cpp_runtime_object_init_t)(Il2CppObject* obj);
typedef void (*il2cpp_runtime_object_init_exception_t)(Il2CppObject* obj, void** exc);
typedef Il2CppObject* (*il2cpp_value_box_t)(Il2CppClass* klass, void* data);
typedef void* (*il2cpp_object_unbox_t)(Il2CppObject* obj);
typedef bool (*il2cpp_is_vm_thread_t)(void* thread);
typedef void* (*il2cpp_thread_current_t)();
typedef uint32_t(*il2cpp_gchandle_new_t)(Il2CppObject* obj, bool pinned);
typedef Il2CppObject* (*il2cpp_gchandle_get_target_t)(uint32_t gchandle);
typedef void (*il2cpp_gchandle_free_t)(uint32_t gchandle);

// function defines
extern il2cpp_string_new_utf16_t il2cpp_string_new_utf16;
extern il2cpp_string_new_t il2cpp_string_new;
extern il2cpp_domain_get_t il2cpp_domain_get;
extern il2cpp_domain_get_assemblies_t il2cpp_domain_get_assemblies;
extern il2cpp_domain_assembly_open_t il2cpp_domain_assembly_open;
extern il2cpp_assembly_get_image_t il2cpp_assembly_get_image;
extern il2cpp_class_from_name_t il2cpp_class_from_name;
extern il2cpp_class_get_methods_t il2cpp_class_get_methods;
extern il2cpp_class_get_method_from_name_t il2cpp_class_get_method_from_name;
extern il2cpp_method_get_from_reflection_t il2cpp_method_get_from_reflection;
extern il2cpp_method_get_param_t il2cpp_method_get_param;
extern il2cpp_object_new_t il2cpp_object_new;
extern il2cpp_add_internal_call_t il2cpp_add_internal_call;
extern il2cpp_resolve_icall_t il2cpp_resolve_icall;
extern il2cpp_array_new_t il2cpp_array_new;
extern il2cpp_thread_attach_t il2cpp_thread_attach;
extern il2cpp_thread_detach_t il2cpp_thread_detach;
extern il2cpp_class_get_type_t il2cpp_class_get_type;
extern il2cpp_class_get_type_token_t il2cpp_class_get_type_token;
extern il2cpp_class_get_field_from_name_t il2cpp_class_get_field_from_name;
extern il2cpp_field_get_value_t il2cpp_field_get_value;
extern il2cpp_field_set_value_t il2cpp_field_set_value;
extern il2cpp_field_set_value_object_t il2cpp_field_set_value_object;
extern il2cpp_field_static_get_value_t il2cpp_field_static_get_value;
extern il2cpp_field_static_set_value_t il2cpp_field_static_set_value;
extern il2cpp_field_get_type_t il2cpp_field_get_type;
extern il2cpp_type_get_object_t il2cpp_type_get_object;
extern il2cpp_image_get_name_t il2cpp_image_get_name;
extern il2cpp_image_get_class_count_t il2cpp_image_get_class_count;
extern il2cpp_image_get_class_t il2cpp_image_get_class;
extern il2cpp_type_is_byref_t il2cpp_type_is_byref;
extern il2cpp_method_get_flags_t il2cpp_method_get_flags;
extern il2cpp_method_get_return_type_t il2cpp_method_get_return_type;
extern il2cpp_class_from_type_t il2cpp_class_from_type;
extern il2cpp_class_get_name_t il2cpp_class_get_name;
extern il2cpp_class_get_properties_t il2cpp_class_get_properties;
extern il2cpp_class_is_enum_t il2cpp_class_is_enum;
extern il2cpp_class_get_fields_t il2cpp_class_get_fields;
extern il2cpp_method_get_name_t il2cpp_method_get_name;
extern il2cpp_method_get_param_count_t il2cpp_method_get_param_count;
extern il2cpp_method_get_param_name_t il2cpp_method_get_param_name;
extern il2cpp_class_get_parent_t il2cpp_class_get_parent;
extern il2cpp_class_get_interfaces_t il2cpp_class_get_interfaces;
extern il2cpp_class_get_namespace_t il2cpp_class_get_namespace;
extern il2cpp_class_get_flags_t il2cpp_class_get_flags;
extern il2cpp_class_is_valuetype_t il2cpp_class_is_valuetype;
extern il2cpp_property_get_flags_t il2cpp_property_get_flags;
extern il2cpp_property_get_get_method_t il2cpp_property_get_get_method;
extern il2cpp_property_get_set_method_t il2cpp_property_get_set_method;
extern il2cpp_property_get_name_t il2cpp_property_get_name;
extern il2cpp_property_get_parent_t il2cpp_property_get_parent;
extern il2cpp_field_get_flags_t il2cpp_field_get_flags;
extern il2cpp_field_get_name_t il2cpp_field_get_name;
extern il2cpp_field_get_parent_t il2cpp_field_get_parent;
extern il2cpp_field_get_offset_t il2cpp_field_get_offset;
extern il2cpp_class_get_property_from_name_t il2cpp_class_get_property_from_name;
extern il2cpp_runtime_object_init_t il2cpp_runtime_object_init;
extern il2cpp_runtime_object_init_exception_t il2cpp_runtime_object_init_exception;
extern il2cpp_value_box_t il2cpp_value_box;
extern il2cpp_object_unbox_t il2cpp_object_unbox;
extern il2cpp_is_vm_thread_t il2cpp_is_vm_thread;
extern il2cpp_thread_current_t il2cpp_thread_current;
extern il2cpp_gchandle_new_t il2cpp_gchandle_new;
extern il2cpp_gchandle_get_target_t il2cpp_gchandle_get_target;
extern il2cpp_gchandle_free_t il2cpp_gchandle_free;

char* il2cpp_array_addr_with_size(void* arr, int32_t size, uintptr_t idx);

// array macro
#define il2cpp_array_addr(array, type, index) ((type*)(void*) il2cpp_array_addr_with_size (array, sizeof (type), index))

#define il2cpp_array_setref(array, index, value)  \
    do {    \
        void* *__p = (void* *) il2cpp_array_addr ((array), void*, (index));; \
         *__p = (value);    \
    } while (0)

namespace il2cpp_symbols
{
	inline void* il2cpp_domain = nullptr;

	void init(HMODULE game_module);

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
		return reinterpret_cast<MethodInfo_t<T>*>(get_method(assemblyName, namespaze, klassName, name, argsCount));
	}

	Il2CppClass* find_class(const char* assemblyName, const char* namespaze,
		const std::function<bool(Il2CppClass*)>& predict);

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
