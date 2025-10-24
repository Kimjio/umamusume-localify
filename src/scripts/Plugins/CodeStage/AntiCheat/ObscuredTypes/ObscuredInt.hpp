#pragma once

#include "il2cpp/il2cpp_symbols.hpp"

namespace CodeStage
{
	namespace AntiCheat
	{
		namespace ObscuredTypes
		{
			struct ObscuredInt
			{
				ObscuredInt(int value)
				{
					currentCryptoKey = cryptoKey();
					hiddenValue = Encrypt(value);
					bool existsAndIsRunning = il2cpp_symbols::get_method_pointer<bool (*)()>("Plugins.dll", "CodeStage.AntiCheat.Detectors", "ObscuredCheatingDetector", "get_ExistsAndIsRunning", 0)();
					fakeValue = (existsAndIsRunning ? value : 0);
					fakeValueActive = existsAndIsRunning;
					inited = true;
				}

				int Encrypt(int value, int key)
				{
					if (key == 0)
					{
						return value ^ cryptoKey();
					}
					return value ^ key;
				}

				int Encrypt(int value)
				{
					return Encrypt(value, 0);
				}

				int GetDecrypted() const
				{
					if (!currentCryptoKey)
					{
						return hiddenValue ^ 123456;
					}

					return hiddenValue ^ currentCryptoKey;
				}

			private:
				int currentCryptoKey;
				int hiddenValue;
				bool inited;
				int fakeValue;
				bool fakeValueActive;

				static int cryptoKey()
				{
					auto klass = il2cpp_symbols::get_class("Plugins.dll", "CodeStage.AntiCheat.ObscuredTypes", "ObscuredInt");
					auto cryptoKeyField = il2cpp_class_get_field_from_name(klass, "cryptoKey");
					int cryptoKey;
					il2cpp_field_static_get_value(cryptoKeyField, &cryptoKey);

					return cryptoKey;
				}
			};
		}
	}
}
