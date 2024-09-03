#pragma once

namespace CodeStage
{
	namespace AntiCheat
	{
		namespace ObscuredTypes
		{
			struct ObscuredLong
			{
				long long GetDecrypted() const
				{
					if (!currentCryptoKey)
					{
						return hiddenValue ^ 123456LL;
					}

					return hiddenValue ^ currentCryptoKey;
				}

			private:
				long long currentCryptoKey;
				long long hiddenValue;
				bool inited;
				long long fakeValue;
				bool fakeValueActive;
			};
		}
	}
}
