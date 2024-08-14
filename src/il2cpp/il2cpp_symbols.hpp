#pragma once

#define NOMINMAX

#include <Windows.h>

#include <functional>

#include "il2cpp-object-internals.h"

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
struct Nullable
{
	T value;
	bool has_value;
};

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
	bool is3dSound;
	int atomSourceListIndex;
	Il2CppString* cueSheetName;
	Il2CppString* cueName;
	int cueId;
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

struct OrderBlock
{
	int order;
	Il2CppDelegate* callback;
};

struct ObscuredBool
{
	int8_t currentCryptoKey;
	int hiddenValue;
	bool inited;
	bool fakeValue;
	bool fakeValueActive;

	bool GetDecrypted() const
	{
		int8_t key = currentCryptoKey;
		if (!key)
		{
			key = 9;
		}

		return (hiddenValue ^ key) != 181;
	}
};

struct ObscuredInt
{
	int currentCryptoKey;
	int hiddenValue;
	bool inited;
	int fakeValue;
	bool fakeValueActive;

	int GetDecrypted() const
	{
		if (!currentCryptoKey)
		{
			return hiddenValue ^ 123456;
		}

		return hiddenValue ^ currentCryptoKey;
	}
};

struct ObscuredLong
{
	int64_t currentCryptoKey;
	int64_t hiddenValue;
	bool inited;
	int64_t fakeValue;
	bool fakeValueActive;

	int64_t GetDecrypted() const
	{
		if (!currentCryptoKey)
		{
			return hiddenValue ^ 123456LL;
		}

		return hiddenValue ^ currentCryptoKey;
	}
};

enum DialogCommonFormType
{
	SMALL_NO_BUTTON,
	SMALL_ONE_BUTTON,
	SMALL_TWO_BUTTON,
	SMALL_THREE_BUTTON,
	MIDDLE_NO_BUTTON,
	MIDDLE_ONE_BUTTON,
	MIDDLE_TWO_BUTTON,
	MIDDLE_THREE_BUTTON,
	BIG_NO_BUTTON,
	BIG_ONE_BUTTON,
	BIG_TWO_BUTTON,
	BIG_THREE_BUTTON,
	WITHOUT_FRAME
};

enum LiveState
{
	Play,
	Pause,
	ShowSkipConfirmDialog,
	ShowMenuDialog,
	ShowSettingDialog,
	HideUI
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

namespace il2cpp_symbols
{
	extern Il2CppDomain* il2cpp_domain;

	void init(HMODULE game_module);

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
