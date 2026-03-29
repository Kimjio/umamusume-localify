#pragma once
#include "MonoSingleton.hpp"

namespace Gallop
{
	class WindowsGamepadControl : public MonoSingleton<WindowsGamepadControl>
	{
	public:
		static WindowsGamepadControl Instance()
		{
			auto klass = il2cpp_symbols::get_class("umamusume.dll", WindowsGamepadControl::namespaze, WindowsGamepadControl::klassName);

			if (!klass)
			{
				klass = il2cpp_symbols::get_class("umamusume.dll", WindowsGamepadControl::namespaze, WindowsGamepadControl::klassNameLegacy);
			}

			if (!klass)
			{
				return nullptr;
			}

			auto instanceField = il2cpp_class_get_field_from_name(klass, "_instance");
			Il2CppObject* instance;
			il2cpp_field_static_get_value(instanceField, &instance);
			return WindowsGamepadControl(instance);
		}

		void ReleaseRenderTexture();

		void UpdateInputControls();

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
		static const char* klassNameLegacy;
	};
}
