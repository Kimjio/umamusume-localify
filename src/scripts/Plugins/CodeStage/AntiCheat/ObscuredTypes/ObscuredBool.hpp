#pragma once

namespace CodeStage
{
	namespace AntiCheat
	{
		namespace ObscuredTypes
		{
			struct ObscuredBool
			{
				bool GetDecrypted() const
				{
					char key = currentCryptoKey;
					if (!key)
					{
						key = 9;
					}

					return (hiddenValue ^ key) != 181;
				}

			private:
				char currentCryptoKey;
				int hiddenValue;
				bool inited;
				bool fakeValue;
				bool fakeValueActive;
			};
		}
	}
}
