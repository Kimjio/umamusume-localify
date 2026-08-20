#pragma once

#include <string>
#include <cstdint>

#ifndef _MSC_VER

#include <unistd.h>

#endif

#define Unity2022 "2022.3."s

using namespace std;

namespace Game {
    enum class Region : uint8_t {
        UNKNOWN,
        JPN,
        KOR,
        TWN,
#ifdef __ANDROID__
        CHN,
#endif
        ENG,
    };

    enum class Store : uint8_t {
        Google,
#ifdef _MSC_VER
        Steam,
#endif
#ifdef __ANDROID__
        MyCard,
        Tencent,
#endif
        Other
    };

    enum class UnityVersion : uint8_t {
        Unity22,
        Unity20,
        Unknown,
    };

    inline auto CurrentGameRegion = Region::UNKNOWN;
    inline auto CurrentGameStore = Store::Google;
    inline auto CurrentUnityVersion = UnityVersion::Unknown;

#ifdef __ANDROID__
    inline auto GamePackageName = "jp.co.cygames.umamusume"s;
    inline auto GamePackageNameKor = "com.kakaogames.umamusume"s;
    inline auto GamePackageNameTwnGoogle = "com.komoe.kmumamusumegp"s;
    inline auto GamePackageNameTwnMyCard = "com.komoe.umamusumeofficial"s;
    inline auto GamePackageNameChn = "com.bilibili.umamusu"s;
    inline auto GamePackageNameChnAlt = "com.tencent.tmgp.bilibili.umamusu"s;
    inline auto GamePackageNameEng = "com.cygames.umamusume"s;

    static bool IsPackageNameEqualsByGameRegion(const char *pkgNm, Region gameRegion) {
        const string pkgNmStr = string(pkgNm);
        if (pkgNmStr.empty()) {
            return false;
        }
        switch (gameRegion) {
            case Region::JPN:
                if (pkgNmStr == GamePackageName) {
                    CurrentGameRegion = Region::JPN;
                    CurrentGameStore = Store::Google;
                    return true;
                }
                break;
            case Region::KOR:
                if (pkgNmStr == GamePackageNameKor) {
                    CurrentGameRegion = Region::KOR;
                    CurrentGameStore = Store::Google;
                    return true;
                }
                break;
            case Region::TWN:
                if (pkgNmStr == GamePackageNameTwnGoogle) {
                    CurrentGameRegion = Region::TWN;
                    CurrentGameStore = Store::Google;
                    return true;
                } else if (pkgNmStr == GamePackageNameTwnMyCard) {
                    CurrentGameRegion = Region::TWN;
                    CurrentGameStore = Store::MyCard;
                    return true;
                }
                break;
            case Region::CHN:
                if (pkgNmStr == GamePackageNameChn) {
                    CurrentGameRegion = Region::CHN;
                    CurrentGameStore = Store::Other;
                    return true;
                } else if (pkgNmStr == GamePackageNameChnAlt) {
                    CurrentGameRegion = Region::CHN;
                    CurrentGameStore = Store::Tencent;
                    return true;
                }
                break;
            case Region::ENG:
                if (pkgNmStr == GamePackageNameEng) {
                    CurrentGameRegion = Region::ENG;
                    CurrentGameStore = Store::Google;
                    return true;
                }
                break;
            case Region::UNKNOWN:
            default:
                break;
        }
        return false;
    }

    static string
    GetPackageNameByGameRegionAndGameStore(Region gameRegion, Store gameStore) {
        if (gameRegion == Region::JPN) {
            return GamePackageName;
        }
        if (gameRegion == Region::KOR) {
            return GamePackageNameKor;
        }
        if (gameRegion == Region::TWN) {
            if (gameStore == Store::MyCard) {
                return GamePackageNameTwnMyCard;
            }
            return GamePackageNameTwnGoogle;
        }
        if (gameRegion == Region::ENG) {
            return GamePackageNameEng;
        }
        if (gameRegion == Region::CHN) {
            if (gameStore == Store::Tencent) {
                return GamePackageNameChnAlt;
            }

            return GamePackageNameChn;
        }
        return "";
    }

    static string GetCurrentPackageName() {
        return GetPackageNameByGameRegionAndGameStore(CurrentGameRegion, CurrentGameStore);
    }

    static Region CheckPackageNameByDataPath() {
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::JPN,
                                                                       Store::Google)).append(
                        "/cache").data(),
                F_OK) == 0) {
            return Region::JPN;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::KOR,
                                                                       Store::Google)).append(
                        "/cache").data(),
                F_OK) == 0) {
            return Region::KOR;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::TWN,
                                                                       Store::Google)).append(
                        "/cache").data(),
                F_OK) == 0) {
            return Region::TWN;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::TWN,
                                                                       Store::MyCard)).append(
                        "/cache").data(),
                F_OK) == 0) {
            CurrentGameStore = Store::MyCard;
            return Region::TWN;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::CHN,
                                                                       Store::Other)).append(
                        "/cache").data(),
                F_OK) == 0) {
            CurrentGameStore = Store::Other;
            return Region::CHN;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::CHN,
                                                                       Store::Tencent)).append(
                        "/cache").data(),
                F_OK) == 0) {
            CurrentGameStore = Store::Tencent;
            return Region::CHN;
        }
        if (access(
                "/data/data/"s
                        .append(GetPackageNameByGameRegionAndGameStore(Region::ENG,
                                                                       Store::Google)).append(
                        "/cache").data(),
                F_OK) == 0) {
            return Region::ENG;
        }

        return Region::UNKNOWN;
    }

#endif
}
