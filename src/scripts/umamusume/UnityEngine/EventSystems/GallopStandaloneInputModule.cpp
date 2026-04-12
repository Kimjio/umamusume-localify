#include "../../umamusume.hpp"
#include "scripts/ScriptInternal.hpp"
#include "GallopStandaloneInputModule.hpp"

#include "../../Gallop/Screen.hpp"
#include "../../Gallop/UIManager.hpp"
#include "../../Gallop/LandscapeUIManager.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Screen.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Mathf.hpp"
#include "scripts/UnityEngine.UI/UnityEngine/EventSystems/PointerEventData.hpp"
#include "scripts/UnityEngine.UI/UnityEngine/EventSystems/RaycastResult.hpp"

namespace
{
	Il2CppClass* ExecuteEvents_klass = nullptr;
	Il2CppClass* LandscapeUIGraphicRaycaster_klass = nullptr;

	const MethodInfo* ExecuteEvents_GetEventHandler = nullptr;

	const MethodInfo* ExecuteEvents_ExecuteHierarchy = nullptr;

	void* ExecuteEvents_get_scrollHandler_addr = nullptr;

	void* PointerEventData_get_dragging_addr = nullptr;

	void* PointerEventData_set_position_addr = nullptr;

	void* PointerEventData_get_position_addr = nullptr;

	void* PointerEventData_get_pointerPressRaycast_addr = nullptr;

	void* PointerEventData_get_pressPosition_addr = nullptr;

	void* PointerEventData_set_pressPosition_addr = nullptr;

	void* PointerEventData_set_delta_addr = nullptr;

	void* PointerEventData_set_pointerCurrentRaycast_addr = nullptr;

	void* PointerEventData_get_scrollDelta_addr = nullptr;
	
	void* PointerEventData_set_scrollDelta_addr = nullptr;

	void* GallopPointerInputModule_MouseState_GetButtonState_addr = nullptr;

	void* GallopPointerInputModule_ButtonState_get_eventData_addr = nullptr;

	void* LandscapeUIGraphicRaycaster_get_CameraType_addr = nullptr;

	void* GallopStandaloneInputModule_AdjustSplitWindowTouchPos_addr = nullptr;
	void* GallopStandaloneInputModule_AdjustSplitWindowTouchPos_orig = nullptr;

	void* GallopStandaloneInputModule_GetMousePointerEventData_addr = nullptr;

	void* GallopStandaloneInputModule_ProcessMouseEvent_addr = nullptr;
	void* GallopStandaloneInputModule_ProcessMouseEvent_orig = nullptr;

	void* GallopStandaloneInputModule_ProcessMousePress_addr = nullptr;

	void* GallopStandaloneInputModule_ProcessMove_addr = nullptr;

	void* GallopStandaloneInputModule_ProcessDrag_addr = nullptr;

	FieldInfo* GallopStandaloneInputModule_pointerEventData = nullptr;
	FieldInfo* GallopStandaloneInputModule_m_MousePosition = nullptr;
	FieldInfo* GallopStandaloneInputModule_m_LastMousePosition = nullptr;
	FieldInfo* GallopStandaloneInputModule_CurrentRaycast = nullptr;

	constexpr float ratio_16_9 = 1.7777f;
}

static Gallop::LandscapeUIManager::CameraType GallopStandaloneInputModule_AdjustSplitWindowTouchPos_hook(UnityEngine::Vector2* touchPos)
{
	auto TouchPositionCameraType = Gallop::LandscapeUIManager::GetTouchPositionCameraType(touchPos->x);
	touchPos->x -= Gallop::LandscapeUIManager::GetTouchPositionOffsetX(TouchPositionCameraType);

	if (TouchPositionCameraType != Gallop::LandscapeUIManager::CameraType::Left)
	{
		if (TouchPositionCameraType == Gallop::LandscapeUIManager::CameraType::Right)
		{
			*touchPos = *touchPos * ratio_16_9;
		}
		else
		{
			*touchPos = *touchPos * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
		}
	}

	if (TouchPositionCameraType != Gallop::LandscapeUIManager::CameraType::Center)
	{
		*touchPos = *touchPos / Gallop::LandscapeUIManager::WindowScaleRate();
	}

	return TouchPositionCameraType;
}

static void GallopStandaloneInputModule_ProcessMouseEvent_hook(Il2CppObject* self, int id)
{
	auto GallopStandaloneInputModule = UnityEngine::EventSystems::GallopStandaloneInputModule(self);
	auto mouseState = GallopStandaloneInputModule.GetMousePointerEventData(id);

	auto buttonState = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, UnityEngine::EventSystems::PointerEventData::InputButton)>(GallopPointerInputModule_MouseState_GetButtonState_addr)(mouseState, UnityEngine::EventSystems::PointerEventData::InputButton::Left);

	auto eventData = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*)>(GallopPointerInputModule_ButtonState_get_eventData_addr)(buttonState);

	auto buttonDataField = il2cpp_class_get_field_from_name(eventData->klass, "buttonData");
	Il2CppObject* buttonData;
	il2cpp_field_get_value(eventData, buttonDataField, &buttonData);

	Il2CppObject* pointerEventData = GallopStandaloneInputModule.pointerEventData();

	bool dragging = reinterpret_cast<bool (*)(Il2CppObject*)>(PointerEventData_get_dragging_addr)(buttonData);
	if (dragging)
	{
		if (!Gallop::UIManager::IsLandscapeMode() || !Gallop::Screen::IsVertical())
		{
			UnityEngine::Vector2 pointerEventDataPosition = il2cpp_class_get_method_from_name_type<UnityEngine::Vector2(*)(Il2CppObject*)>(pointerEventData->klass, "get_position", 0)->methodPointer(pointerEventData);

			reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_position_addr)(buttonData, pointerEventDataPosition);
		}
		else
		{
			UnityEngine::Vector2 position = reinterpret_cast<UnityEngine::Vector2(*)(Il2CppObject*)>(PointerEventData_get_position_addr)(buttonData);

			if (Gallop::Screen::IsSplitWindow())
			{
				auto pointerPressRaycast = reinterpret_cast<UnityEngine::EventSystems::RaycastResult(*)(Il2CppObject*)>(PointerEventData_get_pointerPressRaycast_addr)(buttonData);

				Gallop::LandscapeUIManager::CameraType cameraType = Gallop::LandscapeUIManager::CameraType::Center;

				if (pointerPressRaycast.module->klass == LandscapeUIGraphicRaycaster_klass)
				{
					cameraType = reinterpret_cast<Gallop::LandscapeUIManager::CameraType(*)(Il2CppObject*)>(LandscapeUIGraphicRaycaster_get_CameraType_addr)(pointerPressRaycast.module);
				}

				if (cameraType != Gallop::LandscapeUIManager::CameraType::Left)
				{
					position.x -= Gallop::LandscapeUIManager::GetTouchPositionOffsetX(cameraType);

					if (cameraType == Gallop::LandscapeUIManager::CameraType::Right)
					{
						position = position * ratio_16_9;
					}
					else
					{
						position = position * Gallop::Screen::Height() / static_cast<float>(UnityEngine::Screen::height());
					}
				}

				if (cameraType != Gallop::LandscapeUIManager::CameraType::Center)
				{
					position = position / Gallop::LandscapeUIManager::WindowScaleRate();
				}
			}

			reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_position_addr)(buttonData, position);
		}
	}
	else
	{
		auto pointerEventDataPosition = reinterpret_cast<UnityEngine::Vector2(*)(Il2CppObject*)>(PointerEventData_get_position_addr)(pointerEventData);

		reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_position_addr)(buttonData, pointerEventDataPosition);

		auto pointerEventDataPressPosition = reinterpret_cast<UnityEngine::Vector2(*)(Il2CppObject*)>(PointerEventData_get_pressPosition_addr)(pointerEventData);

		reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_pressPosition_addr)(buttonData, pointerEventDataPressPosition);
	}

	UnityEngine::Vector2 m_MousePosition = GallopStandaloneInputModule.m_MousePosition();
	UnityEngine::Vector2 m_LastMousePosition = GallopStandaloneInputModule.m_LastMousePosition();

	UnityEngine::Vector2 delta = m_MousePosition - m_LastMousePosition;
	reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_delta_addr)(buttonData, delta);

	UnityEngine::EventSystems::RaycastResult CurrentRaycast = GallopStandaloneInputModule.CurrentRaycast();

	reinterpret_cast<void(*)(Il2CppObject*, UnityEngine::EventSystems::RaycastResult)>(PointerEventData_set_pointerCurrentRaycast_addr)(buttonData, CurrentRaycast);

	UnityEngine::Vector2 scrollDelta = reinterpret_cast<UnityEngine::Vector2(*)(Il2CppObject*)>(PointerEventData_get_scrollDelta_addr)(buttonData);

	scrollDelta = scrollDelta * 100.0f;

	if (Gallop::UIManager::Instance().IsLockGameCanvas())
	{
		scrollDelta = UnityEngine::Vector2{ 0, 0 };
	}

	reinterpret_cast<void (*)(Il2CppObject*, UnityEngine::Vector2)>(PointerEventData_set_scrollDelta_addr)(buttonData, scrollDelta);

	GallopStandaloneInputModule.ProcessMousePress(eventData);
	GallopStandaloneInputModule.ProcessMove(buttonData);
	GallopStandaloneInputModule.ProcessDrag(buttonData);

	if (!UnityEngine::Mathf::Approximately(scrollDelta.sqrMagnitude(), 0.0f))
	{
		auto eventHandler = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, const MethodInfo*)>(ExecuteEvents_GetEventHandler->methodPointer)(CurrentRaycast.m_GameObject, ExecuteEvents_GetEventHandler);

		auto scrollHandler = reinterpret_cast<Il2CppObject * (*)()>(ExecuteEvents_get_scrollHandler_addr)();

		reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, const MethodInfo*)>(ExecuteEvents_ExecuteHierarchy->methodPointer)(eventHandler, buttonData, scrollHandler, ExecuteEvents_ExecuteHierarchy);
	}
}

static void InitAddress()
{
	auto GallopStandaloneInputModule_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "UnityEngine.EventSystems", "GallopStandaloneInputModule");
	ExecuteEvents_klass = il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.EventSystems", "ExecuteEvents");
	auto IScrollHandler = GetRuntimeType(il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.EventSystems", "IScrollHandler"));
	ExecuteEvents_GetEventHandler = GetGenericMethod(il2cpp_class_get_method_from_name(ExecuteEvents_klass, "GetEventHandler", 1), IScrollHandler);
	ExecuteEvents_ExecuteHierarchy = GetGenericMethod(il2cpp_class_get_method_from_name(ExecuteEvents_klass, "ExecuteHierarchy", 3), IScrollHandler);
	ExecuteEvents_get_scrollHandler_addr = il2cpp_symbols::get_method_pointer(ExecuteEvents_klass, "get_scrollHandler", 0);
	auto PointerEventData_klass = il2cpp_symbols::get_class("UnityEngine.UI.dll", "UnityEngine.EventSystems", "PointerEventData");
	PointerEventData_get_dragging_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "get_dragging", 0);
	PointerEventData_set_position_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "set_position", 1);
	PointerEventData_get_position_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "get_position", 0);
	PointerEventData_get_pointerPressRaycast_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "get_pointerPressRaycast", 0);
	PointerEventData_get_pressPosition_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "get_pressPosition", 0);
	PointerEventData_set_pressPosition_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "set_pressPosition", 1);
	PointerEventData_set_delta_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "set_delta", 1);
	PointerEventData_set_pointerCurrentRaycast_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "set_pointerCurrentRaycast", 1);
	PointerEventData_get_scrollDelta_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "get_scrollDelta", 0);
	PointerEventData_set_scrollDelta_addr = il2cpp_symbols::get_method_pointer(PointerEventData_klass, "set_scrollDelta", 1);
	GallopPointerInputModule_MouseState_GetButtonState_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GallopPointerInputModule/MouseState", "GetButtonState", 1);
	GallopPointerInputModule_ButtonState_get_eventData_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "GallopPointerInputModule/ButtonState", "get_eventData", 0);
	LandscapeUIGraphicRaycaster_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "LandscapeUIGraphicRaycaster");
	if (!LandscapeUIGraphicRaycaster_klass)
	{
		LandscapeUIGraphicRaycaster_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "SteamGraphicRaycaster");
	}
	LandscapeUIGraphicRaycaster_get_CameraType_addr = il2cpp_symbols::get_method_pointer(LandscapeUIGraphicRaycaster_klass, "get_CameraType", 0);
	GallopStandaloneInputModule_pointerEventData = il2cpp_class_get_field_from_name(GallopStandaloneInputModule_klass, "pointerEventData");
	GallopStandaloneInputModule_m_MousePosition = il2cpp_class_get_field_from_name(GallopStandaloneInputModule_klass, "m_MousePosition");
	GallopStandaloneInputModule_m_LastMousePosition = il2cpp_class_get_field_from_name(GallopStandaloneInputModule_klass, "m_LastMousePosition");
	GallopStandaloneInputModule_CurrentRaycast = il2cpp_class_get_field_from_name(GallopStandaloneInputModule_klass, "CurrentRaycast");
	GallopStandaloneInputModule_AdjustSplitWindowTouchPos_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "AdjustSplitWindowTouchPos", 1);
	GallopStandaloneInputModule_GetMousePointerEventData_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "GetMousePointerEventData", 1);
	GallopStandaloneInputModule_ProcessMouseEvent_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "ProcessMouseEvent", 1);
	GallopStandaloneInputModule_ProcessMousePress_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "ProcessMousePress", 1);
	GallopStandaloneInputModule_ProcessMove_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "ProcessMove", 1);
	GallopStandaloneInputModule_ProcessDrag_addr = il2cpp_symbols::get_method_pointer(GallopStandaloneInputModule_klass, "ProcessDrag", 1);

}

static void HookMethods()
{
	if (config::unlock_size || config::freeform_window)
	{
		if (LandscapeUIGraphicRaycaster_klass)
		{
			ADD_HOOK(GallopStandaloneInputModule_AdjustSplitWindowTouchPos, "UnityEngine.EventSystems.GallopStandaloneInputModule::AdjustSplitWindowTouchPos at %p\n");
			ADD_HOOK(GallopStandaloneInputModule_ProcessMouseEvent, "UnityEngine.EventSystems.GallopStandaloneInputModule::ProcessMouseEvent at %p\n");
		}
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace UnityEngine
{
	namespace EventSystems
	{
		Il2CppObject* GallopStandaloneInputModule::GetMousePointerEventData(int id)
		{
			return reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, int)>(GallopStandaloneInputModule_GetMousePointerEventData_addr)(instance, id);
		}

		void GallopStandaloneInputModule::ProcessMousePress(Il2CppObject* data)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(GallopStandaloneInputModule_ProcessMousePress_addr)(instance, data);
		}

		void GallopStandaloneInputModule::ProcessMove(Il2CppObject* pointerEvent)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(GallopStandaloneInputModule_ProcessMove_addr)(instance, pointerEvent);
		}

		void GallopStandaloneInputModule::ProcessDrag(Il2CppObject* pointerEvent)
		{
			reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(GallopStandaloneInputModule_ProcessDrag_addr)(instance, pointerEvent);
		}

		Il2CppObject* GallopStandaloneInputModule::pointerEventData()
		{
			Il2CppObject* value;
			il2cpp_field_get_value(instance, GallopStandaloneInputModule_pointerEventData, &value);
			return value;
		}

		Vector2 GallopStandaloneInputModule::m_MousePosition()
		{
			Vector2 value;
			il2cpp_field_get_value(instance, GallopStandaloneInputModule_m_MousePosition, &value);
			return value;
		}

		Vector2 GallopStandaloneInputModule::m_LastMousePosition()
		{
			Vector2 value;
			il2cpp_field_get_value(instance, GallopStandaloneInputModule_m_LastMousePosition, &value);
			return value;
		}

		RaycastResult GallopStandaloneInputModule::CurrentRaycast()
		{
			RaycastResult value;
			il2cpp_field_get_value(instance, GallopStandaloneInputModule_CurrentRaycast, &value);
			return value;
		}
	}
}
