#pragma once

namespace System
{
	template<typename T1, typename T2>
	struct ValueTuple
	{
	public:
		T1 Item1;
		T2 Item2;
	};

	template<typename T1, typename T2, typename T3>
	struct ValueTuple3
	{
	public:
		T1 Item1;
		T2 Item2;
		T3 Item3;
	};
}
