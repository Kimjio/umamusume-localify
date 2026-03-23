#include "SettingsUI.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <Shlobj.h>
#include <ShObjIdl.h>
#include <wrl.h>
#include <wil/com.h>

#include "UIParts.hpp"
#include "NotificationManager.hpp"
#include "config/config.hpp"
#include "scripts/ScriptInternal.hpp"
#include "string_utils.hpp"
#include "settings_text.hpp"

#include "notification/DesktopNotificationManagerCompat.h"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Application.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/ResourcesAPIInternal.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/RectTransform.hpp"
#include "scripts/umamusume/Gallop/DialogCommon.hpp"
#include "scripts/umamusume/Gallop/DialogManager.hpp"
#include "scripts/umamusume/Gallop/UIManager.hpp"
#include "scripts/umamusume/Gallop/Localize.hpp"

#include "openxr/openxr.hpp"

#include "msgpack/msgpack_data.hpp"

using namespace std;
using namespace Microsoft::WRL;

namespace Localify
{
	static Gallop::DialogCommon settingsDialog = nullptr;
	static Gallop::DialogCommon selectOptionDialog = nullptr;
	static function<void(int)> optionSelected;

	static vector<string> GetGraphicsQualityOptions()
	{
		vector<string> options = { "Default" };

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings/GraphicsQuality");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				if (name != "Max"s)
				{
					options.emplace_back(name);
				}
			}
		}

		return options;
	}

	static vector<string> GetFontColorOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "FontColorType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	static vector<string> GetOutlineSizeOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "OutlineSizeType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	static vector<string> GetOutlineColorOptions()
	{
		vector<string> options;

		const auto klass = il2cpp_symbols::get_class("umamusume.dll", "Gallop", "OutlineColorType");

		void* iter = nullptr;
		while (auto field = il2cpp_class_get_fields(klass, &iter))
		{
			auto attrs = il2cpp_field_get_flags(field);

			if (attrs & FIELD_ATTRIBUTE_LITERAL && il2cpp_class_is_enum(klass))
			{
				auto name = il2cpp_field_get_name(field);

				options.emplace_back(name);
			}
		}

		return options;
	}

	void SettingsUI::AddOrSetString(U16Document& document, const Il2CppChar* name, il2cppstring value)
	{
		auto length = value.size();
		Il2CppChar* copy = new Il2CppChar[length + 1];
		char_traits<Il2CppChar>::copy(copy, value.data(), length);

		if (document.HasMember(name))
		{
			document[name].SetString(rapidjson::StringRef(copy, length));
		}
		else
		{
			U16Value v;
			v.SetString(rapidjson::StringRef(copy, length));
			document.AddMember(rapidjson::StringRef(name), v, document.GetAllocator());
		}
	}

	// FolderOpen helpers
	class CDialogEventHandler : public IFileDialogEvents
	{
	public:
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			static const QITAB qit[] = { QITABENT(CDialogEventHandler, IFileDialogEvents), { 0 }, };
			return QISearch(this, qit, riid, ppv);
		}
		IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&_cRef); }
		IFACEMETHODIMP_(ULONG) Release() { long cRef = InterlockedDecrement(&_cRef); if (!cRef) delete this; return cRef; }
		IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; };
		IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; };
		IFACEMETHODIMP OnTypeChange(IFileDialog* pfd) { return S_OK; };
		IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; };
		IFACEMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };
		IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD) { return S_OK; };
		IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL) { return S_OK; };
		IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD) { return S_OK; };
		CDialogEventHandler() : _cRef(1) {};
	private:
		~CDialogEventHandler() {};
		long _cRef;
	};

	HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
	{
		*ppv = NULL;
		CDialogEventHandler* pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
		HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			hr = pDialogEventHandler->QueryInterface(riid, ppv);
			pDialogEventHandler->Release();
		}
		return hr;
	}

	PWSTR SettingsUI::FolderOpen()
	{
		PWSTR pszFilePath = NULL;
		IFileDialog* pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			wil::com_ptr<IFileDialogEvents> pfde;
			hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
			if (SUCCEEDED(hr))
			{
				DWORD dwCookie;
				hr = pfd->Advise(pfde.get(), &dwCookie);
				if (SUCCEEDED(hr))
				{
					DWORD dwFlags;
					hr = pfd->GetOptions(&dwFlags);
					if (SUCCEEDED(hr))
					{
						hr = pfd->SetOptions(dwFlags | FOS_PICKFOLDERS | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM);
						if (SUCCEEDED(hr))
						{
							hr = pfd->Show(NULL);
							if (SUCCEEDED(hr))
							{
								IShellItem* psiResult;
								hr = pfd->GetResult(&psiResult);
								if (SUCCEEDED(hr))
								{
									hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									psiResult->Release();
								}
							}
						}
					}
					pfd->Unadvise(dwCookie);
				}
				pfde->Release();
			}
			pfd->Release();
		}
		return pszFilePath;
	}

	void SettingsUI::OpenSelectOption(const Il2CppChar* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected, function<void(string, il2cppstring)> optionStyle)
	{
		Localify::optionSelected = optionSelected;
		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog) { selectOptionDialog.Close(); });
		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog) {
			selectOptionDialog.Close();
			Localify::optionSelected(UIParts::GetToggleGroupCommonValue("option_toggle_group_content"));
			});

		auto dialogData = Gallop::DialogCommon::Data();
		dialogData.SetSimpleTwoButtonMessage(il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(IL2CPP_STRING("Common0004")), GetTextIdByName(IL2CPP_STRING("Common0003")), onLeft, Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON);
		dialogData.DispStackType(Gallop::DialogCommon::DispStackType::DialogOnDialog);
		dialogData.ObjParentType(Gallop::DialogCommon::Data::ObjectParentType::Base);
		dialogData.AutoClose(false);

		auto gameObject = UnityEngine::GameObject();
		UnityEngine::RectTransform rootTransform = gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));
		rootTransform.sizeDelta({ 0, 0 });
		rootTransform.anchorMax({ 1, 1 });
		rootTransform.anchorMin({ 0, 0 });
		rootTransform.pivot({ 0.5, 0.5 });
		rootTransform.anchoredPosition({ 0, 0 });

		UnityEngine::GameObject scrollViewBase = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		Il2CppObject* _mainCanvas = Gallop::UIManager::Instance()._mainCanvas();
		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);
		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto scrollRect = scrollViewBase.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ScrollRectCommon"), false)->vector[0];
		auto m_ViewportField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);
		UnityEngine::RectTransform scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);
		scrollRectTransform.sizeDelta({ -24, -12 });
		scrollRectTransform.anchorMax({ 1, 1 });
		scrollRectTransform.anchorMin({ 0, 0 });
		scrollRectTransform.pivot({ 0.5, 0.5 });
		scrollRectTransform.anchoredPosition({ 0, -6 });
		scrollRectTransform.SetParent(rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		UnityEngine::GameObject contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);
		auto gridLayoutGroup = contentGameObject.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = contentGameObject.AddComponent(GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));
		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;
		for (auto& option : options)
		{
			string name = "radio_"s + option;
			il2cppstring value = u8_il2cpp(option);
			toggles.emplace_back(UIParts::GetRadioButtonWithText(name.data(), value.data()));
			optionStyle(name, value);
		}
		UIParts::AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());
		for (int i = 0; i < options.size(); i++)
		{
			auto& option = options[i];
			il2cpp_array_setref(toggleArray, i, UIParts::GetToggleCommon(("radio_"s + option).data()));
		}
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		dialogData.ContentsObject(gameObject);
		selectOptionDialog = Gallop::DialogManager::Instance().PushDialog(dialogData);
	}

	void SettingsUI::OpenSelectGraphicsQualityOption(const Il2CppChar* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected)
	{
		Localify::optionSelected = optionSelected;

		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog) { selectOptionDialog.Close(); });
		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog) {
			selectOptionDialog.Close();
			Localify::optionSelected(UIParts::GetToggleGroupCommonValue("option_toggle_group_content"));
			});

		auto dialogData = Gallop::DialogCommon::Data();
		dialogData.SetSimpleTwoButtonMessage(il2cpp_string_new16(title), nullptr, onRight, GetTextIdByName(IL2CPP_STRING("Common0004")), GetTextIdByName(IL2CPP_STRING("Common0003")), onLeft, Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON);
		dialogData.DispStackType(Gallop::DialogCommon::DispStackType::DialogOnDialog);
		dialogData.ObjParentType(Gallop::DialogCommon::Data::ObjectParentType::Base);
		dialogData.AutoClose(false);

		auto gameObject = UnityEngine::GameObject();
		UnityEngine::RectTransform rootTransform = gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));
		rootTransform.sizeDelta({ 0, 0 });
		rootTransform.anchorMax({ 1, 1 });
		rootTransform.anchorMin({ 0, 0 });
		rootTransform.pivot({ 0.5, 0.5 });
		rootTransform.anchoredPosition({ 0, 0 });

		UnityEngine::GameObject scrollViewBase = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));
		Il2CppObject* _mainCanvas = Gallop::UIManager::Instance()._mainCanvas();
		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);
		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto scrollRect = scrollViewBase.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ScrollRectCommon"), false)->vector[0];
		auto m_ViewportField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);
		UnityEngine::RectTransform scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);
		scrollRectTransform.sizeDelta({ -24, -12 });
		scrollRectTransform.anchorMax({ 1, 1 });
		scrollRectTransform.anchorMin({ 0, 0 });
		scrollRectTransform.pivot({ 0.5, 0.5 });
		scrollRectTransform.anchoredPosition({ 0, -6 });
		scrollRectTransform.SetParent(rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 150.0f * ceilf(options.size() / 2.0f) });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		UnityEngine::GameObject contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);
		auto gridLayoutGroup = contentGameObject.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "GridLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(gridLayoutGroup, 0);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(gridLayoutGroup->klass, "set_constraintCount", 1)->methodPointer(gridLayoutGroup, 2);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_cellSize", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 400, 100 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(gridLayoutGroup->klass, "set_spacing", 1)->methodPointer(gridLayoutGroup, UnityEngine::Vector2{ 34, 50 });

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(gridLayoutGroup->klass, "get_padding", 0)->methodPointer(gridLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, 26);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_left", 1)->methodPointer(padding, 48);

		auto toggleGroupCommon = contentGameObject.AddComponent(GetRuntimeType("umamusume.dll", "Gallop", "ToggleGroupCommon"));
		UnityEngine::Object::Name(contentGameObject, il2cpp_string_new("option_toggle_group_content"));

		vector<Il2CppObject*> toggles;
		for (auto& pair : options)
		{
			toggles.emplace_back(UIParts::GetOptionItemOnOffQualityRich(("radio_"s + pair).data(), u8_il2cpp(pair).data()));
		}
		UIParts::AddToLayout(m_Content, toggles);

		auto toggleArray = il2cpp_array_new_type<Il2CppObject*>(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "ToggleCommon"), toggles.size());
		for (int i = 0; i < options.size(); i++)
		{
			auto& pair = options[i];
			il2cpp_array_setref(toggleArray, i, UIParts::GetToggleCommon(("radio_"s + pair).data()));
		}
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppArraySize_t<Il2CppObject*>*)>(toggleGroupCommon->klass, "set_ToggleArray", 1)->methodPointer(toggleGroupCommon, toggleArray);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(toggleGroupCommon->klass, "SetToggleOnFromNumber", 1)->methodPointer(toggleGroupCommon, selectedIndex);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(toggleGroupCommon->klass, "Awake", 0)->methodPointer(toggleGroupCommon);

		dialogData.ContentsObject(gameObject);
		selectOptionDialog = Gallop::DialogManager::Instance().PushDialog(dialogData);
	}

	void SettingsUI::OpenSettings()
	{
		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				settingsDialog.Close();
				settingsDialog = nullptr;

				NotificationManager::SetBackgroundAlpha(config::character_system_text_caption_background_alpha);
				NotificationManager::SetFontSize(config::character_system_text_caption_font_size);
				NotificationManager::SetPosition(config::character_system_text_caption_position_x, config::character_system_text_caption_position_y);
				NotificationManager::SetFontColor(config::character_system_text_caption_font_color);
				NotificationManager::SetOutlineSize(config::character_system_text_caption_outline_size);
				NotificationManager::SetOutlineColor(config::character_system_text_caption_outline_color);

				config::rollback_config();
			}
		);

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				auto& configDocument = config::config_document;

				AddOrSet(configDocument, IL2CPP_STRING("antiAliasing"), vector<int>{ -1, 0, 2, 4, 8 }[UIParts::GetOptionSliderValue("anti_aliasing")]);

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaption"), UIParts::GetOptionItemOnOffIsOn("character_system_text_caption"));

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaptionLineCharCount"), static_cast<int>(UIParts::GetOptionSliderValue("character_system_text_caption_line_char_count")));

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaptionFontSize"), static_cast<int>(UIParts::GetOptionSliderValue("character_system_text_caption_font_size")));

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaptionPositionX"), UIParts::GetOptionSliderValue("character_system_text_caption_position_x") / 10);

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaptionPositionY"), UIParts::GetOptionSliderValue("character_system_text_caption_position_y") / 10);

				AddOrSet(configDocument, IL2CPP_STRING("characterSystemTextCaptionBackgroundAlpha"), UIParts::GetOptionSliderValue("character_system_text_caption_background_alpha") / 100);

				AddOrSet(configDocument, IL2CPP_STRING("liveSliderAlwaysShow"), UIParts::GetOptionItemOnOffIsOn("live_slider_always_show"));

				AddOrSet(configDocument, IL2CPP_STRING("livePlaybackLoop"), UIParts::GetOptionItemOnOffIsOn("live_playback_loop"));

				AddOrSet(configDocument, IL2CPP_STRING("championsLiveShowText"), UIParts::GetOptionItemOnOffIsOn("champions_live_show_text"));

				AddOrSet(configDocument, IL2CPP_STRING("allowDeleteCookie"), UIParts::GetOptionItemOnOffIsOn("allow_delete_cookie"));

				AddOrSet(configDocument, IL2CPP_STRING("cySpringUpdateMode"), static_cast<int>(UIParts::GetOptionSliderValue("cyspring_update_mode")));

				AddOrSet(configDocument, IL2CPP_STRING("uiAnimationScale"), static_cast<int>(round(UIParts::GetOptionSliderValue("ui_animation_scale") * 100)) / 100.0);

				AddOrSet(configDocument, IL2CPP_STRING("resolution3dScale"), static_cast<int>(round(UIParts::GetOptionSliderValue("resolution_3d_scale") * 100)) / 100.0);

				AddOrSet(configDocument, IL2CPP_STRING("notificationTp"), UIParts::GetOptionItemOnOffIsOn("notification_tp"));

				AddOrSet(configDocument, IL2CPP_STRING("notificationRp"), UIParts::GetOptionItemOnOffIsOn("notification_rp"));

				AddOrSet(configDocument, IL2CPP_STRING("notificationJobs"), UIParts::GetOptionItemOnOffIsOn("notification_jobs"));

				AddOrSet(configDocument, IL2CPP_STRING("dumpMsgPack"), UIParts::GetOptionItemOnOffIsOn("dump_msgpack"));

				AddOrSet(configDocument, IL2CPP_STRING("dumpMsgPackRequest"), UIParts::GetOptionItemOnOffIsOn("dump_msgpack_request"));

				AddOrSet(configDocument, IL2CPP_STRING("useThirdPartyNews"), UIParts::GetOptionItemOnOffIsOn("use_third_party_news"));

#ifdef EXPERIMENTS
				AddOrSet(configDocument, IL2CPP_STRING("unlockLiveChara"), UIParts::GetOptionItemOnOffIsOn("unlock_live_chara"));
#endif
				AddOrSet(configDocument, IL2CPP_STRING("unlockSize"), UIParts::GetOptionItemOnOffIsOn("unlock_size"));

				AddOrSet(configDocument, IL2CPP_STRING("unlockSizeUseSystemResolution"), UIParts::GetOptionItemOnOffIsOn("use_system_resolution"));

				AddOrSet(configDocument, IL2CPP_STRING("uiScale"), static_cast<int>(round(UIParts::GetOptionSliderValue("ui_scale") * 100)) / 100.0);

				AddOrSet(configDocument, IL2CPP_STRING("autoFullscreen"), UIParts::GetOptionItemOnOffIsOn("auto_fullscreen"));

				AddOrSet(configDocument, IL2CPP_STRING("freeFormWindow"), UIParts::GetOptionItemOnOffIsOn("freeform_window"));

				AddOrSet(configDocument, IL2CPP_STRING("freeFormUiScalePortrait"), static_cast<int>(round(UIParts::GetOptionSliderValue("ui_scale_portrait") * 100)) / 100.0);

				AddOrSet(configDocument, IL2CPP_STRING("freeFormUiScaleLandscape"), static_cast<int>(round(UIParts::GetOptionSliderValue("ui_scale_landscape") * 100)) / 100.0);

				AddOrSet(configDocument, IL2CPP_STRING("taskbarShowProgressOnDownload"), UIParts::GetOptionItemOnOffIsOn("taskbar_show_progress_on_download"));

				AddOrSet(configDocument, IL2CPP_STRING("taskbarShowProgressOnConnecting"), UIParts::GetOptionItemOnOffIsOn("taskbar_show_progress_on_connecting"));

				config::graphics_quality = configDocument[IL2CPP_STRING("graphicsQuality")].GetInt();

				config::anti_aliasing = configDocument[IL2CPP_STRING("antiAliasing")].GetInt();

				config::character_system_text_caption_line_char_count = configDocument[IL2CPP_STRING("characterSystemTextCaptionLineCharCount")].GetInt();

				config::character_system_text_caption_font_size = configDocument[IL2CPP_STRING("characterSystemTextCaptionFontSize")].GetInt();

				config::character_system_text_caption_background_alpha = configDocument[IL2CPP_STRING("characterSystemTextCaptionBackgroundAlpha")].GetFloat();

				config::character_system_text_caption_position_x = configDocument[IL2CPP_STRING("characterSystemTextCaptionPositionX")].GetFloat();

				config::character_system_text_caption_position_y = configDocument[IL2CPP_STRING("characterSystemTextCaptionPositionY")].GetFloat();

				config::character_system_text_caption_font_color = configDocument[IL2CPP_STRING("characterSystemTextCaptionFontColor")].GetString();

				config::character_system_text_caption_outline_size = configDocument[IL2CPP_STRING("characterSystemTextCaptionOutlineSize")].GetString();

				config::character_system_text_caption_outline_color = configDocument[IL2CPP_STRING("characterSystemTextCaptionOutlineColor")].GetString();

				config::live_slider_always_show = configDocument[IL2CPP_STRING("liveSliderAlwaysShow")].GetBool();

				config::live_playback_loop = configDocument[IL2CPP_STRING("livePlaybackLoop")].GetBool();

				config::champions_live_show_text = configDocument[IL2CPP_STRING("championsLiveShowText")].GetBool();

				config::champions_live_year = configDocument[IL2CPP_STRING("championsLiveYear")].GetInt();

				config::champions_live_resource_id = configDocument[IL2CPP_STRING("championsLiveResourceId")].GetInt();

				config::cyspring_update_mode = configDocument[IL2CPP_STRING("cySpringUpdateMode")].GetInt();

				config::ui_animation_scale = configDocument[IL2CPP_STRING("uiAnimationScale")].GetFloat();

				config::resolution_3d_scale = configDocument[IL2CPP_STRING("resolution3dScale")].GetFloat();

				auto graphicSettings = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "GraphicSettings"));

				if (graphicSettings)
				{
					auto _resolutionScaleField = il2cpp_class_get_field_from_name(graphicSettings->klass, "_resolutionScale");

					il2cpp_field_set_value(graphicSettings, _resolutionScaleField, &config::resolution_3d_scale);

					auto _resolutionScale2DField = il2cpp_class_get_field_from_name(graphicSettings->klass, "_resolutionScale2D");

					il2cpp_field_set_value(graphicSettings, _resolutionScale2DField, &config::resolution_3d_scale);
				}

				auto nowLoading = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "NowLoading", "get_Instance", IgnoreNumberOfArguments)();
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(nowLoading->klass, "DeleteMiniCharacter", 0)->methodPointer(nowLoading);
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(nowLoading->klass, "CreateMiniCharacter", 0)->methodPointer(nowLoading);

				config::unlock_size_use_system_resolution = configDocument[IL2CPP_STRING("unlockSizeUseSystemResolution")].GetBool();

				config::ui_scale = configDocument[IL2CPP_STRING("uiScale")].GetFloat();

				config::auto_fullscreen = configDocument[IL2CPP_STRING("autoFullscreen")].GetBool();

				config::freeform_ui_scale_portrait = configDocument[IL2CPP_STRING("freeFormUiScalePortrait")].GetFloat();

				config::freeform_ui_scale_landscape = configDocument[IL2CPP_STRING("freeFormUiScaleLandscape")].GetFloat();

				config::notification_tp = configDocument[IL2CPP_STRING("notificationTp")].GetBool();

				if (config::notification_tp)
				{
					MsgPackData::RegisterTPScheduledToast();
				}
				else
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"TP");
				}

				config::notification_rp = configDocument[IL2CPP_STRING("notificationRp")].GetBool();

				if (config::notification_rp)
				{
					MsgPackData::RegisterRPScheduledToast();
				}
				else
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByTag(L"RP");
				}

				config::notification_jobs = configDocument[IL2CPP_STRING("notificationJobs")].GetBool();

				if (config::notification_jobs)
				{
					MsgPackData::RegisterJobsScheduledToast();
				}
				else
				{
					DesktopNotificationManagerCompat::RemoveFromScheduleByGroup(L"Jobs");
				}

				config::taskbar_show_progress_on_download = configDocument[IL2CPP_STRING("taskbarShowProgressOnDownload")].GetBool();

				config::taskbar_show_progress_on_connecting = configDocument[IL2CPP_STRING("taskbarShowProgressOnConnecting")].GetBool();

				config::dump_msgpack = configDocument[IL2CPP_STRING("dumpMsgPack")].GetBool();

				config::dump_msgpack_request = configDocument[IL2CPP_STRING("dumpMsgPackRequest")].GetBool();

				config::use_third_party_news = configDocument[IL2CPP_STRING("useThirdPartyNews")].GetBool();

#ifdef EXPERIMENTS
				config::unlock_live_chara = configDocument[IL2CPP_STRING("unlockLiveChara")].GetBool();
#endif
				config::write_config();

				auto dialogData = Gallop::DialogCommon::Data();
				dialogData.SetSimpleOneButtonMessage(GetTextIdByName(IL2CPP_STRING("AccoutDataLink0061")), Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0309"))), nullptr, GetTextIdByName(IL2CPP_STRING("Common0007")));

				auto onDestroy = CreateDelegateStatic(*[]()
					{
						settingsDialog.Close();
						settingsDialog = nullptr;
					}
				);

				dialogData.AddDestroyCallback(onDestroy);
				Gallop::DialogManager::Instance().PushDialog(dialogData);
			}
		);

		auto dialogData = Gallop::DialogCommon::Data();

		dialogData.SetSimpleTwoButtonMessage(il2cpp_string_new16(LocalifySettings::GetText("settings_title")), nullptr, onRight, GetTextIdByName(IL2CPP_STRING("Common0004")), GetTextIdByName(IL2CPP_STRING("Common0261")), onLeft, Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON);
		dialogData.DispStackType(Gallop::DialogCommon::DispStackType::DialogOnDialog);
		dialogData.ObjParentType(Gallop::DialogCommon::Data::ObjectParentType::Base);
		dialogData.AutoClose(false);

		auto gameObject = UnityEngine::GameObject();
		UnityEngine::RectTransform rootTransform = gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		rootTransform.sizeDelta({ 0, 0 });
		rootTransform.anchorMax({ 1, 1 });
		rootTransform.anchorMin({ 0, 0 });
		rootTransform.pivot({ 0.5, 0.5 });
		rootTransform.anchoredPosition({ 0, 0 });

		UnityEngine::GameObject scrollViewBase = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto scrollRectArray = scrollViewBase.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ScrollRectCommon"), false);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		UnityEngine::RectTransform scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);

		scrollRectTransform.sizeDelta({ -24, -12 });
		scrollRectTransform.anchorMax({ 1, 1 });
		scrollRectTransform.anchorMin({ 0, 0 });
		scrollRectTransform.pivot({ 0.5, 0.5 });
		scrollRectTransform.anchoredPosition({ 0, -6 });
		scrollRectTransform.SetParent(rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 0 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		UnityEngine::GameObject contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto verticalLayoutGroup = contentGameObject.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, -20);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 16);

		int antiAliasing = 0;
		bool characterSystemTextCaption = false;
		bool liveSliderAlwaysShow = false;
		bool livePlaybackLoop = false;
		bool championsLiveShowText = false;
		int championsLiveYear = 2022;
		int characterSystemTextCaptionLineCharCount = 0;
		int characterSystemTextCaptionFontSize = 0;
		float characterSystemTextCaptionPositionX = 0;
		float characterSystemTextCaptionPositionY = 0;
		float characterSystemTextCaptionBackgroundAlpha = 0;
		bool allowDeleteCookie = false;
		int cySpringUpdateMode = -1;
		float resolution3dScale = 1;
		float uiAnimationScale = 1;
		bool notificationTp = false;
		bool notificationRp = false;
		bool notificationJobs = false;
		bool dumpMsgPack = false;
		bool dumpMsgPackRequest = false;
		bool useThirdPartyNews = false;
		bool unlockLiveChara = false;
		bool unlockSize = false;
		bool unlockSizeUseSystemResolution = false;
		float uiScale = 0;
		bool autoFullscreen = false;
		bool freeFormWindow = false;
		float freeFormUiScalePortrait = 0;
		float freeFormUiScaleLandscape = 0;
		bool taskbarShowProgressOnDownload = true;
		bool taskbarShowProgressOnConnecting = true;

		if (config::read_config())
		{
			auto& configDocument = config::config_document;

			if (configDocument.HasMember(IL2CPP_STRING("antiAliasing")))
			{
				vector<int> options = { -1, 0, 2, 4, 8 };
				antiAliasing = find(options.begin(), options.end(), configDocument[IL2CPP_STRING("antiAliasing")].GetInt()) - options.begin();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaption")))
			{
				characterSystemTextCaption = configDocument[IL2CPP_STRING("characterSystemTextCaption")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("championsLiveShowText")))
			{
				championsLiveShowText = configDocument[IL2CPP_STRING("championsLiveShowText")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("liveSliderAlwaysShow")))
			{
				liveSliderAlwaysShow = configDocument[IL2CPP_STRING("liveSliderAlwaysShow")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("livePlaybackLoop")))
			{
				livePlaybackLoop = configDocument[IL2CPP_STRING("livePlaybackLoop")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("championsLiveYear")))
			{
				championsLiveYear = configDocument[IL2CPP_STRING("championsLiveYear")].GetInt();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaptionLineCharCount")))
			{
				characterSystemTextCaptionLineCharCount = configDocument[IL2CPP_STRING("characterSystemTextCaptionLineCharCount")].GetInt();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaptionFontSize")))
			{
				characterSystemTextCaptionFontSize = configDocument[IL2CPP_STRING("characterSystemTextCaptionFontSize")].GetInt();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaptionPositionX")))
			{
				characterSystemTextCaptionPositionX = configDocument[IL2CPP_STRING("characterSystemTextCaptionPositionX")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaptionPositionY")))
			{
				characterSystemTextCaptionPositionY = configDocument[IL2CPP_STRING("characterSystemTextCaptionPositionY")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("characterSystemTextCaptionBackgroundAlpha")))
			{
				characterSystemTextCaptionBackgroundAlpha = configDocument[IL2CPP_STRING("characterSystemTextCaptionBackgroundAlpha")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("allowDeleteCookie")))
			{
				allowDeleteCookie = configDocument[IL2CPP_STRING("allowDeleteCookie")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("cySpringUpdateMode")))
			{
				cySpringUpdateMode = configDocument[IL2CPP_STRING("cySpringUpdateMode")].GetInt();
			}

			if (configDocument.HasMember(IL2CPP_STRING("resolution3dScale")))
			{
				resolution3dScale = configDocument[IL2CPP_STRING("resolution3dScale")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("uiAnimationScale")))
			{
				uiAnimationScale = configDocument[IL2CPP_STRING("uiAnimationScale")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("notificationTp")))
			{
				notificationTp = configDocument[IL2CPP_STRING("notificationTp")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("notificationRp")))
			{
				notificationRp = configDocument[IL2CPP_STRING("notificationRp")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("notificationJobs")))
			{
				notificationJobs = configDocument[IL2CPP_STRING("notificationJobs")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("dumpMsgPack")))
			{
				dumpMsgPack = configDocument[IL2CPP_STRING("dumpMsgPack")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("dumpMsgPackRequest")))
			{
				dumpMsgPackRequest = configDocument[IL2CPP_STRING("dumpMsgPackRequest")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("useThirdPartyNews")))
			{
				useThirdPartyNews = configDocument[IL2CPP_STRING("useThirdPartyNews")].GetBool();
			}

#ifdef EXPERIMENTS
			if (configDocument.HasMember(IL2CPP_STRING("unlockLiveChara")))
			{
				unlockLiveChara = configDocument[IL2CPP_STRING("unlockLiveChara")].GetBool();
			}
#endif
			if (configDocument.HasMember(IL2CPP_STRING("unlockSize")))
			{
				unlockSize = configDocument[IL2CPP_STRING("unlockSize")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("unlockSizeUseSystemResolution")))
			{
				unlockSizeUseSystemResolution = configDocument[IL2CPP_STRING("unlockSizeUseSystemResolution")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("uiScale")))
			{
				uiScale = configDocument[IL2CPP_STRING("uiScale")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("autoFullscreen")))
			{
				autoFullscreen = configDocument[IL2CPP_STRING("autoFullscreen")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("freeFormWindow")))
			{
				freeFormWindow = configDocument[IL2CPP_STRING("freeFormWindow")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("freeFormUiScalePortrait")))
			{
				freeFormUiScalePortrait = configDocument[IL2CPP_STRING("freeFormUiScalePortrait")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("freeFormUiScaleLandscape")))
			{
				freeFormUiScaleLandscape = configDocument[IL2CPP_STRING("freeFormUiScaleLandscape")].GetFloat();
			}

			if (configDocument.HasMember(IL2CPP_STRING("taskbarShowProgressOnDownload")))
			{
				taskbarShowProgressOnDownload = configDocument[IL2CPP_STRING("taskbarShowProgressOnDownload")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("taskbarShowProgressOnConnecting")))
			{
				taskbarShowProgressOnConnecting = configDocument[IL2CPP_STRING("taskbarShowProgressOnConnecting")].GetBool();
			}
		}

		vector<string> graphicsQualityOptions = GetGraphicsQualityOptions();

		bool isJobsExist = false;

		try
		{
			GetTextIdByName(IL2CPP_STRING("Jobs600005"));
			isJobsExist = true;
		}
		catch (const Il2CppExceptionWrapper& ex)
		{
			wcout << ex.ex->message->chars << endl;
		}

		auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
		auto persistentDataPathField = il2cpp_class_get_field_from_name(CuteCoreDevice, "persistentDataPath");
		Il2CppString* persistentDataPath;
		il2cpp_field_static_get_value(persistentDataPathField, &persistentDataPath);

		string championsResourceText;
		int championsLiveResourceIndex = config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1;
		auto championsResources = MasterDB::GetChampionsResources();

		if (championsResources.size() <= championsLiveResourceIndex)
		{
			championsResourceText = il2cpp_u8(LocalifySettings::GetText("unknown"));
		}
		else
		{
			championsResourceText = championsResources[championsLiveResourceIndex];
		}

		UIParts::AddToLayout(m_Content,
			{
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("graphics")),
				UIParts::GetOptionItemSimpleWithButton("graphics_quality", (LocalifySettings::GetText("graphics_quality") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(graphicsQualityOptions[config::config_document[IL2CPP_STRING("graphicsQuality")].GetInt() + 1])).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionSlider("anti_aliasing", LocalifySettings::GetText("anti_aliasing"), antiAliasing, 0, 4, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = UIParts::GetOptionSliderValue(slider);

						switch (static_cast<int>(value))
						{
						case 0:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("Default"));
							break;
						case 1:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("OFF"));
							break;
						case 2:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("x2"));
							break;
						case 3:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("x4"));
							break;
						case 4:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("x8"));
							break;
						}
					}
				),
				UIParts::GetOptionSlider("ui_animation_scale", LocalifySettings::GetText("ui_animation_scale"), uiAnimationScale, 0.1, 10.0, false),
				UIParts::GetOptionSlider("resolution_3d_scale", LocalifySettings::GetText("resolution_3d_scale"), resolution3dScale, 0.1, 2.0, false),
				UIParts::GetOptionSlider("cyspring_update_mode", LocalifySettings::GetText("cyspring_update_mode"), cySpringUpdateMode, -1, 3, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = UIParts::GetOptionSliderValue(slider);

						switch (static_cast<int>(value))
						{
						case -1:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("Default"));
							break;
						case 0:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("ModeNormal"));
							break;
						case 1:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("Mode60FPS"));
							break;
						case 2:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("SkipFrame"));
							break;
						case 3:
							UIParts::SetTextCommonText(numText, IL2CPP_STRING("SkipFramePostAlways"));
							break;
						}
					}
				),
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("screen")),
				UIParts::GetOptionItemOnOff("unlock_size", LocalifySettings::GetText("unlock_size")),
				UIParts::GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				UIParts::GetOptionItemOnOff("use_system_resolution", LocalifySettings::GetText("use_system_resolution")),
				UIParts::GetOptionSlider("ui_scale", LocalifySettings::GetText("ui_scale"), uiScale, 0.1, 2.0, false),
				UIParts::GetOptionItemOnOff("auto_fullscreen", LocalifySettings::GetText("auto_fullscreen")),
				UIParts::GetOptionItemOnOff("freeform_window", LocalifySettings::GetText("freeform_window")),
				UIParts::GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				UIParts::GetOptionSlider("ui_scale_portrait", LocalifySettings::GetText("ui_scale_portrait"), freeFormUiScalePortrait, 0.1, 2.0, false),
				UIParts::GetOptionSlider("ui_scale_landscape", LocalifySettings::GetText("ui_scale_landscape"), freeFormUiScaleLandscape, 0.1, 2.0, false),
				UIParts::GetOptionItemTitle(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Common0035")))->chars),
				UIParts::GetOptionItemOnOff("live_slider_always_show", LocalifySettings::GetText("live_slider_always_show")),
				UIParts::GetOptionItemOnOff("live_playback_loop", LocalifySettings::GetText("live_playback_loop")),
				UIParts::GetOptionItemOnOff("champions_live_show_text", LocalifySettings::GetText("champions_live_show_text")),
				UIParts::GetOptionItemSimpleWithButton("champions_live_resource_id", (LocalifySettings::GetText("champions_live_resource_id") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(championsResourceText)).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemSimpleWithButton("champions_live_year", (LocalifySettings::GetText("champions_live_year") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(to_string(championsLiveYear))).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemSimple(IL2CPP_STRING("")),
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("character_system_text_caption")),
				UIParts::GetOptionItemOnOff("character_system_text_caption", LocalifySettings::GetText("character_system_text_caption")),
				UIParts::GetOptionSlider("character_system_text_caption_line_char_count", LocalifySettings::GetText("character_system_text_caption_line_char_count"), characterSystemTextCaptionLineCharCount, 0, 100, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = static_cast<int>(UIParts::GetOptionSliderValue(slider));

						UIParts::SetTextCommonText(numText, u8_il2cpp(to_string(value)).data());
					}
				),
				UIParts::GetOptionSlider("character_system_text_caption_font_size", LocalifySettings::GetText("character_system_text_caption_font_size"), characterSystemTextCaptionFontSize, 0, 128, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = static_cast<int>(UIParts::GetOptionSliderValue(slider));

						UIParts::SetTextCommonText(numText, u8_il2cpp(to_string(value)).data());

						NotificationManager::SetFontSize(value);
						NotificationManager::SetDisplayTime(1);
						NotificationManager::Show(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
					}
				),
				UIParts::GetOptionSlider("character_system_text_caption_position_x", LocalifySettings::GetText("character_system_text_caption_position_x"), characterSystemTextCaptionPositionX * 10, -100, 100, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = UIParts::GetOptionSliderValue(slider);
						value = value / 10;

						UIParts::SetTextCommonText(numText, u8_il2cpp(format("{:.2f}", value)).data());

						NotificationManager::SetPosition(value, UIParts::GetOptionSliderValue("character_system_text_caption_position_y") / 10);
						NotificationManager::SetDisplayTime(1);
						NotificationManager::Show(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
					}
				),
				UIParts::GetOptionSlider("character_system_text_caption_position_y", LocalifySettings::GetText("character_system_text_caption_position_y"), characterSystemTextCaptionPositionY * 10, -100, 100, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = UIParts::GetOptionSliderValue(slider);
						value = value / 10;

						UIParts::SetTextCommonText(numText, u8_il2cpp(format("{:.2f}", value)).data());

						NotificationManager::SetPosition(UIParts::GetOptionSliderValue("character_system_text_caption_position_x") / 10, value);
						NotificationManager::SetDisplayTime(1);
						NotificationManager::Show(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
					}
				),
				UIParts::GetOptionSlider("character_system_text_caption_background_alpha", LocalifySettings::GetText("character_system_text_caption_background_alpha"), characterSystemTextCaptionBackgroundAlpha * 100, 0, 100, true,
					*[](Il2CppObject* slider)
					{
						auto numText = UIParts::GetOptionSliderNumText(slider);
						auto value = UIParts::GetOptionSliderValue(slider);
						value = value / 100;

						UIParts::SetTextCommonText(numText, u8_il2cpp(format("{:.2f}", value)).data());

						NotificationManager::SetBackgroundAlpha(value);
						NotificationManager::SetDisplayTime(1);
						NotificationManager::Show(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
					}
				),
				UIParts::GetOptionItemSimpleWithButton("character_system_text_caption_font_color", (LocalifySettings::GetText("character_system_text_caption_font_color") + il2cppstring(IL2CPP_STRING(": ")) + config::config_document[IL2CPP_STRING("characterSystemTextCaptionFontColor")].GetString()).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemSimpleWithButton("character_system_text_caption_outline_size", (LocalifySettings::GetText("character_system_text_caption_outline_size") + il2cppstring(IL2CPP_STRING(": ")) + config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineSize")].GetString()).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
					UIParts::GetOptionItemSimpleWithButton("character_system_text_caption_outline_color", (LocalifySettings::GetText("character_system_text_caption_outline_color") + il2cppstring(IL2CPP_STRING(": ")) + config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineColor")].GetString()).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemButton("show_caption", LocalifySettings::GetText("show_caption")),
				UIParts::GetOptionItemAttention(LocalifySettings::GetText("applied_after_restart")),
				UIParts::GetOptionItemTitle(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0293")))->chars),
				UIParts::GetOptionItemOnOff("notification_tp", Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0294")))->chars),
				UIParts::GetOptionItemOnOff("notification_rp", Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0437")))->chars),
				isJobsExist ? UIParts::GetOptionItemOnOff("notification_jobs", Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Jobs600005")))->chars) : nullptr,
				UIParts::GetOptionItemButton("show_notification", LocalifySettings::GetText("show_notification")),
				UIParts::GetOptionItemAttention(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0297")))->chars),
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("taskbar")),
				UIParts::GetOptionItemOnOff("taskbar_show_progress_on_download", LocalifySettings::GetText("taskbar_show_progress_on_download")),
				UIParts::GetOptionItemOnOff("taskbar_show_progress_on_connecting", LocalifySettings::GetText("taskbar_show_progress_on_connecting")),
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("settings_title")),
				UIParts::GetOptionItemSimpleWithButton("persistent_data_path", LocalifySettings::GetText("persistent_data_path"), Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemInfo("persistent_data_path_detail", persistentDataPath->chars),
				Game::CurrentGameRegion == Game::Region::JPN ?
					UIParts::GetOptionItemButton("clear_webview_cache", LocalifySettings::GetText("clear_webview_cache")) :
					UIParts::GetOptionItemOnOff("allow_delete_cookie", LocalifySettings::GetText("allow_delete_cookie")),
				UIParts::GetOptionItemOnOff("dump_msgpack", LocalifySettings::GetText("dump_msgpack")),
				UIParts::GetOptionItemOnOff("dump_msgpack_request", LocalifySettings::GetText("dump_msgpack_request")),
				Game::CurrentGameRegion == Game::Region::KOR ?
					UIParts::GetOptionItemOnOff("use_third_party_news", LocalifySettings::GetText("use_third_party_news")) : nullptr,
				Game::CurrentGameRegion == Game::Region::KOR ?
					UIParts::GetOptionItemInfo(nullptr, LocalifySettings::GetText("use_third_party_news_info")) : nullptr,
#ifdef EXPERIMENTS
				UIParts::GetOptionItemOnOff("unlock_live_chara", LocalifySettings::GetText("unlock_live_chara")),
				UIParts::GetOptionItemInfo(nullptr, LocalifySettings::GetText("unlock_live_chara_info")),
#endif
				UIParts::GetOptionItemButton("github", IL2CPP_STRING("GitHub")),
				UIParts::GetOptionItemTitle(LocalifySettings::GetText("experiments")),
				UIParts::GetOptionItemButton("toggle_vr", IL2CPP_STRING("Toggle VR")),
			// GetOptionItemSimple("Simple"),
			// GetOptionItemOnOff("on_off", "On Off"),
			// GetOptionItem3ToggleVertical("Text"),
			// GetOptionItem3Toggle("Text"),
			// GetOptionItem2Toggle("Text"),
			/*GetDropdown("dropdown"),
			GetCheckbox("checkbox"),
			GetCheckboxWithText("checkbox_with_text"),
			GetRadioButtonWithText("radiobutton_with_text", "Test"),*/
			/*GetSlider("slider"),*/
			/*GetOptionItemInfo("<color=#ff911c>Info with Color</color>\nInfo"),*/
			}
		);

		UIParts::SetOptionItemOnOffAction("character_system_text_caption", characterSystemTextCaption, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("live_slider_always_show", liveSliderAlwaysShow, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("live_playback_loop", livePlaybackLoop, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("champions_live_show_text", championsLiveShowText, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("notification_tp", notificationTp, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("notification_rp", notificationRp, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("notification_jobs", notificationJobs, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("dump_msgpack", dumpMsgPack, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("dump_msgpack_request", dumpMsgPackRequest, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("use_third_party_news", useThirdPartyNews, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("unlock_size", unlockSize, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("use_system_resolution", unlockSizeUseSystemResolution, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("auto_fullscreen", autoFullscreen, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("freeform_window", freeFormWindow, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("unlock_live_chara", unlockLiveChara, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			UIParts::SetOptionItemOnOffAction("allow_delete_cookie", allowDeleteCookie, *([](Il2CppObject*, bool isOn)
				{
					// TODO
				})
			);
		}

		UIParts::SetOptionItemOnOffAction("on_off", false, *([](Il2CppObject*, bool isOn)
			{
				stringstream text;

				text << "Changed to " << (isOn ? "On" : "Off");

				Gallop::UIManager::Instance().ShowNotification(il2cpp_string_new(text.str().data()));
			})
		);

		UIParts::SetOptionItemButtonAction("show_caption", *([](Il2CppObject*)
			{
				if (config::character_system_text_caption)
				{
					NotificationManager::SetDisplayTime(1);
					NotificationManager::Show(il2cpp_string_new16(LocalifySettings::GetText("sample_caption")));
				}
				else
				{
					Gallop::UIManager::Instance().ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("setting_disabled")));
				}
			})
		);

		UIParts::SetOptionItemButtonAction("show_notification", *([](Il2CppObject*)
			{
				auto leader_chara_id = MsgPackData::user_info["leader_chara_id"].int_value();

				if (!leader_chara_id)
				{
					return;
				}

				auto title = u8_wide(MasterDB::GetTextData(6, leader_chara_id));
				auto contentU8 = MasterDB::GetTextData(163, leader_chara_id);
				replaceAll(contentU8, "\\n", "\n");
				auto content = u8_wide(contentU8);

				DesktopNotificationManagerCompat::ShowToastNotification(title.data(), content.data(), MsgPackData::GetIconPath(Gallop::LocalPushDefine::LocalPushType::Tp)->chars);
			})
		);

		UIParts::SetOptionItemButtonAction("graphics_quality", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("graphics_quality"), GetGraphicsQualityOptions(), config::config_document[IL2CPP_STRING("graphicsQuality")].GetInt() + 1,
					[](int value)
					{
						AddOrSet(config::config_document, IL2CPP_STRING("graphicsQuality"), value - 1);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("graphics_quality");
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("graphics_quality") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(GetGraphicsQualityOptions()[config::config_document[IL2CPP_STRING("graphicsQuality")].GetInt() + 1])).data());
					}
				);
			})
		);

		UIParts::SetOptionItemButtonAction("champions_live_resource_id", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("champions_live_resource_id"), MasterDB::GetChampionsResources(), config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1,
					[](int value)
					{
						AddOrSet(config::config_document, IL2CPP_STRING("championsLiveResourceId"), value + 1);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("champions_live_resource_id");
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_resource_id") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(MasterDB::GetChampionsResources()[config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1])).data());
					}
				);
			})
		);

		UIParts::SetOptionItemButtonAction("champions_live_year", *([](Il2CppObject*)
			{
				auto now = chrono::system_clock::now();
				auto now_time = chrono::system_clock::to_time_t(now);
				auto parts = localtime(&now_time);

				vector<string> championsLiveYears;

				for (int i = 2022; i <= 1900 + parts->tm_year; i++)
				{
					championsLiveYears.emplace_back(to_string(i));
				}

				OpenSelectOption(LocalifySettings::GetText("champions_live_year"), championsLiveYears, config::config_document[IL2CPP_STRING("championsLiveYear")].GetInt() - 2022,
					[](int value)
					{
						AddOrSet(config::config_document, IL2CPP_STRING("championsLiveYear"), value + 2022);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("champions_live_year");
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_year") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(to_string(config::config_document[IL2CPP_STRING("championsLiveYear")].GetInt()))).data());
					}
				);
			})
		);

		auto fontColorTextCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_font_color");
		UIParts::SetTextCommonOutlineColor(fontColorTextCommon, IL2CPP_STRING("Brown"));
		UIParts::SetTextCommonFontColor(fontColorTextCommon,
			config::config_document[IL2CPP_STRING("characterSystemTextCaptionFontColor")].GetString());

		UIParts::SetOptionItemButtonAction("character_system_text_caption_font_color", *([](Il2CppObject*)
			{
				auto options = GetFontColorOptions();
				auto& colorValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionFontColor")];
				auto value = il2cpp_u8(il2cppstring(colorValue.GetString(), colorValue.GetStringLength()));
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectOption(LocalifySettings::GetText("character_system_text_caption_font_color"), options, index,
					[](int value)
					{
						auto options = GetFontColorOptions();
						il2cppstring color = u8_il2cpp(options[value]);
						AddOrSetString(config::config_document, IL2CPP_STRING("characterSystemTextCaptionFontColor"), color);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_font_color");
						auto& colorValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionFontColor")];
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_font_color") + il2cppstring(IL2CPP_STRING(": ")) + il2cppstring(colorValue.GetString(), colorValue.GetStringLength())).data());
						UIParts::SetTextCommonFontColor(textCommon, color.data());
						NotificationManager::SetFontColor(color.data());
					},
					[](string name, il2cppstring value)
					{
						UIParts::SetTextCommonFontColor(UIParts::GetTextCommon(name.data()), value.data());
					}
				);
			})
		);

		auto outlineSizeTextCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_size");
		UIParts::SetTextCommonFontColor(outlineSizeTextCommon, IL2CPP_STRING("White"));
		UIParts::SetTextCommonOutlineColor(outlineSizeTextCommon, IL2CPP_STRING("Brown"));
		UIParts::SetTextCommonOutlineSize(outlineSizeTextCommon,
			config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineSize")].GetString());

		UIParts::SetOptionItemButtonAction("character_system_text_caption_outline_size", *([](Il2CppObject*)
			{
				auto options = GetOutlineSizeOptions();
				auto& sizeValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineSize")];
				auto value = il2cpp_u8(il2cppstring(sizeValue.GetString(), sizeValue.GetStringLength()));
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectOption(LocalifySettings::GetText("character_system_text_caption_outline_size"), options, index,
					[](int value)
					{
						auto options = GetOutlineSizeOptions();
						il2cppstring size = u8_il2cpp(options[value]);
						AddOrSetString(config::config_document, IL2CPP_STRING("characterSystemTextCaptionOutlineSize"), size);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_size");
						auto& sizeValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineSize")];
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_outline_size") + il2cppstring(IL2CPP_STRING(": ")) + il2cppstring(sizeValue.GetString(), sizeValue.GetStringLength())).data());
						UIParts::SetTextCommonOutlineSize(textCommon, size.data());
						NotificationManager::SetOutlineSize(size.data());
					},
					[](string name, il2cppstring value)
					{
						UIParts::SetTextCommonOutlineSize(UIParts::GetTextCommon(name.data()), value.data());
					}
				);
			})
		);

		auto outlineColorTextCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_color");
		UIParts::SetTextCommonOutlineColor(outlineColorTextCommon,
			config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineColor")].GetString());

		UIParts::SetOptionItemButtonAction("character_system_text_caption_outline_color", *([](Il2CppObject*)
			{
				auto options = GetOutlineColorOptions();
				auto& colorValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineColor")];
				auto value = il2cpp_u8(il2cppstring(colorValue.GetString(), colorValue.GetStringLength()));
				auto found = find(options.begin(), options.end(), value);
				int index = 0;

				if (found != options.end())
				{
					index = found - options.begin();
				}

				OpenSelectOption(LocalifySettings::GetText("character_system_text_caption_outline_color"), options, index,
					[](int value)
					{
						auto options = GetOutlineColorOptions();
						il2cppstring color = u8_il2cpp(options[value]);
						AddOrSetString(config::config_document, IL2CPP_STRING("characterSystemTextCaptionOutlineColor"), color);

						auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("character_system_text_caption_outline_color");
						auto& colorValue = config::config_document[IL2CPP_STRING("characterSystemTextCaptionOutlineColor")];
						UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("character_system_text_caption_outline_color") + il2cppstring(IL2CPP_STRING(": ")) + il2cppstring(colorValue.GetString(), colorValue.GetStringLength())).data());
						UIParts::SetTextCommonOutlineColor(textCommon, color.data());
						NotificationManager::SetOutlineColor(color.data());
					},
					[](string name, il2cppstring value)
					{
						UIParts::SetTextCommonOutlineColor(UIParts::GetTextCommon(name.data()), value.data());
					}
				);
			})
		);

		UIParts::SetOptionItemOnOffAction("taskbar_show_progress_on_download", taskbarShowProgressOnDownload, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemOnOffAction("taskbar_show_progress_on_connecting", taskbarShowProgressOnConnecting, *([](Il2CppObject*, bool isOn)
			{
			})
		);

		UIParts::SetOptionItemButtonAction("toggle_vr", *([](Il2CppObject*)
			{
				/*if (!Unity::OpenXR::initialized)
				{
					Unity::OpenXR::InitLibrary(unityInterfaces);
					Unity::OpenXR::Init();
				}*/

				if (Unity::OpenXR::initialized)
				{
					static auto currentCamera = UnityEngine::Behaviour(il2cpp_resolve_icall_type<Il2CppObject * (*)()>("UnityEngine.Camera::get_current()")());
					//wcout << UnityEngine::Object::Name(currentCamera)->chars << endl;
					//wcout << il2cpp_resolve_icall_type<float (*)(Il2CppObject*)>("UnityEngine.Camera::get_depth()")(currentCamera) << endl;
					//Vector3 origPos{};
					//il2cpp_resolve_icall_type<void (*)(Il2CppObject*, Vector3*)>("UnityEngine.Transform::get_position_Injected()")(currentCamera.gameObject().transform(), &origPos);

					//static auto xrRig = UnityEngine::GameObject(il2cpp_string_new("XRRig"));
					//
					//UnityEngine::Object::DontDestroyOnLoad(xrRig);
					//
					//static auto cameraOffset = UnityEngine::GameObject(il2cpp_string_new("Camera Offset"));
					//UnityEngine::Object::DontDestroyOnLoad(cameraOffset);

					//cout << "POS: " << origPos.x << " " << origPos.y << " " << origPos.z << endl;
					//origPos.y += 1000;
					//origPos.z += 1000;

					//static auto gameObject = UnityEngine::GameObject(il2cpp_string_new("Main Camera"));
					//UnityEngine::Object::DontDestroyOnLoad(gameObject);

					//xrRig.transform().SetParent(cameraOffset.transform(), false);
					//cameraOffset.transform().SetParent(gameObject.transform(), false);
					//static auto camera = UnityEngine::Behaviour(gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Camera")));
					//UnityEngine::Object::DontDestroyOnLoad(camera);
					//// il2cpp_resolve_icall_type<Il2CppString* (*)(Il2CppObject*, int)>("UnityEngine.Camera::set_cullingMask()")(camera, 4294967295);
					//il2cpp_resolve_icall_type<void (*)(Il2CppObject*, bool)>("UnityEngine.Camera::set_allowDynamicResolution()")(camera, true);

					//il2cpp_resolve_icall_type<void (*)(Il2CppObject*, Vector3)>("UnityEngine.Transform::set_position_Injected()")(cameraOffset.transform(), origPos);

					auto gameObject = currentCamera.gameObject();
					gameObject.tag(il2cpp_string_new("MainCamera"));

					// il2cpp_resolve_icall_type<void (*)(Il2CppObject*, float)>("UnityEngine.Camera::set_depth()")(camera, 23);

					//il2cpp_resolve_icall_type<Il2CppString* (*)(Il2CppObject*, int)>("UnityEngine.Camera::set_stereoTargetEye()")(camera, 3);

					if (Unity::OpenXR::started)
					{
						// camera.enabled(false);
						// currentCamera.enabled(true);
						Unity::OpenXR::Stop();
					}
					else
					{
						// camera.enabled(true);
						// currentCamera.enabled(false);
						Unity::OpenXR::Start();
					}
				}
			})
		);

		UIParts::SetOptionItemButtonAction("github", *([](Il2CppObject*)
			{
				auto dialogData = Gallop::DialogCommon::Data();
				dialogData.SetSimpleTwoButtonMessage(
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Common0009"))),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Home0073"))),
					CreateDelegateStatic(*[]()
						{
							UnityEngine::Application::OpenURL(il2cpp_string_new("https://github.com/Kimjio/umamusume-localify"));
						}
					),
					GetTextIdByName(IL2CPP_STRING("Common0004")),
					GetTextIdByName(IL2CPP_STRING("Common0003"))
				);

				Gallop::DialogManager::PushDialog(dialogData);
			})
		);

		UIParts::SetOptionItemButtonAction("persistent_data_path", *([](Il2CppObject*)
			{
				auto CuteCoreDevice = il2cpp_symbols::get_class("Cute.Core.Assembly.dll", "Cute.Core", "Device");
				auto persistentDataPathField = il2cpp_class_get_field_from_name(CuteCoreDevice, "persistentDataPath");
				Il2CppString* persistentDataPath;
				il2cpp_field_static_get_value(persistentDataPathField, &persistentDataPath);

				auto result = FolderOpen();

				if (result)
				{
					il2cppstring pathU16 = result;
					AddOrSetString(config::config_document, IL2CPP_STRING("persistentDataPath"), result);
					auto textCommon = UIParts::GetTextCommon("persistent_data_path_detail_info");
					UIParts::SetTextCommonText(textCommon, pathU16.data());
				}
			})
		);

		if (Game::CurrentGameRegion != Game::Region::KOR)
		{
			UIParts::SetOptionItemButtonAction("clear_webview_cache", *([](Il2CppObject*)
				{
					auto dialogData = Gallop::DialogCommon::Data();
					dialogData.SetSimpleTwoButtonMessage(
						Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Race0652"))),
						il2cpp_string_new16(LocalifySettings::GetText("clear_webview_cache_confirm")),
						CreateDelegateStatic(*[]()
							{
								PWSTR path;
								SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &path);

								il2cppstring combinedPath = il2cppstring(path).append(IL2CPP_STRING("\\DMMWebView2"));

								try
								{
									filesystem::remove_all(combinedPath);
									Gallop::UIManager::Instance().ShowNotification(il2cpp_string_new16(LocalifySettings::GetText("deleted")));
								}
								catch (exception& e)
								{
									cout << e.what() << endl;
								}
							}),
						GetTextIdByName(IL2CPP_STRING("Common0004")),
						GetTextIdByName(IL2CPP_STRING("Common0003"))
					);
					Gallop::DialogManager::PushDialog(dialogData);
				})
			);
		}

		auto contentSizeFitter = contentGameObject.AddComponent(GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto _layoutField = il2cpp_class_get_field_from_name(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		dialogData.ContentsObject(gameObject);

		settingsDialog = Gallop::DialogManager::PushDialog(dialogData);
	}

	void SettingsUI::OpenLiveSettings()
	{
		auto onLeft = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				settingsDialog.Close();
				settingsDialog = nullptr;

				config::rollback_config();
			});

		auto onRight = CreateDelegateStatic(*[](void*, Il2CppObject* dialog)
			{
				auto& configDocument = config::config_document;

				AddOrSet(configDocument, IL2CPP_STRING("liveSliderAlwaysShow"), UIParts::GetOptionItemOnOffIsOn("live_slider_always_show"));

				AddOrSet(configDocument, IL2CPP_STRING("livePlaybackLoop"), UIParts::GetOptionItemOnOffIsOn("live_playback_loop"));

				AddOrSet(configDocument, IL2CPP_STRING("championsLiveShowText"), UIParts::GetOptionItemOnOffIsOn("champions_live_show_text"));

				config::live_slider_always_show = configDocument[IL2CPP_STRING("liveSliderAlwaysShow")].GetBool();

				config::live_playback_loop = configDocument[IL2CPP_STRING("livePlaybackLoop")].GetBool();

				config::champions_live_show_text = configDocument[IL2CPP_STRING("championsLiveShowText")].GetBool();

				config::champions_live_year = configDocument[IL2CPP_STRING("championsLiveYear")].GetInt();

				config::champions_live_resource_id = configDocument[IL2CPP_STRING("championsLiveResourceId")].GetInt();

				config::write_config();

				auto dialogData = Gallop::DialogCommon::Data();
				dialogData.SetSimpleOneButtonMessage(GetTextIdByName(IL2CPP_STRING("AccoutDataLink0061")), Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Outgame0309"))), nullptr, GetTextIdByName(IL2CPP_STRING("Common0007")));

				auto onDestroy = CreateDelegateStatic(*[]()
					{
						settingsDialog.Close();
						settingsDialog = nullptr;
					});

				dialogData.AddDestroyCallback(onDestroy);
				Gallop::DialogManager::PushDialog(dialogData);
			});

		auto dialogData = Gallop::DialogCommon::Data();

		dialogData.SetSimpleTwoButtonMessage(il2cpp_string_new16(LocalifySettings::GetText("settings_title")), nullptr, onRight, GetTextIdByName(IL2CPP_STRING("Common0004")), GetTextIdByName(IL2CPP_STRING("Common0261")), onLeft, Gallop::DialogCommonBase::FormType::BIG_TWO_BUTTON);
		dialogData.DispStackType(Gallop::DialogCommon::DispStackType::DialogOnDialog);
		dialogData.ObjParentType(Gallop::DialogCommon::Data::ObjectParentType::Base);
		dialogData.AutoClose(false);

		auto gameObject = UnityEngine::GameObject();
		auto rootTransform = gameObject.AddComponent(GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform"));

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_sizeDelta", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMax", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 1, 1 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchorMin", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_pivot", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0.5, 0.5 });

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(rootTransform->klass, "set_anchoredPosition", 1)->methodPointer(rootTransform, UnityEngine::Vector2{ 0, 0 });

		UnityEngine::GameObject scrollViewBase = UnityEngine::ResourcesAPIInternal::Load(il2cpp_string_new("ui/parts/base/scrollviewbase"), GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject"));

		auto uiManager = Gallop::UIManager::Instance();
		Il2CppObject* _mainCanvas = uiManager._mainCanvas();

		auto transform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(_mainCanvas->klass, "get_transform", 0)->methodPointer(_mainCanvas);

		scrollViewBase = UnityEngine::Object::Internal_CloneSingleWithParent(scrollViewBase, transform, false);

		auto scrollRectArray = scrollViewBase.GetComponentsInChildren(GetRuntimeType("umamusume.dll", "Gallop", "ScrollRectCommon"), false);

		auto scrollRect = scrollRectArray->vector[0];

		auto m_ViewportField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Viewport");
		Il2CppObject* m_Viewport;
		il2cpp_field_get_value(scrollRect, m_ViewportField, &m_Viewport);

		UnityEngine::RectTransform scrollRectTransform = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Viewport->klass, "get_parent", 0)->methodPointer(m_Viewport);
		scrollRectTransform.sizeDelta({ -24, -12 });
		scrollRectTransform.anchorMax({ 1, 1 });
		scrollRectTransform.anchorMin({ 0, 0 });
		scrollRectTransform.pivot({ 0.5, 0.5 });
		scrollRectTransform.anchoredPosition({ 0, -6 });
		scrollRectTransform.SetParent(rootTransform, false);

		auto m_ContentField = il2cpp_class_get_field_from_name(scrollRect->klass, "m_Content");
		Il2CppObject* m_Content;
		il2cpp_field_get_value(scrollRect, m_ContentField, &m_Content);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_sizeDelta", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 56, 0 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMax", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 1, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchorMin", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_pivot", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0.5, 1 });
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Vector2)>(m_Content->klass, "set_anchoredPosition", 1)->methodPointer(m_Content, UnityEngine::Vector2{ 0, 0 });

		UnityEngine::GameObject contentGameObject = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(m_Content->klass, "get_gameObject", 0)->methodPointer(m_Content);

		auto verticalLayoutGroup = contentGameObject.AddComponent(GetRuntimeType("UnityEngine.UI.dll", "UnityEngine.UI", "VerticalLayoutGroup"));
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(verticalLayoutGroup->klass, "set_childAlignment", 1)->methodPointer(verticalLayoutGroup, 1);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childForceExpandWidth", 1)->methodPointer(verticalLayoutGroup, true);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, bool)>(verticalLayoutGroup->klass, "set_childControlWidth", 1)->methodPointer(verticalLayoutGroup, true);

		auto padding = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(verticalLayoutGroup->klass, "get_padding", 0)->methodPointer(verticalLayoutGroup);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_top", 1)->methodPointer(padding, -20);
		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int)>(padding->klass, "set_bottom", 1)->methodPointer(padding, 16);

		bool liveSliderAlwaysShow = false;
		bool livePlaybackLoop = false;
		bool championsLiveShowText = false;
		int championsLiveYear = 2023;

		if (config::read_config())
		{
			auto& configDocument = config::config_document;

			if (configDocument.HasMember(IL2CPP_STRING("liveSliderAlwaysShow")))
			{
				liveSliderAlwaysShow = configDocument[IL2CPP_STRING("liveSliderAlwaysShow")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("livePlaybackLoop")))
			{
				livePlaybackLoop = configDocument[IL2CPP_STRING("livePlaybackLoop")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("championsLiveShowText")))
			{
				championsLiveShowText = configDocument[IL2CPP_STRING("championsLiveShowText")].GetBool();
			}

			if (configDocument.HasMember(IL2CPP_STRING("championsLiveYear")))
			{
				championsLiveYear = configDocument[IL2CPP_STRING("championsLiveYear")].GetInt();
			}
		}

		string championsResourceText;
		int championsLiveResourceIndex = config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1;
		auto championsResources = MasterDB::GetChampionsResources();

		if (championsResources.size() <= championsLiveResourceIndex)
		{
			championsResourceText = il2cpp_u8(LocalifySettings::GetText("unknown"));
		}
		else
		{
			championsResourceText = championsResources[championsLiveResourceIndex];
		}

		UIParts::AddToLayout(m_Content,
			{
				UIParts::GetOptionItemTitle(Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Common0035")))->chars),
				UIParts::GetOptionItemOnOff("live_slider_always_show", LocalifySettings::GetText("live_slider_always_show")),
				UIParts::GetOptionItemOnOff("live_playback_loop", LocalifySettings::GetText("live_playback_loop")),
				UIParts::GetOptionItemOnOff("champions_live_show_text", LocalifySettings::GetText("champions_live_show_text")),
				UIParts::GetOptionItemSimpleWithButton("champions_live_resource_id", (LocalifySettings::GetText("champions_live_resource_id") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(championsResourceText)).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemSimpleWithButton("champions_live_year", (LocalifySettings::GetText("champions_live_year") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(to_string(championsLiveYear))).data(),
					Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("Circle0206")))->chars),
				UIParts::GetOptionItemSimple(IL2CPP_STRING("")),
			}
			);

		UIParts::SetOptionItemOnOffAction("live_slider_always_show", liveSliderAlwaysShow, *([](Il2CppObject*, bool isOn)
			{
			}));

		UIParts::SetOptionItemOnOffAction("live_playback_loop", livePlaybackLoop, *([](Il2CppObject*, bool isOn)
			{
			}));

		UIParts::SetOptionItemOnOffAction("champions_live_show_text", championsLiveShowText, *([](Il2CppObject*, bool isOn)
			{
			}));

		UIParts::SetOptionItemButtonAction("champions_live_resource_id", *([](Il2CppObject*)
			{
				OpenSelectOption(LocalifySettings::GetText("champions_live_resource_id"), MasterDB::GetChampionsResources(), config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1, [](int value) {
					AddOrSet(config::config_document, IL2CPP_STRING("championsLiveResourceId"), value + 1);

					auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("champions_live_resource_id");
					UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_resource_id") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(MasterDB::GetChampionsResources()[config::config_document[IL2CPP_STRING("championsLiveResourceId")].GetInt() - 1])).data());
					});
			}));

		UIParts::SetOptionItemButtonAction("champions_live_year", *([](Il2CppObject*)
			{
				auto now = chrono::system_clock::now();
				auto now_time = chrono::system_clock::to_time_t(now);
				auto parts = localtime(&now_time);

				vector<string> championsLiveYears;

				for (int i = 2022; i <= 1900 + parts->tm_year; i++)
				{
					championsLiveYears.emplace_back(to_string(i));
				}

				OpenSelectOption(LocalifySettings::GetText("champions_live_year"), championsLiveYears, config::config_document[IL2CPP_STRING("championsLiveYear")].GetInt() - 2022, [](int value) {
					AddOrSet(config::config_document, IL2CPP_STRING("championsLiveYear"), value + 2022);

					auto textCommon = UIParts::GetOptionItemSimpleWithButtonTextCommon("champions_live_year");
					UIParts::SetTextCommonText(textCommon, (LocalifySettings::GetText("champions_live_year") + il2cppstring(IL2CPP_STRING(": ")) + u8_il2cpp(to_string(config::config_document[IL2CPP_STRING("championsLiveYear")].GetInt()))).data());
					});
			}));

		auto contentSizeFitter = contentGameObject.AddComponent(GetRuntimeType("umamusume.dll", "Gallop", "LayoutGroupContentSizeFitter"));

		auto _layoutField = il2cpp_class_get_field_from_name(contentSizeFitter->klass, "_layout");
		il2cpp_field_set_value(contentSizeFitter, _layoutField, verticalLayoutGroup);

		bool _autoUpdate = true;
		auto _autoUpdateField = il2cpp_class_get_field_from_name(contentSizeFitter->klass, "_autoUpdate");
		il2cpp_field_set_value(contentSizeFitter, _autoUpdateField, &_autoUpdate);

		il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(contentSizeFitter->klass, "SetSize", 0)->methodPointer(contentSizeFitter);

		dialogData.ContentsObject(gameObject);

		settingsDialog = Gallop::DialogManager::PushDialog(dialogData);
	}
}
