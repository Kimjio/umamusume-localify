#pragma once
#include <stdinclude.hpp>
#include "SettingsDOM.hpp"

typedef rapidjson::GenericDocument<rapidjson::UTF16<>> U16Document;
typedef rapidjson::GenericValue<rapidjson::UTF16<>> U16Value;

namespace Localify
{
	class SettingsUI
	{
	public:
		static void OpenSettings();
		static void OpenLiveSettings();

	private:
		static void OpenSelectOption(const Il2CppChar* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected, function<void(string, il2cppstring)> optionStyle = [](string, il2cppstring) {});
		static void OpenSelectGraphicsQualityOption(const Il2CppChar* title, vector<string> options, int selectedIndex, function<void(int)> optionSelected);

		static PWSTR FolderOpen();

		template<typename T>
		static void AddOrSet(U16Document& document, const Il2CppChar* name, T value)
		{
			if (document.HasMember(name))
			{
				document[name].Set(value);
			}
			else
			{
				U16Value v;
				v.Set(value);

				document.AddMember(rapidjson::StringRef(name), v, document.GetAllocator());
			}
		}

		static void AddOrSetString(U16Document& document, const Il2CppChar* name, il2cppstring value);
	};
}
