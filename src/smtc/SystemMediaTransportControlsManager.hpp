#pragma once

#define NOMINMAX

#include <Windows.h>
#include <shlobj.h>

#include <wrl.h>
#include <wil/com.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <windows.media.control.h>

#include <string>
#include <filesystem>

#include <SystemMediaTransportControlsInterop.h>

#include "il2cpp/il2cpp_symbols.hpp"
#include "il2cpp/il2cpp-api-functions.hpp"
#include "string_utils.hpp"
#include "masterdb/masterdb.hpp"

#include "scripts/UnityEngine.CoreModule/UnityEngine/Rect.hpp"

using namespace std;

using namespace ABI::Windows::Media;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;

namespace SystemMediaTransportControlsManager
{
	inline winrt::Windows::Media::ISystemMediaTransportControls instance;

	inline int musicId = 0;

	void Initialze(HWND hWnd)
	{
		winrt::com_ptr<ABI::Windows::Media::ISystemMediaTransportControls> abiInstance;
		winrt::get_activation_factory<ISystemMediaTransportControlsInterop>(RuntimeClass_Windows_Media_SystemMediaTransportControls)->GetForWindow(hWnd, __uuidof(abiInstance), abiInstance.put_void());

		instance = abiInstance.as<winrt::Windows::Media::ISystemMediaTransportControls>();
		instance.IsEnabled(false);

		auto updater = instance.DisplayUpdater();
		updater.Type(winrt::Windows::Media::MediaPlaybackType::Music);
		updater.Update();
	}

	void CreateShortcutForSMTC(const wchar_t* fileName)
	{
		HRESULT hres;
		IShellLinkW* psl = nullptr;

		PWSTR path;
		SHGetKnownFolderPath(FOLDERID_Programs, 0, NULL, &path);

		wstring lnkPath = wstring(path).append(L"\\").append(fileName).append(L".lnk");

		if (filesystem::exists(lnkPath))
		{
			return;
		}

		// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
		// has already been called.
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<LPVOID*>(&psl));
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf = nullptr;

			// Set the path to the shortcut target and add the description. 
			psl->SetPath(filesystem::current_path().append(L"umamusume.exe").wstring().data());
			psl->SetWorkingDirectory(filesystem::current_path().wstring().data());
			psl->SetDescription(fileName);

			// Query IShellLink for the IPersistFile interface, used for saving the 
			// shortcut in persistent storage. 
			hres = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID*>(&ppf));

			if (SUCCEEDED(hres))
			{
				// Save the link by calling IPersistFile::Save. 
				ppf->Save(lnkPath.data(), TRUE);
				ppf->Release();
			}
			psl->Release();
		}
	}

	void UpdateMetadata(int music_id = musicId)
	{
		if (music_id == 0)
		{
			return;
		}

		if (musicId != music_id)
		{
			instance.DisplayUpdater().Thumbnail(nullptr);
		}

		musicId = music_id;

		try
		{
			auto& smtc = instance;
			smtc.IsPlayEnabled(true);
			smtc.IsPauseEnabled(true);

			auto updater = smtc.DisplayUpdater();

			auto props = updater.MusicProperties();

			props.Title(u8_wide(MasterDB::GetTextData(16, music_id)));

			auto artist = MasterDB::GetTextData(17, music_id);
			replaceAll(artist, "\\n", ", ");
			props.Artist(u8_wide(artist));

			if (updater.Thumbnail())
			{
				return;
			}

			auto music_id_str = to_string(music_id);
			size_t n_zero = 4;
			auto new_str = std::string(n_zero - std::min(n_zero, music_id_str.length()), '0') + music_id_str;

			auto jacket_icon = "jacket_icon_m_"s + new_str;
			auto path = "Live/Jacket/"s + jacket_icon;

			auto loader = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("umamusume.dll", "Gallop", "AssetManager", "get_Loader", IgnoreNumberOfArguments)();
			auto asset = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, bool)>(loader->klass, "LoadAssetHandle", 2)->methodPointer(loader, il2cpp_string_new(path.data()), false);
			if (!asset)
			{
				return;
			}

			auto assetBundle = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(asset->klass, "get_assetBundle", 0)->methodPointer(asset);
			if (!assetBundle)
			{
				return;
			}

			auto texture2DType = reinterpret_cast<Il2CppType*>(::GetRuntimeType("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			auto texture2D = reinterpret_cast<Il2CppObject * (*)(Il2CppObject*, Il2CppString*, const Il2CppType*)>(il2cpp_resolve_icall("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)"))(assetBundle, il2cpp_string_new(jacket_icon.data()), texture2DType);

			winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;

			winrt::Windows::Storage::Streams::DataWriter dataWriter{ stream };

			auto width = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture2D->klass, "get_width", 0)->methodPointer(texture2D);

			auto height = il2cpp_class_get_method_from_name_type<int (*)(Il2CppObject*)>(texture2D->klass, "get_height", 0)->methodPointer(texture2D);

			auto renderTexture = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int, int, int, int)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "GetTemporary", 4)(width, height, 0, 0);

			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "Graphics", "Blit", 2)(texture2D, renderTexture);

			auto previous = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)()>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "get_active", -1)();

			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "set_active", 1)(renderTexture);

			auto readableTexture = il2cpp_object_new(il2cpp_symbols::get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Texture2D"));
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, int, int)>(readableTexture->klass, ".ctor", 2)->methodPointer(readableTexture, width, height);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, UnityEngine::Rect, int, int)>(readableTexture->klass, "ReadPixels", 3)->methodPointer(readableTexture, UnityEngine::Rect{ 0, 0, static_cast<float>(width), static_cast<float>(height) }, 0, 0);
			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(readableTexture->klass, "Apply", 0)->methodPointer(readableTexture);

			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "set_active", 1)(previous);

			il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*)>("UnityEngine.CoreModule.dll", "UnityEngine", "RenderTexture", "ReleaseTemporary", 1)(renderTexture);

			auto method = il2cpp_symbols::get_method("UnityEngine.ImageConversionModule.dll", "UnityEngine", "ImageConversion", "EncodeToPNG", 1);

			void** params = new void* [1];
			params[0] = readableTexture;

			Il2CppException* exception;

			auto pngData = reinterpret_cast<Il2CppArraySize_t<uint8_t>*>(il2cpp_runtime_invoke(method, nullptr, params, &exception));

			if (!exception)
			{
				dataWriter.WriteBytes(winrt::array_view(pngData->vector, pngData->max_length));
				dataWriter.StoreAsync().Completed([dataWriter, updater, stream](winrt::Windows::Foundation::IAsyncOperation<uint32_t> const& asyncInfo, winrt::Windows::Foundation::AsyncStatus const& asyncStatus)
					{
						updater.Thumbnail(winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromStream(stream));
						updater.Update();
					});
			}
			else
			{
				wcout << exception->message->chars << endl;
			}
		}
		catch (winrt::hresult_error& e)
		{
			wcout << e.code().value << " " << e.message().data() << endl;
		}
	}
}
