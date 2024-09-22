#pragma once

namespace Gallop
{
	class LocalPushDefine
	{
	public:
		enum LocalPushType
		{
			Tp,
			Rp,
			Job1,
			Job2,
			Job3,
			Job4,
			Job5,
			Job6,
			Job7,
			Job8,
		};

		static LocalPushType GetJobsLocalPushTypeByIndex(int index)
		{
			switch (index)
			{
			case 1:
				return LocalPushType::Job2;
			case 2:
				return LocalPushType::Job3;
			case 3:
				return LocalPushType::Job4;
			case 4:
				return LocalPushType::Job5;
			case 5:
				return LocalPushType::Job6;
			case 6:
				return LocalPushType::Job7;
			case 7:
				return LocalPushType::Job8;
			}

			return LocalPushType::Job1;
		}
	};
}
