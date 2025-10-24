#pragma once
#include "game.hpp"

namespace System
{
	template<typename T>
	struct Nullable
	{
	private:
		union
		{
			T value2020;
			bool has_value;
		};
		union
		{
			T value;
			bool has_value2020;
		};

	public:
		bool HasValue() const
		{
			if (Game::CurrentUnityVersion != Game::UnityVersion::Unity22)
			{
				return has_value2020;
			}

			return has_value;
		}

		T Value() const
		{
			if (Game::CurrentUnityVersion != Game::UnityVersion::Unity22)
			{
				return value2020;
			}

			return value;
		}

		void Value(T v)
		{
			if (Game::CurrentUnityVersion != Game::UnityVersion::Unity22)
			{
				value2020 = v;
				has_value2020 = true;
			}
			else
			{
				value = v;
				has_value = true;
			}
		}
	};
}
