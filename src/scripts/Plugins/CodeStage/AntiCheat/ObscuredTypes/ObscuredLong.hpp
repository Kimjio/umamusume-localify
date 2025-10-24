#pragma once

#include <cstdint>
#include "il2cpp/il2cpp_symbols.hpp"

namespace CodeStage
{
	namespace AntiCheat
	{
		namespace ObscuredTypes
		{
			struct ObscuredLong
			{
				ObscuredLong(int64_t value)
				{
					currentCryptoKey = cryptoKey();
					hiddenValue = Encrypt(value);
					bool existsAndIsRunning = il2cpp_symbols::get_method_pointer<bool (*)()>("Plugins.dll", "CodeStage.AntiCheat.Detectors", "ObscuredCheatingDetector", "get_ExistsAndIsRunning", 0)();
					fakeValue = (existsAndIsRunning ? value : 0);
					fakeValueActive = existsAndIsRunning;
					inited = true;
				}

				int64_t Encrypt(int64_t value, int64_t key)
				{
					if (key == 0)
					{
						return value ^ cryptoKey();
					}
					return value ^ key;
				}

				int64_t Encrypt(int64_t value)
				{
					return Encrypt(value, 0);
				}

				int64_t GetDecrypted() const
				{
					if (!currentCryptoKey)
					{
						return hiddenValue ^ 123456LL;
					}

					return hiddenValue ^ currentCryptoKey;
				}

			private:
				int64_t currentCryptoKey;
				int64_t hiddenValue;
				bool inited;
				int64_t fakeValue;
				bool fakeValueActive;

				static int64_t cryptoKey()
				{
					auto klass = il2cpp_symbols::get_class("Plugins.dll", "CodeStage.AntiCheat.ObscuredTypes", "ObscuredLong");
					auto cryptoKeyField = il2cpp_class_get_field_from_name(klass, "cryptoKey");
					int64_t cryptoKey;
					il2cpp_field_static_get_value(cryptoKeyField, &cryptoKey);

					return cryptoKey;
				}
			};
		}
	}
}
