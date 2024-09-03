#pragma once

namespace System
{
	template<typename T>
	struct Nullable
	{
	public:
		T value;
		bool has_value;
	};
}
