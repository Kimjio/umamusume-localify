#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Object.hpp"

#include "../../umamusume/Gallop/DialogCommon.hpp"
#include "../../umamusume/Gallop/DialogManager.hpp"

namespace
{
void* FindObjectsByType_addr = nullptr;

void* FindObjectsOfType_addr = nullptr;

void* Internal_CloneSingleWithParent_addr = nullptr;
void* Internal_CloneSingleWithParent_orig = nullptr;

void* Internal_CloneSingle_addr = nullptr;
void* Internal_CloneSingle_orig = nullptr;

void* DontDestroyOnLoad_addr = nullptr;

void* GetName_addr = nullptr;

void* SetName_addr = nullptr;

void* IsNativeObjectAlive_addr = nullptr;
}

static bool UpdateHomeMenuMainButton()
{
	if (Game::CurrentGameRegion != Game::Region::KOR)
	{
		return true;
	}

	auto dialog = GetFrontDialog();
	auto data = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(dialog->klass, "get_DialogData", 0)->methodPointer(dialog);

	auto ContentsObjectField = il2cpp_class_get_field_from_name_wrap(data->klass, "ContentsObject");
	Il2CppObject* ContentsObject;
	il2cpp_field_get_value(data, ContentsObjectField, &ContentsObject);

	auto homeMenuMain = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(ContentsObject->klass, "GetComponent", 1)->methodPointer(ContentsObject, GetRuntimeType("umamusume.dll", "Gallop", "DialogHomeMenuMain"));

	if (homeMenuMain)
	{
		auto _serialButtonField = il2cpp_class_get_field_from_name_wrap(homeMenuMain->klass, "_serialButton");
		Il2CppObject* _serialButton;
		il2cpp_field_get_value(homeMenuMain, _serialButtonField, &_serialButton);

		if (!GetButtonCommonOnClickDelegate(_serialButton))
		{
			return false;
		}

		auto fn = *[](void*)
			{
				auto gallopCpnFn = *[](void*)
					{
						il2cpp_symbols::get_method_pointer<void (*)()>("umamusume.dll", "Gallop", "DialogSerialInput", "CreateDialog", -1)();
					};

				auto kakaoCpnFn = *[](void*)
					{
						auto KakaoManager = il2cpp_symbols::get_class("umamusume.dll", "", "KakaoManager");
						auto managerInstanceField = il2cpp_class_get_field_from_name_wrap(KakaoManager, "instance");

						Il2CppObject* manager;
						il2cpp_field_static_get_value(managerInstanceField, &manager);

						il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*, Il2CppDelegate*)>(manager->klass, "OnKakaoShowCouponPopup", 2)->methodPointer(manager, nullptr, nullptr);
					};

				auto dialogData = Gallop::DialogCommon::Data();
				dialogData.SetSimpleTwoButtonMessage(
					il2cpp_symbols::get_method_pointer<Il2CppString * (*)(uint64_t)>(
						"umamusume.dll", "Gallop", "LocalizeExtention", "Text", 1
					)(GetTextIdByName(L"Menu0136")),
					il2cpp_string_new("Kakao Games 쿠폰 입력 창을 열겠습니까?"),
					CreateDelegateStatic(kakaoCpnFn),
					GetTextIdByName(L"Common0002"),
					GetTextIdByName(L"Common0003"),
					CreateDelegateStatic(gallopCpnFn));

				Gallop::DialogManager::PushDialog(dialogData);
			};

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(_serialButton->klass, "SetOnClick", 1)->methodPointer(_serialButton, &CreateUnityActionStatic(fn)->delegate);

		return true;
	}
	return false;
}

static Il2CppObject* Internal_CloneSingle_hook(Il2CppObject* original)
{
	auto cloned = reinterpret_cast<decltype(Internal_CloneSingle_hook)*>(Internal_CloneSingle_orig)(original);

	if (Game::CurrentGameRegion == Game::Region::KOR && wstring(UnityEngine::Object::Name(cloned)->chars).find(L"DialogHomeMenuMain") != wstring::npos)
	{
		auto homeMenuMain = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppReflectionType*)>(cloned->klass, "GetComponent", 1)->methodPointer(cloned, GetRuntimeType("umamusume.dll", "Gallop", "DialogHomeMenuMain"));

		auto _dataTransButtonField = il2cpp_class_get_field_from_name_wrap(homeMenuMain->klass, "_dataTransButton");
		Il2CppObject* _dataTransButton;
		il2cpp_field_get_value(homeMenuMain, _dataTransButtonField, &_dataTransButton);

		auto gameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_dataTransButton->klass, "get_gameObject", 0)->methodPointer(_dataTransButton);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(gameObject->klass, "SetActive", 1)->methodPointer(gameObject, true);

		static Il2CppDelegate* updateHomeMenuMainButton;
		updateHomeMenuMainButton = &CreateDelegateWithClassStatic(il2cpp_symbols::get_class("DOTween.dll", "DG.Tweening", "TweenCallback"), *([](void*)
			{
				if (!UpdateHomeMenuMainButton())
				{
					il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateHomeMenuMainButton, true);
				}
			}))->delegate;

		// Delay 50ms
		il2cpp_symbols::get_method_pointer<Il2CppObject* (*)(float, Il2CppDelegate*, bool)>("DOTween.dll", "DG.Tweening", "DOVirtual", "DelayedCall", 3)(0.05, updateHomeMenuMainButton, true);
	}

	return cloned;
}

static void InitAddress()
{
	FindObjectsByType_addr = il2cpp_resolve_icall("UnityEngine.Object::FindObjectsByType()");
	FindObjectsOfType_addr = il2cpp_resolve_icall("UnityEngine.Object::FindObjectsOfType()");
	Internal_CloneSingleWithParent_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingleWithParent()");
	Internal_CloneSingle_addr = il2cpp_resolve_icall("UnityEngine.Object::Internal_CloneSingle()");
	DontDestroyOnLoad_addr = il2cpp_resolve_icall("UnityEngine.Object::DontDestroyOnLoad()");
	GetName_addr = il2cpp_resolve_icall("UnityEngine.Object::GetName()");
	SetName_addr = il2cpp_resolve_icall("UnityEngine.Object::SetName()");
	IsNativeObjectAlive_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "UnityEngine", "Object", "IsNativeObjectAlive", 1);
}

static void HookMethods()
{
	ADD_HOOK(Internal_CloneSingle, "UnityEngine.Object::Internal_CloneSingle at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	Il2CppArraySize_t<Il2CppObject*>* Object::FindObjectsByType(Il2CppReflectionType* type, FindObjectsInactive findObjectsInactive, FindObjectsSortMode sortMode)
	{
		if (!FindObjectsByType_addr)
		{
			return FindObjectsOfType(type, findObjectsInactive == FindObjectsInactive::Include);
		}

		return reinterpret_cast<decltype(FindObjectsByType)*>(FindObjectsByType_addr)(type, findObjectsInactive, sortMode);
	}

	Il2CppArraySize_t<Il2CppObject*>* Object::FindObjectsOfType(Il2CppReflectionType* type, bool includeInactive)
	{
		return reinterpret_cast<decltype(FindObjectsOfType)*>(FindObjectsOfType_addr)(type, includeInactive);
	}

	Il2CppObject* Object::Internal_CloneSingleWithParent(Il2CppObject* data, Il2CppObject* parent, bool worldPositionStays)
	{
		return reinterpret_cast<decltype(Internal_CloneSingleWithParent)*>(Internal_CloneSingleWithParent_addr)(data, parent, worldPositionStays);
	}

	Il2CppObject* Object::Internal_CloneSingle(Il2CppObject* data)
	{
		return reinterpret_cast<decltype(Internal_CloneSingle)*>(Internal_CloneSingle_addr)(data);
	}

	void Object::DontDestroyOnLoad(Il2CppObject* obj)
	{
		reinterpret_cast<decltype(DontDestroyOnLoad)*>(DontDestroyOnLoad_addr)(obj);
	}

	Il2CppString* Object::Name(Il2CppObject* obj)
	{
		return reinterpret_cast<Il2CppString* (*)(Il2CppObject*)>(GetName_addr)(obj);
	}

	void Object::Name(Il2CppObject* obj, Il2CppString* name)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppString*)>(SetName_addr)(obj, name);
	}

	bool Object::IsNativeObjectAlive(Il2CppObject* obj)
	{
		return reinterpret_cast<decltype(IsNativeObjectAlive)*>(IsNativeObjectAlive_addr)(obj);
	}
}