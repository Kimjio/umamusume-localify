// ******************************************************************
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE CODE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
// THE CODE OR THE USE OR OTHER DEALINGS IN THE CODE.
// ******************************************************************

#include "DesktopNotificationManagerCompat.h"
#include <appmodel.h>
#include <wrl\wrappers\corewrappers.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <NotificationActivationCallback.h>

#include <filesystem>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "SaveIcon.h"

#include "il2cpp/il2cpp_symbols.hpp"
#include "local/local.hpp"
#include "game.hpp"

#define RETURN_IF_FAILED(hr) do { HRESULT _hrTemp = hr; if (FAILED(_hrTemp)) { cout << #hr << " FAILED Last Error: " << GetLastError() << endl; return _hrTemp; } } while (false)

using namespace std;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

namespace DesktopNotificationManagerCompat
{
	HRESULT RegisterAUMID(const wchar_t* aumid, GUID clsid, const wchar_t* displayName, const wchar_t exePath[]);
	HRESULT RegisterComServer(GUID clsid, const wchar_t exePath[]);
	HRESULT EnsureRegistered();
	wstring GetAppDataFolderIconPath(const wchar_t* amuid);

	bool s_registeredAumidAndComServer = false;
	// wstring s_aumid;
	bool s_registeredActivator = false;
	bool s_hasCheckedIsRunningAsUwp = false;
	bool s_isRunningAsUwp = false;

	HRESULT GetGuidByRegion(LPIID lpiid)
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			return IIDFromString(L"{A42CD370-8D06-4E3C-BC45-32A61F9E375B}", lpiid);
		}

		if (Game::CurrentGameRegion == Game::Region::ENG)
		{
			return IIDFromString(L"{A42CD370-8D06-4E3C-BC45-32A61F9E375C}", lpiid);
		}

		return IIDFromString(L"{A42CD370-8D06-4E3C-BC45-32A61F9E375A}", lpiid);
	}

	HRESULT RegisterAumidAndComServer(const wchar_t* aumid, const wchar_t* displayName)
	{
		GUID clsid;

		GetGuidByRegion(&clsid);

		// Copy the aumid
		s_aumid = wstring(aumid);

		// Get the EXE path
		wchar_t exePath[MAX_PATH];
		DWORD charWritten = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
		RETURN_IF_FAILED(charWritten > 0 ? S_OK : HRESULT_FROM_WIN32(::GetLastError()));

		HICON hIcon;
		ExtractIconExW(exePath, 0, &hIcon, nullptr, 1);

		auto path = GetAppDataFolderIconPath(aumid);

		if (!filesystem::exists(filesystem::path(path).parent_path()))
		{
			filesystem::create_directories(filesystem::path(path).parent_path());
		}

		SaveIconToFile(hIcon, GetAppDataFolderIconPath(aumid).data(), 32);

		RETURN_IF_FAILED(RegisterAUMID(aumid, clsid, displayName, exePath));

		// Register the COM server
		RETURN_IF_FAILED(RegisterComServer(clsid, exePath));

		s_registeredAumidAndComServer = true;
		return S_OK;
	}

	HRESULT RegisterActivator()
	{
		// Module<OutOfProc> needs a callback registered before it can be used.
		// Since we don't care about when it shuts down, we'll pass an empty lambda here.
		Module<OutOfProc>::Create([] {});

		// If a local server process only hosts the COM object then COM expects
		// the COM server host to shutdown when the references drop to zero.
		// Since the user might still be using the program after activating the notification,
		// we don't want to shutdown immediately.  Incrementing the object count tells COM that
		// we aren't done yet.
		Module<OutOfProc>::GetModule().IncrementObjectCount();

		RETURN_IF_FAILED(Module<OutOfProc>::GetModule().RegisterObjects());

		s_registeredActivator = true;
		return S_OK;
	}

	HRESULT RegisterAUMID(const wchar_t* aumid, GUID clsid, const wchar_t* displayName, const wchar_t exePath[])
	{
		// Turn the GUID into a string
		OLECHAR* clsidOlechar;
		StringFromCLSID(clsid, &clsidOlechar);
		wstring clsidStr(clsidOlechar);
		::CoTaskMemFree(clsidOlechar);

		// Create the subkey
		// Something like SOFTWARE\Classes\AppUserModelId\Cygames.Gallop
		wstring subKey = LR"(SOFTWARE\Classes\AppUserModelId\)" + s_aumid;

		DWORD dataSize = static_cast<DWORD>((clsidStr.length() + 1) * sizeof(WCHAR));

		RETURN_IF_FAILED(HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"CustomActivator",
			REG_SZ,
			reinterpret_cast<const BYTE*>(clsidStr.c_str()),
			dataSize)));

		wstring displayNameStr(displayName);

		dataSize = static_cast<DWORD>((displayNameStr.length() + 1) * sizeof(WCHAR));

		RETURN_IF_FAILED(HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"DisplayName",
			REG_SZ,
			reinterpret_cast<const BYTE*>(displayNameStr.c_str()),
			dataSize)));

		wstring color(L"FFDDDDDD");

		dataSize = static_cast<DWORD>((color.length() + 1) * sizeof(WCHAR));

		RETURN_IF_FAILED(HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"IconBackgroundColor",
			REG_SZ,
			reinterpret_cast<const BYTE*>(color.c_str()),
			dataSize)));

		// Include -ToastActivated launch args on the exe
		wstring iconPath = GetAppDataFolderIconPath(aumid);

		// We don't need to worry about overflow here as ::GetModuleFileName won't
		// return anything bigger than the max file system path (much fewer than max of DWORD).
		dataSize = static_cast<DWORD>((iconPath.length() + 1) * sizeof(WCHAR));

		// Register the EXE for the COM server
		RETURN_IF_FAILED(HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"IconUri",
			REG_SZ,
			reinterpret_cast<const BYTE*>(iconPath.c_str()),
			dataSize)));

		return S_OK;
	}

	HRESULT RegisterComServer(GUID clsid, const wchar_t exePath[])
	{
		// Turn the GUID into a string
		OLECHAR* clsidOlechar;
		StringFromCLSID(clsid, &clsidOlechar);
		wstring clsidStr(clsidOlechar);
		::CoTaskMemFree(clsidOlechar);

		// Create the subkey
		// Something like SOFTWARE\Classes\CLSID\{23A5B06E-20BB-4E7E-A0AC-6982ED6A6041}\LocalServer32
		wstring subKey = LR"(SOFTWARE\Classes\CLSID\)" + clsidStr + LR"(\LocalServer32)";

		wstring exePathStr(exePath);
		exePathStr = L"\"" + exePathStr + L"\"";

		// We don't need to worry about overflow here as ::GetModuleFileName won't
		// return anything bigger than the max file system path (much fewer than max of DWORD).
		DWORD dataSize = static_cast<DWORD>((exePathStr.length() + 1) * sizeof(WCHAR));

		// Register the EXE for the COM server
		return HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			nullptr,
			REG_SZ,
			reinterpret_cast<const BYTE*>(exePathStr.c_str()),
			dataSize));
	}

	HRESULT CreateToastNotifier(ABI::IToastNotifier** notifier)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		ComPtr<ABI::IToastNotificationManagerStatics> toastStatics;
		RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
			HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(),
			&toastStatics));

		return toastStatics->CreateToastNotifierWithId(HStringReference(s_aumid.c_str()).Get(), notifier);
	}

	HRESULT CreateXmlDocumentFromString(const wchar_t* xmlString, IXmlDocument** doc)
	{
		ComPtr<IXmlDocument> answer;
		RETURN_IF_FAILED(Windows::Foundation::ActivateInstance(HStringReference(RuntimeClass_Windows_Data_Xml_Dom_XmlDocument).Get(), &answer));

		ComPtr<IXmlDocumentIO> docIO;
		RETURN_IF_FAILED(answer.As(&docIO));

		// Load the XML string
		RETURN_IF_FAILED(docIO->LoadXml(HStringReference(xmlString).Get()));

		return answer.CopyTo(doc);
	}

	HRESULT CreateToastNotification(IXmlDocument* content, ABI::IToastNotification** notification)
	{
		ComPtr<ABI::IToastNotificationFactory> factory;
		RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
			HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(),
			&factory));

		return factory->CreateToastNotification(content, notification);
	}

	HRESULT CreateScheduledToastNotification(IXmlDocument* content, DateTime deliveryTime, ABI::IScheduledToastNotification** notification)
	{
		ComPtr<ABI::IScheduledToastNotificationFactory> factory;
		RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
			HStringReference(RuntimeClass_Windows_UI_Notifications_ScheduledToastNotification).Get(),
			&factory));

		return factory->CreateScheduledToastNotification(content, deliveryTime, notification);
	}

	HRESULT get_History(unique_ptr<DesktopNotificationHistoryCompat>* history)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		ComPtr<ABI::IToastNotificationManagerStatics> toastStatics;
		RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
			HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(),
			&toastStatics));

		ComPtr<ABI::IToastNotificationManagerStatics2> toastStatics2;
		RETURN_IF_FAILED(toastStatics.As(&toastStatics2));

		ComPtr<ABI::IToastNotificationHistory> nativeHistory;
		RETURN_IF_FAILED(toastStatics2->get_History(&nativeHistory));

		*history = unique_ptr<DesktopNotificationHistoryCompat>(new DesktopNotificationHistoryCompat(s_aumid.c_str(), nativeHistory));
		return S_OK;
	}

	HRESULT PreRegisterIdentityLessApp()
	{
		RETURN_IF_FAILED(EnsureRegistered());

		wstring subKey = LR"(SOFTWARE\Classes\AppUserModelId\)" + s_aumid;

		DWORD HasSentNotification = FALSE;

		DWORD cbData = sizeof(DWORD);
		RegGetValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"HasSentNotification",
			RRF_RT_REG_DWORD, nullptr, &HasSentNotification, &cbData);

		if (HasSentNotification == TRUE)
		{
			return S_OK;
		}

		wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
            <text>New Notification</text>
        </binding></visual></toast>)";

		ComPtr<IXmlDocument> doc;
		RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));

		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		ComPtr<ABI::IToastNotification> toast1;
		RETURN_IF_FAILED(CreateToastNotification(doc.Get(), &toast1));

		ComPtr<ABI::IToastNotification2> toast2;
		RETURN_IF_FAILED(toast1.As(&toast2));

		winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::DateTime> dt{ winrt::clock::now() + chrono::seconds(15) };
		winrt::com_ptr<IReference<DateTime>> ptr{ dt.as<IReference<DateTime>>() };
		RETURN_IF_FAILED(toast2.Get()->put_SuppressPopup(true));
		RETURN_IF_FAILED(toast2.Get()->put_Group(HStringReference(L"toolkitGroupNull").Get()));
		RETURN_IF_FAILED(toast2.Get()->put_Tag(HStringReference(L"toolkit1stNotif").Get()));
		RETURN_IF_FAILED(toast1.Get()->put_ExpirationTime(ptr.get()));

		RETURN_IF_FAILED(notifier->Show(toast1.Get()));

		unique_ptr<DesktopNotificationHistoryCompat> history;
		RETURN_IF_FAILED(get_History(&history));
		RETURN_IF_FAILED(history->RemoveGroupedTag(L"toolkit1stNotif", L"toolkitGroupNull"));

		DWORD data = 1;

		return HRESULT_FROM_WIN32(RegSetKeyValueW(
			HKEY_CURRENT_USER,
			subKey.c_str(),
			L"HasSentNotification",
			REG_DWORD,
			&data,
			sizeof(data)));
	}

	HRESULT ShowToastNotification(const wchar_t* title, const wchar_t* content, const wchar_t* iconPath, bool suppressPopup)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		ComPtr<IXmlDocument> doc;

		if (!iconPath)
		{
			wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
				<text>)"s + title + LR"(</text>
				<text>)" + content + LR"(</text>
			</binding></visual></toast>)";
			RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));
		}
		else
		{
			wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
				<image placement='appLogoOverride' src=')"s + L"file://" + iconPath + LR"('/>
				<text>)" + title + LR"(</text>
				<text>)" + content + LR"(</text>
			</binding></visual></toast>)";
			RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));
		}

		// Create the notifier
		// Desktop apps must use the compat method to create the notifier.
		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		unique_ptr<DesktopNotificationHistoryCompat> history;
		RETURN_IF_FAILED(get_History(&history));
		history->RemoveGroupedTag(L"Preview", L"Generic");

		// Create the notification itself (using helper method from compat library)
		ComPtr<ABI::IToastNotification> toast;
		RETURN_IF_FAILED(CreateToastNotification(doc.Get(), &toast));

		ComPtr<ABI::IToastNotification2> toast2;
		RETURN_IF_FAILED(toast.As(&toast2));

		toast2.Get()->put_Group(HStringReference(L"Generic").Get());
		toast2.Get()->put_Tag(HStringReference(L"Preview").Get());
		toast2.Get()->put_SuppressPopup(suppressPopup);

		return notifier->Show(toast.Get());
	}

	HRESULT ShowToastNotification(const wchar_t* title, const wchar_t* content, const wchar_t* tag, const wchar_t* iconPath, bool removeHistory, bool suppressPopup)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		ComPtr<IXmlDocument> doc;

		if (!iconPath)
		{
			wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
				<text>)"s + title + LR"(</text>
				<text>)" + content + LR"(</text>
			</binding></visual></toast>)";
			RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));
		}
		else
		{
			wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
				<image placement='appLogoOverride' src=')"s + L"file://" + iconPath + LR"('/>
				<text>)" + title + LR"(</text>
				<text>)" + content + LR"(</text>
			</binding></visual></toast>)";
			RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));
		}

		// Create the notifier
		// Desktop apps must use the compat method to create the notifier.
		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		if (removeHistory)
		{
			unique_ptr<DesktopNotificationHistoryCompat> history;
			RETURN_IF_FAILED(get_History(&history));
			history->RemoveGroupedTag(tag, L"Generic");
		}

		// Create the notification itself (using helper method from compat library)
		ComPtr<ABI::IToastNotification> toast;
		RETURN_IF_FAILED(CreateToastNotification(doc.Get(), &toast));

		ComPtr<ABI::IToastNotification2> toast2;
		RETURN_IF_FAILED(toast.As(&toast2));

		toast2.Get()->put_Group(HStringReference(L"Generic").Get());
		toast2.Get()->put_Tag(HStringReference(tag).Get());
		toast2.Get()->put_SuppressPopup(suppressPopup);

		return notifier->Show(toast.Get());
	}

	HRESULT AddScheduledToastNotification(const wchar_t* title, const wchar_t* content, const wchar_t* tag, const wchar_t* iconPath, int64_t epoch, const wchar_t* group)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		RETURN_IF_FAILED(PreRegisterIdentityLessApp());

		wstring templateString = LR"(<toast><visual><binding template='ToastGeneric'>
            <image placement='appLogoOverride' src=')"s + L"file://" + iconPath + LR"('/>
            <text>)" + title + LR"(</text>
            <text>)" + content + LR"(</text>
        </binding></visual></toast>)";

		ComPtr<IXmlDocument> doc;
		RETURN_IF_FAILED(CreateXmlDocumentFromString(templateString.data(), &doc));

		// Create the notifier
		// Desktop apps must use the compat method to create the notifier.
		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		auto time = winrt::clock::from_sys(chrono::time_point<chrono::system_clock, chrono::system_clock::duration>(
			winrt::clock::duration(chrono::milliseconds(epoch))
		)).time_since_epoch().count();

		if (time < winrt::clock::now().time_since_epoch().count())
		{
			return S_OK;
		}

		// Create the notification itself (using helper method from compat library)
		ComPtr<ABI::IScheduledToastNotification> toast;
		RETURN_IF_FAILED(CreateScheduledToastNotification(doc.Get(), { time }, &toast));

		ComPtr<ABI::IScheduledToastNotification2> toast2;
		RETURN_IF_FAILED(toast.As(&toast2));

		toast2.Get()->put_Group(HStringReference(group).Get());
		toast2.Get()->put_Tag(HStringReference(tag).Get());

		return notifier->AddToSchedule(toast.Get());
	}

	HRESULT RemoveFromScheduleByTag(const wchar_t* tag)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		// Create the notifier
		// Desktop apps must use the compat method to create the notifier.
		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		ABI::Windows::Foundation::Collections::IVectorView<ABI::ScheduledToastNotification*>* toasts;
		RETURN_IF_FAILED(notifier.Get()->GetScheduledToastNotifications(&toasts));

		uint32_t size;
		RETURN_IF_FAILED(toasts->get_Size(&size));
		for (int i = 0; i < size; i++)
		{
			ABI::IScheduledToastNotification* toast;
			RETURN_IF_FAILED(toasts->GetAt(i, &toast));

			ABI::IScheduledToastNotification2* toast2;
			RETURN_IF_FAILED(toast->QueryInterface(&toast2));

			HSTRING hTag;
			RETURN_IF_FAILED(toast2->get_Tag(&hTag));

			auto tag1 = WindowsGetStringRawBuffer(hTag, nullptr);

			if (tag1 == wstring(tag))
			{
				RETURN_IF_FAILED(notifier.Get()->RemoveFromSchedule(toast));
			}
		}

		return S_OK;
	}

	HRESULT RemoveFromScheduleByGroup(const wchar_t* group)
	{
		RETURN_IF_FAILED(EnsureRegistered());

		// Create the notifier
		// Desktop apps must use the compat method to create the notifier.
		ComPtr<ABI::IToastNotifier> notifier;
		RETURN_IF_FAILED(CreateToastNotifier(&notifier));

		ABI::Windows::Foundation::Collections::IVectorView<ABI::ScheduledToastNotification*>* toasts;
		RETURN_IF_FAILED(notifier.Get()->GetScheduledToastNotifications(&toasts));

		uint32_t size;
		RETURN_IF_FAILED(toasts->get_Size(&size));
		for (int i = 0; i < size; i++)
		{
			ABI::IScheduledToastNotification* toast;
			RETURN_IF_FAILED(toasts->GetAt(i, &toast));

			ABI::IScheduledToastNotification2* toast2;
			RETURN_IF_FAILED(toast->QueryInterface(&toast2));

			HSTRING hGroup;
			RETURN_IF_FAILED(toast2->get_Group(&hGroup));

			auto group1 = WindowsGetStringRawBuffer(hGroup, nullptr);

			if (group1 == wstring(group))
			{
				RETURN_IF_FAILED(notifier.Get()->RemoveFromSchedule(toast));
			}
		}

		return S_OK;
	}

	HRESULT EnsureRegistered()
	{
		// If not registered AUMID yet
		if (!s_registeredAumidAndComServer)
		{
			// Otherwise, incorrect usage, must call RegisterAumidAndComServer first
			return E_ILLEGAL_METHOD_CALL;
		}

		// If not registered activator yet
		if (!s_registeredActivator)
		{
			// Incorrect usage, must call RegisterActivator first
			return E_ILLEGAL_METHOD_CALL;
		}

		return S_OK;
	}

	wstring GetAppDataFolderIconPath(const wchar_t* amuid)
	{
		PWSTR path;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

		wstringstream stream;
		stream << path << LR"(\ToastNotificationManagerCompat\Apps\)" << amuid << LR"(\Icon.ico)";

		return stream.str();
	}

	class DECLSPEC_UUID("A42CD370-8D06-4E3C-BC45-32A61F9E375A") NotificationActivator WrlSealed
		: public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE Activate(
			_In_ LPCWSTR appUserModelId,
			_In_ LPCWSTR invokedArgs,
			_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
			ULONG dataCount) override
		{
			return S_OK;
		}
	};

	class DECLSPEC_UUID("A42CD370-8D06-4E3C-BC45-32A61F9E375B") NotificationActivator2 WrlSealed
		: public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE Activate(
			_In_ LPCWSTR appUserModelId,
			_In_ LPCWSTR invokedArgs,
			_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
			ULONG dataCount) override
		{
			return S_OK;
		}
	};

	class DECLSPEC_UUID("A42CD370-8D06-4E3C-BC45-32A61F9E375C") NotificationActivator3 WrlSealed
		: public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE Activate(
			_In_ LPCWSTR appUserModelId,
			_In_ LPCWSTR invokedArgs,
			_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
			ULONG dataCount) override
		{
			return S_OK;
		}
	};

	// Flag class as COM creatable
	CoCreatableClass(NotificationActivator);
	CoCreatableClass(NotificationActivator2);
	CoCreatableClass(NotificationActivator3);
}

DesktopNotificationHistoryCompat::DesktopNotificationHistoryCompat(const wchar_t* aumid, ComPtr<ABI::IToastNotificationHistory> history)
{
	m_aumid = wstring(aumid);
	m_history = history;
}

HRESULT DesktopNotificationHistoryCompat::Clear()
{
	return m_history->ClearWithId(HStringReference(m_aumid.c_str()).Get());
}

HRESULT DesktopNotificationHistoryCompat::GetHistory(ABI::Windows::Foundation::Collections::IVectorView<ABI::ToastNotification*>** toasts)
{
	ComPtr<ABI::IToastNotificationHistory2> history2;
	RETURN_IF_FAILED(m_history.As(&history2));

	return history2->GetHistoryWithId(HStringReference(m_aumid.c_str()).Get(), toasts);
}

HRESULT DesktopNotificationHistoryCompat::Remove(const wchar_t* tag)
{
	return m_history->Remove(HStringReference(tag).Get());
}

HRESULT DesktopNotificationHistoryCompat::RemoveGroupedTag(const wchar_t* tag, const wchar_t* group)
{
	return m_history->RemoveGroupedTagWithId(HStringReference(tag).Get(), HStringReference(group).Get(), HStringReference(m_aumid.c_str()).Get());

}

HRESULT DesktopNotificationHistoryCompat::RemoveGroup(const wchar_t* group)
{
	return m_history->RemoveGroupWithId(HStringReference(group).Get(), HStringReference(m_aumid.c_str()).Get());

}
