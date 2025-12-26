#pragma once

#include <string>

#define Unity2022 "2022.3."s

using namespace std;

namespace Game {
    enum class Region {
        UNKNOWN,
        JPN,
        KOR,
        ENG,
    };

    enum class Store {
        Google,
        Steam,
        // Ex. OneStore, MyCard...
        Other,
    };

    enum class UnityVersion
    {
        Unity22,
		Unknown,
    };

    inline auto CurrentGameRegion = Region::UNKNOWN;
    inline auto CurrentGameStore = Store::Other;
	inline auto CurrentUnityVersion = UnityVersion::Unknown;
}
