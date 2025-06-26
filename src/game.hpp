#pragma once

#include <string>

#define Unity2019 "2019.4."s
#define Unity2020 "2020.3."s
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

    inline auto CurrentGameRegion = Region::UNKNOWN;
    inline auto CurrentGameStore = Store::Other;
}
