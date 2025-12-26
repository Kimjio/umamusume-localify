#pragma once
#include "game.hpp"

namespace System
{
	template<typename T>
	struct Nullable
	{
	private:
		bool has_value;
		T value;

	public:
		bool HasValue() const
		{
			return has_value;
		}

		T Value() const
		{
			return value;
		}

		void Value(T v)
		{
			value = v;
			has_value = true;
		}
	};
}
