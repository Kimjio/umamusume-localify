#pragma once

#include <string>

#define Unity2020 "2020.3."s

using namespace std;

namespace Game {
    enum class Region {
        UNKNOWN,
        JAP,
        KOR,
    };

    inline auto CurrentGameRegion = Region::UNKNOWN;
}
