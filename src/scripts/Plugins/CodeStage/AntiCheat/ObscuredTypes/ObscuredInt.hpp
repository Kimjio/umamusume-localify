#pragma once

namespace CodeStage
{
	namespace AntiCheat
	{
		namespace ObscuredTypes
		{
			struct ObscuredInt
			{
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
			};
		}
	}
}
