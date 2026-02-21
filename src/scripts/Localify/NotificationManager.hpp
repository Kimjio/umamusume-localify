#pragma once
#include <stdinclude.hpp>

namespace Localify
{
	class NotificationManager
	{
	public:
		static void Init();
		static void Cleanup();
		static void Reset();
		static void Show(Il2CppString* text);
		static void SetDisplayTime(float time);
		static void SetFontSize(int size);
		static void SetFontColor(il2cppstring color);
		static void SetOutlineSize(il2cppstring size);
		static void SetOutlineColor(il2cppstring color);
		static void SetBackgroundAlpha(float alpha);
		static void SetPosition(float x, float y);

	private:
		static Il2CppObject* notification;
		static bool isRequiredInit;
	};
}
